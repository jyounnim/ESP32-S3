// Source: 02_GPIO_INPUT_LAB.md
// Section: 코드 (디바운싱 — 클릭 횟수 카운트)

#include <Arduino.h>

const int LED_PIN = 4;
const int BUTTON_PIN = 5;
const unsigned long DEBOUNCE_MS = 30;

int lastStableState = HIGH;
int lastReading = HIGH;
unsigned long lastChangeTime = 0;
int clickCount = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastReading) {
    lastChangeTime = millis();
  }

  if ((millis() - lastChangeTime) > DEBOUNCE_MS) {
    if (reading != lastStableState) {
      lastStableState = reading;
      if (lastStableState == LOW) {          // 눌리는 순간(falling edge)만 카운트
        clickCount++;
        Serial.printf("Click count: %d\n", clickCount);
      }
      digitalWrite(LED_PIN, lastStableState == LOW ? HIGH : LOW);
    }
  }

  lastReading = reading;
}
