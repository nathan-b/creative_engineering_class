#include <FastLED.h>

const int num_leds = 16;
CRGB leds[num_leds];

const int micpin = 4; // Analog
const int led_pin = 10;

const int nsmooth = 100;
int smooth_buf[nsmooth] = {};
int smoothp = 0;
int smooth_max = 0;
int smooth_min = 1024;

CRGB color_map(int idx) {
  if (idx < num_leds * 0.25) {
    return CRGB::Red;
  } else if (idx < num_leds * 0.5) {
    return CRGB::Yellow;
  } else if (idx < num_leds * 0.75) {
    return CRGB::Green;
  }
  return CRGB::Blue;
}

void set_leds(int max_high) {
  for (int i = 0; i < num_leds; ++i) {
    if (i > max_high) {
      leds[i] = CRGB::Black;
    } else {
      leds[i] = color_map(i);
    }
  }
}

int get_smooth() {
  long sum = 0;
  for (int i = 0; i < nsmooth; ++i) {
    sum += smooth_buf[i];
  }
  return sum / nsmooth;
}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  FastLED.addLeds<WS2812B, led_pin, GRB>(leds, num_leds);
}

// the loop function runs over and over again forever
void loop() {
  //int val = analogRead(potpin);
  int val = analogRead(micpin);

  Serial.println(val);
  //if (val != 754 && val > 20) Serial.println(val);

  //if (val > 700) val = 700;
  
  //smooth_buf[smoothp++] = val;
  //if (val > smooth_max) smooth_max = val;
  //if (val < smooth_min) smooth_min = val;
  //val = map(val, 0, 700, 0, num_leds);
  //set_leds(val);
#if 0
  if (smoothp == nsmooth) {
    smoothp = 0;
    int v = get_smooth();
    if (v > 0) {
      Serial.println(v);
      /*Serial.print("Min: ");
      Serial.print(smooth_min);
      Serial.print(" Max: ");
      Serial.println(smooth_max);*/
      v = map(v, smooth_min, smooth_max, 0, num_leds);
      //set_leds(get_smooth());
      set_leds(v);
      smooth_min = 1024;
      smooth_max = 0;
    }
  }
#endif
  //Serial.print(":");
  //Serial.println(val);
  //set_leds(val);
  val = map(val, 15, 100, 0, num_leds);
  set_leds(val);
  FastLED.show();
  

  //delay(10);
}
