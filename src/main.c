#include <Windows.h>
#include <stdio.h>
#include "common.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "plugin\lua_log.h"
#include "plugin\lua_serial.h"
#include "plugin\lua_alg.h"
#include "plugin\lua_ts.h"
#include "plugin\lua_timer.h"
#include "plugin\lua_net.h"

static lua_State * _creaetLuaEnv(void)
{
	lua_State *L;

	L = luaL_newstate();
	if (NULL == L){
		return NULL;
	}

	luaL_openlibs(L);

	return L;
}

static int _runScript(lua_State *L, char* filename)
{
	int ret;

	//load script
	ret = luaL_loadfile(L, filename);
	if (0 != ret){
		printf("Error: %s", lua_tostring(L, -1));
		lua_pop(L, 1);
		return ERR_FAILED;
	}

	//run
	ret = lua_pcall(L, 0, 0, 0);	
	if (0 != ret){
		printf("Error: %s", lua_tostring(L, -1));
		lua_pop(L, 1);
		return ERR_FAILED;
	}

	return ERR_OK;
}

int main(int argc, char* argv[])
{
	lua_State *L;
	char *script;
	int ret;

	//create lua environment
	L = _creaetLuaEnv();
	if (NULL == L){
		printf("Error create Lua environment\n");
		return -1;
	}

	//regist functions
	ret = LOG_Regist(L);
	if (ERR_OK != ret){
		printf("Error regist log\n");
		return -1;
	}
	
	ret = SERIAL_Regist(L);
	if (ERR_OK != ret){
		printf("Error regist Serial\n");
		return -1;
	}

	ret = ALG_Regist(L);
	if (ERR_OK != ret){
		printf("Error regist ALG\n");
		return -1;
	}

	ret = TIMER_Regist(L);
	if (ERR_OK != ret){
		printf("Error regist Timer\n");
		return -1;
	}

	ret = NET_Regist(L);
	if (ERR_OK != ret){
		printf("Error regist NET\n");
		return -1;
	}

	ret = BUF_Regist(L);
	if (ERR_OK != ret){
		printf("Error regist BUF\n");
		return -1;	
	}

	ret = TS_Regist(L);
	if (ERR_OK != ret){
		printf("Error regist TS");
		return -1;
	}
	
	//load script file
	if (argc < 2){
		script = "script.lua";
	} else {
		script = argv[1];
	}
	ret = _runScript(L, script);

	//pool timer
	while(1){
		ret = TIMER_Poll(L);
		NET_Poll(L);
		Sleep(10);
	}

	//close lua env
	lua_close(L);
    //must clean up after close lua
    NET_Cleanup();

	return ret;
}

