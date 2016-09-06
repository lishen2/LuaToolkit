#include <winsock2.h>
#include <Ws2tcpip.h>
#include <in6addr.h>
#include <stdint.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "common.h"
#include "list.h"
#include "lua_net.h"

#define NET_METATABLE_SERVER "LuaToolkit.Server"
#define NET_METATABLE_CONN   "LuaToolkit.Connection"
#define NET_METATABLE_UDP    "LuaToolkit.UDP"

enum NET_SERVER_STATE
{
    NET_SERVER_LISTENING, //监听中
    NET_SERVER_ERROR,     //连接出错
    NET_SERVER_CLOSED,    //已经断开
};

enum NET_CONN_STATE
{
    NET_CONN_CONNECTING,  //连接中
    NET_CONN_CONNECTED,   //已经连接
    NET_CONN_ERROR,       //连接出现错误
    NET_CONN_CLOSED,      //连接已经关闭
};

enum NET_UDP_STATE
{
    NET_UDP_READY,       //连接完成的状态
    NET_UDP_ERROR,       //连接出现错误
    NET_UDP_CLOSED,      //连接已经关闭
};

struct NET_Server{
    struct list_head node;
	int fd;              //服务端FD
	int state;           //连接状态
	//参数
	int family;
    struct sockaddr_in  addr; //参数
    struct sockaddr_in6 addr6;//参数
    //事件句柄
	int onConnect;       //事件，客户端连接
};

struct NET_Conn{
    struct list_head node;
    //fd和状态
	int fd;              //连接的FD
	int state;           //连接状态
	//参数
	int family;
    struct sockaddr_in  addr; //参数
    struct sockaddr_in6 addr6;//参数
    //事件句柄
	int onConnect;       //事件，连接到服务器
	int onData;          //事件，收到数据
	int onClose;         //事件，连接断开
};

struct NET_Udp{
    struct list_head node;

    //fd和状态
    int fd;
    int state;
    //参数
	int family;
    struct sockaddr_in  addr; 
    struct sockaddr_in6 addr6;
    //事件句柄
    int onData;
};

static struct list_head g_servers;
static struct list_head g_conns;
static struct list_head g_udps;

static void _readIP4(lua_State *L, const char* key, struct in_addr *addr)
{
    const char* str;
    int ret;
    
    luaL_checktype(L, -1, LUA_TTABLE);
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    luaL_checkstring(L, -1);
    str = lua_tostring(L, -1);

    if (0 == strcmp(str, "any")){
        addr->s_addr = INADDR_ANY;
    } else {
        ret = inet_pton(AF_INET, str, addr);
        if (1 != ret){
            luaL_error(L, "ipv4 addr error[%s]\n", str);
        }
    }
    
    lua_pop(L, 1);

    return;
}

static void _readIP6(lua_State *L, const char* key, struct in_addr6 *addr)
{
    const char* str;
    int ret;
    
    luaL_checktype(L, -1, LUA_TTABLE);
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    luaL_checkstring(L, -1);
    str = lua_tostring(L, -1);
    
    if (0 == strcmp(str, "any")){
        memcpy(addr, &in6addr_any, sizeof(struct in_addr6));
    } else {
        ret = inet_pton(AF_INET6, str, addr);
        if (1 != ret){
            luaL_error(L, "ipv6 addr error[%s]\n", str);
        }
    }
    
    lua_pop(L, 1);

    return;
}

static int _readFamily(lua_State *L, const char* key)
{
    const char* str;
    int family;
    
    luaL_checktype(L, -1, LUA_TTABLE);
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    luaL_checkstring(L, -1);
    str = lua_tostring(L, -1);
    if (0 == strcmp(str, "ipv4")){
        family = AF_INET;
    } else if (0 == strcmp(str, "ipv6")){
        family = AF_INET6;
    } else {
        family = 0;
        luaL_error(L, "Family type error, options \"ipv4\" or \"ipv6\"\n");
    }
    lua_pop(L, 1);

    return family;    
}

