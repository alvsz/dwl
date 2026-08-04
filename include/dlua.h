#ifndef DLUA_H
#define DLUA_H

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include "types.h"

void lua_autostart(lua_State *L);
int lua_getclient(lua_State *L);
int lua_getconfig(lua_State *L, const char *key, int t);
int lua_getconfigfield(lua_State *L, const char *key, int t);
int lua_getmonitor(lua_State *L);
int lua_getmonitors(lua_State *L);
int lua_getselmon(lua_State *L);
int lua_getfocusedclient(lua_State *L);
void lua_inputconfig(lua_State *L);
void lua_loadrules(lua_State *L);
void lua_loadtheme(lua_State *L);
void lua_openconfigfile(lua_State *L);
int lua_reloadconfig(lua_State *L);
int lua_reloadtheme(lua_State *L);
void lua_setaccelprofile(lua_State *L);
void lua_setaccelspeed(lua_State *L);
void lua_setclickmethod(lua_State *L);
void lua_setdwt(lua_State *L);
void lua_setlefthanded(lua_State *L);
void lua_setmiddleemul(lua_State *L);
void lua_setnaturalscroll(lua_State *L);
void lua_settap(lua_State *L);
void lua_settapanddrag(lua_State *L);
void lua_setscrollmethod(lua_State *L);
void lua_setup(lua_State **L);
void lua_setupenv(lua_State *L);
void lua_spawn(const Arg *arg);
int lua_quit(lua_State *L);

#endif
