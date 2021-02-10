#include <FastLED.h>
#include "led.h"

#define BRIGHTNESS 0x80

const int led_pin = 10;
const int num_leds = 60;
CRGB leds[num_leds];

unsigned long prev_time;
unsigned long interval_ms = 10;

int increment = 0x08;

ledinfo info = {{0x80, UP}, {0, NONE}, {0x80, DOWN}};

const int ivals[] = {678,557,329,83,32,374,217,55,136,505,628,204,420,106,65,547,51,376,494,464,275,130,215,282,660,607,45,381,19,225,528,304,693,80,90,316,333,570,119,555,163,563,496,247,567,318,268,576,380,91,180,448,151,336,241,118,911,379,597,814,517,606,893,311,909,501,39,747,511,122,502,223,792,393,503,815,467,818,430,777,428,293,684,742,678,972,830,835,770,192,555,860,940,837,356,766,782,772,954,688,707,234,940,802,598,576,157,649,89,997,798,268,796,997,981,564,930,981,905,26,542,723,829,161,527,947,579,277,620,890,144,139,891,676,870,583,175,131,925,912,905,267,462,301,839,991,701,976,758,693,116,57,406,404,605,356,485,570,288,116,140,240,279,70,390,9,598,435,319,200,148,357,449,583,262,439,453,511,207,436,95,265,454,96,348,458,417,73,509,585,622,459,118,593,637,289,234,494,14,108,438,390,572,30,396,474,613,527,505,492,635,377,415,519,673,326,149,643,547,566,153,348,435,29,306};
const int nvals = sizeof(ivals) / sizeof(ivals[0]);
int iptr = 0;

int get_dynamic_value() {
  if (iptr == nvals) {
    iptr = 0;
  }
  return ivals[iptr++];
}

void set_info(ledinfo& info) {
  info.r.color = get_dynamic_value() % 0xff;
  info.r.dir = UP;
  info.g.color = 0;
  info.g.color = NONE;
  info.b.color = get_dynamic_value() % 0xff;
  info.b.color = DOWN;
}

bool shift_one(ledval& l) {
  bool ret = false;
  if (l.dir == UP) {
    if (l.color + increment > 0xff) {
      l.dir = DOWN;
    } else {
      l.color += increment;
    }
  } else if (l.dir == DOWN) {
    if ((int)l.color - increment < 0) {
      ret = true;
      l.color = 0;
      l.dir = NONE;
    } else {
      l.color -= increment;
    }
  }
  return ret;
}

void bump_none() {
  if (info.r.dir == NONE) {
    info.r.dir = UP;
  }
  if (info.g.dir == NONE) {
    info.g.dir = UP;
  }
  if (info.b.dir == NONE) {
    info.b.dir = UP;
  }
}

void led_shift(ledinfo& info) {
  /*
   * - When an UP maxes out, it becomes DOWN
   * - When a DOWN mins out, it becomes NONE and all NONEs become UP
   */
   bool r = shift_one(info.r),
        g = shift_one(info.g),
        b = shift_one(info.b);
   if (r || g || b) {
    bump_none();
   }
   if (r) {
    info.r.dir = NONE;
   }
   if (g) {
    info.g.dir = NONE;
   }
   if (b) {
    info.b.dir = NONE;
   }
}

void print_color(const ledinfo& c) {
  Serial.print("<");
  Serial.print(c.r.color);
  Serial.print(", ");
  Serial.print(c.g.color);
  Serial.print(", ");
  Serial.print(c.b.color);
  Serial.print(">");
}


void increment_colors(int seed_len) {
  for (int i = 0; i < num_leds; ++i) {
    if (i % seed_len == 0) {
      set_info(info);
    }
    Serial.print("Setting ");
    Serial.print(i);
    Serial.print(" to ");
    print_color(info);
    Serial.print("\n");
    leds[i] = CRGB(info.r.color, info.g.color, info.b.color);
    led_shift(info);
    FastLED.show();
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(led_pin, OUTPUT);
  FastLED.addLeds<WS2812B, led_pin, GRB>(leds, num_leds);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  unsigned long current_time = millis();
  
  if (current_time - prev_time >= interval_ms) {
    prev_time = current_time;

    increment_colors(15);
    FastLED.show();
  }
}