static int _readNumber(lua_State *L, const char* key)
{
    int num;
    
    luaL_checktype(L, -1, LUA_TTABLE);
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    num = (int)luaL_checkinteger(L, -1);
    lua_pop(L, 1);

    return num;
}

static void _serverInit(struct NET_Server* srv)
{
    memset(srv, 0, sizeof(*srv));
    srv->fd = -1;
    srv->onConnect = -1;

    return;    
}

static int _serverSocketCreate(struct NET_Server *srv)
{
    int fd;
    int ret;
    unsigned long opt = 1;

    //建立socket
    fd = socket(srv->family, SOCK_STREAM, 0);
    if (fd < 0){
        return ERR_FAILED;
    }
    srv->fd = fd;

    //绑定地址
    if (AF_INET6 == srv->family){
        ret = bind(srv->fd, (struct sockaddr*)&srv->addr6, sizeof(struct sockaddr_in6));
    } else {
        ret = bind(srv->fd, (struct sockaddr*)&srv->addr, sizeof(struct sockaddr_in));
    }
    if (0 != ret){
        return ERR_FAILED;
    }

    //设置异步
    ret = ioctlsocket(srv->fd, FIONBIO, (unsigned long *)&opt);//设置成非阻塞模式。   
    if (0 != ret){
        return ERR_FAILED;
    }

    //监听
    ret = listen(srv->fd, 5);
    if (0 != ret){
        return ERR_FAILED;
    }

    return ERR_OK;
}

static int _serverCreate(lua_State *L)
{
    struct NET_Server *srv;
    struct sockaddr_in addr;
    struct sockaddr_in6 addr6;    
    int family;
    int ret;
    uint16_t port;

	//保证栈上有一个空闲位置
	luaL_checkstack(L, 1, "LUA Stack OverFlow");    
    
    //读取配置
    port = (uint16_t)_readNumber(L, "port");
    family = _readFamily(L, "family");
    if (AF_INET6 == family){
        memset(&addr6, 0, sizeof(addr6));
        addr6.sin6_family = AF_INET6;
        addr6.sin6_port = htons(port);
        _readIP6(L, "ip_addr", &addr6.sin6_addr);
    } else {
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);    
        _readIP4(L, "ip_addr", &addr.sin_addr);
    }

    //创建服务器结构
    srv = (struct NET_Server *)lua_newuserdata(L, sizeof(struct NET_Server));
    luaL_getmetatable(L, NET_METATABLE_SERVER);
    lua_setmetatable(L, -2);
    _serverInit(srv);

    //设置参数
    srv->state = NET_SERVER_LISTENING;
    srv->family = family;
    memcpy(&(srv->addr6), &addr6, sizeof(addr6));
    memcpy(&(srv->addr), &addr, sizeof(addr));

    //创建socket并监听
    ret = _serverSocketCreate(srv);
    if (ERR_OK != ret){
        srv->state = NET_SERVER_ERROR;
    }

    //挂接全局链表
    list_add_tail(&(srv->node), &g_servers);
        
    return 1;
}

static int _serverOn(lua_State *L)
{
    struct NET_Server *srv;
    const char* event;
    
    srv = (struct NET_Server *)luaL_checkudata(L, -3, NET_METATABLE_SERVER);
    event = luaL_checkstring(L, -2);
    luaL_checktype(L, -1, LUA_TFUNCTION);

    if (0 == strcmp(event, "connect")){
        if (-1 != srv->onConnect){
            luaL_unref(L, LUA_REGISTRYINDEX, srv->onConnect);
        }
        srv->onConnect = luaL_ref(L, LUA_REGISTRYINDEX);
        //TODO, luaL_ref会把参数出栈，这样无法返回正确的数据
        //按照理解，lua函数中是不应该改变堆栈中的数据的
    }
    
    return 0;
}

static int _serverClose(lua_State *L)
{
    struct NET_Server *srv;
    srv = (struct NET_Server *)luaL_checkudata(L, -1, NET_METATABLE_SERVER);

    //关闭socket
    if (-1 != srv->fd){
        closesocket(srv->fd);
        srv->fd = -1;
    }

    //摘链表
    list_del(&(srv->node));

    return 0;
}

