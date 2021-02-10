enum led_dir {
  UP,
  DOWN,
  NONE
};

struct ledval {
  int color;
  led_dir dir;
};

struct ledinfo {
  ledval r, g, b;
};
