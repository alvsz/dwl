// #include "dwl-ipc-unstable-v2-protocol.h"
#include "dwlmsg.h"
#include "dwl-ipc-unstable-v2-client-protocol.h"
#include <poll.h>
#include <stddef.h>
#include <stdint.h>
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

static Layout *layouts;
static size_t layoutcount;

static uint32_t tagcount;

typedef struct tag Tag;
struct tag {
  uint32_t state;
  uint32_t clients;
  uint32_t focused;
  uint32_t index;
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

  const char *new_layout;
  const char *old_layout;
  Output *next;
};

static Output *outputs;
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
  Layout *l, *a;

  if (debug)
    fprintf(stderr, "novo layout detectado: %s\n", name);

  if (!layouts)
    l = layouts;
  else {
    a = last_layout();
    a->next = l;
  }

  strcpy(l->layout_name, name);
}

static Layout *last_layout() {
  Layout *l;

  for (l = layouts; l->next; l = l->next)
    ;

  return l;
}

static Layout *get_layout(const char *name) {
  Layout *l;
  for (l = layouts; l; l = l->next)
    if (strcmp(l->layout_name, name) == 0)
      return l;

  return layouts;
}

static Tag *last_tag(Tag *tag) {
  Tag *t;

  for (t = tag; t->next; t = t->next)
    ;

  return t;
}

static Output *last_output() {
  Output *o;

  for (o = outputs; o->next; o = o->next)
    ;

  return o;
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
  Output *o = (struct output *)data;
  Tag *t, *a;

  if (!o->tags)
    o->tags = t;
  else {
    a = last_tag(o->tags);
    a->next = t;
  }

  t->index = tag_index;
  t->state = state;
  t->clients = clients;
  t->focused = focused;

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
  o->new_layout = strdup(layout);

  if (debug)
    fprintf(stderr, "novo layout no monitor %s: %s\n", o->output_name,
            o->new_layout);
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
  struct output *o = (struct output *)data;

  if (debug)
    fprintf(stderr, "evento frame no monitor %s\n", o->output_name);

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
    Output *o, *a;

    struct wl_output *wl_o = wl_registry_bind(
        wl_registry, name, &wl_output_interface, WL_OUTPUT_NAME_SINCE_VERSION);

    if (debug)
      fprintf(stderr, "monitor novo\n");

    if (!outputs)
      o = outputs;
    else {
      a = last_output();
      a->next = o;
    }

    o->name = name;
    o->output = wl_o;

    if (debug)
      fprintf(stderr, "setou o nome\n");

    wl_output_add_listener(wl_o, &output_listener, NULL);

    if (debug) {
      fprintf(stderr, "criou o listener\n");
      fprintf(stderr, "novo output detectado: %u\n", o->name);
    }
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
  Output *o, *a;

  if (!outputs)
    return;

  for (a = outputs; a; a = a->next) {
    if (a->next->name == name) {
      o = a->next;
      a->next = o->next;

      if (debug)
        fprintf(stderr, "output removido: %s\n", o->output_name);

      wl_output_release(o->output);
      free(o->output_name);
      free(o->tags);
      free(o->appid);
      free(o->title);
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
  Output *o;
  Tag *t;
  int i;

  printf("[ ");
  for (i = 0, o = outputs; o->next; o = o->next, i++) {
    int j;

    if (i)
      printf(", ");

    printf("{ ");
    printf("\"id\": %u, ", o->name);

    printf("\"name\": \"");
    escape_special_characters(o->output_name);
    printf("\", ");

    printf("\"title\": \"");
    escape_special_characters(o->title);
    printf("\", ");

    printf("\"appid\": \"");
    escape_special_characters(o->appid);
    printf("\", ");

    printf("\"active\": %u, ", o->active);

    printf("\"layout\": { ");

    printf("\"new\": \"%s\", ", o->new_layout);

    printf("\"old\": \"%s\" ", o->old_layout);

    printf("}, ");

    printf("\"tags\": [ ");
    for (j = 0, t = o->tags; t->next; t = t->next) {
      if (j)
        printf(", ");

      printf("{ ");
      printf("\"state\": %u, ", t->state);
      printf("\"clients\": %u, ", t->clients);
      printf("\"focused\": %u, ", t->focused);
      printf("\"index\": %d, ", t->index);
      printf("\"bit_mask\": %u", 1 << t->index);
      printf("}");
    }
    printf("] ");

    printf("}");
  }
  printf("]\n");
  fflush(stdout);
}
