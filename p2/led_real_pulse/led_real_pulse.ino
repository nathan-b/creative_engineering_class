#include <FastLED.h>

#define BRIGHTNESS 0x80

const int ivals[] = {711,622,647,721,715,624,639,713,721,627,635,711,719,628,636,713,715,632,634,706,728,630,629,697,729,635,622,701,726,631,615,701,722,642,613,690,729,636,602,688,727,641,615,684,734,650,605,685,727,648,606,692,727,1004,1009,1005,1001,1006,1016,1016,1011,1008,1008,1010,1005,1003,1008,1002,1010,1005,1008,1002,1008,1004,1008,1007,1005,1010,1007,1007,1005,1014,1013,1004,1015,1008,1007,1009,1007,1008,1010,1005,1011,1016,1007,1014,1009,1009,1011,1018,1010,1010,1014,1015,1019,1017,1013,1013,1007,1012,1011,1014,1017,1018,1016,1016,1011,1014,1013,1012,1019,1011,1015,1015,1020,1012,1010,1014,1012,1011,1012,1012,1009,1020,1020,1016,1016,1014,1014,1015,1017,1018,1018,1011,1015,1018,1014,1011,1014,720,628,598,676,714,644,592,675,713,637,589,676,721,638,587,680,717,652,597,677,712,653,590,661,718,654,588,665,718,658,584,670,719,664,586,667,724,677,586,664,719,670,589,661,722,675,598,660,728,693,601,660,723,694,602,660,728,686,605,653,723,698,610,649,724,701,612,653,731,703,614,655,724,716,613};
const int nvals = sizeof(ivals) / sizeof(ivals[0]);
int iptr = 0;

const int led_pin = 10;
const int num_leds = 60;
CRGB leds[num_leds];

unsigned long prev_time;
const unsigned long interval_ms = 10;

unsigned int increment = 0x10;

int get_dynamic_value() {
  if (iptr == nvals) {
    iptr = 0;
  }
  return ivals[iptr++];
}

void increment_color(int led_idx) {
  int amount = increment + led_idx;
  if (leds[led_idx].r + amount > 0xff) {
    leds[led_idx].r = 0;
    if (leds[led_idx].g + amount > 0xff) {
      if (leds[led_idx].b + amount > 0xff) {
        leds[led_idx].b = 0;
      } else {
        leds[led_idx].b += amount;
      }
    } else {
      leds[led_idx].g += amount;
    }
  } else {
    leds[led_idx].r += amount;
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(led_pin, OUTPUT);
  FastLED.addLeds<WS2812B, led_pin, GRB>(leds, num_leds);
  FastLED.setBrightness(BRIGHTNESS);
}

int hue = 0;

void loop() {
  unsigned long current_time = millis();
  
  if (current_time - prev_time >= interval_ms) {
    prev_time = current_time;
/*
    for (int i = 0; i < num_leds; ++i) {
      increment_color(i);
    }
    */
    fill_rainbow(&leds[0], num_leds - 1, hue++);
    if (hue == 0xff) {
      hue = 0;
    }
    FastLED.show();
  }
}
