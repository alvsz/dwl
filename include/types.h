#ifndef TYPES_H
#define TYPES_H

/* enums */
#include <stdint.h>
#include <wayland-server.h>
#include <wayland-util.h>
#include <wlr/util/box.h>
#include <xkbcommon/xkbcommon.h>

enum { CurNormal, CurPressed, CurMove, CurResize }; /* cursor */
enum { XDGShell, LayerShell, X11 };                 /* client types */
enum {
  LyrBg,
  LyrBottom,
  LyrTile,
  LyrFloat,
  LyrTop,
  LyrFS,
  LyrOverlay,
  LyrIMPopup,
  LyrBlock,
  NUM_LAYERS
}; /* scene layers */

typedef union {
  int i;
  uint32_t ui;
  float f;
  const void *v;
} Arg;

typedef struct {
  unsigned int mod;
  unsigned int button;
  void (*func)(const Arg *);
  const Arg arg;
} Button;

typedef struct Pertag Pertag;
typedef struct Monitor Monitor;
typedef struct {
  /* Must keep this field first */
  unsigned int type; /* XDGShell or X11* */

  Monitor *mon;
  struct wlr_scene_tree *scene;
  struct wlr_scene_rect *border[4]; /* top, bottom, left, right */
  struct wlr_scene_tree *scene_surface;
  struct wl_list link;
  struct wl_list flink;
  struct wlr_box geom;   /* layout-relative, includes border */
  struct wlr_box prev;   /* layout-relative, includes border */
  struct wlr_box bounds; /* only width and height are used */
  union {
    struct wlr_xdg_surface *xdg;
    struct wlr_xwayland_surface *xwayland;
  } surface;
  struct wlr_xdg_toplevel_decoration_v1 *decoration;
  struct wl_listener commit;
  struct wl_listener map;
  struct wl_listener maximize;
  struct wl_listener unmap;
  struct wl_listener destroy;
  struct wl_listener set_title;
  struct wl_listener fullscreen;
  struct wl_listener set_decoration_mode;
  struct wl_listener destroy_decoration;
#ifdef XWAYLAND
  struct wl_listener activate;
  struct wl_listener associate;
  struct wl_listener dissociate;
  struct wl_listener configure;
  struct wl_listener set_hints;
#endif
  unsigned int bw;
  uint32_t tags;
  int isfloating, isurgent, isfullscreen, nokill;
  char scratchkey;
  uint32_t resize; /* configure serial of a pending resize */
} Client;

typedef struct {
  uint32_t mod;
  xkb_keysym_t keysym;
  int locked;
  void (*func)(const Arg *);
  const Arg arg;
} Key;

typedef struct {
  struct wlr_keyboard_group *wlr_group;

  int nsyms;
  const xkb_keysym_t *keysyms; /* invalid if nsyms == 0 */
  uint32_t mods;               /* invalid if nsyms == 0 */
  struct wl_event_source *key_repeat_source;

  struct wl_listener modifiers;
  struct wl_listener key;
  struct wl_listener destroy;
} KeyboardGroup;

typedef struct {
  /* Must keep this field first */
  unsigned int type; /* LayerShell */

  Monitor *mon;
  struct wlr_scene_tree *scene;
  struct wlr_scene_tree *popups;
  struct wlr_scene_layer_surface_v1 *scene_layer;
  struct wl_list link;
  int mapped;
  struct wlr_layer_surface_v1 *layer_surface;

  struct wl_listener destroy;
  struct wl_listener unmap;
  struct wl_listener surface_commit;
} LayerSurface;

typedef struct {
  const char *symbol;
  void (*arrange)(Monitor *);
} Layout;

struct Monitor {
  struct wl_list link;
  struct wlr_output *wlr_output;
  struct wlr_scene_output *scene_output;
  struct wlr_scene_rect *fullscreen_bg; /* See createmon() for info */
  struct wl_listener frame;
  struct wl_listener destroy;
  struct wl_listener request_state;
  struct wl_listener destroy_lock_surface;
  struct wlr_session_lock_surface_v1 *lock_surface;
  struct wlr_box m;         /* monitor area, layout-relative */
  struct wlr_box w;         /* window area, layout-relative */
  struct wl_list layers[4]; /* LayerSurface.link */
  const Layout *lt[2];
  int gappih; /* horizontal gap between windows */
  int gappiv; /* vertical gap between windows */
  int gappoh; /* horizontal outer gaps */
  int gappov; /* vertical outer gaps */
  Pertag *pertag;
  unsigned int seltags;
  unsigned int sellt;
  uint32_t tagset[2];
  float mfact;
  int gamma_lut_changed;
  int nmaster;
  char ltsymbol[16];
  int asleep;
  char *name;
  float scale;
};

typedef struct {
  const char *name;
  float mfact;
  int nmaster;
  float scale;
  const Layout *lt;
  enum wl_output_transform rr;
  int x, y;
} MonitorRule;

typedef struct {
  struct wlr_pointer_constraint_v1 *constraint;
  struct wl_listener destroy;
} PointerConstraint;

typedef struct {
  const char *id;
  const char *title;
  uint32_t tags;
  int isfloating;
  int nokill;
  int monitor;
  char scratchkey;
} Rule;

typedef struct {
  struct wlr_scene_tree *scene;

  struct wlr_session_lock_v1 *lock;
  struct wl_listener new_surface;
  struct wl_listener unlock;
  struct wl_listener destroy;
} SessionLock;

typedef struct {
  Client *c;
} LuaClient;

typedef struct {
  Monitor *m;
} LuaMonitor;

#endif
