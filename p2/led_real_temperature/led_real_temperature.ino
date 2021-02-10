#include <FastLED.h>

const int num_leds = 16;
CRGB leds[num_leds];

const int tmpin = 0;
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
  float voltage = analogRead(tmpin) * 5.0;  
  voltage /= 1024.0; 
  
  // print out the voltage
  Serial.print(voltage); Serial.println(" volts");
  
  // now print out the temperature
  float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                               //to degrees ((voltage - 500mV) times 100)
  Serial.print(temperatureC); Serial.println(" degrees C");
  //FastLED.show();
}
