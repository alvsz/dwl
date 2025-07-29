#ifndef DLUAMONITOR_H
#define DLUAMONITOR_H

#include "types.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

int lua_createmonitor(lua_State *L, Monitor *m);
int lua_getclients(lua_State *L);
int lua_getclientsformonitor(lua_State *L, LuaMonitor *lm);
int lua_monitorindex(lua_State *L);
int lua_monitornewindex(lua_State *L);
int lua_monitorserialize(lua_State *L);
int lua_monitorsetgaps(lua_State *L);
int lua_monitorsetgapsdefault(lua_State *L);
int lua_monitorsetlayout(lua_State *L);
int lua_monitorsetmfact(lua_State *L);
int lua_monitorsettags(lua_State *L);
int lua_monitorsetnmaster(lua_State *L);
int lua_monitortogglegaps(lua_State *L);
int lua_monitortoggletags(lua_State *L);

#endif
