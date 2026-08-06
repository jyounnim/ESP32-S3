// Source: 28_BLE_GATT_SERVER_LAB.md
// Section: 코드

#include <Arduino.h>
#include <NimBLEDevice.h>

#define SERVICE_UUID        "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID "12345678-1234-1234-1234-1234567890ac"

void setup() {
  Serial.begin(115200);
  delay(1000);

  NimBLEDevice::init("ESP32-S3-Hello");

  NimBLEServer *pServer = NimBLEDevice::createServer();
  NimBLEService *pService = pServer->createService(SERVICE_UUID);

  NimBLECharacteristic *pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      NIMBLE_PROPERTY::READ
  );
  pCharacteristic->setValue("Hello, BLE!");

  pService->start();

  NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("BLE GATT server started, advertising as 'ESP32-S3-Hello'");
}

void loop() {
  delay(1000);
}
