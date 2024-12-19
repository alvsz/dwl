#include "xwayland.h"

void activatex11(struct wl_listener *listener, void *data) {
  Client *c = wl_container_of(listener, c, activate);

  /* Only "managed" windows can be activated */
  if (!client_is_unmanaged(c))
    wlr_xwayland_surface_activate(c->surface.xwayland, 1);
}

void associatex11(struct wl_listener *listener, void *data) {
  Client *c = wl_container_of(listener, c, associate);

  LISTEN(&client_surface(c)->events.map, &c->map, mapnotify);
  LISTEN(&client_surface(c)->events.unmap, &c->unmap, unmapnotify);
}

void configurex11(struct wl_listener *listener, void *data) {
  Client *c = wl_container_of(listener, c, configure);
  struct wlr_xwayland_surface_configure_event *event = data;
  /* TODO: figure out if there is another way to do this */
  if (!c->mon) {
    wlr_xwayland_surface_configure(c->surface.xwayland, event->x, event->y,
                                   event->width, event->height);
    return;
  }
  if (c->isfloating || client_is_unmanaged(c))
    resize(c,
           (struct wlr_box){.x = event->x,
                            .y = event->y,
                            .width = event->width + c->bw * 2,
                            .height = event->height + c->bw * 2},
           0);
  else
    arrange(c->mon);
}

void createnotifyx11(struct wl_listener *listener, void *data) {
  struct wlr_xwayland_surface *xsurface = data;
  Client *c;

  /* Allocate a Client for this surface */
  c = xsurface->data = ecalloc(1, sizeof(*c));
  c->surface.xwayland = xsurface;
  c->type = X11;
  c->bw = client_is_unmanaged(c) ? 0 : borderpx;

  /* Listen to the various events it can emit */
  LISTEN(&xsurface->events.associate, &c->associate, associatex11);
  LISTEN(&xsurface->events.destroy, &c->destroy, destroynotify);
  LISTEN(&xsurface->events.dissociate, &c->dissociate, dissociatex11);
  LISTEN(&xsurface->events.request_activate, &c->activate, activatex11);
  LISTEN(&xsurface->events.request_configure, &c->configure, configurex11);
  LISTEN(&xsurface->events.request_fullscreen, &c->fullscreen,
         fullscreennotify);
  LISTEN(&xsurface->events.set_hints, &c->set_hints, sethints);
  LISTEN(&xsurface->events.set_title, &c->set_title, updatetitle);
}

void dissociatex11(struct wl_listener *listener, void *data) {
  Client *c = wl_container_of(listener, c, dissociate);
  wl_list_remove(&c->map.link);
  wl_list_remove(&c->unmap.link);
}

xcb_atom_t getatom(xcb_connection_t *xc, const char *name) {
  xcb_atom_t atom = 0;
  xcb_intern_atom_reply_t *reply;
  xcb_intern_atom_cookie_t cookie = xcb_intern_atom(xc, 0, strlen(name), name);
  if ((reply = xcb_intern_atom_reply(xc, cookie, NULL)))
    atom = reply->atom;
  free(reply);

  return atom;
}

void sethints(struct wl_listener *listener, void *data) {
  Client *c = wl_container_of(listener, c, set_hints);
  struct wlr_surface *surface = client_surface(c);
  if (c == focustop(selmon))
    return;

  c->isurgent = xcb_icccm_wm_hints_get_urgency(c->surface.xwayland->hints);
  printstatus();

  if (c->isurgent && surface && surface->mapped)
    client_set_border_color(c, urgentcolor);
}

void xwaylandready(struct wl_listener *listener, void *data) {
  struct wlr_xcursor *xcursor;
  xcb_connection_t *xc = xcb_connect(xwayland->display_name, NULL);
  int err = xcb_connection_has_error(xc);
  if (err) {
    fprintf(stderr,
            "xcb_connect to X server failed with code %d\n. Continuing with "
            "degraded functionality.\n",
            err);
    return;
  }

  /* Collect atoms we are interested in. If getatom returns 0, we will
   * not detect that window type. */
  netatom[NetWMWindowTypeDialog] = getatom(xc, "_NET_WM_WINDOW_TYPE_DIALOG");
  netatom[NetWMWindowTypeSplash] = getatom(xc, "_NET_WM_WINDOW_TYPE_SPLASH");
  netatom[NetWMWindowTypeToolbar] = getatom(xc, "_NET_WM_WINDOW_TYPE_TOOLBAR");
  netatom[NetWMWindowTypeUtility] = getatom(xc, "_NET_WM_WINDOW_TYPE_UTILITY");

  /* assign the one and only seat */
  wlr_xwayland_set_seat(xwayland, seat);

  /* Set the default XWayland cursor to match the rest of dwl. */
  if ((xcursor = wlr_xcursor_manager_get_xcursor(cursor_mgr, "default", 1)))
    wlr_xwayland_set_cursor(
        xwayland, xcursor->images[0]->buffer, xcursor->images[0]->width * 4,
        xcursor->images[0]->width, xcursor->images[0]->height,
        xcursor->images[0]->hotspot_x, xcursor->images[0]->hotspot_y);

  xcb_disconnect(xc);
}
