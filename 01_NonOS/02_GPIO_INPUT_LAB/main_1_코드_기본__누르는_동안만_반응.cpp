// Source: 02_GPIO_INPUT_LAB.md
// Section: 코드 (기본 — 누르는 동안만 반응)

#include <Arduino.h>

const int LED_PIN = 4;
const int BUTTON_PIN = 5;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);   // 안 누르면 HIGH, 누르면 LOW
}

void loop() {
  bool pressed = (digitalRead(BUTTON_PIN) == LOW);
  digitalWrite(LED_PIN, pressed ? HIGH : LOW);
}
