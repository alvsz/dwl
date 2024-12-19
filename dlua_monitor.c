#include <lauxlib.h>
#include <lua.h>
#include <stdint.h>
#include <string.h>

#include "dlua.h"
#include "dwl.h"

int lua_createmonitor(lua_State *L, Monitor *m) {
  LuaMonitor *lm = (LuaMonitor *)lua_newuserdata(L, sizeof(LuaMonitor));

  luaL_getmetatable(L, "Monitor");
  lua_setmetatable(L, -2);
  lm->m = m;

  return 1;
}

int lua_getclients(lua_State *L) {
  LuaMonitor *lm = (LuaMonitor *)luaL_checkudata(L, 1, "Monitor");
  lua_getclientsformonitor(L, lm);
  return 1;
}

int lua_getclientsformonitor(lua_State *L, LuaMonitor *lm) {
  Client *c;
  int i = 1;

  lua_newtable(L);

  wl_list_for_each(c, &clients, link) {
    if (c->mon == lm->m) {
      lua_pushinteger(L, i);
      lua_createclient(L, c);
      lua_rawset(L, -3);
      i++;
    }
  }
  return 1;
}

int lua_monitorindex(lua_State *L) {
  LuaMonitor *lm = (LuaMonitor *)luaL_checkudata(L, 1, "Monitor");
  const char *key = luaL_checkstring(L, 2);

  if (strcmp(key, "layout") == 0) {
    lua_pushstring(L, lm->m->ltsymbol);
    return 1;
  } else if (strcmp(key, "clients") == 0) {
    lua_getclientsformonitor(L, lm);
    return 1;
  } else if (strcmp(key, "focused") == 0) {
    lua_pushboolean(L, lm->m == selmon);
    return 1;
  } else if (strcmp(key, "seltags") == 0) {
    lua_pushinteger(L, lm->m->tagset[lm->m->seltags]);
    return 1;
  } else if (strcmp(key, "name") == 0) {
    lua_pushstring(L, lm->m->name);
    return 1;
  } else if (strcmp(key, "mfact") == 0) {
    lua_pushnumber(L, lm->m->mfact);
    return 1;
  } else if (strcmp(key, "nmaster") == 0) {
    lua_pushinteger(L, lm->m->nmaster);
    return 1;
  } else if (strcmp(key, "scale") == 0) {
    lua_pushnumber(L, lm->m->scale);
    return 1;
  } else if (strcmp(key, "gaps") == 0) {
    lua_newtable(L);
    lua_pushstring(L, "ih");
    lua_pushnumber(L, lm->m->gappih);
    lua_rawset(L, -3);

    lua_pushstring(L, "iv");
    lua_pushnumber(L, lm->m->gappiv);
    lua_rawset(L, -3);

    lua_pushstring(L, "oh");
    lua_pushnumber(L, lm->m->gappoh);
    lua_rawset(L, -3);

    lua_pushstring(L, "ov");
    lua_pushnumber(L, lm->m->gappov);
    lua_rawset(L, -3);

    return 1;
  } else if (strcmp(key, "x") == 0) {
    lua_pushnumber(L, lm->m->m.x);
    return 1;
  } else if (strcmp(key, "y") == 0) {
    lua_pushnumber(L, lm->m->m.y);
    return 1;
  } else if (strcmp(key, "address") == 0) {
    lua_pushinteger(L, (uintptr_t)lm->m);
    return 1;
  }

  luaL_getmetatable(L, "Monitor");
  lua_getfield(L, -1, key);
  return 1;
}
