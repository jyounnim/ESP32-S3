# 28. BLE GATT 서버 — Hello BLE

## 목표

ESP32-S3를 BLE Peripheral(서버)로 동작시켜, Service 하나와 Characteristic 하나를 만들고 스마트폰에서 그 값을 읽어봅니다.

## 코드

```cpp
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
```

## 실행 & 확인

1. 업로드 후 스마트폰의 **nRF Connect for Mobile** 앱 실행
2. Scanner 탭에서 `ESP32-S3-Hello` 검색 → CONNECT
3. Service 목록에서 `12345678-...90ab` Service 확장 → Characteristic `...90ac` 선택
4. Read(↓ 아이콘) 클릭 → `Hello, BLE!` 값이 보이는지 확인

## 관찰 포인트

- **UUID**는 Service/Characteristic을 구분하는 고유 식별자입니다 — 128비트 UUID를 직접 만들 때는 온라인 UUID 생성기를 쓰거나, 표준 규격이 정의한 짧은(16비트) UUID를 재사용할 수도 있습니다. 지금은 임의로 만든 값을 썼습니다
- `pAdvertising->addServiceUUID(SERVICE_UUID)`를 빼면, 광고 패킷에 Service 정보가 안 실려서 스캐너에서 기기 이름은 보이지만 무슨 서비스를 제공하는지는 연결해봐야 알 수 있습니다
- `NIMBLE_PROPERTY::READ`만 지정했기 때문에, 클라이언트는 값을 읽을 수만 있고 쓸 수는 없습니다 — 쓰기까지 허용하려면 `NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE`로 바꾸면 됩니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 스캐너에 기기가 안 보임 | 광고(`pAdvertising->start()`)가 호출됐는지 확인, 스마트폰 블루투스 켜져 있는지 확인 |
| 연결은 되는데 Service가 안 보임 | `pService->start()`를 `createCharacteristic` 이후, 광고 시작 이전에 호출했는지 확인 (순서 중요) |
| 값이 안 읽힘 | `NIMBLE_PROPERTY::READ`가 설정되어 있는지 확인 |

## 다음

`29_BLE_NOTIFY_SENSOR_LAB.md` — 센서 값을 Notify로 실시간 전송합니다.
