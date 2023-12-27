// clang-format off

#include <X11/XF86keysym.h>

/* appearance */
static const int sloppyfocus               = 1;  /* focus follows mouse */
static const int bypass_surface_visibility = 0;  /* 1 means idle inhibitors will disable idle tracking even if it's surface isn't visible  */
static const unsigned int borderpx         = 2;  /* border pixel of windows */

/* ---- gaps ---- */
static const int smartgaps                 = 0;  /* 1 means no outer gap when there is only one window */
static const int monoclegaps               = 0;  /* 1 means outer gaps in monocle layout */

static const unsigned int gappih           = 10; /* horiz inner gap between windows */
static const unsigned int gappiv           = 10; /* vert inner gap between windows */
static const unsigned int gappoh           = 10; /* horiz outer gap between windows and screen edge */
static const unsigned int gappov           = 10; /* vert outer gap between windows and screen edge */

/* Taken from https://github.com/djpohly/dwl/issues/466 */
                        // (hex & 0xFF) / 255.0f }
#define COLOR(hex)    { ((hex >> 24) & 0xFF) / 255.0f, \
                        ((hex >> 16) & 0xFF) / 255.0f, \
                        ((hex >> 8) & 0xFF) / 255.0f, \
                        0.5 }

/* ---- borders ---- */
#include "/home/mamba/.cache/wal/colors-wal-dwl.h"

static const float *bordercolor            = background;
static const float *focuscolor             = color4;
static const float *urgentcolor            = color1;
static const float *floatcolor						 = color3;
static const float *scratchcolor					 = color5;

/* To conform the xdg-protocol, set the alpha to zero to restore the old behavior */
static const float fullscreen_bg[]         = {0.1, 0.1, 0.1, 1.0}; /* You can also use glsl colors */

/* Autostart */
#include "autostart.h"

/* tagging - TAGCOUNT must be no greater than 31 */
#define TAGCOUNT (7)

/* logging */
static int log_level = WLR_ERROR;

static const int allow_constrain      = 1;

#define APP_TERM                "alacritty"
#define CLASS_SP                "ScratchPad"
#define TITLE_SP                "scratchpad"
#define GNOME_PORTAL            "xdg-desktop-portal-gnome"
#define GTK_PORTAL              "xdg-desktop-portal-gtk"
#define PRISM_LAUNCHER          "org.prismlauncher.PrismLauncher"
#define RARE                    "Rare - GUI for legendary"

#include "rules.h"

/* layout(s) */
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "",      tile },
	{ "",      NULL },    /* no layout function means floating behavior */
	{ "",      monocle },
};

/* monitors */
static const MonitorRule monrules[] = {
	/* name       mfact nmaster scale layout       rotate/reflect                x    y */
	/* example of a HiDPI laptop monitor:
	{ "eDP-1",    0.5,  1,      2,    &layouts[0], WL_OUTPUT_TRANSFORM_NORMAL,   -1,  -1 },
	*/
	/* defaults */
	{ NULL,       0.55, 1,      1,    &layouts[0], WL_OUTPUT_TRANSFORM_NORMAL,   -1,  -1 },
};

/* keyboard */
static const struct xkb_rule_names xkb_rules = {
	/* can specify fields: rules, model, layout, variant, options */
	/* example:
	.options = "ctrl:nocaps",
	*/
	.options = NULL,
	.layout = "br",
  .variant = "abnt2"
};

/* numlock and capslock */
static const int numlock = 1;
static const int capslock = 0;

static const int repeat_rate = 25;
static const int repeat_delay = 600;

/* Trackpad */
static const int tap_to_click = 1;
static const int tap_and_drag = 1;
static const int drag_lock = 1;
static const int natural_scrolling = 1;
static const int disable_while_typing = 1;
static const int left_handed = 0;
static const int middle_button_emulation = 0;
/* You can choose between:
LIBINPUT_CONFIG_SCROLL_NO_SCROLL
LIBINPUT_CONFIG_SCROLL_2FG
LIBINPUT_CONFIG_SCROLL_EDGE
LIBINPUT_CONFIG_SCROLL_ON_BUTTON_DOWN
*/
static const enum libinput_config_scroll_method scroll_method = LIBINPUT_CONFIG_SCROLL_EDGE;

