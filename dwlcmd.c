/* Copyright (c), Charlotte Meyer <dev@buffet.sh>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#define UNUSED(x) UNUSED_##x __attribute__((__unused__))

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <wayland-client.h>

#include "dwl-ipc-client-protocol.h"

#define MONITOR_ADDED_EVENT "monitor_added"
#define MONITOR_REMOVED_EVENT "monitor_removed"
#define CLIENT_OPENED_EVENT "client_opened"
#define CLIENT_CLOSED_EVENT "client_closed"
#define CLIENT_TITLE_CHANGED_EVENT "client_title_changed"
#define CLIENT_STATE_CHANGED_EVENT "client_state_changed"

struct wl_display *display;

struct cmd_data {
  struct dwl_ipc *ipc;
  const char *cmd;
  const char *event;
};

static void command_done(void *data, struct dwl_command *UNUSED(dwl_command),
                         uint32_t error, const char *message) {
  int *exit_code = data;
  FILE *out;

  if (error == DWL_COMMAND_ERROR_SUCCESS) {
    *exit_code = EXIT_SUCCESS;
    out = stdout;
  } else {
    *exit_code = EXIT_FAILURE;
    out = stderr;
  }

  if (message[0] != '\0') {
    fprintf(out, "%s\n", message);
  }
}

static const struct dwl_command_listener command_listener = {
    .done = command_done,
};

static void registry_global(void *data, struct wl_registry *registry,
                            uint32_t name, const char *interface,
                            uint32_t UNUSED(version)) {
  struct dwl_ipc **ipc = data;
  if (strcmp(interface, dwl_ipc_interface.name) == 0) {
    *ipc = wl_registry_bind(registry, name, &dwl_ipc_interface, 1);
  }
}

static void registry_global_remove(void *UNUSED(data),
                                   struct wl_registry *UNUSED(registry),
                                   uint32_t UNUSED(name)) {
  // EMPTY
}

static const struct wl_registry_listener registry_listener = {
    .global = registry_global,
    .global_remove = registry_global_remove,
};

static void run_command(struct dwl_ipc *ipc, const char *cmd, int *exit_code) {
  struct dwl_command *command = dwl_ipc_eval(ipc, cmd);
  dwl_command_add_listener(command, &command_listener, exit_code);

  wl_display_roundtrip(display);
}

static void run_command_follow(struct dwl_ipc *ipc, struct cmd_data *data,
                               const char *address) {
  int exit_code;

  printf("%s, %s\n", data->cmd, data->event);

  if (data->cmd == NULL) {
    if (address == NULL)
      printf("%s\n", data->event);
    else
      printf("%s", address);
  } else {
    run_command(ipc, data->cmd, &exit_code);
  }
}

static void on_frame(void *data, struct dwl_ipc *ipc) {
  struct cmd_data *d = (struct cmd_data *)data;
  if (strcmp(d->event, "follow") == 0)
    run_command_follow(ipc, data, NULL);
}

static void on_monitor_added(void *data, struct dwl_ipc *ipc,
                             const char *address) {
  struct cmd_data *d = (struct cmd_data *)data;
  if (strcmp(d->event, MONITOR_ADDED_EVENT) == 0)
    run_command_follow(ipc, data, address);
}

static void on_monitor_removed(void *data, struct dwl_ipc *ipc,
                               const char *address) {
  struct cmd_data *d = (struct cmd_data *)data;
  if (strcmp(d->event, MONITOR_REMOVED_EVENT) == 0)
    run_command_follow(ipc, data, address);
}

static void on_client_opened(void *data, struct dwl_ipc *ipc,
                             const char *address) {
  struct cmd_data *d = (struct cmd_data *)data;
  if (strcmp(d->event, CLIENT_OPENED_EVENT) == 0)
    run_command_follow(ipc, data, address);
}

static void on_client_closed(void *data, struct dwl_ipc *ipc,
                             const char *address) {
  struct cmd_data *d = (struct cmd_data *)data;
  if (strcmp(d->event, CLIENT_CLOSED_EVENT) == 0)
    run_command_follow(ipc, data, address);
}

static void on_client_title_changed(void *data, struct dwl_ipc *ipc,
                                    const char *address) {
  struct cmd_data *d = (struct cmd_data *)data;
  if (strcmp(d->event, CLIENT_TITLE_CHANGED_EVENT) == 0)
    run_command_follow(ipc, data, address);
}

static void on_client_state_changed(void *data, struct dwl_ipc *ipc,
                                    const char *address) {
  struct cmd_data *d = (struct cmd_data *)data;
  if (strcmp(d->event, CLIENT_STATE_CHANGED_EVENT) == 0)
    run_command_follow(ipc, data, address);
}

static const struct dwl_ipc_listener listener = {
    .frame = on_frame,
    .monitor_added = on_monitor_added,
    .monitor_removed = on_monitor_removed,
    .client_opened = on_client_opened,
    .client_closed = on_client_closed,
    .client_title_changed = on_client_title_changed,
    .client_state_changed = on_client_state_changed,
};

int main(int argc, char **argv) {
  struct wl_registry *registry;
  struct dwl_ipc *ipc;
  struct cmd_data data;
  int exit_code;

  display = wl_display_connect(NULL);
  if (!display) {
    fprintf(stderr, "Failed to connect to display\n");
    exit(EXIT_FAILURE);
  }

  registry = wl_display_get_registry(display);

  wl_registry_add_listener(registry, &registry_listener, &ipc);
  wl_display_roundtrip(display);

  if (!ipc) {
    fprintf(stderr, "Failed to bind to dwl_ipc\n");
    exit(EXIT_FAILURE);
  }

  if (argc > 1) {
    if (strcmp(argv[1], "follow") == 0) {
      data.ipc = ipc;
      data.event = "follow";
      data.cmd = argc > 2 ? argv[2] : NULL;
    } else if (argc > 2) {
      if (strcmp(argv[1], "subscribe") == 0) {
        if (strcmp(argv[2], MONITOR_ADDED_EVENT) == 0) {
          data.event = MONITOR_ADDED_EVENT;
        } else if (strcmp(argv[2], MONITOR_REMOVED_EVENT) == 0) {
          data.event = MONITOR_REMOVED_EVENT;
        } else if (strcmp(argv[2], CLIENT_OPENED_EVENT) == 0) {
          data.event = CLIENT_OPENED_EVENT;
        } else if (strcmp(argv[2], CLIENT_CLOSED_EVENT) == 0) {
          data.event = CLIENT_CLOSED_EVENT;
        } else if (strcmp(argv[2], CLIENT_TITLE_CHANGED_EVENT) == 0) {
          data.event = CLIENT_TITLE_CHANGED_EVENT;
        } else if (strcmp(argv[2], CLIENT_STATE_CHANGED_EVENT) == 0) {
          data.event = CLIENT_STATE_CHANGED_EVENT;
        } else {
          fprintf(stderr, "invalid event\n");
          goto quit_error;
        }
        data.ipc = ipc;
        data.cmd = argc > 3 ? argv[3] : NULL;
      }
    } else {
      goto quit_error;
    }

    dwl_ipc_add_listener(ipc, &listener, &data);

    wl_display_roundtrip(display);

    while (wl_display_dispatch(display) != -1)
      ;

    wl_display_disconnect(display);

  } else if (argc > 2 && strcmp(argv[1], "run") == 0) {
    run_command(ipc, argv[2], &exit_code);

    wl_display_disconnect(display);
    exit(exit_code);
  } else {
    goto quit_error;
  }

quit_error:
  fprintf(stderr, "Usage: dwlcmd run|follow|subscribe SIGNAL COMMAND\n");
  exit(EXIT_FAILURE);
}
