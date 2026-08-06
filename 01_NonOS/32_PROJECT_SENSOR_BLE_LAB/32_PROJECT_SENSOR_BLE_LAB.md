# 32. 종합 프로젝트 — 센서 값 → BLE 전송 + OLED 동시 표시

## 목표

`29`(BLE Notify)와 `31`(OLED 표시)을 합쳐서, 센서 값을 **로컬 화면(OLED)**과 **원격 스마트폰(BLE)** 양쪽에 동시에 내보내는 완결된 미니 IoT 기기를 만듭니다.

## 준비물

- AHT20 모듈, SSD1306 OLED

## 코드

```cpp
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
```

## 실행 & 확인

- OLED에 온도/습도와 BLE 연결 상태("waiting..." / "connected")가 표시되는지 확인
- nRF Connect로 연결하면 OLED의 상태 텍스트가 즉시 "connected"로 바뀌는지 확인
- Notify를 구독하면 스마트폰에서도 같은 값이 실시간으로 갱신되는지 확인

## 관찰 포인트

- `display.print(pServer->getConnectedCount() > 0 ? "..." : "...")`처럼, OLED 화면 자체가 "지금 BLE 클라이언트가 연결되어 있는가"라는 시스템 상태를 보여주는 디버깅 정보를 겸하고 있습니다 — 실제 제품에서도 이런 상태 표시는 매우 유용합니다
- `if (millis() - lastUpdate < 1000) return;`처럼 `loop()` 맨 앞에서 주기를 체크하고 조건에 안 맞으면 즉시 `return`하는 패턴은, 여러 작업(OLED 갱신 + BLE 전송)을 항상 같은 타이밍에 묶어서 실행하고 싶을 때 유용합니다
- 지금까지 만든 것: **로컬 디스플레이 + 무선 전송을 동시에 하는 완결된 IoT 센서 노드**입니다 — 이 구조에 `18`(SD카드)을 추가하면 오프라인 로깅까지, `06`(PWM)을 추가하면 임계값 경고까지 확장할 수 있습니다

## 다음

`33_PROJECT_COMPASS_7SEGMENT_LAB.md` — 나침반 방향을 7-Segment에 표시하는 마지막 종합 프로젝트를 다룹니다.