static void _connInit(struct NET_Conn *conn)
{
    memset(conn, 0, sizeof(*conn));
    conn->fd = -1;
    conn->onClose = -1;
    conn->onConnect = -1;
    conn->onData = -1;

    return;
}

static int _connSocketCreate(struct NET_Conn* conn)
{
    unsigned long opt = 1;
    int fd;
    int ret;
        
    //建立socket
    fd = socket(conn->family, SOCK_STREAM, 0);
    if (fd >= 0){

        //设置异步
        conn->fd = fd;
        ret = ioctlsocket(conn->fd, FIONBIO, (unsigned long *)&opt);
    }

    //设置异步失败，后续有流程关闭conn->fd中保存的句柄
    return ret;
}

static int _connSocketConnect(struct NET_Conn *conn)
{
    int ret;
    
    if (AF_INET6 == conn->family){
        ret = connect(conn->fd, (struct sockaddr*)&(conn->addr6), sizeof(struct sockaddr_in6));
    } else {
        ret = connect(conn->fd, (struct sockaddr*)&(conn->addr), sizeof(struct sockaddr_in));
    }

    return ret;
}

static int _connCreate(lua_State *L)
{
    struct NET_Conn *conn;
    struct sockaddr_in addr;
    struct sockaddr_in6 addr6;
    int family;
    int ret;
    uint16_t port;
    
	//保证栈上有一个空闲位置
	luaL_checkstack(L, 1, "LUA Stack OverFlow");    

	//读取配置
    port = (uint16_t)_readNumber(L, "port");
    family = _readFamily(L, "family");
    if (AF_INET6 == family){
        memset(&addr6, 0, sizeof(addr6));
        addr6.sin6_family = AF_INET6;
        addr6.sin6_port = htons(port);
        _readIP6(L, "ip_addr", &addr6.sin6_addr);
    } else {
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);    
        _readIP4(L, "ip_addr", &addr.sin_addr);
    }

    //创建连接结构
    conn = (struct NET_Conn *)lua_newuserdata(L, sizeof(struct NET_Conn));
    luaL_getmetatable(L, NET_METATABLE_CONN);
    lua_setmetatable(L, -2);
    _connInit(conn);

    //设置参数部分
    conn->state = NET_CONN_CONNECTING;
    conn->family = family;
    memcpy(&(conn->addr6), &addr6, sizeof(addr6));
    memcpy(&(conn->addr), &addr, sizeof(addr));

    //创建socket
    ret = _connSocketCreate(conn);
    if (0 != ret){
        luaL_error(L, "Error create socket\n");
    }

    //连接socket
    ret = _connSocketConnect(conn);
    if ((SOCKET_ERROR == ret) && (WSAEWOULDBLOCK != WSAGetLastError())){
        conn->state = NET_CONN_ERROR;
    }

    //挂接全局链表
    list_add_tail(&(conn->node), &g_conns);
    
    return 1;
}

