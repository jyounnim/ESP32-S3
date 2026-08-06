// Source: 04_GPIO_MULTI_OUTPUT_LAB.md
// Section: 코드

#include <Arduino.h>

const int LED_PINS[] = {4, 5, 6, 7, 15, 16, 17, 18};
const int NUM_LEDS = sizeof(LED_PINS) / sizeof(LED_PINS[0]);

void setup() {
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }
}

void loop() {
  // left to right
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(LED_PINS[i], HIGH);
    delay(80);
    digitalWrite(LED_PINS[i], LOW);
  }
  // right to left
  for (int i = NUM_LEDS - 1; i >= 0; i--) {
    digitalWrite(LED_PINS[i], HIGH);
    delay(80);
    digitalWrite(LED_PINS[i], LOW);
  }
}
