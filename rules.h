// clang-format off

static const Rule rules[] = {
	/* app_id                           title                  tags mask   isfloating   nokill   monitor   scratchkey */
  { CLASS_SP,                         TITLE_SP,              0,          1,           0,       -1,       't' },
  { CLASS_SP,                         "spotify",             0,          1,           0,       -1,       's' },
                                                                                               
	{ "qalculate-qt",                   NULL,                  0,          1,           0,       -1,       'c' },
	{ "gtk3-icon-browser",              NULL,                  0,          1,           0,       -1,       'c' },
	{ "org.gnome.Characters",           NULL,                  0,          1,           0,       -1,       'c' },
	{ "eww",                            "eww",                 0,          1,           0,       -1,       'c' },
	{ "ags",                            "ags",                 0,          1,           0,       -1,       'c' },
	{ "gtklock",                        "gtklock",             0,          1,           0,       -1,       'c' },
                                                                                               
  { GNOME_PORTAL,                     NULL,                  0,          1,           0,       -1,       0 },
  { GTK_PORTAL,                       NULL,                  0,          1,           0,       -1,       0 },
	{ "blueberry.py",                   NULL,                  0,          1,           0,       -1,       0 },
	{ "pavucontrol",                    NULL,                  0,          1,           0,       -1,       0 },
	{ "org.kde.kdeconnect-indicator",   NULL,                  0,          1,           0,       -1,       0 },
	{ "org.kde.kdeconnect.daemon",      NULL,                  0,          1,           0,       -1,       0 },
	{ "org.kde.kdeconnect-settings",    NULL,                  0,          1,           0,       -1,       0 },
	{ "org.kde.kdeconnect.handler",     NULL,                  0,          1,           0,       -1,       0 },
	{ "wev",                            "wev",                 0,          1,           0,       -1,       0 },	
                                    
	{ "wlroots",                        NULL,                  0,          0,           1,       -1,       0 },
                                                                                               
  { "Alacritty",                      NULL,                  1,          0,           0,       -1,       0 },
  { "org.gnome.Console",              NULL,                  1,          0,           0,       -1,       0 },
                                                                                               
  { "firefox",                        NULL,                  1 << 1,     0,           0,       -1,       0 },
  { "LibreWolf",                      NULL,                  1 << 1,     0,           0,       -1,       0 },
	{ NULL,                             "Picture-in-Picture",  ~0,         1,           0,       -1,       0 },
	{ NULL,                             "Biblioteca",          0,          1,           0,       -1,       0 },
                                    
  { "org.gnome.Epiphany",             NULL,                  1 << 1,     0,           0,       -1,       0 },
                                                                                               
	{ "Gimp-2.10",                      NULL,                  1 << 2,     0,           0,       -1,       0 },
	{ "calibre-gui",                    NULL,                  1 << 2,     0,           0,       -1,       0 },
	{ "org.pwmt.zathura",               NULL,                  1 << 2,     0,           0,       -1,       0 },
	{ "evince",                         NULL,                  1 << 2,     0,           0,       -1,       0 },
                                                                                               
  { "org.gnome.Nautilus",             NULL,                  1 << 3,     0,           0,       -1,       0 },
  { "com.usebottles.bottles",         NULL,                  1 << 3,     0,           0,       -1,       0 },
                                                                                               
	{ "discord",                        NULL,                  1 << 4,     0,           0,       -1,       0 },
	{ "discord",                        "Discord Updater",     1 << 4,     1,           0,       -1,       0 },
	{ "geary",                          NULL,                  1 << 4,     0,           0,       -1,       0 },
	{ "dev.alextren.Spot",              NULL,                  1 << 4,     0,           0,       -1,       0 },
                                                                                               
	{ NULL,                             "Steam",               1 << 5,     0,           0,       -1,       0 },
	{ "Steam",                          "Steam",               1 << 5,     0,           0,       -1,       0 },
	{ NULL,                             RARE,                  1 << 5,     0,           0,       -1,       0 },
	{ "yuzu",                           NULL,                  1 << 5,     0,           0,       -1,       0 },
	{ "soh.elf",                        NULL,                  1 << 5,     0,           0,       -1,       0 },
                                                                                               
	{ "steam",                          "Lista de amigos",     1 << 5,     1,           0,       -1,       0 },
	{ "steam",                          "Steam  Novidades",    1 << 5,     1,           0,       -1,       0 },
	{ "steam",                          "Ofertas especiais",   1 << 5,     1,           0,       -1,       0 },
	{ PRISM_LAUNCHER,                   NULL,                  1 << 5,     1,           0,       -1,       0 },
	{ NULL,                             "Protontricks",        1 << 5,     1,           0,       -1,       0 },
	{ NULL,                             "Rare - Console",      1 << 5,     1,           0,       -1,       0 },
	{ "net.davidotek.pupgui2",          NULL,                  1 << 5,     1,           0,       -1,       0 },
};
