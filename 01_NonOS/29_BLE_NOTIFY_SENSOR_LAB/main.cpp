// Source: 29_BLE_NOTIFY_SENSOR_LAB.md
// Section: 코드

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <NimBLEDevice.h>

#define SERVICE_UUID        "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID "12345678-1234-1234-1234-1234567890ad"

Adafruit_AHTX0 aht;
NimBLECharacteristic *pCharacteristic;
NimBLEServer *pServer;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();

  if (!aht.begin()) {
    Serial.println("AHT20 not found");
  }

  NimBLEDevice::init("ESP32-S3-Sensor");
  pServer = NimBLEDevice::createServer();
  NimBLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
  );

  pService->start();

  NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("Advertising as 'ESP32-S3-Sensor'");
}

void loop() {
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 2000) {
    lastSend = millis();

    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);

    char payload[32];
    snprintf(payload, sizeof(payload), "%.1f,%.1f", temp.temperature, humidity.relative_humidity);
    pCharacteristic->setValue(payload);

    if (pServer->getConnectedCount() > 0) {
      pCharacteristic->notify();
      Serial.printf("Notified: %s\n", payload);
    }
  }
}
