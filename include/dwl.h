#ifndef DWL_H
#define DWL_H

#include <getopt.h>
#include <lauxlib.h>
#include <libinput.h>
#include <linux/input-event-codes.h>
#include <lua.h>
#include <lualib.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wayland-server-core.h>
#include <wayland-util.h>
#include <wlr/backend.h>
#include <wlr/backend/libinput.h>
#include <wlr/interfaces/wlr_keyboard.h>
#include <wlr/render/allocator.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_alpha_modifier_v1.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_data_control_v1.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_drm.h>
#include <wlr/types/wlr_export_dmabuf_v1.h>
#include <wlr/types/wlr_fractional_scale_v1.h>
#include <wlr/types/wlr_gamma_control_v1.h>
#include <wlr/types/wlr_idle_inhibit_v1.h>
#include <wlr/types/wlr_idle_notify_v1.h>
#include <wlr/types/wlr_input_device.h>
#include <wlr/types/wlr_keyboard.h>
#include <wlr/types/wlr_keyboard_group.h>
#include <wlr/types/wlr_linux_dmabuf_v1.h>
#include <wlr/types/wlr_linux_drm_syncobj_v1.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_output_layout.h>
#include <wlr/types/wlr_output_management_v1.h>
#include <wlr/types/wlr_pointer.h>
#include <wlr/types/wlr_pointer_gestures_v1.h>
#include <wlr/types/wlr_presentation_time.h>
#include <wlr/types/wlr_primary_selection.h>
#include <wlr/types/wlr_primary_selection_v1.h>
#include <wlr/types/wlr_relative_pointer_v1.h>
#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_screencopy_v1.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/types/wlr_server_decoration.h>
#include <wlr/types/wlr_session_lock_v1.h>
#include <wlr/types/wlr_single_pixel_buffer_v1.h>
#include <wlr/types/wlr_subcompositor.h>
#include <wlr/types/wlr_viewporter.h>
#include <wlr/types/wlr_virtual_keyboard_v1.h>
#include <wlr/types/wlr_virtual_pointer_v1.h>
#include <wlr/types/wlr_xcursor_manager.h>
#include <wlr/types/wlr_xdg_activation_v1.h>
#include <wlr/types/wlr_xdg_output_v1.h>
#include <wlr/util/log.h>
#include <wlr/util/region.h>
#include <xkbcommon/xkbcommon.h>

#include "types.h"

void applybounds(Client *c, struct wlr_box *bbox);
void applyrules(Client *c);
void arrange(Monitor *m);
void arrangelayer(Monitor *m, struct wl_list *list, struct wlr_box *usable_area,
                  int exclusive);
void arrangelayers(Monitor *m);
void axisnotify(struct wl_listener *listener, void *data);
void buttonpress(struct wl_listener *listener, void *data);
void swipe_begin(struct wl_listener *listener, void *data);
void swipe_update(struct wl_listener *listener, void *data);
void swipe_end(struct wl_listener *listener, void *data);
void pinch_begin(struct wl_listener *listener, void *data);
void pinch_update(struct wl_listener *listener, void *data);
void pinch_end(struct wl_listener *listener, void *data);
void hold_begin(struct wl_listener *listener, void *data);
void hold_end(struct wl_listener *listener, void *data);
void chvt(const Arg *arg);
void checkidleinhibitor(struct wlr_surface *exclude);
void cleanup(void);
void cleanupmon(struct wl_listener *listener, void *data);
void closemon(Monitor *m);
void commitlayersurfacenotify(struct wl_listener *listener, void *data);
void commitnotify(struct wl_listener *listener, void *data);
void commitpopup(struct wl_listener *listener, void *data);
void createdecoration(struct wl_listener *listener, void *data);
void createidleinhibitor(struct wl_listener *listener, void *data);
void createkeyboard(struct wlr_keyboard *keyboard);
KeyboardGroup *createkeyboardgroup(void);
void createlayersurface(struct wl_listener *listener, void *data);
void createlocksurface(struct wl_listener *listener, void *data);
void createmon(struct wl_listener *listener, void *data);
void createnotify(struct wl_listener *listener, void *data);
void createpointer(struct wlr_pointer *pointer);
void createpointerconstraint(struct wl_listener *listener, void *data);
void createpopup(struct wl_listener *listener, void *data);
void cursorconstrain(struct wlr_pointer_constraint_v1 *constraint);
void cursorframe(struct wl_listener *listener, void *data);
void cursorwarptohint(void);
void defaultgaps(const Arg *arg);
void destroydecoration(struct wl_listener *listener, void *data);
void destroydragicon(struct wl_listener *listener, void *data);
void destroyidleinhibitor(struct wl_listener *listener, void *data);
void destroylayersurfacenotify(struct wl_listener *listener, void *data);
void destroylock(SessionLock *lock, int unlocked);
void destroylocksurface(struct wl_listener *listener, void *data);
void destroynotify(struct wl_listener *listener, void *data);
void destroypointerconstraint(struct wl_listener *listener, void *data);
void destroysessionlock(struct wl_listener *listener, void *data);
void destroysessionmgr(struct wl_listener *listener, void *data);
void destroykeyboardgroup(struct wl_listener *listener, void *data);
Monitor *dirtomon(enum wlr_direction dir);
void focusclient(Client *c, int lift);
void focusmon(const Arg *arg);
void focusortogglematchingscratch(const Arg *arg);
void focusortogglescratch(const Arg *arg);
void focusstack(const Arg *arg);
Client *focustop(Monitor *m);
void fullscreennotify(struct wl_listener *listener, void *data);
void gpureset(struct wl_listener *listener, void *data);
void handlesig(int signo);
void incnmaster(const Arg *arg);
void incgaps(const Arg *arg);
void incigaps(const Arg *arg);
void incihgaps(const Arg *arg);
void incivgaps(const Arg *arg);
void incogaps(const Arg *arg);
void incohgaps(const Arg *arg);
void incovgaps(const Arg *arg);
void inputdevice(struct wl_listener *listener, void *data);
int keybinding(uint32_t mods, xkb_keysym_t sym);
void keypress(struct wl_listener *listener, void *data);
void keypressmod(struct wl_listener *listener, void *data);
int keyrepeat(void *data);
void killclient(const Arg *arg);
void locksession(struct wl_listener *listener, void *data);
void mapnotify(struct wl_listener *listener, void *data);
void maximizenotify(struct wl_listener *listener, void *data);
void monocle(Monitor *m);
void motionabsolute(struct wl_listener *listener, void *data);
void motionnotify(uint32_t time, struct wlr_input_device *device, double sx,
                  double sy, double sx_unaccel, double sy_unaccel);
