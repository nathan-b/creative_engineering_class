/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/

#define DIR_UP 0
#define DIR_DOWN 1

const int num_leds = 8;
const int led_pins[num_leds] = {2, 3, 4, 5, 6, 7, 8, 9};
int led_states[num_leds] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};

int dir = DIR_UP;

unsigned long prev_time = 0;
const long interval_ms = 300;

int get_next_led() {
  for (int i = 0; i < num_leds; ++i) {
    if (dir == DIR_UP && led_states[i] == LOW) return i;
    if (dir == DIR_DOWN && led_states[i] == LOW) return i - 1;
  }
  // Only other option is that the entire strip is full
  dir = DIR_DOWN;
  return num_leds - 1;
}

// the setup function runs once when you press reset or power the board
void setup() {
  for (int i = 0; i < num_leds; ++i) {
    pinMode(led_pins[i], OUTPUT);
  }
}

// the loop function runs over and over again forever
void loop() {
  unsigned long current_time = millis();

  if (current_time - prev_time >= interval_ms) {
    prev_time = current_time;

    int state = LOW;
    int led = get_next_led();
    // Error check
    if (led < 0) return;
    int pin = led_pins[led];
    if (dir == DIR_UP) {
      state = HIGH;
 
      if (led == num_leds - 1) {
        // Switch direction once we reach the end
        dir = DIR_DOWN;
      }
    } else { // DIR_DOWN
      state = LOW;

      if (led == 0) {
        // Switch direction once we reach the beginning
        dir = DIR_UP;
      }
    }

    // Actually set the LED
    led_states[led] = state;
    digitalWrite(pin, state);
  }
}
