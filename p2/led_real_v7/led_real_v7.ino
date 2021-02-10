#include <FastLED.h>

#include "phototransistor.h"
#include "sonar.h"

#define NUM_LEDS     300
#define VOLTS        5
#define MAX_MA       4000

#define SECONDS_PER_PALETTE  10

#define tmp_pin  0   // Analog, stable (temperature)
#define echo_pin 2
#define trg_pin  3
#define mic_pin  4   // Analog, unstable (microphone)
#define pt_pin   5   // Analog, unstable (light)
#define led_pin  10

enum rgb_color {
  RED,
  GREEN,
  BLUE,
  NONE
};

/*
 * Knobs that we can turn for variability:
 *  - Dominant color: What color shows up the most? 
 *    - Controlled by sonar
 *  - Background color: What color will be the base unlit color for the LEDs?
 *    - Controlled by sonar
 *  - Twinkle speed: Affects the overall speed the display runs at.
 *    - Controlled by sonar
 *  - Twinkle density: How many LEDs are twinkling vs unlit at any time?
 */
 
CRGB background_color = CRGB::Black;
CRGB background_target = CRGB::Black;
rgb_color dominant = NONE;

CRGBArray<NUM_LEDS> leds;
const int8_t increment = 0x8;

// 0 (VERY slow) to 8 (VERY fast).  
uint32_t twinkle_speed = 4;
// 0 (NONE lit) to 8 (ALL lit at once).  
uint32_t twinkle_density = 6;

void print_rgb(int r, int g, int b) {
  Serial.print("<");
  Serial.print(r);
  Serial.print(", ");
  Serial.print(g);
  Serial.print(", ");
  Serial.print(b);
  Serial.print(">");
}

void print_rgb(CRGB rgb) {
  Serial.print("<");
  Serial.print(rgb.r);
  Serial.print(", ");
  Serial.print(rgb.g);
  Serial.print(", ");
  Serial.print(rgb.b);
  Serial.print(">");
}

int get_dynamic_value() {
  uint16_t pn = ping(trg_pin, echo_pin, 10000);
  uint16_t pt = get_raw_pt_value(pt_pin);
  uint16_t mc = analogRead(mic_pin);

  return pt + pn + mc;
}

void get_dynamic_values(int& r, int& g, int& b) {
  r = get_dynamic_value();
  g = get_raw_pt_value(pt_pin);
  b = get_dynamic_value();
}

void increment_color(int& color, int8_t inc = increment) {
  if (color + increment < 0xff) {
    color += increment;
  } else {
    color = 0xff;
  }
}

void decrement_color(int& color, int8_t inc = increment) {
  if ((color - increment > 0) && (color - increment < 0xff)) {
    color -= increment;
  } else {
    color = 0;
  }
}

inline void int_to_rgb(uint32_t rgb, int& r, int& g, int& b) {
  r = (rgb >> 16) & 0xff;
  g = (rgb >> 8) & 0xff;
  b = rgb & 0xff;
}

inline uint32_t rgb_to_int(int r, int g, int b) {
  // Have to be super careful here so we don't get truncated by the compiler
  uint32_t ur, ug, ub;
  ur = r & 0xff;
  ur <<= 16;
  ug = g & 0xff;
  ug <<= 8;
  ub = b & 0xff;
  return ur | ug | ub;
}

void set_dominant(int& r, int& g, int& b) {
  if (dominant == NONE) return;
  rgb_color max_color = NONE;
  int tmp;

  r &= 0xff;
  g &= 0xff;
  b &= 0xff;

  if (r > g) {
    if (r > b) {
      max_color = RED;
    } else {
      max_color = BLUE;
    }
  } else if (b > g) {
    max_color = BLUE;
  } else {
    max_color = GREEN;
  }

  if (max_color == dominant || max_color == NONE) return;

  if (dominant == RED) {
    tmp = r;
    if (max_color == BLUE) {
      r = b;
      b = tmp;
    } else {
      r = g;
      g = tmp;
    }
  } else if (dominant == GREEN) {
    tmp = g;
    if (max_color == BLUE) {
      g = b;
      b = tmp;
    } else {
      g = r;
      r = tmp;
    }
  } else {
    tmp = b;
    if (max_color == GREEN) {
      b = g;
      g = tmp;
    } else {
      b = r;
      r = tmp;
    }
  }
}

