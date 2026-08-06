# 30. BLE Scan/Client — 주변 기기 검색

## 목표

지금까지는 ESP32-S3가 BLE Peripheral(서버)이었다면, 이번엔 반대로 **Central(스캐너)** 역할을 맡아 주변의 BLE 기기를 검색합니다. `28`, `29`에서 만든 서버를 다른 ESP32-S3(또는 스마트폰)에서 이 코드로 검색해볼 수도 있습니다.

## 코드

```cpp
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
```

## 실행 & 확인

- 5초 스캔 후, 주변 BLE 기기 목록(이름, 신호세기, 주소)이 출력되는지 확인
- 다른 보드에서 `28`/`29`의 코드가 돌고 있다면, `ESP32-S3-Hello` 또는 `ESP32-S3-Sensor`라는 이름으로 목록에 잡히는지 확인

## 관찰 포인트

- `setActiveScan(true)`는 광고 패킷을 받는 것에 더해 기기에게 추가 정보(스캔 응답)를 요청합니다 — 기기 이름 등을 얻으려면 보통 필요하지만, 그만큼 전력을 더 씁니다. 이름이 필요 없고 신호만 감지하면 되는 경우 `false`로 두면 더 절전됩니다
- `24`(Wi-Fi 스캔)와 구조적으로 매우 비슷합니다 — "주변 무선 신호를 찾아 목록화"한다는 개념 자체는 Wi-Fi든 BLE든 동일하고, API 이름만 다를 뿐입니다
- 이 스캔 결과에서 원하는 기기(예: 이름이 `ESP32-S3-Sensor`인 것)를 찾아 실제로 연결하고 Characteristic 값을 읽어오는 것까지 하려면 `NimBLEClient`를 만들어 `connect()`해야 합니다 — 이건 Central 역할의 다음 단계(응용 과제)로, 두 ESP32-S3가 스마트폰 없이 서로 직접 통신하는 구조(예: 여러 센서 노드를 하나의 허브가 모으는 구조)에 활용됩니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 아무 기기도 안 잡힘 | 주변에 광고 중인 BLE 기기가 실제로 있는지 확인 — 스마트폰 블루투스도 잡히는지 테스트 |
| 같은 기기가 여러 번 나옴 | 정상 — 광고 패킷을 여러 번 받으면 그때마다 새 항목으로 기록됨. 중복 제거가 필요하면 주소 기준으로 직접 필터링 |

## 다음

`31_PROJECT_SENSOR_OLED_LAB.md`부터 지금까지 배운 걸 조합한 종합 프로젝트를 다룹니다.
