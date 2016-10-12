#include <Windows.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "common.h"
#include "lua_buf.h"

static int _createRamdom(lua_State *L)
{
	uint32_t len;
	uint8_t *p;
	int i;

	//检查栈空间
	luaL_checkstack(L, 1, "LUA Stack OverFlow");

	//获取参数
	len = (uint32_t)luaL_checkinteger(L, -1);
	p = (uint8_t*)malloc(len);
	if (NULL != p){
		srand(GetTickCount());
		for (i = 0; i < len; ++i){
			p[i] = (uint8_t)(rand()%255);
		}
		lua_pushlstring(L, p, len);
	} else {
		lua_pushnil(L);
	}
	
	return 1;
}

static int _memcmp(lua_State *L)
{
	uint8_t *p1;
	uint8_t *p2;
	uint32_t len;

	//检查栈空间
	luaL_checkstack(L, 1, "LUA Stack OverFlow");

	p1 = (uint8_t *)luaL_checkstring(L, -3);
	p2 = (uint8_t *)luaL_checkstring(L, -2);
	len = (uint32_t)luaL_checkinteger(L, -1); 

	if (0 == memcmp(p1, p2, len)){
		lua_pushinteger(L, 0);
	} else {
		lua_pushinteger(L, 1);
	}

	return 1;
}

static const struct luaL_Reg g_buflib [] = {
	{"random", _createRamdom},
	{"memcmp", _memcmp},
	{NULL, NULL} /* sentinel */
};

int BUF_Regist(lua_State *L)
{
	luaL_newlib(L, g_buflib);
	lua_setglobal(L, "buf");

	return ERR_OK;
}

