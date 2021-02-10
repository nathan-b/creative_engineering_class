

const int prpin = 0;

const int trigger_val = 10;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  int val = analogRead(prpin);
  
  if (val < trigger_val) {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("dark");
  } else {
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("light");
  }
}
