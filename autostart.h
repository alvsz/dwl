// clang-format off
static const char *const autostart[] = {
    "systemctl", "--user", "import-environment", "WAYLAND_DISPLAY", "XDG_CURRENT_DESKTOP", NULL,
    "dbus-update-activation-environment", "--systemd", "WAYLAND_DISPLAY", "XDG_CURRENT_DESKTOP=wlroots", NULL,

    "sh", "-c", "/usr/lib/polkit-gnome/polkit-gnome-authentication-agent-1", NULL,
    "sh", "-c", "/usr/lib/gsd-xsettings", NULL,

    // "fnott", NULL,

    // "ags", NULL,
    "sh", "-c", "while true; do ags & inotifywait -rqe create,modify ~/.config/ags/js; ags -q; wait $!; done", NULL,

    "polychromatic-tray-applet", NULL,
    "openrgb", "--startminimized", NULL,
    // "sh", "-c", "eww daemon", NULL,
    // "eww", "open", "bar-wayland", NULL,

    "systemctl", "--user", "start", "gammastep-indicator", NULL,
    // "systemctl", "--user", "start", "notif-log", NULL,

    "sh", "-c", "flatpak run --command=geary org.gnome.Geary --gapplication-service", NULL,

    // "sh", "-c", "xss-lock -- bloqueador", NULL,
    "sh", "-c", "swayidle -w lock 'bloqueador' timeout 720 'bloqueador' timeout 420 'dpms off' resume 'dpms on'", NULL,

    "sh", "-c", "scratchpad vdirsyncer sync", NULL,

    "waypaper", "--restore", NULL,

    // "swww", "init", NULL,
    // "sh", "-c", "swww img $HOME/Imagens/EOS-SnowCappedMountain02.jpg", NULL,

    "sh", "-c", "/usr/lib/kdeconnectd", NULL,
    "sh", "-c", "sleep 5; kdeconnect-indicator", NULL,
    NULL /* terminate */
};

