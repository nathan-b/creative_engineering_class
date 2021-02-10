#include <FastLED.h>

const int num_leds = 16;
CRGB leds[num_leds];

const int prpin = 5;
const int led_pin = 10;

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

void setup() {
  Serial.begin(9600);
  pinMode(led_pin, OUTPUT);
  FastLED.addLeds<WS2812B, led_pin, GRB>(leds, num_leds);
}

// the loop function runs over and over again forever
void loop() {
  int min = 20;
  int max = 800;
  int val = analogRead(prpin);
  Serial.print(val);
  Serial.print(" => ");
  if (val > max) {
    val = max;
  }
  if (val < min) {
    min = val;
  }

  Serial.print(val);
  Serial.print(" => ");
  //if (val != 754 && val > 20) Serial.println(val);

  //if (val > 700) val = 700;
  
  
  
  val = map(val, min, max, 0, num_leds);
  set_leds(val);

  Serial.println(val);
  //Serial.print(":");
  //Serial.println(val);
  //set_leds(val);

  //delay(10);
  FastLED.show();
}
