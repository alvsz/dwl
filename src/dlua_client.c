#include <json-c/json.h>
#include <json-c/json_object.h>
#include <lauxlib.h>
#include <lua.h>
#include <stdint.h>
#include <string.h>

#include "client.h"
#include "dlua_client.h"
#include "dlua_monitor.h"
#include "dwl.h"
#include "util.h"

int lua_clientindex(lua_State *L) {
  LuaClient *lc = (LuaClient *)luaL_checkudata(L, 1, "Client");
  const char *key = luaL_checkstring(L, 2);
  const char *appid, *title;
  const char *broken = get_broken();
  char str[2];

  if (strcmp(key, "app_id") == 0) {
    if (!(appid = client_get_appid(lc->c)))
      appid = broken;

    lua_pushstring(L, appid);
    return 1;
  } else if (strcmp(key, "title") == 0) {
    if (!(title = client_get_title(lc->c)))
      title = broken;

    lua_pushstring(L, title);
    return 1;
  } else if (strcmp(key, "focused") == 0) {
    lua_pushboolean(L, lc->c == focustop(lc->c->mon));
    return 1;
  } else if (strcmp(key, "tags") == 0) {
    lua_pushinteger(L, lc->c->tags);
    return 1;
  } else if (strcmp(key, "x11") == 0) {
    lua_pushboolean(L, client_is_x11(lc->c));
    return 1;
  } else if (strcmp(key, "monitor") == 0) {
    lua_createmonitor(L, lc->c->mon);
    return 1;
  } else if (strcmp(key, "geometry") == 0) {
    lua_newtable(L);
    lua_pushstring(L, "x");
    lua_pushinteger(L, lc->c->geom.x);
    lua_rawset(L, -3);

    lua_pushstring(L, "y");
    lua_pushinteger(L, lc->c->geom.y);
    lua_rawset(L, -3);

    lua_pushstring(L, "width");
    lua_pushinteger(L, lc->c->geom.width);
    lua_rawset(L, -3);

    lua_pushstring(L, "height");
    lua_pushinteger(L, lc->c->geom.height);
    lua_rawset(L, -3);

    return 1;
  } else if (strcmp(key, "floating") == 0) {
    lua_pushboolean(L, lc->c->isfloating);
    return 1;
  } else if (strcmp(key, "urgent") == 0) {
    lua_pushboolean(L, lc->c->isurgent);
    return 1;
  } else if (strcmp(key, "fullscreen") == 0) {
    lua_pushboolean(L, lc->c->isfullscreen);
    return 1;
  } else if (strcmp(key, "nokill") == 0) {
    lua_pushboolean(L, lc->c->nokill);
    return 1;
  } else if (strcmp(key, "scratchkey") == 0) {
    str[0] = lc->c->scratchkey;
    str[1] = '\0';
    lua_pushstring(L, str);
    return 1;
  } else if (strcmp(key, "address") == 0) {
    lua_pushinteger(L, (uintptr_t)lc->c);
    return 1;
  }

  luaL_getmetatable(L, "Client");
  lua_getfield(L, -1, key);
  return 1;
}

int lua_clientnewindex(lua_State *L) {
  LuaClient *lc = (LuaClient *)luaL_checkudata(L, 1, "Client");
  const char *key = luaL_checkstring(L, 2);
  const char *value;
  LuaMonitor *lm;
  unsigned int i;
  Monitor *selmon = get_selmon();

  if (strcmp(key, "tags") == 0) {
    i = luaL_checkinteger(L, 3);

    if ((i & TAGMASK) == 0)
      return 0;

    lc->c->tags = i & TAGMASK;
    focusclient(focustop(selmon), 1);
    arrange(selmon);

    printstatus();
    return 0;
  } else if (strcmp(key, "monitor") == 0) {
    lm = (LuaMonitor *)luaL_checkudata(L, 3, "Monitor");
    setmon(lc->c, lm->m, 0);

    return 0;
  } else if (strcmp(key, "floating") == 0) {
    i = luaL_checkinteger(L, 3);
    if (lc->c && !lc->c->isfullscreen)
      setfloating(lc->c, i);

    return 0;
  } else if (strcmp(key, "nokill") == 0) {
    i = luaL_checkinteger(L, 3);
    lc->c->nokill = i;

    return 0;
  } else if (strcmp(key, "scratchkey") == 0) {
    value = luaL_checkstring(L, 3);
    lc->c->scratchkey = value[0];

    return 0;
  }

  return 0;
}

int lua_clientkill(lua_State *L) {
  LuaClient *lc = (LuaClient *)luaL_checkudata(L, 1, "Client");
  if (lc->c && !lc->c->nokill)
    client_send_close(lc->c);
  return 0;
}

