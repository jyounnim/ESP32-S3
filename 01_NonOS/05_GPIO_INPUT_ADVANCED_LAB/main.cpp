// Source: 05_GPIO_INPUT_ADVANCED_LAB.md
// Section: 코드

#include <Arduino.h>

const int BUTTON_PIN = 5;
const unsigned long DEBOUNCE_MS = 30;
const unsigned long LONG_PRESS_MS = 800;
const unsigned long DOUBLE_CLICK_WINDOW_MS = 300;

int lastReading = HIGH;
int stableState = HIGH;
unsigned long lastChangeTime = 0;
unsigned long pressStartTime = 0;
unsigned long lastReleaseTime = 0;
bool waitingForSecondClick = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.println("Ready. Try single click, double click, and long press.");
}

void loop() {
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastReading) {
    lastChangeTime = millis();
  }

  if ((millis() - lastChangeTime) > DEBOUNCE_MS) {
    if (reading != stableState) {
      stableState = reading;

      if (stableState == LOW) {
        pressStartTime = millis();               // press started
      } else {
        unsigned long heldMs = millis() - pressStartTime;   // released

        if (heldMs >= LONG_PRESS_MS) {
          Serial.println("LONG PRESS");
          waitingForSecondClick = false;
        } else if (waitingForSecondClick && (millis() - lastReleaseTime) < DOUBLE_CLICK_WINDOW_MS) {
          Serial.println("DOUBLE CLICK");
          waitingForSecondClick = false;
        } else {
          waitingForSecondClick = true;
          lastReleaseTime = millis();
        }
      }
    }
  }

  // double-click window expired without a second click -> confirm single click
  if (waitingForSecondClick && (millis() - lastReleaseTime) >= DOUBLE_CLICK_WINDOW_MS) {
    Serial.println("SINGLE CLICK");
    waitingForSecondClick = false;
  }

  lastReading = reading;
}
