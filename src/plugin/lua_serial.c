#include <Windows.h>
#include <stdlib.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "common.h"
#include "lua_serial.h"

/* ͨ�ų�ʱʱ�� */
#define SERIAL_TIMEOUT  100
#define SERIAL_INVALID_PORT_HANDLE  INVALID_HANDLE_VALUE

static HANDLE g_portHandle = INVALID_HANDLE_VALUE;

static int _setupPort(HANDLE hCom, DWORD baud, BYTE data, BYTE stop, BYTE parity)
{
	BOOL ret;
	COMMTIMEOUTS TimeOuts;
	DCB dcb;

	//���û�����
	ret = SetupComm(hCom, 8192, 8192);
	if (FALSE == ret){
		return ERR_FAILED;
	}

	//�趨����ʱ  
	TimeOuts.ReadIntervalTimeout = SERIAL_TIMEOUT;  
	TimeOuts.ReadTotalTimeoutMultiplier = SERIAL_TIMEOUT;  
	TimeOuts.ReadTotalTimeoutConstant = 0;
	//�趨д��ʱ  
	TimeOuts.WriteTotalTimeoutMultiplier = SERIAL_TIMEOUT;
	TimeOuts.WriteTotalTimeoutConstant = 0;  
	ret = SetCommTimeouts(hCom,&TimeOuts); //���ó�ʱ
	if (FALSE == ret){
		return ERR_FAILED;
	}

	//���ò���
	ret = GetCommState(hCom,&dcb);  
	if (FALSE == ret){
		return ERR_FAILED;
	}
	dcb.BaudRate = baud;
	dcb.ByteSize = data;
	dcb.Parity = parity;
	dcb.StopBits = stop;
	ret = SetCommState(hCom, &dcb);  
	if (FALSE == ret){
		return ERR_FAILED;
	}
 
	(void)PurgeComm(hCom, PURGE_TXCLEAR|PURGE_RXCLEAR);  

	return ERR_OK;
}

//����:�˿ں�(�ַ���),������(����),����λ(����),ֹͣλ(����),У��λ(�ַ���)
//����ֵ:�ɹ�����0��ʧ��-1
static int _openPort(lua_State *L)
{
	char buf[32];
	int ret;
	HANDLE hCom;  //ȫ�ֱ��������ھ��
	char *port;
	DWORD  baud;
	BYTE  data;
	lua_Number lstop;
	BYTE stop;
	char *lparity;
	BYTE parity;

	//���ջ�ռ�
	luaL_checkstack(L, 1, "LUA Stack OverFlow");

	//��ȡ����
	// -5 �˿ں�
	// -4 ������
	// -3 ����λ
	// -2 ֹͣλ
	// -1 У��
	port = luaL_checkstring(L, -5);
	baud = (DWORD)luaL_checkinteger(L, -4);
	data = (BYTE)luaL_checkinteger(L, -3);
	lstop = luaL_checknumber(L, -2);
	lparity = luaL_checkstring(L, -1);

	//��������ת��
	if (data < 4 || data > 8){
		luaL_error(L, "Invalid data [%hhu]. Options [4-8]", data);
		return 0;
	}

	if (2 == lstop){
		stop = TWOSTOPBITS;
	} else if (1.5 == lstop){
		stop = ONE5STOPBITS;
	} else if (1 == lstop){
		stop = ONESTOPBIT;
	} else {
		luaL_error(L, "Invalid stopbit [%f]. Options [1, 1.5, 2]", (float)lstop);
		return 0;
	}

	if (0 == strcmp("none", lparity)){
		parity = NOPARITY;
	} else if (0 == strcmp("odd", lparity)){
		parity = ODDPARITY;
	} else if (0 == strcmp("even", lparity)){
		parity = EVENPARITY;
	} else if (0 == strcmp("mark", lparity)){
		parity = MARKPARITY;
	} else if (0 == strcmp("space", lparity)){
		parity = SPACEPARITY;
	} else {
		luaL_error(L, "Invalid parity [%s]. Options [none, odd, even, mark, space]", lparity);
		return 0;
	}

	//�ر�֮ǰ�ľ��
	if (SERIAL_INVALID_PORT_HANDLE != g_portHandle){
		CloseHandle(g_portHandle);
		g_portHandle = SERIAL_INVALID_PORT_HANDLE;
	}

	snprintf(buf, sizeof(buf), "\\\\.\\%s", port);
	buf[sizeof(buf) - 1] = 0;
	hCom = CreateFile(buf, 
					GENERIC_READ|GENERIC_WRITE, //�������д  
					0, //��ռ��ʽ  
					NULL,  
					OPEN_EXISTING, //�򿪶����Ǵ���  
					0, //ͬ����ʽ  
					NULL);  
	if(SERIAL_INVALID_PORT_HANDLE == hCom){ 
		lua_pushinteger (L, -1);
		return 1;
	}
	
	ret = _setupPort(hCom, baud, data, stop, parity);
	if (ERR_OK != ret){
		CloseHandle(g_portHandle);
		lua_pushinteger (L, -1);
		return 1;
	}

	g_portHandle = hCom;

	lua_pushinteger (L, 0);
	return 1;
}

