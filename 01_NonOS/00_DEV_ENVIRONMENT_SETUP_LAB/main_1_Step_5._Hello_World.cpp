// Source: 00_DEV_ENVIRONMENT_SETUP_LAB.md
// Section: Step 5. Hello World

#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Hello, ESP32-S3!");
}

void loop() {
  Serial.println("Running...");
  delay(1000);
}
