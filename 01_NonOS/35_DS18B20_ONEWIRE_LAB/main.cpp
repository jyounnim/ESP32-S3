// Source: 35_DS18B20_ONEWIRE_LAB.md
// Section: 코드

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const int ONE_WIRE_PIN = 4;

OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(115200);
  delay(1000);
  sensors.begin();

  int count = sensors.getDeviceCount();
  Serial.printf("Found %d DS18B20 device(s) on the bus\n", count);
}

void loop() {
  sensors.requestTemperatures();          // 변환 시작 (최대 750ms 소요)
  float tempC = sensors.getTempCByIndex(0);

  if (tempC == DEVICE_DISCONNECTED_C) {
    Serial.println("Sensor disconnected");
  } else {
    Serial.printf("Temperature: %.2f C\n", tempC);
  }

  delay(1000);
}