int lua_clientresize(lua_State *L) {
  LuaClient *lc = (LuaClient *)luaL_checkudata(L, 1, "Client");
  int x = luaL_checkinteger(L, 2);
  int y = luaL_checkinteger(L, 3);
  int w = luaL_checkinteger(L, 4);
  int h = luaL_checkinteger(L, 5);

  resize(lc->c, (struct wlr_box){.x = x, .y = y, .width = w, .height = h}, 0);
  return 0;
}

int lua_clientserialize(lua_State *L) {
  LuaClient *lc = (LuaClient *)luaL_checkudata(L, 1, "Client");
  Client *c = lc->c;
  const char *appid, *title;
  const char *broken = get_broken();
  struct json_object *json = json_object_new_object();
  struct json_object *geom;
  char str[2];
  str[0] = c->scratchkey;
  str[1] = '\0';

  if (!(appid = client_get_appid(c)))
    appid = broken;

  if (!(title = client_get_title(c)))
    title = broken;

  json_object_object_add(json, "app_id", json_object_new_string(appid));
  json_object_object_add(json, "title", json_object_new_string(title));
  json_object_object_add(json, "focused",
                         json_object_new_boolean(c == focustop(c->mon)));
  json_object_object_add(json, "tags", json_object_new_int(c->tags));

  geom = json_object_new_object();
  json_object_object_add(geom, "x", json_object_new_int(c->geom.x));
  json_object_object_add(geom, "y", json_object_new_int(c->geom.y));
  json_object_object_add(geom, "width", json_object_new_int(c->geom.width));
  json_object_object_add(geom, "height", json_object_new_int(c->geom.height));
  json_object_object_add(json, "geometry", geom);

  json_object_object_add(json, "x11",
                         json_object_new_boolean(client_is_x11(c)));
  json_object_object_add(json, "floating",
                         json_object_new_boolean(c->isfloating));
  json_object_object_add(json, "urgent", json_object_new_boolean(c->isurgent));
  json_object_object_add(json, "fullscreen",
                         json_object_new_boolean(c->isfullscreen));
  json_object_object_add(json, "nokill", json_object_new_boolean(c->nokill));
  json_object_object_add(json, "scratchkey", json_object_new_string(str));

  json_object_object_add(json, "address", json_object_new_int64((uintptr_t)c));
  json_object_object_add(json, "monitor",
                         json_object_new_int64((uintptr_t)c->mon));

  lua_pushstring(L, json_object_to_json_string(json));

  json_object_put(json);
  return 1;
}

int lua_clientsettags(lua_State *L) {
  LuaClient *lc = (LuaClient *)luaL_checkudata(L, 1, "Client");
  uint32_t tag = (uint32_t)luaL_checkinteger(L, 2);
  Monitor *selmon = get_selmon();

  if ((tag & TAGMASK) == 0)
    return 0;

  lc->c->tags = tag & TAGMASK;
  focusclient(focustop(selmon), 1);
  arrange(selmon);

  printstatus();
  return 0;
}

int lua_clientsetmon(lua_State *L) {
  LuaClient *lc = (LuaClient *)luaL_checkudata(L, 1, "Client");
  LuaMonitor *lm = (LuaMonitor *)luaL_checkudata(L, 2, "Monitor");

  setmon(lc->c, lm->m, 0);
  return 0;
}

int lua_clienttogglefloating(lua_State *L) {
  LuaClient *lc = (LuaClient *)luaL_checkudata(L, 1, "Client");

  if (lc->c && !lc->c->isfullscreen)
    setfloating(lc->c, !lc->c->isfloating);

  lua_pushboolean(L, lc->c->isfloating);
  printstatus();
  return 1;
}

int lua_clienttoggleinscratch(lua_State *L) {
  LuaClient *lc = (LuaClient *)luaL_checkudata(L, 1, "Client");
  const char *key = luaL_checkstring(L, 2);

  if (lc->c->scratchkey != 0)
    lc->c->scratchkey = 0;
  else
    lc->c->scratchkey = key[0];

  printstatus();
  return 0;
}

int lua_clientvisibleon(lua_State *L) {
  LuaClient *lc = (LuaClient *)luaL_checkudata(L, 1, "Client");
  LuaMonitor *lm = (LuaMonitor *)luaL_checkudata(L, 2, "Monitor");
  lua_pushboolean(L, VISIBLEON(lc->c, lm->m));
  return 1;
}

int lua_createclient(lua_State *L, Client *c) {
  LuaClient *lc = (LuaClient *)lua_newuserdata(L, sizeof(LuaClient));

  luaL_getmetatable(L, "Client");
  lua_setmetatable(L, -2);
  lc->c = c;

  return 1;
}
