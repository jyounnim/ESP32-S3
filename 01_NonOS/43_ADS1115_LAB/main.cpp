// Source: 43_ADS1115_LAB.md
// Section: 코드

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();

  if (!ads.begin()) {
    Serial.println("ADS1115 not found - check wiring");
    while (1) delay(1000);
  }
}

void loop() {
  int16_t raw = ads.readADC_SingleEnded(0);   // 채널 0 (A0)
  float volts = ads.computeVolts(raw);

  Serial.printf("raw=%d  voltage=%.4fV\n", raw, volts);
  delay(200);
}
