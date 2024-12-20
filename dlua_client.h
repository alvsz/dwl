#ifndef DLUACLIENT_H
#define DLUACLIENT_H

#include "types.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

static int lua_clientindex(lua_State *L);
static int lua_clientkill(lua_State *L);
static int lua_clientresize(lua_State *L);
static int lua_clientsettags(lua_State *L);
static int lua_clientsetmon(lua_State *L);
static int lua_clienttogglefloating(lua_State *L);
static int lua_clienttoggleinscratch(lua_State *L);
static int lua_clientvisibleon(lua_State *L);
static int lua_createclient(lua_State *L, Client *c);

#endif
