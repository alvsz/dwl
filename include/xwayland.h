#ifndef XWAYLAND_HTETE
#define XWAYLAND_HTETE

#include <wlr/xwayland.h>
#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>

void activatex11(struct wl_listener *listener, void *data);
void associatex11(struct wl_listener *listener, void *data);
void configurex11(struct wl_listener *listener, void *data);
void createnotifyx11(struct wl_listener *listener, void *data);
void dissociatex11(struct wl_listener *listener, void *data);
xcb_atom_t getatom(xcb_connection_t *xc, const char *name);
void sethints(struct wl_listener *listener, void *data);
void xwaylandready(struct wl_listener *listener, void *data);

struct wlr_xwayland **get_xwayland(void);
xcb_atom_t *get_netatom(void);

#endif // !XWAYLAND_HTETE
