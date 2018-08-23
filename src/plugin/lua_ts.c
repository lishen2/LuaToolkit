#include <Windows.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "common.h"
#include "lua_ts.h"

LARGE_INTEGER g_nFreq;
LARGE_INTEGER g_nLastTime;  

static int _init(void)
{
	g_nLastTime.QuadPart = 0;
	QueryPerformanceFrequency(&g_nFreq); // ��ȡʱ������ 
	return 0;
}

static int _resetInterval(lua_State *L)
{
	QueryPerformanceCounter(&g_nLastTime); // ��ȡʱ�Ӽ��� 
	return 0;
}

static int _getInterval(lua_State *L)
{
	lua_Number ms;
	LARGE_INTEGER time;

	QueryPerformanceCounter(&time); // ��ȡʱ�Ӽ��� 
	ms = (lua_Number)((time.QuadPart - g_nLastTime.QuadPart)*100/g_nFreq.QuadPart);

	luaL_checkstack(L, 1, "LUA Stack OverFlow");
	lua_pushinteger(L, (lua_Integer)ms);

	return 1;
}

static int _getTs(lua_State *L)
{
	lua_Number ts;
	LARGE_INTEGER time;

	QueryPerformanceCounter(&time); // ��ȡʱ�Ӽ��� 
	ts = (lua_Number)(time.QuadPart*1000/g_nFreq.QuadPart);

	luaL_checkstack(L, 1, "LUA Stack OverFlow");
	lua_pushinteger(L, (lua_Integer)ts);

	return 1;
}

static int _delayMs(lua_State *L)
{
	unsigned int ms;

	//��֤ջ����һ������λ��
	luaL_checkstack(L, 1, "LUA Stack OverFlow");

	//������
	ms = (unsigned int)luaL_checkinteger(L, -1);

	Sleep(ms);

	return 0;
}

static const struct luaL_Reg g_tslib[] = {
	{"get_ts", _getTs},
	{"reset_interval", _resetInterval},
	{"get_interval", _getInterval},
	{"delay", _delayMs},
	{NULL, NULL}
};

int TS_Regist(lua_State *L)
{
	_init();
	luaL_newlib(L, g_tslib);
	lua_setglobal(L, "ts");

	return ERR_OK;
}

