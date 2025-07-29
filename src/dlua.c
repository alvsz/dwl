#define _GNU_SOURCE

#include <lauxlib.h>
#include <libinput.h>
#include <lua.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "dlua.h"
#include "dlua_client.h"
#include "dlua_monitor.h"
#include "dwl.h"
#include "ipc.h"
#include "types.h"

void lua_autostart(lua_State *L) {
  if (lua_getconfig(L, "autostart", LUA_TFUNCTION)) {
    if (lua_pcall(L, 0, 0, 0))
      fprintf(stderr, "Erro ao executar o script: %s\n", lua_tostring(L, -1));
  }
}

int lua_getclient(lua_State *L) {
  Client *c;
  const char *key = luaL_checkstring(L, 1);
  unsigned long long int i = strtoll(key, NULL, 10);

  wl_list_for_each(c, get_clients(), link) {
    if (i == (uintptr_t)c) {
      lua_createclient(L, c);
      return 1;
    }
  }

  fprintf(stderr, "o endereço %llu não foi encontrado\n", i);
  return luaL_error(L, "o endereço %s não foi encontrado", key);
}

int lua_getconfig(lua_State *L, const char *key, int t) {
  lua_getglobal(L, "dwl");

  if (lua_isnil(L, -1)) {
    fprintf(stderr, "módulo não encontrado???\n");
    lua_pop(L, 1);
    return 0;
  }

  if (lua_getconfigfield(L, "cfg", LUA_TTABLE))
    if (lua_getconfigfield(L, key, t))
      return 1;

  return 0;
}

int lua_getconfigfield(lua_State *L, const char *key, int t) {
  int type;

  lua_getfield(L, -1, key);

  if (lua_isnil(L, -1)) {
    fprintf(stderr, "não existe campo %s\n", key);
    lua_pop(L, 1);
    return 0;
  }

  type = lua_type(L, -1);

  if (type != t) {
    fprintf(stderr, "%s não é um %s\n", key, lua_typename(L, t));
    lua_pop(L, 1);
    return 0;
  }

  return 1;
}

int lua_getmonitor(lua_State *L) {
  Monitor *m;
  const char *key = luaL_checkstring(L, 1);
  unsigned long long int i = strtoll(key, NULL, 10);

  wl_list_for_each(m, get_mons(), link) {
    if (i == (uintptr_t)m) {
      lua_createmonitor(L, m);
      return 1;
    }
  }

  fprintf(stderr, "o endereço %llu não foi encontrado\n", i);
  return luaL_error(L, "o endereço %s não foi encontrado", key);
}

int lua_getmonitors(lua_State *L) {
  Monitor *m;
  int i = 1;

  lua_newtable(L);

  wl_list_for_each(m, get_mons(), link) {
    lua_pushinteger(L, i);
    lua_createmonitor(L, m);
    lua_rawset(L, -3);
    i++;
  }
  return 1;
}

int lua_getselmon(lua_State *L) {
  lua_createmonitor(L, get_selmon());
  return 1;
}

int lua_getfocusedclient(lua_State *L) {
  lua_createclient(L, focustop(get_selmon()));
  return 1;
}

void lua_inputconfig(lua_State *L) {
  if (!lua_getconfig(L, "input_config", LUA_TTABLE))
    return;

  lua_setclickmethod(L);
  lua_settap(L);
  lua_settapanddrag(L);
  lua_setnaturalscroll(L);
  lua_setaccelprofile(L);
  lua_setaccelspeed(L);
  lua_setscrollmethod(L);
  lua_setdwt(L);
  lua_setmiddleemul(L);
  lua_setlefthanded(L);
}

void lua_loadtheme(lua_State *L) {
  const char *val;
  unsigned int tmp;

  if (!lua_getconfig(L, "theme", LUA_TTABLE))
    return;

  if (lua_getconfigfield(L, "root", LUA_TSTRING)) {
    val = lua_tostring(L, -1);
    lua_pop(L, 1);

    parsecolor(val, get_config_rootcolor());
  }

  if (lua_getconfigfield(L, "normal", LUA_TSTRING)) {
    val = lua_tostring(L, -1);
    lua_pop(L, 1);

    parsecolor(val, get_config_bordercolor());
  }

  if (lua_getconfigfield(L, "focus", LUA_TSTRING)) {
    val = lua_tostring(L, -1);
    lua_pop(L, 1);

    parsecolor(val, get_config_focuscolor());
  }

  if (lua_getconfigfield(L, "urgent", LUA_TSTRING)) {
    val = lua_tostring(L, -1);
    lua_pop(L, 1);

    parsecolor(val, get_config_urgentcolor());
  }

  if (lua_getconfigfield(L, "float", LUA_TSTRING)) {
    val = lua_tostring(L, -1);
    lua_pop(L, 1);

    parsecolor(val, get_config_floatcolor());
  }

  if (lua_getconfigfield(L, "border_width", LUA_TNUMBER)) {
    tmp = lua_tointeger(L, -1);
    lua_pop(L, 1);

    *get_config_borderpx() = tmp;
  }
}