/* You can choose between:
LIBINPUT_CONFIG_CLICK_METHOD_NONE
LIBINPUT_CONFIG_CLICK_METHOD_BUTTON_AREAS
LIBINPUT_CONFIG_CLICK_METHOD_CLICKFINGER
*/
static const enum libinput_config_click_method click_method = LIBINPUT_CONFIG_CLICK_METHOD_CLICKFINGER;

/* You can choose between:
LIBINPUT_CONFIG_SEND_EVENTS_ENABLED
LIBINPUT_CONFIG_SEND_EVENTS_DISABLED
LIBINPUT_CONFIG_SEND_EVENTS_DISABLED_ON_EXTERNAL_MOUSE
*/
static const uint32_t send_events_mode = LIBINPUT_CONFIG_SEND_EVENTS_ENABLED;

/* You can choose between:
LIBINPUT_CONFIG_ACCEL_PROFILE_FLAT
LIBINPUT_CONFIG_ACCEL_PROFILE_ADAPTIVE
*/
static const enum libinput_config_accel_profile accel_profile = LIBINPUT_CONFIG_ACCEL_PROFILE_ADAPTIVE;
static const double accel_speed = 0.0;
/* You can choose between:
LIBINPUT_CONFIG_TAP_MAP_LRM -- 1/2/3 finger tap maps to left/right/middle
LIBINPUT_CONFIG_TAP_MAP_LMR -- 1/2/3 finger tap maps to left/middle/right
*/
static const enum libinput_config_tap_button_map button_map = LIBINPUT_CONFIG_TAP_MAP_LRM;

/* If you want to use the windows key for MODKEY, use WLR_MODIFIER_LOGO */
#define MODKEY WLR_MODIFIER_LOGO

#define TAGKEYS(KEY,SKEY,TAG) \
	{ MODKEY,                    KEY,            view,            {.ui = 1 << TAG} }, \
	{ MODKEY|WLR_MODIFIER_CTRL,  KEY,            toggleview,      {.ui = 1 << TAG} }, \
	{ MODKEY|WLR_MODIFIER_SHIFT, SKEY,           tag,             {.ui = 1 << TAG} }, \
	{ MODKEY|WLR_MODIFIER_CTRL|WLR_MODIFIER_SHIFT,SKEY,toggletag, {.ui = 1 << TAG} }

#define CHVT(n)                                                                \
  {                                                                            \
    WLR_MODIFIER_CTRL | WLR_MODIFIER_ALT, XKB_KEY_XF86Switch_VT_##n, chvt, {   \
      .ui = (n)                                                                \
    }                                                                          \
  }

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static const char *termcmd[] = {"alacritty", NULL};
static const char *menucmd[] = {"ags", "-t", "applauncher", NULL};

/* named scratchpads - First arg only serves to match against key in rules*/
static const char *scratchcmd[] = { "t", APP_TERM, "--class", CLASS_SP, "-t", TITLE_SP, NULL };
static const char *scratchspt[] = { "s", APP_TERM, "--class", CLASS_SP, "-t", "spotify", "-e", "spt", NULL };
static const char *scratchpwcalc[] = { "c", "eww", "open", "pwcalc-wayland", NULL };
static const char *scratchcalc[] = { "c", "sh", "-c", "$CALCULATOR", NULL };

void toggleinscratch(const Arg *arg) {
  Client *c = focustop(selmon);
  if (!c)
    return;

  if (c->scratchkey != 0)
    c->scratchkey = 0;
  else 
    c->scratchkey = ((char **)arg->v)[0][0];
}

#include "keys.h"

static const Button buttons[] = {
	{ MODKEY, BTN_LEFT,   moveresize,      {.ui = CurMove} },
	{ MODKEY, BTN_RIGHT,  togglefloating,  {0} },
	{ MODKEY, BTN_MIDDLE,  moveresize,     {.ui = CurResize} },
};
