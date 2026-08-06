// Source: 09_I2C_BASICS_LAB.md
// Section: 코드

#include <Arduino.h>
#include <Wire.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();   // 기본 핀: SDA=8, SCL=9
  Serial.println("I2C Scanner starting...");
}

void loop() {
  int deviceCount = 0;

  for (uint8_t address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();

    if (error == 0) {
      Serial.printf("Found device at 0x%02X\n", address);
      deviceCount++;
    }
  }

  Serial.printf("Total: %d device(s)\n\n", deviceCount);
  delay(3000);
}
