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
    NET_SERVER_LISTENING, //������
    NET_SERVER_ERROR,     //���ӳ���
    NET_SERVER_CLOSED,    //�Ѿ��Ͽ�
};

enum NET_CONN_STATE
{
    NET_CONN_CONNECTING,  //������
    NET_CONN_CONNECTED,   //�Ѿ�����
    NET_CONN_ERROR,       //���ӳ��ִ���
    NET_CONN_CLOSED,      //�����Ѿ��ر�
};

enum NET_UDP_STATE
{
    NET_UDP_READY,       //������ɵ�״̬
    NET_UDP_ERROR,       //���ӳ��ִ���
    NET_UDP_CLOSED,      //�����Ѿ��ر�
};

struct NET_Server{
    struct list_head node;
	int fd;              //�����FD
	int state;           //����״̬
	//����
	int family;
    struct sockaddr_in  addr; //����
    struct sockaddr_in6 addr6;//����
    //�¼����
	int onConnect;       //�¼����ͻ�������
};

struct NET_Conn{
    struct list_head node;
    //fd��״̬
	int fd;              //���ӵ�FD
	int state;           //����״̬
	//����
	int family;
    struct sockaddr_in  addr; //����
    struct sockaddr_in6 addr6;//����
    //�¼����
	int onConnect;       //�¼������ӵ�������
	int onData;          //�¼����յ�����
	int onClose;         //�¼������ӶϿ�
};

struct NET_Udp{
    struct list_head node;

    //fd��״̬
    int fd;
    int state;
    //����
	int family;
    struct sockaddr_in  addr; 
    struct sockaddr_in6 addr6;
    //�¼����
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

    //����socket
    fd = socket(srv->family, SOCK_STREAM, 0);
    if (fd < 0){
        return ERR_FAILED;
    }
    srv->fd = fd;

    //�󶨵�ַ
    if (AF_INET6 == srv->family){
        ret = bind(srv->fd, (struct sockaddr*)&srv->addr6, sizeof(struct sockaddr_in6));
    } else {
        ret = bind(srv->fd, (struct sockaddr*)&srv->addr, sizeof(struct sockaddr_in));
    }
    if (0 != ret){
        return ERR_FAILED;
    }

    //�����첽
    ret = ioctlsocket(srv->fd, FIONBIO, (unsigned long *)&opt);//���óɷ�����ģʽ��   
    if (0 != ret){
        return ERR_FAILED;
    }

    //����
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

	//��֤ջ����һ������λ��
	luaL_checkstack(L, 1, "LUA Stack OverFlow");    
    
    //��ȡ����
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

    //�����������ṹ
    srv = (struct NET_Server *)lua_newuserdata(L, sizeof(struct NET_Server));
    luaL_getmetatable(L, NET_METATABLE_SERVER);
    lua_setmetatable(L, -2);
    _serverInit(srv);

    //���ò���
    srv->state = NET_SERVER_LISTENING;
    srv->family = family;
    memcpy(&(srv->addr6), &addr6, sizeof(addr6));
    memcpy(&(srv->addr), &addr, sizeof(addr));

    //����socket������
    ret = _serverSocketCreate(srv);
    if (ERR_OK != ret){
        srv->state = NET_SERVER_ERROR;
    }

    //�ҽ�ȫ������
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
        //TODO, luaL_ref��Ѳ�����ջ�������޷�������ȷ������
        //������⣬lua�������ǲ�Ӧ�øı��ջ�е����ݵ�
    }
    
    return 0;
}

