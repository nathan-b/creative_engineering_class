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

const int potpin = 0;
const int micpin = 1;

void set_leds(int max_high) {
  for (int i = 0; i < num_leds; ++i) {
    if (i < max_high) digitalWrite(led_pins[i], HIGH);
    else digitalWrite(led_pins[i], LOW);
  }
}

// the setup function runs once when you press reset or power the board
void setup() {
  for (int i = 0; i < num_leds; ++i) {
    pinMode(led_pins[i], OUTPUT);
  }
}

// the loop function runs over and over again forever
void loop() {
  //int val = analogRead(potpin);
  int val = analogRead(micpin);

  if (val > 700) val = 700;

  val = map(val, 0, 700, 0, num_leds);
  set_leds(val);

  delay(10);
}