static int _connOn(lua_State *L)
{
    struct NET_Conn *conn;
    const char* event;
    
    conn = (struct NET_Conn *)luaL_checkudata(L, -3, NET_METATABLE_CONN);
    event = luaL_checkstring(L, -2);
    luaL_checktype(L, -1, LUA_TFUNCTION);

    if (0 == strcmp(event, "connect")){
        if (-1 != conn->onConnect){
            luaL_unref(L, LUA_REGISTRYINDEX, conn->onConnect);
        }
        conn->onConnect = luaL_ref(L, LUA_REGISTRYINDEX);
    } else if (0 == strcmp(event, "disconnect")){
        if (-1 != conn->onClose){
            luaL_unref(L, LUA_REGISTRYINDEX, conn->onClose);
        }    
        conn->onClose = luaL_ref(L, LUA_REGISTRYINDEX);
    } else if (0 == strcmp(event, "data")){
        if (-1 != conn->onData){
            luaL_unref(L, LUA_REGISTRYINDEX, conn->onData);
        }
        conn->onData = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    
    return 0;
}

static int _connSend(lua_State *L)
{
    struct NET_Conn *conn;
    const uint8_t *data;
    int len;
    int pos;
    int ret;
    uint32_t ts;

  	//保证栈上有一个空闲位置
	luaL_checkstack(L, 1, "LUA Stack OverFlow"); 

    conn = (struct NET_Conn *)luaL_checkudata(L, -3, NET_METATABLE_CONN);
    data = luaL_checkstring(L, -2);
    len = (int)luaL_checkinteger(L, -1);

    ts = GetTickCount() + 100;
    pos = 0;
    while(pos < len){
        ret = send(conn->fd, data + pos, len - pos, 0);

        //错误发生，直接退出
        if (SOCKET_ERROR == ret && WSAEWOULDBLOCK != WSAGetLastError()){
            break;
        }

        //收到数据，增加读写指针
        if (ret >= 0) {
            pos += ret;
        } 

        //超时退出
        if (GetTickCount() > ts){
            break;
        }
    }

    lua_pushinteger(L, pos);
    
    return 1;
}

static int _connClose(lua_State *L)
{
    struct NET_Conn *conn;
    conn = (struct NET_Conn *)luaL_checkudata(L, -1, NET_METATABLE_CONN);

    //关闭socket
    if (-1 != conn->fd){
        closesocket(conn->fd);
        conn->fd = -1;
    }

    //摘链表
    list_del(&(conn->node));

    return 0;
}

static void _udpInit(struct NET_Udp *udpsock)
{
    memset(udpsock, 0, sizeof(*udpsock));
    udpsock->fd = -1;
    udpsock->onData = -1;

    return;
}

static int _udpSocketCreate(struct NET_Udp* udp)
{
    unsigned long opt = 1;
    int fd;
    int ret;
        
    //建立socket
    fd = socket(udp->family, SOCK_DGRAM, 0);
    if (fd < 0){
        return ERR_FAILED;
    }
    udp->fd = fd;

    //绑定地址
    if (AF_INET6 == udp->family){
        ret = bind(udp->fd, (struct sockaddr*)&udp->addr6, sizeof(struct sockaddr_in6));
    } else {
        ret = bind(udp->fd, (struct sockaddr*)&udp->addr, sizeof(struct sockaddr_in));
    }
    if (0 != ret){
        return ERR_FAILED;
    }

    //设置异步
    ret = ioctlsocket(udp->fd, FIONBIO, (unsigned long *)&opt);//设置成非阻塞模式。   
    if (0 != ret){
        return ERR_FAILED;
    }

    return ERR_OK;
}

static int _udpCreate(lua_State *L){
    
    struct NET_Udp *udp;
    struct sockaddr_in addr;
    struct sockaddr_in6 addr6;
    int family;
    int ret;
    uint16_t port;
    
	//保证栈上有一个空闲位置
	luaL_checkstack(L, 1, "LUA Stack OverFlow");    

	//读取配置
    port = (uint16_t)_readNumber(L, "port");
    family = _readFamily(L, "family");
    if (AF_INET6 == family){
        memset(&addr6, 0, sizeof(addr6));
        addr6.sin6_family = AF_INET6;
        addr6.sin6_port = htons(port);
        _readIP6(L, "ip_addr", &addr6.sin6_addr);
    } else {
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);    
        _readIP4(L, "ip_addr", &addr.sin_addr);
    }

    //创建连接结构
    udp = (struct NET_Udp *)lua_newuserdata(L, sizeof(struct NET_Udp));
    luaL_getmetatable(L, NET_METATABLE_UDP);
    lua_setmetatable(L, -2);
    _udpInit(udp);

    //设置参数部分
    udp->state = NET_UDP_READY;
    udp->family = family;
    memcpy(&(udp->addr6), &addr6, sizeof(addr6));
    memcpy(&(udp->addr), &addr, sizeof(addr));

    //创建socket
    ret = _udpSocketCreate(udp);
    if (0 != ret){
        luaL_error(L, "Error create socket\n");
    }

    //挂接全局链表
    list_add_tail(&(udp->node), &g_udps);
    
    return 1;
}