static int _serverClose(lua_State *L)
{
    struct NET_Server *srv;
    srv = (struct NET_Server *)luaL_checkudata(L, -1, NET_METATABLE_SERVER);

    //�ر�socket
    if (-1 != srv->fd){
        closesocket(srv->fd);
        srv->fd = -1;
    }

    //ժ����
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
        
    //����socket
    fd = socket(conn->family, SOCK_STREAM, 0);
    if (fd >= 0){

        //�����첽
        conn->fd = fd;
        ret = ioctlsocket(conn->fd, FIONBIO, (unsigned long *)&opt);
    }

    //�����첽ʧ�ܣ����������̹ر�conn->fd�б���ľ��
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
    
	//��֤ջ����һ������λ��
	luaL_checkstack(L, 1, "LUA Stack OverFlow");    

	//��ȡ����
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

    //�������ӽṹ
    conn = (struct NET_Conn *)lua_newuserdata(L, sizeof(struct NET_Conn));
    luaL_getmetatable(L, NET_METATABLE_CONN);
    lua_setmetatable(L, -2);
    _connInit(conn);

    //���ò�������
    conn->state = NET_CONN_CONNECTING;
    conn->family = family;
    memcpy(&(conn->addr6), &addr6, sizeof(addr6));
    memcpy(&(conn->addr), &addr, sizeof(addr));

    //����socket
    ret = _connSocketCreate(conn);
    if (0 != ret){
        luaL_error(L, "Error create socket\n");
    }

    //����socket
    ret = _connSocketConnect(conn);
    if ((SOCKET_ERROR == ret) && (WSAEWOULDBLOCK != WSAGetLastError())){
        conn->state = NET_CONN_ERROR;
    }

    //�ҽ�ȫ������
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

  	//��֤ջ����һ������λ��
	luaL_checkstack(L, 1, "LUA Stack OverFlow"); 

    conn = (struct NET_Conn *)luaL_checkudata(L, -3, NET_METATABLE_CONN);
    data = luaL_checkstring(L, -2);
    len = (int)luaL_checkinteger(L, -1);

    ts = GetTickCount() + 100;
    pos = 0;
    while(pos < len){
        ret = send(conn->fd, data + pos, len - pos, 0);

        //��������ֱ���˳�
        if (SOCKET_ERROR == ret && WSAEWOULDBLOCK != WSAGetLastError()){
            break;
        }

        //�յ����ݣ����Ӷ�дָ��
        if (ret >= 0) {
            pos += ret;
        } 

        //��ʱ�˳�
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

    //�ر�socket
    if (-1 != conn->fd){
        closesocket(conn->fd);
        conn->fd = -1;
    }

    //ժ����
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
        
    //����socket
    fd = socket(udp->family, SOCK_DGRAM, 0);
    if (fd < 0){
        return ERR_FAILED;
    }
    udp->fd = fd;

    //�󶨵�ַ
    if (AF_INET6 == udp->family){
        ret = bind(udp->fd, (struct sockaddr*)&udp->addr6, sizeof(struct sockaddr_in6));
    } else {
        ret = bind(udp->fd, (struct sockaddr*)&udp->addr, sizeof(struct sockaddr_in));
    }
    if (0 != ret){
        return ERR_FAILED;
    }

    //�����첽
    ret = ioctlsocket(udp->fd, FIONBIO, (unsigned long *)&opt);//���óɷ�����ģʽ��   
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
    
	//��֤ջ����һ������λ��
	luaL_checkstack(L, 1, "LUA Stack OverFlow");    

	//��ȡ����
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

    //�������ӽṹ
    udp = (struct NET_Udp *)lua_newuserdata(L, sizeof(struct NET_Udp));
    luaL_getmetatable(L, NET_METATABLE_UDP);
    lua_setmetatable(L, -2);
    _udpInit(udp);

    //���ò�������
    udp->state = NET_UDP_READY;
    udp->family = family;
    memcpy(&(udp->addr6), &addr6, sizeof(addr6));
    memcpy(&(udp->addr), &addr, sizeof(addr));

    //����socket
    ret = _udpSocketCreate(udp);
    if (0 != ret){
        luaL_error(L, "Error create socket\n");
    }

    //�ҽ�ȫ������
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
    
	//��֤ջ����һ������λ��
	luaL_checkstack(L, 1, "LUA Stack OverFlow");    

	//��ȡ����
	udp = (struct NET_Udp *)luaL_checkudata(L, -5, NET_METATABLE_UDP);
    ipstr = luaL_checkstring(L, -4);
    port = (uint16_t)luaL_checkinteger(L, -3);
    data = luaL_checkstring(L, -2);
    len = (int)luaL_checkinteger(L, -1);

    //ת����ַ
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
        //����
        if (AF_INET6 == udp->family){
            ret = sendto(udp->fd, data + pos, len - pos, 0, (struct sockaddr*)&addr6, sizeof(addr6));
        } else {
            ret = sendto(udp->fd, data + pos, len - pos, 0, (struct sockaddr*)&addr, sizeof(addr));
        }

        //��������ֱ���˳�
        if (SOCKET_ERROR == ret && WSAEWOULDBLOCK != WSAGetLastError()){
            break;
        }

        //�յ����ݣ����Ӷ�дָ��
        if (ret >= 0) {
            pos += ret;
        } 

        //��ʱ�˳�
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

    //�ر�socket
    if (-1 != udp->fd){
        closesocket(udp->fd);
        udp->fd = -1;
    }

    //ժ����
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
    
    //�����ڼ���
    if (NET_SERVER_LISTENING == srv->state){
        
        //�ж��Ƿ�������
        if (AF_INET6 == srv->family){
            addrlen = sizeof(addr6);
            fd = accept(srv->fd, (struct sockaddr *)&addr6, &addrlen);
        } else {
            addrlen = sizeof(addr);
            fd = accept(srv->fd, (struct sockaddr *)&addr, &addrlen);
        }

        //����
        if (fd >= 0){
            //���luaδע�����������ֱ�ӹر�����
            if (-1 == srv->onConnect){
                closesocket(fd);
            } 
            //���������Ӷ��󣬲����ظ�lua
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
        //������
        else {
            
            //�������
            lasterror = WSAGetLastError();
            if (WSAEWOULDBLOCK == lasterror || WSAECONNRESET == lasterror){
            } 
            //�쳣�������server socket
            else {
                srv->state = NET_SERVER_ERROR;
            }
        }
    } 
    //���ִ���
    else if (NET_SERVER_ERROR == srv->state) {
        //����״̬
        srv->state = NET_SERVER_CLOSED;
        //�ر�socket
        if (-1 != srv->fd){
            closesocket(srv->fd);
            srv->fd = -1;
        }
    }     
    //error״̬�ȴ�lua gc����

    return;
}

static void _connPoll(lua_State *L, struct NET_Conn* conn)
{
    char buf[8192];    
    int ret;
    int lasterror;

    //��������
    if (NET_CONN_CONNECTING == conn->state){
        ret = _connSocketConnect(conn);
        lasterror = WSAGetLastError();
        //���ӳɹ�
        if (SOCKET_ERROR == ret && WSAEISCONN == lasterror){
            conn->state = NET_CONN_CONNECTED;
            //�ص�lua
            if (-1 != conn->onConnect){
                lua_rawgeti(L, LUA_REGISTRYINDEX, conn->onConnect);
                lua_pcall(L, 0, 0, 0);
            }
        } 
        //��������
        else if (SOCKET_ERROR == ret && (WSAEALREADY == lasterror ||
                                         WSAEINVAL == lasterror ||
                                         WSAEWOULDBLOCK == lasterror)){
        } 
        //����
        else {
            conn->state = NET_CONN_ERROR;
        }
    } 
    //�Ѿ�����
    else if (NET_CONN_CONNECTED == conn->state){
        //�ж��Ƿ��յ�����
        ret = recv(conn->fd, buf, sizeof(buf), 0);
        lasterror = WSAGetLastError();
        //�ɹ��յ����ݣ�ִ�лص�����
        if (ret > 0){
            if (-1 != conn->onData){
                lua_rawgeti(L, LUA_REGISTRYINDEX, conn->onData);
                lua_pushlstring(L, buf, ret);
                lua_pushinteger(L, ret);
                lua_pcall(L, 2, 0, 0);
            }                
        }
        //��������������
        else if (ret <= 0 && WSAEWOULDBLOCK != lasterror){
            conn->state = NET_CONN_ERROR;
        }
    }
    //����״̬
    else if (NET_CONN_ERROR == conn->state){
        //����״̬
        conn->state = NET_CONN_CLOSED;
        //�ص�lua
        if (-1 != conn->onClose){
            lua_rawgeti(L, LUA_REGISTRYINDEX, conn->onClose);
            lua_pcall(L, 0, 0, 0);
        }
        //�ر�socket
        if (-1 != conn->fd){
            closesocket(conn->fd);
            conn->fd = -1;
        }
    }
    //error״̬�ȴ�lua gc����

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
        //�ж��Ƿ��յ�����
        if (AF_INET6 == udp->family){
            addrlen = sizeof(addr6);
            ret = recvfrom(udp->fd, buf, sizeof(buf), 0, (struct sockaddr*)&addr6, &addrlen);
        } else {
            addrlen = sizeof(addr);
            ret = recvfrom(udp->fd, buf, sizeof(buf), 0, (struct sockaddr*)&addr, &addrlen);
        }
        
        lasterror = WSAGetLastError();
        //�ɹ��յ����ݣ�ִ�лص�����
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
        //��������������
        else if (ret <= 0 && WSAEWOULDBLOCK != lasterror){
            udp->state = NET_UDP_ERROR;
        }
    }
    //����״̬
    else if (NET_UDP_ERROR == udp->state){
        //����״̬
        udp->state = NET_UDP_CLOSED;
        //�ر�socket
        if (-1 != udp->fd){
            closesocket(udp->fd);
            udp->fd = -1;
        }
    }
    //error״̬�ȴ�lua gc����

    return;
}


int NET_Poll(lua_State *L)
{
    struct list_head *head;

    //���������������������
    list_for_each(head, &g_servers){
        _serverPoll(L, (struct NET_Server *)head);       
    }

    //�����ͻ���������ȡ����
    list_for_each(head, &g_conns){
        _connPoll(L, (struct NET_Conn *)head);
    }

    //����DUP����
    list_for_each(head, &g_udps){
        _udpPoll(L, (struct NET_Udp *)head);
    }

    return 0;
}

void NET_Cleanup(void)
{
    WSACleanup();
}

