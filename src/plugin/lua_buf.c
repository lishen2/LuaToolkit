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
	uint32_t i;

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

static uint8_t _char2hex(char c)
{
	uint8_t b;
	if (c >= '0' && c <= '9') {
		b = c - '0';
	}
	else if (c >= 'a' && c <= 'f') {
		b = c - 'a' + 10;
	}
	else if (c >= 'A' && c <= 'F') {
		b = c - 'A' + 10;
	}
	else {
		b = 0;
	}

	return b;
}

static uint8_t _nibble2char(uint8_t nibble){
    char c;

    if (nibble >= 0 && nibble <= 9){
        c = '0' + nibble;
    } else if (nibble >= 10 && nibble <= 15){
        c = 'A' + nibble - 10;
    } else {
        c = '0';
    }

    return c;
}

/* 参数：string
   返回值：binary_buffer, size */
static int _str2bin(lua_State *L)
{
	const char* buf;
	uint8_t *hexbuf;
	uint32_t len;
	uint32_t i;
	uint8_t highbyte;
	uint8_t lowbyte;

	//检查栈空间
	luaL_checkstack(L, 2, "LUA Stack OverFlow");

	buf = luaL_checkstring(L, -1);
	len = strlen(buf);

	hexbuf = malloc(len / 2);
	if (NULL == hexbuf) {
		lua_pushnil(L);
		lua_pushinteger(L, 0);
		return 2;
	}

	for (i = 0; i < len/2; ++i) {
		highbyte  = _char2hex(buf[i * 2 + 0]);
		lowbyte   = _char2hex(buf[i * 2 + 1]);
		hexbuf[i] = (0xF0 & (highbyte << 4)) | (0x0F & (lowbyte << 0));
	}

	lua_pushlstring(L, hexbuf, len / 2);
	lua_pushinteger(L, len / 2);
    
    free(hexbuf);
    
	return 2;
}

/* 参数：binary_buf, size
   返回值：string */
static int _bin2str(lua_State *L)
{
	const char* buf;
	uint8_t *strbuf;
	uint32_t len;
	uint32_t i;

	//检查栈空间
	luaL_checkstack(L, 1, "LUA Stack OverFlow");

	buf = luaL_checkstring(L, -2);
    len = (uint32_t)luaL_checkinteger(L, -1); 

	strbuf = malloc(len * 2 + 1);
	if (NULL == strbuf) {
		lua_pushnil(L);
		return 1;
	}

	for (i = 0; i < len; ++i) {
        strbuf[i * 2 + 0] = _nibble2char((buf[i] >> 4) & 0x0F);
        strbuf[i * 2 + 1] = _nibble2char((buf[i] >> 0) & 0x0F);
	}
    strbuf[len * 2] = 0;

	lua_pushlstring(L, strbuf, len*2);    
    
    free(strbuf);

    return 1;
}

static const struct luaL_Reg g_buflib [] = {
	{"random", _createRamdom},
	{"memcmp", _memcmp},
	{"str2bin", _str2bin},
	{"bin2str", _bin2str},
	{NULL, NULL} /* sentinel */
};

int BUF_Regist(lua_State *L)
{
	luaL_newlib(L, g_buflib);
	lua_setglobal(L, "buf");

	return ERR_OK;
}

