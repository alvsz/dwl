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

struct wl_display *display;

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
  struct kiwmi_ipc **ipc = data;
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

static void on_frame(void *data, struct dwl_ipc *ipc) {
  const char *cmd;
  int exit_code;

  if (data == NULL)
    printf("frame\n");
  else {
    cmd = (char *)data;
    run_command(ipc, cmd, &exit_code);
  }
}

static const struct dwl_ipc_listener listener = {.frame = on_frame};

int main(int argc, char **argv) {
  struct wl_registry *registry;
  struct dwl_ipc *ipc;
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

  if (argc > 1 && strcmp(argv[1], "follow") == 0) {
    printf("follow\n");

    dwl_ipc_add_listener(ipc, &listener, argc > 2 ? argv[2] : NULL);

    wl_display_roundtrip(display);

    while (wl_display_dispatch(display) != -1)
      ;

    wl_display_disconnect(display);
  } else if (argc > 2 && strcmp(argv[1], "run") == 0) {
    printf("run\n");

    run_command(ipc, argv[2], &exit_code);

    wl_display_disconnect(display);
    exit(exit_code);
  } else {
    fprintf(stderr, "Usage: dwlcmd run|follow COMMAND\n");
    exit(EXIT_FAILURE);
  }
}
