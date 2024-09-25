static int
isenabled(const char *val, int def)
{
	return ((def && (!val || !val[0] || (val[0] != '0'))) || (!def && (val && val[0] && (val[0] != '0'))));
}

static void
setclickmethod(struct libinput_device *libinput_device)
{
	const char *val;
	long l;
	char *end = NULL;

	val = getenv("LIBINPUT_DEFAULT_CLICK_METHOD");
	if (!val || !val[0])
		return;

	errno = 0;
	l = strtol(val, &end, 10);
	if (errno || (end && *end))
		return;

	libinput_device_config_click_set_method(libinput_device,
		(enum libinput_config_click_method)l);
}

static void
settap(struct libinput_device *libinput_device)
{
	const char *val;

	val = getenv("LIBINPUT_DEFAULT_TAP");
	if (val) {
		if (!val[0])
			return;

		libinput_device_config_tap_set_enabled(libinput_device,
			isenabled(val, 1) ? LIBINPUT_CONFIG_TAP_ENABLED :
				LIBINPUT_CONFIG_TAP_DISABLED);
	} else if (tap_to_click && libinput_device_config_tap_get_finger_count(libinput_device))
		libinput_device_config_tap_set_enabled(libinput_device,
			LIBINPUT_CONFIG_TAP_ENABLED);
}

static void
settapanddrag(struct libinput_device *libinput_device)
{
	const char *val;

	val = getenv("LIBINPUT_DEFAULT_DRAG");
	if (val && val[0])
		libinput_device_config_tap_set_drag_enabled(libinput_device,
			isenabled(val, 1) ? LIBINPUT_CONFIG_DRAG_ENABLED :
				LIBINPUT_CONFIG_DRAG_DISABLED);
}

static void
setnaturalscroll(struct libinput_device *libinput_device)
{
	const char *val;

	val = getenv("LIBINPUT_DEFAULT_NATURAL_SCROLL");
	if (val && val[0])
		libinput_device_config_scroll_set_natural_scroll_enabled(
			libinput_device, isenabled(val, 0));
	else if (!val && libinput_device_config_scroll_has_natural_scroll(libinput_device))
		libinput_device_config_scroll_set_natural_scroll_enabled(
			libinput_device, natural_scrolling);
}

static void
setaccelprofile(struct libinput_device *libinput_device)
{
	const char *val;
	double profile;
	char *end = NULL;

	val = getenv("LIBINPUT_DEFAULT_ACCELERATION_PROFILE");
	if (!val || !val[0])
		return;

	errno = 0;
	profile = strtod(val, &end);
	if (errno || (end && *end))
		return;

	libinput_device_config_accel_set_profile(libinput_device,
		(enum libinput_config_accel_profile)profile);
}

static void
setaccelspeed(struct libinput_device *libinput_device)
{
	const char *val;
	double accel = 0;
	char *end = NULL;

	val = getenv("LIBINPUT_DEFAULT_ACCELERATION");
	if (!val || !val[0])
		return;

	errno = 0;
	accel = strtod(val, &end);
	if (errno || (end && *end) || (accel < -1) || (accel > 1))
		return;

	libinput_device_config_accel_set_speed(libinput_device, accel);
}

static void
setscrollmethod(struct libinput_device *libinput_device)
{
	const char *val;
	long l;
	char *end = NULL;

	val = getenv("LIBINPUT_DEFAULT_SCROLL_METHOD");
	if (!val || !val[0])
		return;

	errno = 0;
	l = strtol(val, &end, 10);
	if (errno || (end && *end))
		return;

	libinput_device_config_scroll_set_method(libinput_device,
		(enum libinput_config_scroll_method)l);
}

static void
setdwt(struct libinput_device *libinput_device)
{
	const char *val;

	val = getenv("LIBINPUT_DEFAULT_DISABLE_WHILE_TYPING");
	if (val && val[0])
		libinput_device_config_dwt_set_enabled(libinput_device,
			isenabled(val, false) ? LIBINPUT_CONFIG_DWT_ENABLED :
				LIBINPUT_CONFIG_DWT_DISABLED);
}

static void
setmiddleemul(struct libinput_device *libinput_device)
{
	const char *val;

	val = getenv("LIBINPUT_DEFAULT_MIDDLE_EMULATION");
	if (val && val[0])
		libinput_device_config_middle_emulation_set_enabled(libinput_device,
			isenabled(val, false) ? LIBINPUT_CONFIG_MIDDLE_EMULATION_ENABLED :
				LIBINPUT_CONFIG_MIDDLE_EMULATION_DISABLED);
}

static void
setlefthanded(struct libinput_device *libinput_device)
{
	const char *val;

	val = getenv("LIBINPUT_DEFAULT_LEFT_HANDED");
	if (val && val[0])
		libinput_device_config_left_handed_set(libinput_device,
			isenabled(val, 0));
}

static void
inputconfig(struct libinput_device *libinput_device)
{
	setclickmethod(libinput_device);
	settap(libinput_device);
	settapanddrag(libinput_device);
	setnaturalscroll(libinput_device);
	setaccelprofile(libinput_device);
	setaccelspeed(libinput_device);
	setscrollmethod(libinput_device);
	setdwt(libinput_device);
	setmiddleemul(libinput_device);
	setlefthanded(libinput_device);
}

static void
parsecolor(const char *val, float color[4])
{
	uint8_t r, g, b;
	if (sscanf(val, "#%02hhx%02hhx%02hhx", &r, &g, &b) == 3) {
		color[0] = (float)r / 0xFF;
		color[1] = (float)g / 0xFF;
		color[2] = (float)b / 0xFF;
		color[3] = 1.0;
	}
}

static void
loadtheme(void)
{
	const char *val;
	unsigned int tmp;

	val = getenv("DWL_ROOT_COLOR");
	if (val)
		parsecolor(val, rootcolor);

	val = getenv("DWL_BORDER_COLOR");
	if (val)
		parsecolor(val, bordercolor);

	val = getenv("DWL_FOCUS_COLOR");
	if (val)
		parsecolor(val, focuscolor);

	val = getenv("DWL_URGENT_COLOR");
	if (val)
		parsecolor(val, urgentcolor);

	val = getenv("DWL_BORDER");
	if (val && sscanf(val, "%u", &tmp) == 1)
		borderpx = tmp;
}
