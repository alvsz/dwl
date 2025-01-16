/* Copyright (c), Charlotte Meyer <dev@buffet.sh>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <lauxlib.h>
#include <lua.h>
#include <stdbool.h>
#include <wayland-server-core.h>
#include <wayland-util.h>

#include "dwl-ipc-protocol.h"
#include "ipc.h"
#include "types.h"
#include "util.h"

struct dwl_ipc_client {
  struct wl_list link;
  struct wl_resource *resource;
  lua_State *L;
};

struct wl_list ipc_clients;

static void ipc_eval(struct wl_client *client, struct wl_resource *resource,
                     uint32_t id, const char *message) {
  struct dwl_ipc_client *c = wl_resource_get_user_data(resource);
  lua_State *L = c->L;
  int top, results;

  struct wl_resource *command_resource =
      wl_resource_create(client, &dwl_command_interface, 1, id);

  top = lua_gettop(L);

  lua_pushboolean(L, true);
  lua_setglobal(L, "FROM_KIWMIC");

  if (luaL_dostring(L, message)) {
    const char *error = lua_tostring(L, -1);
    /* wlr_log(WLR_ERROR, "Error running IPC command: %s", error); */
    dwl_command_send_done(command_resource, DWL_COMMAND_ERROR_FAILURE, error);
    lua_pop(L, 1);

    lua_pushboolean(L, false);
    lua_setglobal(L, "FROM_KIWMIC");

    return;
  }

  lua_pushboolean(L, false);
  lua_setglobal(L, "FROM_KIWMIC");

  results = top - lua_gettop(L);

  if (results == 0) {
    dwl_command_send_done(command_resource, DWL_COMMAND_ERROR_SUCCESS, "");
  } else {
    lua_getglobal(L, "tostring");
    lua_insert(L, -2);

    if (lua_pcall(L, 1, 1, 0)) {
      const char *error = lua_tostring(L, -1);
      /* wlr_log(WLR_ERROR, "Error running IPC command: %s", error); */
      dwl_command_send_done(command_resource, DWL_COMMAND_ERROR_FAILURE, error);
      lua_pop(L, 1);
      return;
    }

    dwl_command_send_done(command_resource, DWL_COMMAND_ERROR_SUCCESS,
                          lua_tostring(L, -1));
  }

  lua_pop(L, results);
}

static const struct dwl_ipc_interface dwl_ipc_implementation = {
    .eval = ipc_eval,
};

static void dwl_server_resource_destroy(struct wl_resource *resource) {
  struct dwl_ipc_client *c = wl_resource_get_user_data(resource);
  wl_list_remove(&c->link);
  free(c);
  // EMPTY
}

static void ipc_server_bind(struct wl_client *client, void *data,
                            uint32_t version, uint32_t id) {
  lua_State *L = data;

  struct dwl_ipc_client *c = calloc(1, sizeof(struct dwl_ipc_client));

  /* struct wl_resource *resource = */
  c->resource = wl_resource_create(client, &dwl_ipc_interface, version, id);
  if (!c->resource) {
    wl_client_post_no_memory(client);
    return;
  }

  c->L = L;

  wl_resource_set_implementation(c->resource, &dwl_ipc_implementation, c,
                                 dwl_server_resource_destroy);

  dwl_ipc_send_frame(c->resource);

  wl_list_insert(&ipc_clients, &c->link);
}

bool lua_ipc_init(lua_State *L) {
  struct wl_global *ipc_global =
      wl_global_create(dpy, &dwl_ipc_interface, 1, L, ipc_server_bind);

  if (ipc_global) {
    /* wlr_log(WLR_ERROR, "Failed to create IPC global"); */
    return false;
  }

  return true;
}

void dwl_ipc_send_client_opened_event(Client *b) {
  struct dwl_ipc_client *c;
  char *str;
  int err;

  wl_list_for_each(c, &ipc_clients, link) {
    if (c->resource) {
      err = asprintf(&str, "%llu", (uintptr_t)b);

      if (err == -1)
        fprintf(stderr, "erro no asprintf");

      dwl_ipc_send_client_opened(c->resource, str);
    }
  }
}

void dwl_ipc_send_client_closed_event(Client *b) {
  struct dwl_ipc_client *c;
  char *str;
  int err;

  wl_list_for_each(c, &ipc_clients, link) {
    if (c->resource) {
      err = asprintf(&str, "%llu", (uintptr_t)b);

      if (err == -1)
        fprintf(stderr, "erro no asprintf");

      dwl_ipc_send_client_closed(c->resource, str);
    }
  }
}

void dwl_ipc_send_client_title_changed_event(Client *b) {
  struct dwl_ipc_client *c;
  char *str;
  int err;

  wl_list_for_each(c, &ipc_clients, link) {
    if (c->resource) {
      err = asprintf(&str, "%llu", (uintptr_t)b);

      if (err == -1)
        fprintf(stderr, "erro no asprintf");

      dwl_ipc_send_client_title_changed(c->resource, str);
    }
  }
}

void dwl_ipc_send_client_state_changed_event(Client *b) {
  struct dwl_ipc_client *c;
  char *str;
  int err;

  wl_list_for_each(c, &ipc_clients, link) {
    if (c->resource) {
      err = asprintf(&str, "%llu", (uintptr_t)b);

      if (err == -1)
        fprintf(stderr, "erro no asprintf");

      dwl_ipc_send_client_state_changed(c->resource, str);
    }
  }
}

void dwl_ipc_send_frame_event() {
  struct dwl_ipc_client *c;

  wl_list_for_each(c, &ipc_clients, link) {
    if (c->resource)
      dwl_ipc_send_frame(c->resource);
  }
}

void dwl_ipc_send_monitor_added_event(Monitor *m) {
  struct dwl_ipc_client *c;
  char *str;
  int err;

  wl_list_for_each(c, &ipc_clients, link) {
    if (c->resource) {
      err = asprintf(&str, "%llu", (uintptr_t)m);

      if (err == -1)
        fprintf(stderr, "erro no asprintf");

      dwl_ipc_send_monitor_added(c->resource, str);
    }
  }
}

void dwl_ipc_send_monitor_removed_event(Monitor *m) {
  struct dwl_ipc_client *c;
  char *str;
  int err;

  wl_list_for_each(c, &ipc_clients, link) {
    if (c->resource) {
      err = asprintf(&str, "%llu", (uintptr_t)m);

      if (err == -1)
        fprintf(stderr, "erro no asprintf");

      dwl_ipc_send_monitor_removed(c->resource, str);
    }
  }
}
