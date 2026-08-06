// Source: 30_BLE_SCAN_CLIENT_LAB.md
// Section: 코드

#include <Arduino.h>
#include <NimBLEDevice.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
  NimBLEDevice::init("");   // Central 역할은 이름이 필요 없음
}

void loop() {
  NimBLEScan *pScan = NimBLEDevice::getScan();
  pScan->setActiveScan(true);   // Active scan: 기기 이름까지 요청 (전력 소모는 더 큼)

  Serial.println("Scanning for 5 seconds...");
  NimBLEScanResults results = pScan->getResults(5000, false);

  Serial.printf("Found %d device(s)\n", results.getCount());
  for (int i = 0; i < results.getCount(); i++) {
    NimBLEAdvertisedDevice device = results.getDevice(i);
    String name = device.getName().empty() ? "(no name)" : device.getName().c_str();
    Serial.printf("%2d: %-20s RSSI:%4d  Addr:%s\n",
      i + 1, name.c_str(), device.getRSSI(), device.getAddress().toString().c_str());
  }

  pScan->clearResults();
  Serial.println();
  delay(3000);
}
