void lua_autostart(lua_State *L) {
  if (lua_getconfig(L, "autostart", LUA_TFUNCTION)) {
    if (lua_pcall(L, 0, 0, 0))
      fprintf(stderr, "Erro ao executar o script: %s\n", lua_tostring(L, -1));
  }
}

int lua_clientindex(lua_State *L) {
  const char *appid, *title;
  LuaClient *lc;
  const char *key;

  lc = (LuaClient *)luaL_checkudata(L, 1, "Client");
  key = luaL_checkstring(L, 2);

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
  }

  luaL_getmetatable(L, "Client");
  lua_getfield(L, -1, key);
  return 1;
}

int lua_clientkill(lua_State *L) {
  LuaClient *lc = (LuaClient *)luaL_checkudata(L, 1, "Client");
  if (lc->c && !lc->c->nokill)
    client_send_close(lc->c);
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

int lua_getmonitors(lua_State *L) {
  Monitor *m;
  int i = 1;

  lua_newtable(L);

  wl_list_for_each(m, &mons, link) {
    lua_pushinteger(L, i);
    lua_createmonitor(L, m);
    lua_rawset(L, -3);
    i++;
  }
  return 1;
}

int lua_getconfig(lua_State *L, const char *key, int t) {
  lua_getglobal(L, "dwl_cfg");

  if (lua_isnil(L, -1)) {
    fprintf(stderr, "não tem variável dwl_cfg\n");
    lua_pop(L, 1);
    return 0;
  }

  if (!lua_istable(L, -1)) {
    fprintf(stderr, "dwl_cfg não é uma tabela\n");
    lua_pop(L, 1);
    return 0;
  }

  if (lua_getconfigfield(L, key, t))
    return 1;

  return 0;
}

int lua_getconfigfield(lua_State *L, const char *key, int t) {
  int type;

  lua_pushstring(L, key);
  lua_gettable(L, -2);

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
  }

  luaL_getmetatable(L, "Monitor");
  lua_getfield(L, -1, key);
  return 1;
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

void lua_reloadconfig(const Arg *arg) {
  lua_openconfigfile(H);

  if (lua_getconfig(H, "reload", LUA_TFUNCTION)) {
    if (lua_pcall(H, 0, 0, 0))
      fprintf(stderr, "Erro ao executar o script: %s\n", lua_tostring(H, -1));
  }
}

void lua_setaccelprofile(lua_State *L) {
  const char *val;

  if (lua_getconfigfield(L, "LIBINPUT_DEFAULT_ACCELERATION_PROFILE",
                         LUA_TSTRING)) {
    val = lua_tostring(L, -1);
    lua_pop(L, 1);

    if (strcmp(val, "FLAT") == 0)
      accel_profile = LIBINPUT_CONFIG_ACCEL_PROFILE_FLAT;
    else if (strcmp(val, "ADAPTIVE") == 0)
      accel_profile = LIBINPUT_CONFIG_ACCEL_PROFILE_ADAPTIVE;
  }
}

void lua_setaccelspeed(lua_State *L) {
  double val;

  if (lua_getconfigfield(L, "LIBINPUT_DEFAULT_ACCELERATION", LUA_TNUMBER)) {
    val = lua_tonumber(L, -1);
    lua_pop(L, 1);

    accel_speed = val;
  }
}

void lua_setclickmethod(lua_State *L) {
  const char *val;

  if (lua_getconfigfield(L, "LIBINPUT_DEFAULT_CLICK_METHOD", LUA_TSTRING)) {
    val = lua_tostring(L, -1);
    lua_pop(L, 1);

    if (strcmp(val, "NONE") == 0)
      click_method = LIBINPUT_CONFIG_CLICK_METHOD_NONE;
    else if (strcmp(val, "BUTTON_AREAS") == 0)
      click_method = LIBINPUT_CONFIG_CLICK_METHOD_BUTTON_AREAS;
    else if (strcmp(val, "CLICKFINGER") == 0)
      click_method = LIBINPUT_CONFIG_CLICK_METHOD_CLICKFINGER;
  }
}

void lua_setdwt(lua_State *L) {
  int val;

  if (lua_getconfigfield(L, "LIBINPUT_DEFAULT_DISABLE_WHILE_TYPING",
                         LUA_TNUMBER)) {
    val = lua_tointeger(L, -1);
    lua_pop(L, 1);

    disable_while_typing = val;
  }
}

void lua_setlefthanded(lua_State *L) {
  int val;

  if (lua_getconfigfield(L, "LIBINPUT_DEFAULT_LEFT_HANDED", LUA_TNUMBER)) {
    val = lua_tointeger(L, -1);
    lua_pop(L, 1);

    left_handed = val;
  }
}

void lua_setmiddleemul(lua_State *L) {
  int val;

  if (lua_getconfigfield(L, "LIBINPUT_DEFAULT_MIDDLE_EMULATION", LUA_TNUMBER)) {
    val = lua_tointeger(L, -1);
    lua_pop(L, 1);

    middle_button_emulation = val;
  }
}

void lua_setnaturalscroll(lua_State *L) {
  int val;

  if (lua_getconfigfield(L, "LIBINPUT_DEFAULT_NATURAL_SCROLL", LUA_TNUMBER)) {
    val = lua_tointeger(L, -1);
    lua_pop(L, 1);

    natural_scrolling = val;
  }
}

void lua_settap(lua_State *L) {
  int val;

  if (lua_getconfigfield(L, "LIBINPUT_DEFAULT_TAP", LUA_TNUMBER)) {
    val = lua_tointeger(L, -1);
    lua_pop(L, 1);

    tap_to_click = val;
  }
}

void lua_settapanddrag(lua_State *L) {
  int val;

  if (lua_getconfigfield(L, "LIBINPUT_DEFAULT_DRAG", LUA_TNUMBER)) {
    val = lua_tointeger(L, -1);
    lua_pop(L, 1);

    tap_to_click = val;
  }
}

void lua_setscrollmethod(lua_State *L) {
  const char *val;

  if (lua_getconfigfield(L, "LIBINPUT_DEFAULT_SCROLL_METHOD", LUA_TSTRING)) {
    val = lua_tostring(L, -1);
    lua_pop(L, 1);

    if (strcmp(val, "NO_SCROLL") == 0)
      scroll_method = LIBINPUT_CONFIG_SCROLL_NO_SCROLL;
    else if (strcmp(val, "2FG") == 0)
      scroll_method = LIBINPUT_CONFIG_SCROLL_2FG;
    else if (strcmp(val, "EDGE") == 0)
      scroll_method = LIBINPUT_CONFIG_SCROLL_EDGE;
    else if (strcmp(val, "ON_BUTTON_DOWN") == 0)
      scroll_method = LIBINPUT_CONFIG_SCROLL_ON_BUTTON_DOWN;
  }
}

void lua_setup(void) {
  const luaL_Reg client_metatable[] = {{"visibleon", lua_clientvisibleon},
                                       {"kill", lua_clientkill},
                                       {NULL, NULL}};

  const luaL_Reg monitor_metatable[] = {{"get_clients", lua_getclients},
                                        {NULL, NULL}};

  H = luaL_newstate();
  luaL_openlibs(H);

  fprintf(stderr, "lua criado\n");

  luaL_newmetatable(H, "Client");
  lua_pushcfunction(H, lua_clientindex);
  lua_setfield(H, -2, "__index");
  luaL_setfuncs(H, client_metatable, 0);

  luaL_newmetatable(H, "Monitor");
  lua_pushcfunction(H, lua_monitorindex);
  lua_setfield(H, -2, "__index");
  luaL_setfuncs(H, monitor_metatable, 0);

  lua_register(H, "get_monitors", lua_getmonitors);

  lua_openconfigfile(H);
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
