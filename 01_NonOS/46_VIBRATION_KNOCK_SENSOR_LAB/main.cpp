// Source: 46_VIBRATION_KNOCK_SENSOR_LAB.md
// Section: 코드

#include <Arduino.h>

const int SW420_PIN = 5;
const int KNOCK_PIN = 6;

volatile bool vibrationFlag = false;
volatile bool knockFlag = false;
volatile unsigned long lastVibrationIsr = 0;
volatile unsigned long lastKnockIsr = 0;
const unsigned long ISR_DEBOUNCE_MS = 100;

void IRAM_ATTR onVibration() {
  unsigned long now = millis();
  if (now - lastVibrationIsr > ISR_DEBOUNCE_MS) {
    vibrationFlag = true;
    lastVibrationIsr = now;
  }
}

void IRAM_ATTR onKnock() {
  unsigned long now = millis();
  if (now - lastKnockIsr > ISR_DEBOUNCE_MS) {
    knockFlag = true;
    lastKnockIsr = now;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(SW420_PIN, INPUT);
  pinMode(KNOCK_PIN, INPUT);

  attachInterrupt(digitalPinToInterrupt(SW420_PIN), onVibration, RISING);
  attachInterrupt(digitalPinToInterrupt(KNOCK_PIN), onKnock, RISING);

  Serial.println("Ready. Shake the board or knock near the sensor.");
}

void loop() {
  if (vibrationFlag) {
    vibrationFlag = false;
    Serial.println("SW-420: vibration detected");
  }
  if (knockFlag) {
    knockFlag = false;
    Serial.println("Knock sensor: knock detected");
  }
}
