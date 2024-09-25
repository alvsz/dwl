// clang-format off
static const Key keys[] = {
    /* Note that Shift changes certain key codes: c -> C, 2 -> at, etc. */
    /* modifier                                          key                   function            argument */
    { MODKEY,                                            XKB_KEY_d,            spawn,              {.v = menucmd} },
    { MODKEY,                                            XKB_KEY_t,            spawn,              {.v = termcmd} },
    { MODKEY | WLR_MODIFIER_SHIFT,                       XKB_KEY_Return,       spawn,              {.v = termcmd} },
    { MODKEY | WLR_MODIFIER_SHIFT,                       XKB_KEY_L,            spawn,              SHCMD("loginctl lock-session") },

    /* -------- bar -------- */
    { MODKEY,                                            XKB_KEY_p,            spawn,              SHCMD("ags -t bar-0") },
    // { MODKEY,                                            XKB_KEY_p,            spawn,              SHCMD("eww open bar-wayland --toggle") },
    // { MODKEY | WLR_MODIFIER_SHIFT,                       XKB_KEY_P,            spawn,              SHCMD("pkill eww; eww daemon") },

    { MODKEY,                                            XKB_KEY_q,            killclient,         {0} },
    { MODKEY | WLR_MODIFIER_SHIFT | WLR_MODIFIER_CTRL,   XKB_KEY_Q,            quit,               {0} },
    { MODKEY | WLR_MODIFIER_SHIFT,                       XKB_KEY_Q,            lua_reloadconfig,   {0} },

    /* -------- layouts -------- */
    { MODKEY,                                            XKB_KEY_u,            setlayout,          {.v = &layouts[0]} },
    { MODKEY,                                            XKB_KEY_i,            setlayout,          {.v = &layouts[1]} },
    { MODKEY,                                            XKB_KEY_o,            setlayout,          {.v = &layouts[2]} },

    /* -------- scratchpad -------- */
    { MODKEY,                                            XKB_KEY_x,            togglescratch,      {.v = scratchcmd } },
    { MODKEY | WLR_MODIFIER_CTRL,                        XKB_KEY_x,            toggleinscratch,    {.v = scratchcmd } },
    { MODKEY,                                            XKB_KEY_s,            togglescratch,      {.v = scratchspt } },
    { MODKEY | WLR_MODIFIER_CTRL,                        XKB_KEY_s,            toggleinscratch,    {.v = scratchspt } },
    { MODKEY,                                            XKB_KEY_w,            togglescratch,      {.v = scratchpwcalc } },
    { MODKEY | WLR_MODIFIER_CTRL,                        XKB_KEY_w,            toggleinscratch,    {.v = scratchpwcalc } },
    { 0,                                                 XF86XK_Calculator,    togglescratch,      {.v = scratchcalc } },

    /* -------- stack -------- */
    { MODKEY,                                            XKB_KEY_Return,       zoom,               {0} },
    { MODKEY | WLR_MODIFIER_SHIFT,                       XKB_KEY_Tab,          view,               {0} },

    { MODKEY,                                            XKB_KEY_Tab,          focusstack,         {.i = +1} },
    { MODKEY | WLR_MODIFIER_SHIFT,                       XKB_KEY_Up,           incnmaster,         {.i = +1} },
    { MODKEY | WLR_MODIFIER_SHIFT,                       XKB_KEY_Down,         incnmaster,         {.i = -1} },

    { MODKEY,                                            XKB_KEY_Left,         setmfact,           {.f = -0.05} },
    { MODKEY,                                            XKB_KEY_Right,        setmfact,           {.f = +0.05} },
    { MODKEY,                                            XKB_KEY_space,        togglefloating,     {0} },
    { MODKEY,                                            XKB_KEY_F11,          togglefullscreen,   {0} },

    /* -------- monitores -------- */
    { MODKEY,                                            XKB_KEY_comma,        focusmon,           {.i = WLR_DIRECTION_LEFT} },
    { MODKEY,                                            XKB_KEY_period,       focusmon,           {.i = WLR_DIRECTION_RIGHT} },
    { MODKEY | WLR_MODIFIER_SHIFT,                       XKB_KEY_less,         tagmon,             {.i = WLR_DIRECTION_LEFT} },
    { MODKEY | WLR_MODIFIER_SHIFT,                       XKB_KEY_greater,      tagmon,             {.i = WLR_DIRECTION_RIGHT} },

    /* -------- gaps -------- */
    { MODKEY | WLR_MODIFIER_SHIFT,                       XKB_KEY_underscore,   togglegaps,         {0} },
    { MODKEY | WLR_MODIFIER_SHIFT,                       XKB_KEY_plus,         defaultgaps,        {0} },
    { MODKEY,                                            XKB_KEY_minus,        incgaps,            {.i = +1} },
    { MODKEY,                                            XKB_KEY_equal,        incgaps,            {.i = -1} },
    { MODKEY | WLR_MODIFIER_CTRL,                        XKB_KEY_Right,        incovgaps,          {.i = +1} },
    { MODKEY | WLR_MODIFIER_CTRL,                        XKB_KEY_Left,         incovgaps,          {.i = -1} },
    { MODKEY | WLR_MODIFIER_CTRL,                        XKB_KEY_Down,         incohgaps,          {.i = +1} },
    { MODKEY | WLR_MODIFIER_CTRL,                        XKB_KEY_Up,           incohgaps,          {.i = -1} },

    /* -------- tags -------- */
    TAGKEYS(XKB_KEY_1,                                   XKB_KEY_exclam,          0),
    TAGKEYS(XKB_KEY_2,                                   XKB_KEY_at,              1),
    TAGKEYS(XKB_KEY_3,                                   XKB_KEY_numbersign,      2),
    TAGKEYS(XKB_KEY_4,                                   XKB_KEY_dollar,          3),
    TAGKEYS(XKB_KEY_5,                                   XKB_KEY_percent,         4),
    TAGKEYS(XKB_KEY_6,                                   XKB_KEY_dead_diaeresis,  5),
    TAGKEYS(XKB_KEY_7,                                   XKB_KEY_ampersand,       6),

    { MODKEY,                                            XKB_KEY_0,                  view,           {.ui = ~0} },
    { MODKEY | WLR_MODIFIER_SHIFT,                       XKB_KEY_parenright,         tag,            {.ui = ~0} },

    { MODKEY,                                            XKB_KEY_r,                  spawn,          SHCMD("ags -t dashboard") },
    { MODKEY,                                            XKB_KEY_c,                  spawn,          SHCMD("ags -t calendar") },

    { 0,                                                 XF86XK_AudioStop,           spawn,          SHCMD("midia stop") },
    { 0,                                                 XF86XK_AudioPrev,           spawn,          SHCMD("midia previous") },
    { 0,                                                 XF86XK_AudioPlay,           spawn,          SHCMD("midia play-pause") },
    { 0,                                                 XF86XK_AudioNext,           spawn,          SHCMD("midia next") },
    { MODKEY,                                            XKB_KEY_j,                  spawn,          SHCMD("midia previous") },
    { MODKEY,                                            XKB_KEY_k,                  spawn,          SHCMD("midia play-pause") },
    { MODKEY,                                            XKB_KEY_l,                  spawn,          SHCMD("midia next") },
                                                
    { 0,                                                 XF86XK_AudioMute,           spawn,          SHCMD("volume mute") },
    { 0,                                                 XF86XK_AudioMute,           spawn,          SHCMD("volume mute") },
    { 0,                                                 XF86XK_AudioLowerVolume,    spawn,          SHCMD("volume down") },
    { 0,                                                 XF86XK_AudioRaiseVolume,    spawn,          SHCMD("volume up") },
    { MODKEY,                                            XKB_KEY_F1,                 spawn,          SHCMD("volume mute") },
    { MODKEY,                                            XKB_KEY_F2,                 spawn,          SHCMD("volume down") },
    { MODKEY,                                            XKB_KEY_F3,                 spawn,          SHCMD("volume up") },	

    { WLR_MODIFIER_SHIFT,                                XF86XK_AudioMute,           spawn,          SHCMD("volume mic mute") },
    { WLR_MODIFIER_SHIFT,                                XF86XK_AudioLowerVolume,    spawn,          SHCMD("volume mic down") },
    { WLR_MODIFIER_SHIFT,                                XF86XK_AudioRaiseVolume,    spawn,          SHCMD("volume mic up") },
    { MODKEY | WLR_MODIFIER_SHIFT,                       XKB_KEY_F1,                 spawn,          SHCMD("volume mic mute") },
    { MODKEY | WLR_MODIFIER_SHIFT,                       XKB_KEY_F2,                 spawn,          SHCMD("volume mic down") },
    { MODKEY | WLR_MODIFIER_SHIFT,                       XKB_KEY_F3,                 spawn,          SHCMD("volume mic up") },	

    { 0,                                                 XF86XK_MonBrightnessDown,   spawn,          SHCMD("backlight down") },
    { 0,                                                 XF86XK_MonBrightnessUp,     spawn,          SHCMD("backlight up") },
    { MODKEY,                                            XKB_KEY_F4,                 spawn,          SHCMD("backlight down") },
    { MODKEY,                                            XKB_KEY_F5,                 spawn,          SHCMD("backlight up") },

    { 0,                                                 XF86XK_Tools,               spawn,          SHCMD("mouse dpi up") },
    { 0,                                                 XF86XK_Launch5,             spawn,          SHCMD("mouse dpi down") },

	/* ------------ apps ----------- */
    { MODKEY,                                            XKB_KEY_f,                  spawn,          SHCMD("$EXPLORER") },
    { MODKEY,                                            XKB_KEY_b,                  spawn,          SHCMD("$BROWSER") },
    { MODKEY,                                            XKB_KEY_e,                  spawn,          SHCMD("$EMAIL") },
    { MODKEY,                                            XKB_KEY_m,                  spawn,          SHCMD("rofi -show emoji -emoji-mode copy") },
    // { MODKEY,                                            XKB_KEY_v,                  spawn,          SHCMD("CM_LAUNCHER=rofi clipmenu -i ") },
    // { 0,                                                 XF86XK_Calculator,          spawn,          SHCMD("$CALCULATOR") },
    { 0,                                                 XKB_KEY_Print,              spawn,          SHCMD("screenshot") },
    { WLR_MODIFIER_SHIFT,                                XKB_KEY_Print,              spawn,          SHCMD("screenshot select") },
    { WLR_MODIFIER_CTRL,                                 XKB_KEY_Print,              spawn,          SHCMD("screenshot pixel") },
                                                                                                                                             
    /* -------- vt -------- */
    CHVT(1),
    CHVT(2),
    CHVT(3),
    CHVT(4),
    CHVT(5),
    CHVT(6),
    CHVT(7),
    CHVT(8),
    CHVT(9),
    CHVT(10),
    CHVT(11),
    CHVT(12),
};

