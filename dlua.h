#ifndef DLUA_H
#define DLUA_H

#include "types.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

static void lua_autostart(lua_State *L);
static int lua_getclient(lua_State *L);
static int lua_getconfig(lua_State *L, const char *key, int t);
static int lua_getconfigfield(lua_State *L, const char *key, int t);
static int lua_getmonitor(lua_State *L);
static int lua_getmonitors(lua_State *L);
static int lua_getselmon(lua_State *L);
static int lua_getfocusedclient(lua_State *L);
static void lua_inputconfig(lua_State *L);
static void lua_loadtheme(lua_State *L);
static void lua_openconfigfile(lua_State *L);
static int lua_reloadconfig(lua_State *L);
static int lua_reloadtheme(lua_State *L);
static void lua_setaccelprofile(lua_State *L);
static void lua_setaccelspeed(lua_State *L);
static void lua_setclickmethod(lua_State *L);
static void lua_setdwt(lua_State *L);
static void lua_setlefthanded(lua_State *L);
static void lua_setmiddleemul(lua_State *L);
static void lua_setnaturalscroll(lua_State *L);
static void lua_settap(lua_State *L);
static void lua_settapanddrag(lua_State *L);
static void lua_setscrollmethod(lua_State *L);
static void lua_setup(void);
static void lua_setupenv(lua_State *L);
static int lua_quit(lua_State *L);

#endif