static int _udpOn(lua_State *L){
    struct NET_Udp *udp;
    const char* event;
    
    udp = (struct NET_Udp *)luaL_checkudata(L, -3, NET_METATABLE_UDP);
    event = luaL_checkstring(L, -2);
    luaL_checktype(L, -1, LUA_TFUNCTION);

    if (0 == strcmp(event, "data")){
        if (-1 != udp->onData){
            luaL_unref(L, LUA_REGISTRYINDEX, udp->onData);
        }
        udp->onData = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    
    return 0;    
}

static int _udpSend(lua_State *L){
    
    struct NET_Udp *udp;
    struct sockaddr_in addr;
    struct sockaddr_in6 addr6;
    const char *ipstr;
    uint16_t port;
    const uint8_t *data;
    int len;
    uint32_t ts;
    int pos;
    int ret;
    
	//保证栈上有一个空闲位置
	luaL_checkstack(L, 1, "LUA Stack OverFlow");    

	//读取配置
	udp = (struct NET_Udp *)luaL_checkudata(L, -5, NET_METATABLE_UDP);
    ipstr = luaL_checkstring(L, -4);
    port = (uint16_t)luaL_checkinteger(L, -3);
    data = luaL_checkstring(L, -2);
    len = (int)luaL_checkinteger(L, -1);

    //转换地址
    if (AF_INET6 == udp->family){
        memset(&addr6, 0, sizeof(addr6));
        addr6.sin6_family = AF_INET6;
        addr6.sin6_port = htons(port);
        ret = inet_pton(AF_INET6, ipstr, &addr6.sin6_addr);
    } else {
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);    
        ret = inet_pton(AF_INET, ipstr, &addr.sin_addr);
    }
    if (1 != ret){
        luaL_error(L, "Addr error[%s]\n", ipstr);
    }

    ts = GetTickCount() + 100;
    pos = 0;
    while(pos < len){
        //发送
        if (AF_INET6 == udp->family){
            ret = sendto(udp->fd, data + pos, len - pos, 0, (struct sockaddr*)&addr6, sizeof(addr6));
        } else {
            ret = sendto(udp->fd, data + pos, len - pos, 0, (struct sockaddr*)&addr, sizeof(addr));
        }

        //错误发生，直接退出
        if (SOCKET_ERROR == ret && WSAEWOULDBLOCK != WSAGetLastError()){
            break;
        }

        //收到数据，增加读写指针
        if (ret >= 0) {
            pos += ret;
        } 

        //超时退出
        if (GetTickCount() > ts){
            break;
        }
    }

    lua_pushinteger(L, pos);
    
    return 1;
}

static int _udpClose(lua_State *L)
{
    struct NET_Udp *udp;
    udp = (struct NET_Udp *)luaL_checkudata(L, -1, NET_METATABLE_UDP);

    //关闭socket
    if (-1 != udp->fd){
        closesocket(udp->fd);
        udp->fd = -1;
    }

    //摘链表
    list_del(&(udp->node));

    return 0;
}

static const struct luaL_Reg g_netlib [] = {
	{"create_server", _serverCreate},
	{"create_client", _connCreate},
    {"create_udp", _udpCreate},
	{NULL, NULL},
};

static const struct luaL_Reg g_serverlib [] = {
	{"on", _serverOn},  
	{"close", _serverClose},
    {"__gc", _serverClose},   
	{NULL, NULL},
};

static const struct luaL_Reg g_connlib [] = {
	{"on", _connOn}, 
	{"send", _connSend},
	{"close", _connClose},
	{"__gc", _connClose},
	{NULL, NULL},
};

static const struct luaL_Reg g_udpsocklib[] = {
	{"on", _udpOn},
    {"send", _udpSend},
	{"close", _udpClose},
    {"__gc", _udpClose},   
	{NULL, NULL},
};