uint32_t get_dynamic_color()
{
  int trio[3];
  int r, g, b;
  static uint8_t seed = 0;

  get_dynamic_values(trio[0], trio[1], trio[2]);

  r = trio[++seed %= 3];
  g = trio[++seed %= 3];
  b = trio[++seed %= 3];

  set_dominant(r, g, b);

  return rgb_to_int(r, g, b);
}

void mutate(int& c_in, int8_t inc) {
  int r = random(100);
  if (r < 40) { // 40% chance to increment
    increment_color(c_in, inc);
  } else if (r < 80) { // 40% chance to decrement
    decrement_color(c_in, inc);
  }
}

uint32_t shift_dynamic_color(uint32_t c_in, rgb_color tilt = NONE, int8_t inc = increment)
{
  int r, g, b;
  int_to_rgb(c_in, r, g, b);

  if (tilt == NONE) tilt = dominant;
  
  if (tilt == RED) {
    increment_color(r, inc);
    mutate(g, inc);
    mutate(b, inc);
  } else if (tilt == GREEN) {
    mutate(r, inc);
    increment_color(g, inc);
    mutate(b, inc);
  } else if (tilt == BLUE) {
    mutate(r, inc);
    mutate(g, inc);
    increment_color(b, inc);
  } else {
    mutate(r, inc);
    mutate(g, inc);
    mutate(b, inc);
  }
  return rgb_to_int(r, g, b);
}

#define palette_size 16
CRGB curr_palette[palette_size];
CRGB new_palette[palette_size];

void build_dynamic_palette(CRGB* pal)
{
  /**
   * RED        GREEN        BLUE        RANDOM
   * RED/BLUE   GREEN/RED    BLUE/GREEN  RANDOM
   * SHIFT      SHIFT        SHIFT       SHIFT
   * SHIFT      SHIFT        SHIFT       SHIFT
   */
  uint32_t cb[16];
  for (int i = 0; i < palette_size; ++i) {
    if (i == 0) {
      cb[i] = rgb_to_int(get_dynamic_value(), 0, 0);
    } else if (i == 1) {
      cb[i] = rgb_to_int(0, get_dynamic_value(), 0);
    } else if (i == 2) {
      cb[i] = rgb_to_int(0, 0, get_dynamic_value());
    } else if (i == 3) {
      cb[i] = get_dynamic_color();
    } else if (i == 4) {
      cb[i] = shift_dynamic_color(cb[0], BLUE, 0x40);
    } else if (i == 5) {
      cb[i] = shift_dynamic_color(cb[0], RED, 0x40);
    } else if (i == 6) {
      cb[i] = shift_dynamic_color(cb[0], GREEN, 0x40);
    } else if (i == 8) {
      cb[i] = get_dynamic_color();
    } else {
      cb[i] = shift_dynamic_color(cb[i - 1]);
    }
  }
  for (int i = 0; i < palette_size; ++i) {
    pal[i] = cb[i];
  }
}

void setup() {
  delay(1000);
  
  pinMode(echo_pin, INPUT);
  pinMode(trg_pin, OUTPUT);

  Serial.begin(9600);
  
  FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS, MAX_MA);
  FastLED.addLeds<WS2812B, led_pin, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  build_dynamic_palette(curr_palette);
  memcpy(new_palette, curr_palette, sizeof(new_palette));
  
  randomSeed(analogRead(1));
}


void loop() {
  EVERY_N_SECONDS(SECONDS_PER_PALETTE) {
    build_dynamic_palette(new_palette);
  }
  
  EVERY_N_MILLISECONDS(1) {
    background_color = blend(background_color, background_target, 0x4);
  }

  EVERY_N_MILLISECONDS(10) {
    for (int i = 0; i < palette_size; ++i) {
      curr_palette[i] = blend(curr_palette[i], new_palette[i], 0x4);
    }
  }

  EVERY_N_MILLISECONDS(100) {
    // Check distance and change background color based on reading
    int cm = distance_cm(trg_pin, echo_pin);
    if (cm < 75) { // Something is quite close
      background_target = CRGB(0x10, 0, 0);
      dominant = RED;
      twinkle_speed = 5;
    } else if (cm < 115) {
      dominant = GREEN;
      background_target = CRGB(0, 0x10, 0);
    } else if (cm < 2) {
      dominant = BLUE;
      background_target = CRGB(0, 0, 0x10);
      twinkle_speed = 4;
    } else {
      dominant = NONE;
      background_target = CRGB::Black;
      twinkle_speed = 3;
    }
  }

  drawTwinkles(leds);
  
  FastLED.show();
}

