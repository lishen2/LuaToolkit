#ifndef _LUA_NET_H_
#define _LUA_NET_H_

int NET_Regist(lua_State *L);
int NET_Poll(lua_State *L);
void NET_Cleanup(void);

#endif