int NET_Regist(lua_State *L)
{
    WSADATA  Ws;
    
    if (WSAStartup(MAKEWORD(2,2), &Ws) != 0 )
    {
        printf("Init Windows Socket Failed %d\n", WSAGetLastError());
        return -1;
    }

    INIT_LIST_HEAD(&g_servers);
    INIT_LIST_HEAD(&g_conns);
    INIT_LIST_HEAD(&g_udps);

    luaL_newmetatable(L, NET_METATABLE_SERVER);
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2); /* pushes the metatable */
    lua_settable(L, -3);  /* metatable.__index = metatable */
    luaL_setfuncs(L, g_serverlib, 0); /* set functions to metatable */
    lua_pop(L, 1);

    luaL_newmetatable(L, NET_METATABLE_CONN);
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2); /* pushes the metatable */
    lua_settable(L, -3);  /* metatable.__index = metatable */
    luaL_setfuncs(L, g_connlib, 0); /* set functions to metatable */
    lua_pop(L, 1);

    luaL_newmetatable(L, NET_METATABLE_UDP);
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2); /* pushes the metatable */
    lua_settable(L, -3);  /* metatable.__index = metatable */
    luaL_setfuncs(L, g_udpsocklib, 0); /* set functions to metatable */
    lua_pop(L, 1);
    
	luaL_newlib(L, g_netlib);
	lua_setglobal(L, "net");

	return ERR_OK;
}

static void _serverPoll(lua_State *L, struct NET_Server* srv)
{
    struct NET_Conn* conn;
    struct sockaddr_in addr;
    struct sockaddr_in6 addr6;
    int addrlen;
    int fd;
    int lasterror;
    
    //判正在监听
    if (NET_SERVER_LISTENING == srv->state){
        
        //判断是否有连接
        if (AF_INET6 == srv->family){
            addrlen = sizeof(addr6);
            fd = accept(srv->fd, (struct sockaddr *)&addr6, &addrlen);
        } else {
            addrlen = sizeof(addr);
            fd = accept(srv->fd, (struct sockaddr *)&addr, &addrlen);
        }

        //正常
        if (fd >= 0){
            //如果lua未注册监听函数，直接关闭连接
            if (-1 == srv->onConnect){
                closesocket(fd);
            } 
            //否则建立连接对象，并返回给lua
            else {
                lua_rawgeti(L, LUA_REGISTRYINDEX, srv->onConnect);
                conn = (struct NET_Conn *)lua_newuserdata(L, sizeof(struct NET_Conn));
                luaL_getmetatable(L, NET_METATABLE_CONN);
                lua_setmetatable(L, -2);
                _connInit(conn);
                conn->state = NET_CONN_CONNECTED;
                conn->fd = fd;
                conn->family = srv->family;
                memcpy(&(conn->addr6), &addr6, sizeof(addr6));
                memcpy(&(conn->addr), &addr, sizeof(addr));
                list_add_tail(&(conn->node), &g_conns);
                lua_pcall(L, 1, 0, 0);
            }
        } 
        //错误处理
        else {
            
            //正常情况
            lasterror = WSAGetLastError();
            if (WSAEWOULDBLOCK == lasterror || WSAECONNRESET == lasterror){
            } 
            //异常情况，关server socket
            else {
                srv->state = NET_SERVER_ERROR;
            }
        }
    } 
    //出现错误
    else if (NET_SERVER_ERROR == srv->state) {
        //设置状态
        srv->state = NET_SERVER_CLOSED;
        //关闭socket
        if (-1 != srv->fd){
            closesocket(srv->fd);
            srv->fd = -1;
        }
    }     
    //error状态等待lua gc回收

    return;
}

