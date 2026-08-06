// Source: 11_I2C_PRESSURE_LAB.md
// Section: 코드

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();

  bool found = bme.begin(0x76) || bme.begin(0x77);   // 모듈마다 주소가 다를 수 있음
  if (!found) {
    Serial.println("BME280 not found - check wiring/address");
    while (1) delay(1000);
  }
  Serial.println("BME280 initialized");
}

void loop() {
  Serial.printf("Temp: %.2f C, Pressure: %.2f hPa, Humidity: %.2f %%, Altitude: %.2f m\n",
    bme.readTemperature(),
    bme.readPressure() / 100.0F,
    bme.readHumidity(),
    bme.readAltitude(SEALEVELPRESSURE_HPA));
  delay(1000);
}