void lua_openconfigfile(lua_State *L) {
  char *config_dir, *path, *home;
  int err;
  FILE *file;

  config_dir = getenv("XDG_CONFIG_HOME");

  if (config_dir == NULL) {
    fprintf(stderr,
            "A variável de ambiente XDG_CONFIG_HOME não está definida.\n");

    home = getenv("HOME");

    if (home == NULL) {
      fprintf(stderr, "A variável de ambiente HOME não está definida.\n");
      exit(1);
    }
    err = asprintf(&config_dir, "%s/.config", home);

    if (err == -1) {
      fprintf(stderr, "erro no asprintf");
      exit(1);
    }
  }

  err = asprintf(&path, "%s/dwl/rc.lua", config_dir);
  if (err == -1) {
    fprintf(stderr, "erro no asprintf");
    exit(1);
  }

  file = fopen(path, "r");

  if (file) {
    fclose(file);

    if (luaL_loadfile(L, path) || lua_pcall(L, 0, 0, 0))
      fprintf(stderr, "Erro ao executar o script: %s\n", lua_tostring(L, -1));
  } else {
    fprintf(stderr, "O arquivo rc.lua não existe.\n");
  }

  free(path);
}

int lua_reloadconfig(lua_State *L) {
  lua_openconfigfile(L);

  if (lua_getconfig(L, "reload", LUA_TFUNCTION)) {
    if (lua_pcall(L, 0, 0, 0))
      fprintf(stderr, "Erro ao executar o script: %s\n", lua_tostring(L, -1));
  }

  return 0;
}

int lua_reloadtheme(lua_State *L) {
  lua_reloadconfig(L);
  lua_loadtheme(L);
  return 0;
}

void lua_setaccelprofile(lua_State *L) {
  const char *val;

  if (lua_getconfigfield(L, "LIBINPUT_DEFAULT_ACCELERATION_PROFILE",
                         LUA_TSTRING)) {
    val = lua_tostring(L, -1);
    lua_pop(L, 1);

    if (strcmp(val, "FLAT") == 0)
      *get_config_accel_profile() = LIBINPUT_CONFIG_ACCEL_PROFILE_FLAT;
    else if (strcmp(val, "ADAPTIVE") == 0)
      *get_config_accel_profile() = LIBINPUT_CONFIG_ACCEL_PROFILE_ADAPTIVE;
  }
}

void lua_setaccelspeed(lua_State *L) {
  double val;

  if (lua_getconfigfield(L, "LIBINPUT_DEFAULT_ACCELERATION", LUA_TNUMBER)) {
    val = lua_tonumber(L, -1);
    lua_pop(L, 1);

    *get_config_accel_speed() = val;
  }
}

void lua_setclickmethod(lua_State *L) {
  const char *val;
  enum libinput_config_click_method *click_method = get_config_click_method();

  if (lua_getconfigfield(L, "LIBINPUT_DEFAULT_CLICK_METHOD", LUA_TSTRING)) {
    val = lua_tostring(L, -1);
    lua_pop(L, 1);

    if (strcmp(val, "NONE") == 0)
      *click_method = LIBINPUT_CONFIG_CLICK_METHOD_NONE;
    else if (strcmp(val, "BUTTON_AREAS") == 0)
      *click_method = LIBINPUT_CONFIG_CLICK_METHOD_BUTTON_AREAS;
    else if (strcmp(val, "CLICKFINGER") == 0)
      *click_method = LIBINPUT_CONFIG_CLICK_METHOD_CLICKFINGER;
  }
}

void lua_setdwt(lua_State *L) {
  int val;

  if (lua_getconfigfield(L, "LIBINPUT_DEFAULT_DISABLE_WHILE_TYPING",
                         LUA_TNUMBER)) {
    val = lua_tointeger(L, -1);
    lua_pop(L, 1);

    *get_config_disable_while_typing() = val;
  }
}

void lua_setlefthanded(lua_State *L) {
  int val;

  if (lua_getconfigfield(L, "LIBINPUT_DEFAULT_LEFT_HANDED", LUA_TNUMBER)) {
    val = lua_tointeger(L, -1);
    lua_pop(L, 1);

    *get_config_left_handed() = val;
  }
}

void lua_setmiddleemul(lua_State *L) {
  int val;

  if (lua_getconfigfield(L, "LIBINPUT_DEFAULT_MIDDLE_EMULATION", LUA_TNUMBER)) {
    val = lua_tointeger(L, -1);
    lua_pop(L, 1);

    *get_config_middle_button_emulation() = val;
  }
}

void lua_setnaturalscroll(lua_State *L) {
  int val;

  if (lua_getconfigfield(L, "LIBINPUT_DEFAULT_NATURAL_SCROLL", LUA_TNUMBER)) {
    val = lua_tointeger(L, -1);
    lua_pop(L, 1);

    *get_config_natural_scrolling() = val;
  }
}

void lua_settap(lua_State *L) {
  int val;

  if (lua_getconfigfield(L, "LIBINPUT_DEFAULT_TAP", LUA_TNUMBER)) {
    val = lua_tointeger(L, -1);
    lua_pop(L, 1);

    *get_config_tap_to_click() = val;
  }
}

