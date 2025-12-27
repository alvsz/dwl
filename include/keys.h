// clang-format off
static const Key keys[] = {
    /* Note that Shift changes certain key codes: 2 -> at, etc. */
    /* modifier                                      key                  locked     function            argument */
    { MODKEY,                                        XKB_KEY_d,           0,         spawn,              {.v = menucmd} },
    { MODKEY,                                        XKB_KEY_t,           0,         spawn,              {.v = termcmd} },
    { MODKEY | WLR_MODIFIER_SHIFT,                   XKB_KEY_Return,      0,         spawn,              {.v = termcmd} },
    { MODKEY | WLR_MODIFIER_SHIFT,                   XKB_KEY_l,           0,         spawn,              SHCMD("loginctl lock-session") },
                                                                                    
    /* -------- bar -------- */                                                     
    { MODKEY,                                        XKB_KEY_p,           0,         lua_spawn,          LUACMD("toggle_bar()") },
                                                                                    
    { MODKEY,                                        XKB_KEY_q,           0,         killclient,         {0} },
    { MODKEY | WLR_MODIFIER_SHIFT | WLR_MODIFIER_CTRL,       XKB_KEY_q,   0,         quit,               {0} },

    /* -------- layouts -------- */
    { MODKEY,                                       XKB_KEY_u,            0,         setlayout,          {.v = &layouts[0]} },
    { MODKEY,                                       XKB_KEY_i,            0,         setlayout,          {.v = &layouts[1]} },
    { MODKEY,                                       XKB_KEY_o,            0,         setlayout,          {.v = &layouts[2]} },
                                                                                     
    /* -------- scratchpad -------- */                                               
    { MODKEY,                                       XKB_KEY_x,            0,         togglescratch,      {.v = scratchcmd } },
    { MODKEY | WLR_MODIFIER_CTRL,                   XKB_KEY_x,            0,         toggleinscratch,    {.v = scratchcmd } },
    { MODKEY,                                       XKB_KEY_s,            0,         togglescratch,      {.v = scratchspt } },
    { MODKEY | WLR_MODIFIER_CTRL,                   XKB_KEY_s,            0,         toggleinscratch,    {.v = scratchspt } },
    { MODKEY,                                       XKB_KEY_w,            0,         togglescratch,      {.v = scratchpwcalc } },
    { MODKEY | WLR_MODIFIER_CTRL,                   XKB_KEY_w,            0,         toggleinscratch,    {.v = scratchpwcalc } },
    { 0,                                            XF86XK_Calculator,    0,         togglescratch,      {.v = scratchcalc } },
                                                                                     
    /* -------- stack -------- */                                                    
    { MODKEY,                                       XKB_KEY_Return,       0,         zoom,               {0} },
    { MODKEY | WLR_MODIFIER_SHIFT,                  XKB_KEY_Tab,          0,         view,               {0} },
                                                                                     
    { MODKEY,                                       XKB_KEY_Tab,          0,         focusstack,         {.i = +1} },
    { MODKEY | WLR_MODIFIER_SHIFT,                  XKB_KEY_Up,           0,         incnmaster,         {.i = +1} },
    { MODKEY | WLR_MODIFIER_SHIFT,                  XKB_KEY_Down,         0,         incnmaster,         {.i = -1} },
                                                                                     
    { MODKEY,                                       XKB_KEY_Left,         0,         setmfact,           {.f = -0.05} },
    { MODKEY,                                       XKB_KEY_Right,        0,         setmfact,           {.f = +0.05} },
    { MODKEY,                                       XKB_KEY_space,        0,         togglefloating,     {0} },
    { MODKEY,                                       XKB_KEY_F11,          0,         togglefullscreen,   {0} },
                                                                                     
    /* -------- monitores -------- */                                                
    { MODKEY,                                       XKB_KEY_comma,        0,         focusmon,           {.i = WLR_DIRECTION_LEFT} },
    { MODKEY,                                       XKB_KEY_period,       0,         focusmon,           {.i = WLR_DIRECTION_RIGHT} },
    { MODKEY | WLR_MODIFIER_SHIFT,                  XKB_KEY_less,         0,         tagmon,             {.i = WLR_DIRECTION_LEFT} },
    { MODKEY | WLR_MODIFIER_SHIFT,                  XKB_KEY_greater,      0,         tagmon,             {.i = WLR_DIRECTION_RIGHT} },
                                                                                     
    /* -------- gaps -------- */                                                     
    { MODKEY | WLR_MODIFIER_SHIFT,                  XKB_KEY_underscore,   0,         togglegaps,         {0} },
    { MODKEY | WLR_MODIFIER_SHIFT,                  XKB_KEY_plus,         0,         defaultgaps,        {0} },
    { MODKEY,                                       XKB_KEY_minus,        0,         incgaps,            {.i = +1} },
    { MODKEY,                                       XKB_KEY_equal,        0,         incgaps,            {.i = -1} },
    { MODKEY | WLR_MODIFIER_CTRL,                   XKB_KEY_Right,        0,         incovgaps,          {.i = +1} },
    { MODKEY | WLR_MODIFIER_CTRL,                   XKB_KEY_Left,         0,         incovgaps,          {.i = -1} },
    { MODKEY | WLR_MODIFIER_CTRL,                   XKB_KEY_Down,         0,         incohgaps,          {.i = +1} },
    { MODKEY | WLR_MODIFIER_CTRL,                   XKB_KEY_Up,           0,         incohgaps,          {.i = -1} },

    /* -------- tags -------- */
    TAGKEYS(XKB_KEY_1,                                         XKB_KEY_exclam,          0),
    TAGKEYS(XKB_KEY_2,                                         XKB_KEY_at,              1),
    TAGKEYS(XKB_KEY_3,                                         XKB_KEY_numbersign,      2),
    TAGKEYS(XKB_KEY_4,                                         XKB_KEY_dollar,          3),
    TAGKEYS(XKB_KEY_5,                                         XKB_KEY_percent,         4),
    TAGKEYS(XKB_KEY_6,                                         XKB_KEY_dead_diaeresis,  5),
    TAGKEYS(XKB_KEY_7,                                         XKB_KEY_ampersand,       6),

    { MODKEY,                                       XKB_KEY_0,                  0,         view,           {.ui = ~0} },
    { MODKEY | WLR_MODIFIER_SHIFT,                  XKB_KEY_parenright,         0,         tag,            {.ui = ~0} },
                                                                                           
    { MODKEY,                                       XKB_KEY_r,                  0,         spawn,          SHCMD("astal -t Dashboard") },
    { MODKEY,                                       XKB_KEY_c,                  0,         spawn,          SHCMD("astal -t Calendar") },
                                                                                           
    { 0,                                            XF86XK_AudioStop,           1,         spawn,          SHCMD("midia stop") },
    { 0,                                            XF86XK_AudioPrev,           1,         spawn,          SHCMD("midia previous") },
    { 0,                                            XF86XK_AudioPlay,           1,         spawn,          SHCMD("midia play-pause") },
    { 0,                                            XF86XK_AudioNext,           1,         spawn,          SHCMD("midia next") },
    { MODKEY,                                       XKB_KEY_j,                  1,         spawn,          SHCMD("midia previous") },
    { MODKEY,                                       XKB_KEY_k,                  1,         spawn,          SHCMD("midia play-pause") },
    { MODKEY,                                       XKB_KEY_l,                  1,         spawn,          SHCMD("midia next") },
                                                                                           
    { 0,                                            XF86XK_AudioMute,           1,         spawn,          SHCMD("volume mute") },
    { 0,                                            XF86XK_AudioMute,           1,         spawn,          SHCMD("volume mute") },
    { 0,                                            XF86XK_AudioLowerVolume,    1,         spawn,          SHCMD("volume down") },
    { 0,                                            XF86XK_AudioRaiseVolume,    1,         spawn,          SHCMD("volume up") },
    { MODKEY,                                       XKB_KEY_F1,                 1,         spawn,          SHCMD("volume mute") },
    { MODKEY,                                       XKB_KEY_F2,                 1,         spawn,          SHCMD("volume down") },
    { MODKEY,                                       XKB_KEY_F3,                 1,         spawn,          SHCMD("volume up") },	
                                                                                           
    { WLR_MODIFIER_SHIFT,                           XF86XK_AudioMute,           0,         spawn,          SHCMD("volume mic mute") },
    { WLR_MODIFIER_SHIFT,                           XF86XK_AudioLowerVolume,    0,         spawn,          SHCMD("volume mic down") },
    { WLR_MODIFIER_SHIFT,                           XF86XK_AudioRaiseVolume,    0,         spawn,          SHCMD("volume mic up") },
    { MODKEY | WLR_MODIFIER_SHIFT,                  XKB_KEY_F1,                 0,         spawn,          SHCMD("volume mic mute") },
    { MODKEY | WLR_MODIFIER_SHIFT,                  XKB_KEY_F2,                 0,         spawn,          SHCMD("volume mic down") },
    { MODKEY | WLR_MODIFIER_SHIFT,                  XKB_KEY_F3,                 0,         spawn,          SHCMD("volume mic up") },	
                                                                                           
    { 0,                                            XF86XK_MonBrightnessDown,   1,         spawn,          SHCMD("backlight down") },
    { 0,                                            XF86XK_MonBrightnessUp,     1,         spawn,          SHCMD("backlight up") },
    { MODKEY,                                       XKB_KEY_F4,                 1,         spawn,          SHCMD("backlight down") },
    { MODKEY,                                       XKB_KEY_F5,                 1,         spawn,          SHCMD("backlight up") },
                                                                                           
    { 0,                                            XF86XK_Tools,               0,         spawn,          SHCMD("mouse dpi up") },
    { 0,                                            XF86XK_Launch5,             0,         spawn,          SHCMD("mouse dpi down") },
                                                                                           
	/* ------------ apps ----------- */                                                    
    { MODKEY,                                       XKB_KEY_f,                  0,         spawn,          SHCMD("$EXPLORER") },
    { MODKEY,                                       XKB_KEY_b,                  0,         spawn,          SHCMD("$BROWSER") },
    { MODKEY,                                       XKB_KEY_e,                  0,         spawn,          SHCMD("$EMAIL") },
    { 0,                                            XF86XK_Calculator,          0,         spawn,          SHCMD("$CALCULATOR") },
    { 0,                                            XKB_KEY_Print,              0,         spawn,          SHCMD("screenshot") },
    { WLR_MODIFIER_SHIFT,                           XKB_KEY_Print,              0,         spawn,          SHCMD("screenshot select") },
    { WLR_MODIFIER_CTRL,                            XKB_KEY_Print,              0,         spawn,          SHCMD("screenshot pixel") },
                                                                                                                                             
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

