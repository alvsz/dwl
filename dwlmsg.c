// #include "dwl-ipc-unstable-v2-protocol.h"
#include "dwlmsg.h"
#include "dwl-ipc-unstable-v2-client-protocol.h"
#include <poll.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <wayland-util.h>

#define die(fmt, ...)                                                          \
  do {                                                                         \
    fprintf(stderr, fmt "\n", ##__VA_ARGS__);                                  \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

typedef struct layout Layout;
struct layout {
  char *layout_name;
  uint32_t index;
  Layout *next;
};

int debug = 0;

static struct layout *layouts;
static size_t layoutcount;

static uint32_t tagcount;

typedef struct tag Tag;
struct tag {
  uint32_t state;
  uint32_t clients;
  uint32_t focused;
  Tag *next;
};

typedef struct output Output;
struct output {
  struct wl_output *output;
  char *output_name;
  uint32_t name;

  uint32_t active;
  struct tag *tags;

  char *appid;
  char *title;

  uint32_t fullscreen;
  uint32_t floating;

  struct layout new_layout;
  struct layout old_layout;
  Output *next;
};

static struct output *outputs;
static size_t outputcount;

static struct wl_display *display;
static struct wl_registry *registry;
static struct zdwl_ipc_manager_v2 *dwl_ipc_manager;

static const struct zdwl_ipc_manager_v2_listener dwl_ipc_listener = {
    .tags = dwl_ipc_tags, .layout = dwl_ipc_layout};

static const struct wl_output_listener output_listener = {
    .geometry = noop,
    .mode = noop,
    .done = noop,
    .scale = noop,
    .name = wl_output_name,
    .description = noop,
};

static const struct zdwl_ipc_output_v2_listener dwl_ipc_output_listener = {
    .toggle_visibility = dwl_ipc_output_toggle_visibility,
    .active = dwl_ipc_output_active,
    .tag = dwl_ipc_output_tag,
    .layout = dwl_ipc_output_layout,
    .title = dwl_ipc_output_title,
    .appid = dwl_ipc_output_appid,
    .layout_symbol = dwl_ipc_output_layout_symbol,
    .fullscreen = dwl_ipc_output_fullscreen,
    .floating = dwl_ipc_output_floating,
    .frame = dwl_ipc_output_frame,
};

static const struct wl_registry_listener registry_listener = {
    .global = global_add,
    .global_remove = global_remove,
};

int main(int argc, char *argv[]) {
  char *d = getenv("DEBUG");

  if (d && strcmp(d, "full") == 0)
    debug = 1;

  display = wl_display_connect(NULL);
  if (!display)
    die("bad display");

  registry = wl_display_get_registry(display);
  if (!registry)
    die("bad registry");

  wl_registry_add_listener(registry, &registry_listener, NULL);

  wl_display_dispatch(display);
  wl_display_roundtrip(display);

  if (!dwl_ipc_manager)
    die("bad dwl-ipc protocol");

  wl_display_roundtrip(display);

  if (argc >= 2) {
    if (strcmp(argv[1], "follow") == 0) {
      print_status();

      while (wl_display_dispatch(display) != -1)
        print_status();
    } else if (strcmp(argv[1], "status") == 0)
      print_status();
  }

  return 0;
}
static void noop() {}

static void dwl_ipc_tags(void *data, struct zdwl_ipc_manager_v2 *ipc_manager,
                         uint32_t count) {
  tagcount = count;
}

static void dwl_ipc_layout(void *data, struct zdwl_ipc_manager_v2 *ipc_manager,
                           const char *name) {
  if (debug)
    fprintf(stderr, "novo layout detectado: %s\n", name);

  layouts = realloc(layouts, ++layoutcount * sizeof(struct layout));

  if (!layouts)
    die("não tem memória para o layout");

  layouts[layoutcount - 1].layout_name = strdup(name);
  layouts[layoutcount - 1].index = layoutcount - 1;
}

static struct layout get_layout(const char *name) {
  for (size_t i = 0; i < layoutcount; i++) {
    if (strcmp(name, layouts[i].layout_name) == 0) {
      return layouts[i];
    }
  }
  return layouts[0];
}

static size_t get_index_of_output(uint32_t id) {
  for (size_t i = 0; i < outputcount; i++) {
    if (outputs[i].name == id)
      return i;
  }
  return 0;
}

static void
dwl_ipc_output_toggle_visibility(void *data,
                                 struct zdwl_ipc_output_v2 *dwl_ipc_output) {
  struct output *o = (struct output *)data;

  if (debug)
    fprintf(stderr, "output toggle visibility event no monitor %s\n",
            o->output_name);
}

static void dwl_ipc_output_active(void *data,
                                  struct zdwl_ipc_output_v2 *dwl_ipc_output,
                                  uint32_t active) {
  struct output *o = (struct output *)data;

  o->active = active;

  if (debug)
    fprintf(stderr, "output active event no monitor %s: %u\n", o->output_name,
            o->active);
}

static void dwl_ipc_output_tag(void *data,
                               struct zdwl_ipc_output_v2 *dwl_ipc_output,
                               uint32_t tag_index, uint32_t state,
                               uint32_t clients, uint32_t focused) {
  struct output *o = (struct output *)data;

  o->tags = realloc(o->tags, (tag_index + 1) * sizeof(struct tag));

  if (!o->tags) {
    fprintf(stderr, "sem memória para a tag %ul no monitor %s\n", tag_index,
            o->output_name);
    die("sem memória para a tag");
  }

  o->tags[tag_index].state = state;
  o->tags[tag_index].clients = clients;
  o->tags[tag_index].focused = focused;

  if (debug)
    fprintf(stderr, "novo evento de tag no monitor %s\n", o->output_name);
}

static void dwl_ipc_output_layout(void *data,
                                  struct zdwl_ipc_output_v2 *dwl_ipc_output,
                                  uint32_t layout) {}

static void dwl_ipc_output_layout_symbol(
    void *data, struct zdwl_ipc_output_v2 *dwl_ipc_output, const char *layout) {
  struct output *o = (struct output *)data;

  o->old_layout = o->new_layout;
  o->new_layout = get_layout(layout);

  if (debug)
    fprintf(stderr, "novo layout no monitor %s: %s\n", o->output_name,
            o->new_layout.layout_name);
}

static void dwl_ipc_output_title(void *data,
                                 struct zdwl_ipc_output_v2 *dwl_ipc_output,
                                 const char *title) {
  struct output *o = (struct output *)data;

  o->title = strdup(title);

  if (debug)
    fprintf(stderr, "novo title no monitor %s: %s\n", o->output_name, o->title);
}

static void dwl_ipc_output_appid(void *data,
                                 struct zdwl_ipc_output_v2 *dwl_ipc_output,
                                 const char *appid) {
  struct output *o = (struct output *)data;

  o->appid = strdup(appid);

  if (debug)
    fprintf(stderr, "novo appid no monitor %s: %s\n", o->output_name, o->appid);
}

static void dwl_ipc_output_fullscreen(void *data,
                                      struct zdwl_ipc_output_v2 *dwl_ipc_output,
                                      uint32_t is_fullscreen) {
  struct output *o = (struct output *)data;

  o->fullscreen = is_fullscreen;

  if (debug)
    fprintf(stderr, "output fullscreen event no monitor %s: %u\n",
            o->output_name, o->fullscreen);
}

static void dwl_ipc_output_floating(void *data,
                                    struct zdwl_ipc_output_v2 *dwl_ipc_output,
                                    uint32_t is_floating) {
  struct output *o = (struct output *)data;

  o->floating = is_floating;

  if (debug)
    fprintf(stderr, "output floating event no monitor %s: %u\n", o->output_name,
            o->floating);
}

static void dwl_ipc_output_frame(void *data,
                                 struct zdwl_ipc_output_v2 *dwl_ipc_output) {
  uint32_t index;
  struct output *o = (struct output *)data;

  index = get_index_of_output(o->name);

  if (debug) {
    fprintf(stderr, "evento frame no monitor %s\n", o->output_name);
    fprintf(stderr, "index do monitor: %u\n", index);
  }

  outputs[index] = *o;

  // for (size_t i = 0; i < outputcount; i++) {
  //   fprintf(stderr, "monitor %s\n", outputs[i].output_name);
  // }

  fflush(stdout);
}

static void wl_output_name(void *data, struct wl_output *output,
                           const char *name) {
  if (debug)
    fprintf(stderr, "novo monitor detectado: %s\n", name);

  if (outputs && dwl_ipc_manager) {
    struct output *o = (struct output *)data;

    if (!o)
      die("bugou alguma coisa");

    o->output_name = strdup(name);

    if (debug)
      fprintf(stderr, "nome do novo monitor: %s\n", o->output_name);

    struct zdwl_ipc_output_v2 *dwl_ipc_output =
        zdwl_ipc_manager_v2_get_output(dwl_ipc_manager, output);

    if (debug)
      fprintf(stderr, "criado o ipc_output para o monitor\n");

    zdwl_ipc_output_v2_add_listener(dwl_ipc_output, &dwl_ipc_output_listener,
                                    o);

    if (debug)
      fprintf(stderr, "setou o listener\n");
  } else
    die("bugou o outputs ou o ipc manager");
}

static void global_add(void *data, struct wl_registry *wl_registry,
                       uint32_t name, const char *interface, uint32_t version) {
  if (strcmp(interface, wl_output_interface.name) == 0) {

    struct wl_output *o = wl_registry_bind(
        wl_registry, name, &wl_output_interface, WL_OUTPUT_NAME_SINCE_VERSION);

    if (debug)
      fprintf(stderr, "monitor novo\n");

    outputs = realloc(outputs, ++outputcount * sizeof(struct output));

    if (!outputs)
      die("não tem memória para o output");

    outputs[outputcount - 1].name = name;
    outputs[outputcount - 1].output = o;

    if (debug)
      fprintf(stderr, "setou o nome\n");

    wl_output_add_listener(o, &output_listener,
                           outputs ? &outputs[outputcount - 1] : NULL);

    if (debug)
      fprintf(stderr, "criou o listener\n");

    if (debug)
      fprintf(stderr, "novo output detectado: %u\n",
              outputs[outputcount - 1].name);
  } else if (strcmp(interface, zdwl_ipc_manager_v2_interface.name) == 0) {
    if (debug)
      fprintf(stderr, "dwl ipc manager adicionado: %u\n", name);

    dwl_ipc_manager =
        wl_registry_bind(wl_registry, name, &zdwl_ipc_manager_v2_interface,
                         version < 2 ? version : 2);

    zdwl_ipc_manager_v2_add_listener(dwl_ipc_manager, &dwl_ipc_listener, NULL);
  }
}

static void global_remove(void *data, struct wl_registry *wl_registry,
                          uint32_t name) {
  if (!outputs)
    return;

  for (size_t i = 0; i < outputcount; i++) {
    if (outputs[i].name == name) {
      if (debug)
        fprintf(stderr, "output removido: %s\n", outputs[i].output_name);

      wl_output_release(outputs[i].output);
      free(outputs[i].output_name);
      free(outputs[i].tags);
      free(outputs[i].appid);
      free(outputs[i].title);

      // outputs = realloc(outputs, --outputcount * sizeof(struct output));

      if (!outputs && outputcount)
        die("não tem memória para o output");
    }
  }
}

void escape_special_characters(char *str) {
  char *p = str;
  while (*p) {
    switch (*p) {
    case '\"':
    case '\\':
    case '/':
    case '\b':
    case '\f':
    case '\n':
    case '\r':
    case '\t':
      putchar('\\');
      putchar(*p);
      break;
    default:
      putchar(*p);
      break;
    }
    p++;
  }
}

void print_status() {
  printf("[ ");
  for (size_t i = 0; i < outputcount; i++) {
    if (i)
      printf(", ");

    printf("{ ");
    printf("\"id\": %u, ", outputs[i].name);

    printf("\"name\": \"");
    escape_special_characters(outputs[i].output_name);
    printf("\", ");

    printf("\"title\": \"");
    escape_special_characters(outputs[i].title);
    printf("\", ");

    printf("\"appid\": \"");
    escape_special_characters(outputs[i].appid);
    printf("\", ");

    printf("\"active\": %u, ", outputs[i].active);

    printf("\"layout\": { ");

    printf("\"new\": {\"index\": %u, \"symbol\": \"%s\"}, ",
           outputs[i].new_layout.index, outputs[i].new_layout.layout_name);

    printf("\"old\": {\"index\": %u, \"symbol\": \"%s\"} ",
           outputs[i].old_layout.index, outputs[i].old_layout.layout_name);

    printf("}, ");

    printf("\"tags\": [ ");
    for (size_t j = 0; j < tagcount; j++) {
      if (j)
        printf(", ");

      printf("{ ");
      printf("\"state\": %u, ", outputs[i].tags[j].state);
      printf("\"clients\": %u, ", outputs[i].tags[j].clients);
      printf("\"focused\": %u, ", outputs[i].tags[j].focused);
      printf("\"index\": %lu, ", j);
      printf("\"bit_mask\": %u", 1 << j);
      printf("}");
    }
    printf("] ");

    printf("}");
  }
  printf("]\n");
  fflush(stdout);
}