// This function loops over each pixel, calculates the adjusted 'clock'
// that this pixel should use, and calls "CalculateOneTwinkle" on each pixel.
void drawTwinkles(CRGBSet& L) {
  // Use an RNG sequence to get the same clocks for each pixel each time
  uint16_t PRNG16 = 11337;
  uint32_t clock32 = millis();

  CRGB bg = background_color;
  uint8_t backgroundBrightness = bg.getAverageLight();
  
  for (CRGB& pixel: L) {
    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
    uint16_t myclockoffset16 = PRNG16; // use that number as clock offset
    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
    // use that number as clock speed adjustment factor (in 8ths, from 8/8ths to 23/8ths)
    uint8_t myspeedmultiplierQ5_3 =  ((((PRNG16 & 0xFF)>>4) + (PRNG16 & 0x0F)) & 0x0F) + 0x08;
    uint32_t myclock30 = (uint32_t)((clock32 * myspeedmultiplierQ5_3) >> 3) + myclockoffset16;
    uint8_t  myunique8 = PRNG16 >> 8; // get 'salt' value for this pixel

    // We now have the adjusted 'clock' for this pixel, now we call
    // the function that computes what color the pixel should be based
    // on the "brightness = f( time )" idea.
    CRGB c = computeOneTwinkle(myclock30, myunique8);

    uint8_t cbright = c.getAverageLight();
    int16_t deltabright = cbright - backgroundBrightness;
   
    if (deltabright > 0) {
      pixel = c;
    } else {
      pixel = bg;
    }
  }
}


//  This function takes a time in pseudo-milliseconds.
//  The 'low digits' of the millisecond time are used as 
//  input to the brightness wave function.  
//  The 'high digits' are used to select a color, so that the color
//  does not change over the course of the fade-in, fade-out
//  of one cycle of the brightness wave function.
//  The 'high digits' are also used to determine whether this pixel
//  should light at all during this cycle, based on the twinkle_density.
CRGB computeOneTwinkle(uint32_t ms, uint8_t salt) {
  uint16_t ticks = ms >> (8 - twinkle_speed);
  uint8_t fastcycle8 = ticks;
  uint16_t slowcycle16 = (ticks >> 8) + salt;
  slowcycle16 += sin8(slowcycle16);
  slowcycle16 =  (slowcycle16 * 2053) + 1384;
  uint8_t slowcycle8 = (slowcycle16 & 0xFF) + (slowcycle16 >> 8);
  
  uint8_t bright = 0;
  if (((slowcycle8 & 0x0E) / 2) < twinkle_density) {
    bright = attackDecayWave8(fastcycle8);
  }

  uint8_t hue = slowcycle8 - salt;
  CRGB c;
  if (bright > 0) {
    c = curr_palette[hue % 16];
    c.fadeLightBy(bright);
    coolLikeIncandescent(c, fastcycle8);
  } else {
    c = CRGB::Black;
  }
  return c;
}


// This function is like 'triwave8', which produces a 
// symmetrical up-and-down triangle sawtooth waveform, except that this
// function produces a triangle wave with a faster attack and a slower decay:
//
//     / \ 
//    /     \ 
//   /         \ 
//  /             \ 
//

uint8_t attackDecayWave8( uint8_t i) {
  if (i < 86) {
    return i * 3;
  } else {
    i -= 86;
    return 255 - (i + (i / 2));
  }
}

// This function takes a pixel, and if its in the 'fading down'
// part of the cycle, it adjusts the color a little bit like the 
// way that incandescent bulbs fade toward 'red' as they dim.
void coolLikeIncandescent(CRGB& c, uint8_t phase) {
  if (phase < 128) return;

  uint8_t cooling = (phase - 128) >> 4;
  c.g = qsub8(c.g, cooling);
  c.b = qsub8(c.b, cooling * 2);
}
