# 29. BLE Notify — 센서 값 실시간 전송

## 목표

`10`(AHT20 온습도 센서)의 값을 BLE Notify로 2초마다 연결된 스마트폰에 자동으로 밀어줍니다.

## 준비물

- AHT20 모듈 (`10`과 동일 배선)

## 코드

```cpp
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
```

## 실행 & 확인

1. nRF Connect for Mobile에서 `ESP32-S3-Sensor`에 연결
2. Characteristic `...90ad`에서 Notify 구독(아이콘 클릭, 또는 세 개의 아래 화살표 아이콘)
3. 2초마다 "온도,습도" 형식의 값이 자동으로 갱신되어 표시되는지 확인 (앱을 새로고침하거나 다시 읽을 필요 없음)

## 관찰 포인트

- `pServer->getConnectedCount() > 0`으로 연결된 클라이언트가 있을 때만 `notify()`를 호출합니다 — 연결이 없는데 계속 notify를 시도하면 불필요한 오버헤드가 생깁니다
- payload를 `"22.5,45.3"`처럼 콤마로 구분된 문자열로 보냈습니다 — 실제 제품에서는 이보다 더 구조화된 형식(JSON, 또는 고정 바이트 포맷)을 쓰는 경우가 많지만, BLE 패킷 크기 제한(기본 MTU 20바이트 내외)을 고려해 간결하게 유지하는 게 좋습니다
- `26`(Wi-Fi 웹 대시보드)과 비교해보세요 — Wi-Fi는 브라우저가 주기적으로 값을 요청(polling)했지만, BLE Notify는 서버가 알아서 밀어주는(push) 방식입니다. 전력 소모 측면에서 BLE가 훨씬 유리한 이유 중 하나입니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| Notify가 안 옴 | 앱에서 실제로 구독(subscribe)했는지 확인 — 연결만으로는 자동 구독되지 않음 |
| 값이 이상하게 표시됨 | 앱에서 값 표시 형식을 "TEXT" 또는 "UTF-8"로 바꿔서 확인 (기본이 HEX인 경우가 많음) |

## 다음

`30_BLE_SCAN_CLIENT_LAB.md` — ESP32-S3가 반대로 BLE 스캐너(Central) 역할을 합니다.