void lua_settapanddrag(lua_State *L) {
  int val;

  if (lua_getconfigfield(L, "LIBINPUT_DEFAULT_DRAG", LUA_TNUMBER)) {
    val = lua_tointeger(L, -1);
    lua_pop(L, 1);

    *get_config_tap_to_click() = val;
  }
}

void lua_setscrollmethod(lua_State *L) {
  const char *val;
  enum libinput_config_scroll_method *scroll_method =
      get_config_scroll_method();

  if (lua_getconfigfield(L, "LIBINPUT_DEFAULT_SCROLL_METHOD", LUA_TSTRING)) {
    val = lua_tostring(L, -1);
    lua_pop(L, 1);

    if (strcmp(val, "NO_SCROLL") == 0)
      *scroll_method = LIBINPUT_CONFIG_SCROLL_NO_SCROLL;
    else if (strcmp(val, "2FG") == 0)
      *scroll_method = LIBINPUT_CONFIG_SCROLL_2FG;
    else if (strcmp(val, "EDGE") == 0)
      *scroll_method = LIBINPUT_CONFIG_SCROLL_EDGE;
    else if (strcmp(val, "ON_BUTTON_DOWN") == 0)
      *scroll_method = LIBINPUT_CONFIG_SCROLL_ON_BUTTON_DOWN;
  }
}

int lua_openmodule(lua_State *L) {
  luaL_Reg funcoes[] = {{"get_client", lua_getclient},
                        {"get_monitor", lua_getmonitor},
                        {"get_monitors", lua_getmonitors},
                        {"get_focused_monitor", lua_getselmon},
                        {"get_focused_client", lua_getfocusedclient},
                        {"reload_config", lua_reloadconfig},
                        {"reload_theme", lua_reloadtheme},
                        {"quit", lua_quit},
                        {NULL, NULL}};

  luaL_newlib(L, funcoes);
  return 1;
}

void lua_setup(lua_State **L) {
  const luaL_Reg client_metatable[] = {
      {"kill", lua_clientkill},
      {"resize", lua_clientresize},
      {"serialize", lua_clientserialize},
      {"set_tags", lua_clientsettags},
      {"set_mon", lua_clientsetmon},
      {"toggle_floating", lua_clienttogglefloating},
      {"toggle_in_scratch", lua_clienttoggleinscratch},
      {"visible_on", lua_clientvisibleon},
      {NULL, NULL}};

  const luaL_Reg monitor_metatable[] = {
      {"get_clients", lua_getclients},
      {"serialize", lua_monitorserialize},
      {"set_gaps", lua_monitorsetgaps},
      {"set_default_gaps", lua_monitorsetgapsdefault},
      {"set_layout", lua_monitorsetlayout},
      {"set_mfact", lua_monitorsetmfact},
      {"set_tags", lua_monitorsettags},
      {"set_nmaster", lua_monitorsetnmaster},
      {"toggle_gaps", lua_monitortogglegaps},
      {"toggle_tags", lua_monitortoggletags},
      {NULL, NULL}};

  *L = luaL_newstate();
  luaL_openlibs(*L);

  fprintf(stderr, "lua criado\n");

  luaL_requiref(*L, "dwl", lua_openmodule, 1);
  lua_setglobal(*L, "dwl");

  luaL_newmetatable(*L, "Client");

  lua_pushcfunction(*L, lua_clientindex);
  lua_setfield(*L, -2, "__index");
  luaL_setfuncs(*L, client_metatable, 0);

  lua_pushcfunction(*L, lua_clientnewindex);
  lua_setfield(*L, -2, "__newindex");

  luaL_newmetatable(*L, "Monitor");

  lua_pushcfunction(*L, lua_monitorindex);
  lua_setfield(*L, -2, "__index");
  luaL_setfuncs(*L, monitor_metatable, 0);

  lua_pushcfunction(*L, lua_monitornewindex);
  lua_setfield(*L, -2, "__newindex");

  lua_openconfigfile(*L);

  lua_loadtheme(*L);
  lua_ipc_init(*L);
}

void lua_setupenv(lua_State *L) {
  const char *key;
  const char *value;

  if (lua_getconfig(L, "env", LUA_TTABLE)) {
    lua_pushnil(L);

    while (lua_next(L, -2) != 0) {
      if (lua_isstring(L, -1) && lua_isstring(L, -2)) {
        key = lua_tostring(L, -2);
        value = lua_tostring(L, -1);
        setenv(key, value, 1);
      }
      lua_pop(L, 1);
    }
  }
}

void lua_spawn(const Arg *arg) {
  lua_State *L = get_lua();
  char **cmd = (char **)arg->v;

  if (luaL_dostring(L, cmd[0])) {
    const char *error = lua_tostring(L, -1);
    fprintf(stderr, "erro no lua: %s\n", error);
  }
}

int lua_quit(lua_State *L) {
  quit(NULL);
  return 0;
}
