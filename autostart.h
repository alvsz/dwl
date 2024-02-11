// clang-format off
static const char *const autostart[] = {
    "systemctl", "--user", "import-environment", "WAYLAND_DISPLAY", "XDG_CURRENT_DESKTOP", NULL,
    "dbus-update-activation-environment", "--systemd", "WAYLAND_DISPLAY", "XDG_CURRENT_DESKTOP=wlroots", NULL,
    "sh", "-c", "~/.config/dwl/autostart.sh", NULL,
    NULL /* terminate */
};

