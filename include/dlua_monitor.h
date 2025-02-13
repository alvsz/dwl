#ifndef DLUAMONITOR_H
#define DLUAMONITOR_H

#include "types.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

static int lua_createmonitor(lua_State *L, Monitor *m);
static int lua_getclients(lua_State *L);
static int lua_getclientsformonitor(lua_State *L, LuaMonitor *lm);
static int lua_monitorindex(lua_State *L);
static int lua_monitornewindex(lua_State *L);
static int lua_monitorsetgaps(lua_State *L);
static int lua_monitorsetgapsdefault(lua_State *L);
static int lua_monitorsetmfact(lua_State *L);
static int lua_monitorsettags(lua_State *L);
static int lua_monitorsetnmaster(lua_State *L);
static int lua_monitortogglegaps(lua_State *L);
static int lua_monitortoggletags(lua_State *L);

#endif
