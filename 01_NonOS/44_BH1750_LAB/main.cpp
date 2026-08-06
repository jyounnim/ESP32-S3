// Source: 44_BH1750_LAB.md
// Section: 코드

#include <Arduino.h>
#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();

  if (!lightMeter.begin()) {
    Serial.println("BH1750 not found - check wiring");
    while (1) delay(1000);
  }
}

void loop() {
  float lux = lightMeter.readLightLevel();
  Serial.printf("Light: %.1f lux\n", lux);
  delay(500);
}
