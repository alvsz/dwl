#include <libinput.h>

static void parsecolor(const char *val, float color[4]) {
  uint8_t r, g, b;
  if (sscanf(val, "#%02hhx%02hhx%02hhx", &r, &g, &b) == 3) {
    color[0] = (float)r / 0xFF;
    color[1] = (float)g / 0xFF;
    color[2] = (float)b / 0xFF;
    color[3] = 1.0;
  }
}

static void loadtheme(void) {
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