static int _closePort(lua_State *L)
{
	//�ر�֮ǰ�ľ��
	if (SERIAL_INVALID_PORT_HANDLE != g_portHandle){
		CloseHandle(g_portHandle);
		g_portHandle = SERIAL_INVALID_PORT_HANDLE;
	}

	return 0;
}

static int _reads(lua_State *L)
{
	unsigned char buf[4096];
    unsigned int ts, timeout;
	BOOL ret;
	DWORD wCount;
	int length;
	int offset;

	//���ջ�ռ�
	luaL_checkstack(L, 2, "LUA Stack OverFlow");

	//��ȡ����
	length = (unsigned int)luaL_checkinteger(L, -2);
	timeout = (unsigned int)luaL_checkinteger(L, -1);

	//�����
	if (SERIAL_INVALID_PORT_HANDLE == g_portHandle){
		lua_pushnil(L);
		lua_pushinteger(L, 0);	
		return 2;
	}

	if (length > sizeof(buf)){
		length = sizeof(buf);
	}

	//��ȡ
	ts = GetTickCount();
	offset = 0;
	while (offset < length){
        ret = ReadFile(g_portHandle, buf + offset, length - offset, &wCount, NULL); 
                                                                 if (TRUE == ret) {
            offset += wCount;
        }

        if (GetTickCount() - ts > timeout) {
            break;
        }
	}

	if (offset > 0){
		lua_pushlstring(L, buf, offset);
		lua_pushinteger(L, offset);
	} else {
		lua_pushnil(L);
		lua_pushinteger(L, 0);	
	}

	return 2;
}

static int _writes(lua_State *L)
{
	unsigned char *data;
	BOOL ret;
	DWORD wCount;
	int length;
	int offset;
	unsigned int ts;

	//���ջ�ռ�
	luaL_checkstack(L, 1, "LUA Stack OverFlow");

	//��ȡ����
	data = (unsigned int)luaL_checkinteger(L, -2);
	length = (unsigned int)luaL_checkinteger(L, -1);

	//�����
	if (SERIAL_INVALID_PORT_HANDLE == g_portHandle){
		lua_pushinteger(L, 0);	
		return 1;
	}

	//д��
	ts = GetTickCount();
	offset = 0;
	while (offset < length){
		ret = WriteFile(g_portHandle, data + offset, length - offset, &wCount, NULL);
        if (TRUE == ret) {
            offset += wCount;
        }

        if (GetTickCount() - ts > 10 * 1000) {
            break;
        }
	}

	lua_pushinteger(L, offset);

	return 1;
}

static int _purge(lua_State *L)
{
	PurgeComm(g_portHandle, PURGE_TXCLEAR|PURGE_RXCLEAR);
	return 0;
}

static const struct luaL_Reg g_seriallib [] = {
	{"open", _openPort},
	{"close", _closePort},
	{"reads", _reads},
	{"writes", _writes},
	{"purge", _purge},
	{NULL, NULL} /* sentinel */
};

int SERIAL_Regist(lua_State *L)
{
	luaL_newlib(L, g_seriallib);
	lua_setglobal(L, "serial");

	return ERR_OK;
}

