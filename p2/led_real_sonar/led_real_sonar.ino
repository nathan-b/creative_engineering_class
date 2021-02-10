#include <FastLED.h>

const int num_leds = 16;
CRGB leds[num_leds];

const int echo_pin = 2;
const int trg_pin = 3;
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

long ping() {
  // Clears the trigger pin
  digitalWrite(trg_pin, LOW);
  delayMicroseconds(2);
  // Trigger for 10 us
  digitalWrite(trg_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trg_pin, LOW);
  
  // Reads the sound wave duration in us
  return pulseIn(echo_pin, HIGH);
}

void setup() {
  Serial.begin(9600);
  pinMode(led_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
  pinMode(trg_pin, OUTPUT);
  FastLED.addLeds<WS2812B, led_pin, GRB>(leds, num_leds);
}

void loop() {
  long duration = ping();
  
  // Calculate distance based on duration
  //  Speed of sound at sea level = 340 m/s = 0.034 cm/us
  //  Centimeters traveled (cm) = time in microseconds * 0.034 cm/us
  //  Distance of object = cm / 2 (there and back)
  int distance = duration * 0.034 / 2;

  Serial.print(duration);
  Serial.print(" => ");
  Serial.print(distance);
  Serial.print(" => ");

  // Max 4 meters
  if (distance > 400) distance = 400;
  int num_lit = map(distance, 0, 400, 0, num_leds);

  Serial.println(num_lit);
  set_leds(num_lit);
  FastLED.show();
}
