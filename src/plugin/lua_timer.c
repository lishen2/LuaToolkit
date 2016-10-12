#include <stdint.h>
#include <Windows.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "common.h"
#include "lua_timer.h"

#define TIMER_MAX_NUM  512

struct TIMER_CB
{
	uint32_t ts;
	int ridx;
};

static struct TIMER_CB g_timers[TIMER_MAX_NUM];

static int _setTimer(lua_State *L)
{
	int i;
	uint32_t interval;

	//保证栈上有一个空闲位置
	luaL_checkstack(L, 1, "LUA Stack OverFlow");

	//检查参数
	interval = (uint32_t)luaL_checkinteger(L, -2);
	luaL_checktype(L, -1, LUA_TFUNCTION);

	//寻找空闲位置
	for(i = 0; i < TIMER_MAX_NUM; ++i){
		if (0 == g_timers[i].ts){
			break;
		}
	}

	//找到位置，将lua函数注册到references中，并记录下id
	if (i < TIMER_MAX_NUM){
		if (interval < 10){
			interval = 10;
		}
		g_timers[i].ts = GetTickCount() + interval;
		g_timers[i].ridx = luaL_ref(L, LUA_REGISTRYINDEX);
        lua_pushinteger(L, i);
	} else {
		lua_pushinteger(L, -1);
	}

	return 1;
}

static int _clearTimer(lua_State *L)
{
	int timerid;

	//保证栈上有一个空闲位置
	luaL_checkstack(L, 1, "LUA Stack OverFlow");

	//检查参数
	timerid = (int)luaL_checkinteger(L, -1);

    if (timerid >= 0 && timerid < TIMER_MAX_NUM){
        luaL_unref(L, LUA_REGISTRYINDEX, g_timers[timerid].ridx);
        g_timers[timerid].ts = 0;
    }

	return 0;
}

static const struct luaL_Reg g_timerlib [] = {
	{"set_timer", _setTimer},
    {"clear_timer", _clearTimer},
	{NULL, NULL}
};

int TIMER_Regist(lua_State *L)
{
	memset(g_timers, 0, sizeof(g_timers));
	luaL_newlib(L, g_timerlib);
	lua_setglobal(L, "timer");

	return ERR_OK;
}

int TIMER_Poll(lua_State *L)
{
	int i;
	int ret;
    int status;

	ret = BOOL_FALSE;
	for (i = 0; i < TIMER_MAX_NUM; ++i){
		if (0 != g_timers[i].ts &&
			GetTickCount() >= g_timers[i].ts){
			
			//从reference中压入栈
			lua_rawgeti(L, LUA_REGISTRYINDEX, g_timers[i].ridx);
			luaL_unref(L, LUA_REGISTRYINDEX, g_timers[i].ridx);
			//从定时器表中设置为删除
			g_timers[i].ts = 0;

			//执行回调函数
    		status = lua_pcall(L, 0, 0, 0);
            if (LUA_OK != status){
                printf("Error: %s", lua_tostring(L, -1));
            }
			ret = BOOL_TRUE;
		}
	}

	return ret;
}

