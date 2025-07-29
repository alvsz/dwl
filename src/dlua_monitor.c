#include <json-c/json.h>
#include <json-c/json_object.h>
#include <lauxlib.h>
#include <lua.h>
#include <stdint.h>
#include <string.h>

#include "config.h"
#include "dlua_client.h"
#include "dlua_monitor.h"
#include "dwl.h"
#include "pertag.h"
#include "util.h"

#define SETGAPS(lm, oh, ov, ih, iv)                                            \
  do {                                                                         \
    lm->m->gappoh = MAX(oh, 0);                                                \
    lm->m->gappov = MAX(ov, 0);                                                \
    lm->m->gappih = MAX(ih, 0);                                                \
    lm->m->gappiv = MAX(iv, 0);                                                \
  } while (0)

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

  wl_list_for_each(c, get_clients(), link) {
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
    lua_pushboolean(L, lm->m == get_selmon());
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

int lua_monitornewindex(lua_State *L) {
  LuaMonitor *lm = (LuaMonitor *)luaL_checkudata(L, 1, "Monitor");
  const char *key = luaL_checkstring(L, 2);
  unsigned int i;
  double n;

  if (strcmp(key, "seltags") == 0) {
    i = (unsigned int)luaL_checkinteger(L, 3);
    viewmonitor(lm->m, &(const Arg){.ui = i});
    return 0;
  } else if (strcmp(key, "mfact") == 0) {
    i = (unsigned int)luaL_checknumber(L, 3);
    lm->m->mfact = lm->m->pertag->mfacts[lm->m->pertag->curtag] = i;
    arrange(lm->m);
    return 0;
  } else if (strcmp(key, "nmaster") == 0) {
    n = luaL_checknumber(L, 3);
    lm->m->nmaster = lm->m->pertag->nmasters[lm->m->pertag->curtag] =
        MAX((int)n, 0);
    arrange(lm->m);
    return 0;
  }

  return 0;
}

int lua_monitorserialize(lua_State *L) {
  LuaMonitor *lm = (LuaMonitor *)luaL_checkudata(L, 1, "Monitor");
  Monitor *m = lm->m;
  struct json_object *json = json_object_new_object();
  struct json_object *gaps, *geom;

  json_object_object_add(json, "layout", json_object_new_string(m->ltsymbol));
  json_object_object_add(json, "focused",
                         json_object_new_boolean(m == get_selmon()));
  json_object_object_add(json, "seltags",
                         json_object_new_int(m->tagset[m->seltags]));
  json_object_object_add(json, "name", json_object_new_string(m->name));
  json_object_object_add(json, "mfact", json_object_new_double(m->mfact));
  json_object_object_add(json, "nmaster", json_object_new_int(m->nmaster));
  json_object_object_add(json, "scale", json_object_new_double(m->scale));

  gaps = json_object_new_object();
  json_object_object_add(gaps, "iv", json_object_new_int(m->gappiv));
  json_object_object_add(gaps, "ih", json_object_new_int(m->gappih));
  json_object_object_add(gaps, "ov", json_object_new_int(m->gappov));
  json_object_object_add(gaps, "oh", json_object_new_int(m->gappoh));
  json_object_object_add(json, "gaps", gaps);

  geom = json_object_new_object();
  json_object_object_add(geom, "x", json_object_new_double(m->m.x));
  json_object_object_add(geom, "y", json_object_new_double(m->m.y));
  json_object_object_add(json, "geom", geom);

  json_object_object_add(json, "address", json_object_new_int64((uintptr_t)m));

  lua_pushstring(L, json_object_to_json_string(json));

  json_object_put(json);
  return 1;
}

int lua_monitorsetgaps(lua_State *L) {
  LuaMonitor *lm = (LuaMonitor *)luaL_checkudata(L, 1, "Monitor");
  int oh = luaL_checkinteger(L, 2);
  int ov = luaL_checkinteger(L, 3);
  int ih = luaL_checkinteger(L, 4);
  int iv = luaL_checkinteger(L, 5);

  SETGAPS(lm, oh, ov, ih, iv);
  arrange(lm->m);
  return 0;
}

int lua_monitorsetgapsdefault(lua_State *L) {
  LuaMonitor *lm = (LuaMonitor *)luaL_checkudata(L, 1, "Monitor");

  SETGAPS(lm, get_config_gappoh(), get_config_gappov(), get_config_gappih(),
          get_config_gappiv());
  arrange(lm->m);
  return 0;
}

int lua_monitorsetlayout(lua_State *L) {
  LuaMonitor *lm = (LuaMonitor *)luaL_checkudata(L, 1, "Monitor");
  unsigned int i = (int)luaL_checkinteger(L, 2);

  if (i < LENGTH(layouts))
    setlayoutmonitor(lm->m, &(const Arg){.v = &layouts[i]});
  else
    return luaL_error(
        L, "índice de layout inválido: %d. O índice deve estar entre 0 e %d", i,
        LENGTH(layouts) - 1);

  return 0;
}

int lua_monitorsetmfact(lua_State *L) {
  LuaMonitor *lm = (LuaMonitor *)luaL_checkudata(L, 1, "Monitor");
  float mfact = (float)luaL_checknumber(L, 2);

  if (mfact < 0.1 || mfact > 0.9)
    return 0;

  lm->m->mfact = lm->m->pertag->mfacts[lm->m->pertag->curtag] = mfact;
  arrange(lm->m);
  return 0;
}

int lua_monitorsettags(lua_State *L) {
  LuaMonitor *lm = (LuaMonitor *)luaL_checkudata(L, 1, "Monitor");
  unsigned int n = luaL_checkinteger(L, 2);
  viewmonitor(lm->m, &(const Arg){.ui = n});
  arrange(lm->m);
  return 0;
}

int lua_monitorsetnmaster(lua_State *L) {
  LuaMonitor *lm = (LuaMonitor *)luaL_checkudata(L, 1, "Monitor");
  int n = luaL_checkinteger(L, 2);

  lm->m->nmaster = lm->m->pertag->nmasters[lm->m->pertag->curtag] = MAX(n, 0);
  arrange(lm->m);
  return 0;
}

int lua_monitortogglegaps(lua_State *L) {
  LuaMonitor *lm = (LuaMonitor *)luaL_checkudata(L, 1, "Monitor");
  int *enablegaps = get_enablegaps();

  *enablegaps = !*enablegaps;
  arrange(lm->m);

  printstatus();
  return 0;
}

int lua_monitortoggletags(lua_State *L) {
  LuaMonitor *lm = (LuaMonitor *)luaL_checkudata(L, 1, "Monitor");
  unsigned int n = luaL_checkinteger(L, 2);
  toggleviewmonitor(lm->m, &(const Arg){.ui = n});
  arrange(lm->m);
  return 0;
}

/* arrange: */
/* if (lua_getconfig(H, "layouts", LUA_TTABLE)) { */
/*   lua_pushnil(L); */
/**/
/*   while (lua_next(H, -2) != 0) { */
/*     if (lua_isstring(H, -1) && lua_isfunction(H, -2)) { */
/*       symbol = lua_tostring(H, -2); */
/*       if (strcmp(symbol, m->ltsymbol) == 0) { */
/*         lua_createmonitor(H, m); */
/**/
/*         if (lua_pcall(L, 1, 0, 0) != LUA_OK) { */
/*           printf("Erro ao chamar a função Lua: %s\n", lua_tostring(L, -1));
 */
/*         } */
/*       } */
/*     } */
/*     lua_pop(L, 1); */
/*   } */
/* } */
