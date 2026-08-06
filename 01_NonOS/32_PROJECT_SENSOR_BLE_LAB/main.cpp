// Source: 32_PROJECT_SENSOR_BLE_LAB.md
// Section: 코드

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <NimBLEDevice.h>

#define SERVICE_UUID        "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID "12345678-1234-1234-1234-1234567890ad"

Adafruit_AHTX0 aht;
Adafruit_SSD1306 display(128, 64, &Wire, -1);
NimBLECharacteristic *pCharacteristic;
NimBLEServer *pServer;

void setupBLE() {
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
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();

  if (!aht.begin()) Serial.println("AHT20 init failed");
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) Serial.println("OLED init failed");
  display.setTextColor(SSD1306_WHITE);

  setupBLE();
  Serial.println("Ready - advertising as 'ESP32-S3-Sensor'");
}

void loop() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 1000) return;
  lastUpdate = millis();

  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  // 1) OLED 갱신
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.printf("%.1fC", temp.temperature);
  display.setCursor(0, 24);
  display.printf("%.0f%%", humidity.relative_humidity);
  display.setTextSize(1);
  display.setCursor(0, 50);
  display.print(pServer->getConnectedCount() > 0 ? "BLE: connected" : "BLE: waiting...");
  display.display();

  // 2) BLE Notify
  char payload[32];
  snprintf(payload, sizeof(payload), "%.1f,%.1f", temp.temperature, humidity.relative_humidity);
  pCharacteristic->setValue(payload);
  if (pServer->getConnectedCount() > 0) {
    pCharacteristic->notify();
  }

  Serial.printf("Temp:%.1fC Humidity:%.1f%% (BLE clients: %d)\n",
                temp.temperature, humidity.relative_humidity, pServer->getConnectedCount());
}
