// Source: 22_74HC595_MULTI_LED_LAB.md
// Section: 코드

#include <Arduino.h>

const int DATA_PIN = 4;
const int CLOCK_PIN = 5;
const int LATCH_PIN = 6;

void writePattern(byte pattern) {
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, pattern);
  digitalWrite(LATCH_PIN, HIGH);
}

void setup() {
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
}

void loop() {
  for (int i = 0; i < 8; i++) {
    writePattern(1 << i);
    delay(100);
  }
  for (int i = 7; i >= 0; i--) {
    writePattern(1 << i);
    delay(100);
  }
}
