#ifndef DLUACLIENT_H
#define DLUACLIENT_H

#include "types.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

int lua_clientindex(lua_State *L);
int lua_clientnewindex(lua_State *L);
int lua_clientfocus(lua_State *L);
int lua_clientkill(lua_State *L);
int lua_clientresize(lua_State *L);
int lua_clientserialize(lua_State *L);
int lua_clientsettags(lua_State *L);
int lua_clientsetmon(lua_State *L);
int lua_clienttogglefloating(lua_State *L);
int lua_clienttoggleinscratch(lua_State *L);
int lua_clienttoggletags(lua_State *L);
int lua_clientvisibleon(lua_State *L);
int lua_createclient(lua_State *L, Client *c);

#endif