static void _connPoll(lua_State *L, struct NET_Conn* conn)
{
    char buf[8192];    
    int ret;
    int lasterror;

    //正在连接
    if (NET_CONN_CONNECTING == conn->state){
        ret = _connSocketConnect(conn);
        lasterror = WSAGetLastError();
        //连接成功
        if (SOCKET_ERROR == ret && WSAEISCONN == lasterror){
            conn->state = NET_CONN_CONNECTED;
            //回调lua
            if (-1 != conn->onConnect){
                lua_rawgeti(L, LUA_REGISTRYINDEX, conn->onConnect);
                lua_pcall(L, 0, 0, 0);
            }
        } 
        //正在连接
        else if (SOCKET_ERROR == ret && (WSAEALREADY == lasterror ||
                                         WSAEINVAL == lasterror ||
                                         WSAEWOULDBLOCK == lasterror)){
        } 
        //出错
        else {
            conn->state = NET_CONN_ERROR;
        }
    } 
    //已经连接
    else if (NET_CONN_CONNECTED == conn->state){
        //判断是否收到数据
        ret = recv(conn->fd, buf, sizeof(buf), 0);
        lasterror = WSAGetLastError();
        //成功收到数据，执行回调函数
        if (ret > 0){
            if (-1 != conn->onData){
                lua_rawgeti(L, LUA_REGISTRYINDEX, conn->onData);
                lua_pushlstring(L, buf, ret);
                lua_pushinteger(L, ret);
                lua_pcall(L, 2, 0, 0);
            }                
        }
        //出错进入错误流程
        else if (ret <= 0 && WSAEWOULDBLOCK != lasterror){
            conn->state = NET_CONN_ERROR;
        }
    }
    //错误状态
    else if (NET_CONN_ERROR == conn->state){
        //设置状态
        conn->state = NET_CONN_CLOSED;
        //回调lua
        if (-1 != conn->onClose){
            lua_rawgeti(L, LUA_REGISTRYINDEX, conn->onClose);
            lua_pcall(L, 0, 0, 0);
        }
        //关闭socket
        if (-1 != conn->fd){
            closesocket(conn->fd);
            conn->fd = -1;
        }
    }
    //error状态等待lua gc回收

    return;
}

static void _udpPoll(lua_State *L, struct NET_Udp* udp)
{
    struct sockaddr_in addr;
    struct sockaddr_in6 addr6;
    char buf[8192];
    char ipstr[INET6_ADDRSTRLEN + 1];
    int ret;
    int lasterror;
    int addrlen;
    uint16_t port;

    if (NET_UDP_READY == udp->state){
        //判断是否收到数据
        if (AF_INET6 == udp->family){
            addrlen = sizeof(addr6);
            ret = recvfrom(udp->fd, buf, sizeof(buf), 0, (struct sockaddr*)&addr6, &addrlen);
        } else {
            addrlen = sizeof(addr);
            ret = recvfrom(udp->fd, buf, sizeof(buf), 0, (struct sockaddr*)&addr, &addrlen);
        }
        
        lasterror = WSAGetLastError();
        //成功收到数据，执行回调函数
        if (ret > 0){
            if (-1 != udp->onData){
				if (AF_INET6 == udp->family){
					inet_ntop(udp->family, &addr6.sin6_addr, ipstr, sizeof(ipstr));
					port = ntohs(addr6.sin6_port);
				} else {
					inet_ntop(udp->family, &addr.sin_addr, ipstr, sizeof(ipstr));
					port = ntohs(addr.sin_port);
				}
                lua_rawgeti(L, LUA_REGISTRYINDEX, udp->onData);
                lua_pushstring(L, ipstr);
                lua_pushinteger(L, port);
                lua_pushlstring(L, buf, ret);
                lua_pushinteger(L, ret);
                lua_pcall(L, 4, 0, 0);
            }
        }
        //出错进入错误流程
        else if (ret <= 0 && WSAEWOULDBLOCK != lasterror){
            udp->state = NET_UDP_ERROR;
        }
    }
    //错误状态
    else if (NET_UDP_ERROR == udp->state){
        //设置状态
        udp->state = NET_UDP_CLOSED;
        //关闭socket
        if (-1 != udp->fd){
            closesocket(udp->fd);
            udp->fd = -1;
        }
    }
    //error状态等待lua gc回收

    return;
}


int NET_Poll(lua_State *L)
{
    struct list_head *head;

    //遍历服务端链表，接受连接
    list_for_each(head, &g_servers){
        _serverPoll(L, (struct NET_Server *)head);       
    }

    //遍历客户端链表，读取数据
    list_for_each(head, &g_conns){
        _connPoll(L, (struct NET_Conn *)head);
    }

    //遍历DUP连接
    list_for_each(head, &g_udps){
        _udpPoll(L, (struct NET_Udp *)head);
    }

    return 0;
}

void NET_Cleanup(void)
{
    WSACleanup();
}

