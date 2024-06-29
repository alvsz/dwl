#include "dwl-ipc-unstable-v2-client-protocol.h"
#include <poll.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <wayland-util.h>

static void noop();

static void dwl_ipc_tags(void *data, struct zdwl_ipc_manager_v2 *ipc_manager,
                         uint32_t count);

static void dwl_ipc_layout(void *data, struct zdwl_ipc_manager_v2 *ipc_manager,
                           const char *name);

static struct layout *last_layout();

static struct tag *last_tag(struct tag *tag);

static void
dwl_ipc_output_toggle_visibility(void *data,
                                 struct zdwl_ipc_output_v2 *dwl_ipc_output);

static void dwl_ipc_output_active(void *data,
                                  struct zdwl_ipc_output_v2 *dwl_ipc_output,
                                  uint32_t active);

static void dwl_ipc_output_tag(void *data,
                               struct zdwl_ipc_output_v2 *dwl_ipc_output,
                               uint32_t tag_index, uint32_t state,
                               uint32_t clients, uint32_t focused);

static void dwl_ipc_output_layout(void *data,
                                  struct zdwl_ipc_output_v2 *dwl_ipc_output,
                                  uint32_t layout);

static void dwl_ipc_output_layout_symbol(
    void *data, struct zdwl_ipc_output_v2 *dwl_ipc_output, const char *layout);

static void dwl_ipc_output_title(void *data,
                                 struct zdwl_ipc_output_v2 *dwl_ipc_output,
                                 const char *title);

static void dwl_ipc_output_appid(void *data,
                                 struct zdwl_ipc_output_v2 *dwl_ipc_output,
                                 const char *appid);

static void dwl_ipc_output_fullscreen(void *data,
                                      struct zdwl_ipc_output_v2 *dwl_ipc_output,
                                      uint32_t is_fullscreen);

static void dwl_ipc_output_floating(void *data,
                                    struct zdwl_ipc_output_v2 *dwl_ipc_output,
                                    uint32_t is_floating);

static void dwl_ipc_output_frame(void *data,
                                 struct zdwl_ipc_output_v2 *dwl_ipc_output);

static void wl_output_name(void *data, struct wl_output *output,
                           const char *name);

static void global_add(void *data, struct wl_registry *wl_registry,
                       uint32_t name, const char *interface, uint32_t version);

static void global_remove(void *data, struct wl_registry *wl_registry,
                          uint32_t name);

void escape_special_characters(char *str);

void print_status();
