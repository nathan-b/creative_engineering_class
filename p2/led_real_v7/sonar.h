
#define TIMEOUT_US 200000

inline uint32_t ping(int trg_pin, int echo_pin, long timeout = TIMEOUT_US) {
  // Trigger for 10 us
  digitalWrite(trg_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trg_pin, LOW);
  
  // Reads the sound wave duration in us (200ms timeout)
  return pulseIn(echo_pin, HIGH, timeout);
}

uint16_t distance_cm(int trg_pin, int echo_pin) {
  uint32_t duration = ping(trg_pin, echo_pin);
  
  // Calculate distance based on duration
  //  Speed of sound at sea level = 340 m/s = 0.034 cm/us
  //  Centimeters traveled (cm) = time in microseconds * 0.034 cm/us
  //  Distance of object = cm / 2 (there and back)
  return duration * 0.034 / 2;
}