void motionrelative(struct wl_listener *listener, void *data);
void moveresize(const Arg *arg);
void outputmgrapply(struct wl_listener *listener, void *data);
void outputmgrapplyortest(struct wlr_output_configuration_v1 *config, int test);
void outputmgrtest(struct wl_listener *listener, void *data);
void parsecolor(const char *val, float color[4]);
void pointerfocus(Client *c, struct wlr_surface *surface, double sx, double sy,
                  uint32_t time);
void printstatus(void);
void powermgrsetmode(struct wl_listener *listener, void *data);
void quit(const Arg *arg);
void rendermon(struct wl_listener *listener, void *data);
void requestdecorationmode(struct wl_listener *listener, void *data);
void requeststartdrag(struct wl_listener *listener, void *data);
void requestmonstate(struct wl_listener *listener, void *data);
void resize(Client *c, struct wlr_box geo, int interact);
void run(char *startup_cmd);
void setcursor(struct wl_listener *listener, void *data);
void setcursorshape(struct wl_listener *listener, void *data);
void setfloating(Client *c, int floating);
void setfullscreen(Client *c, int fullscreen);
void setgamma(struct wl_listener *listener, void *data);
void setgaps(int oh, int ov, int ih, int iv);
void setlayout(const Arg *arg);
void setlayoutmonitor(Monitor *m, const Arg *arg);
void setmfact(const Arg *arg);
void setmon(Client *c, Monitor *m, uint32_t newtags);
void setpsel(struct wl_listener *listener, void *data);
void setsel(struct wl_listener *listener, void *data);
void setup(void);
void spawn(const Arg *arg);
void spawnscratch(const Arg *arg);
void startdrag(struct wl_listener *listener, void *data);
void tag(const Arg *arg);
void tagmon(const Arg *arg);
void tile(Monitor *m);
void togglefloating(const Arg *arg);
void togglefullscreen(const Arg *arg);
void togglegaps(const Arg *arg);
void toggleinscratch(const Arg *arg);
void togglescratch(const Arg *arg);
void toggletag(const Arg *arg);
void toggleview(const Arg *arg);
void toggleviewmonitor(Monitor *m, const Arg *arg);
void unlocksession(struct wl_listener *listener, void *data);
void unmaplayersurfacenotify(struct wl_listener *listener, void *data);
void unmapnotify(struct wl_listener *listener, void *data);
void updatemons(struct wl_listener *listener, void *data);
void updatetitle(struct wl_listener *listener, void *data);
void urgent(struct wl_listener *listener, void *data);
void view(const Arg *arg);
void viewmonitor(Monitor *m, const Arg *arg);
void virtualkeyboard(struct wl_listener *listener, void *data);
void virtualpointer(struct wl_listener *listener, void *data);
Monitor *xytomon(double x, double y);
void xytonode(double x, double y, struct wlr_surface **psurface, Client **pc,
              LayerSurface **pl, double *nx, double *ny);
void zoom(const Arg *arg);

const char *get_broken(void);
struct wl_list *get_clients(void);
struct wl_list *get_ipc_clients(void);
lua_State *get_lua(void);
struct wlr_xcursor_manager *get_cursor_mgr(void);
Client *get_grabc(void);
struct wl_display *get_dpy(void);
int *get_enablegaps(void);
struct wl_list *get_mons(void);
struct wlr_seat *get_seat(void);
Monitor *get_selmon(void);

float *get_config_rootcolor(void);
float *get_config_bordercolor(void);
float *get_config_focuscolor(void);
float *get_config_urgentcolor(void);
float *get_config_floatcolor(void);
unsigned int *get_config_borderpx(void);
enum libinput_config_accel_profile *get_config_accel_profile(void);
double *get_config_accel_speed(void);
enum libinput_config_click_method *get_config_click_method(void);
enum libinput_config_scroll_method *get_config_scroll_method(void);
int *get_config_tap_to_click(void);
int *get_config_tap_and_drag(void);
int *get_config_drag_lock(void);
int *get_config_natural_scrolling(void);
int *get_config_disable_while_typing(void);
int *get_config_left_handed(void);
int *get_config_middle_button_emulation(void);
Rule **get_config_rules(void);
size_t *get_config_nrules(void);

unsigned int get_config_gappih(void);
unsigned int get_config_gappiv(void);
unsigned int get_config_gappoh(void);
unsigned int get_config_gappov(void);

#endif
