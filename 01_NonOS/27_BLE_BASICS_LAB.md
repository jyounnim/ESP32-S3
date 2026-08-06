# 27. BLE 개념 & 라이브러리 선택

## BLE란

BLE(Bluetooth Low Energy)는 저전력 근거리 무선 통신 규격입니다. Wi-Fi보다 소비 전력이 훨씬 낮고, 스마트폰과 직접 페어링 없이도 통신할 수 있어(GATT 방식) 웨어러블, 센서, IoT 기기에 널리 쓰입니다.

## 핵심 개념

| 용어 | 설명 |
|---|---|
| **GATT** (Generic Attribute Profile) | BLE에서 데이터를 주고받는 표준 구조 — Service와 Characteristic으로 계층화됨 |
| **Service** | 관련된 기능을 묶은 단위 (예: "배터리 서비스", "심박수 서비스") — 고유 UUID로 식별 |
| **Characteristic** | Service 안의 실제 데이터 항목 (예: "배터리 잔량 값") — 마찬가지로 UUID로 식별, Read/Write/Notify 등의 속성을 가짐 |
| **Notify** | 서버가 클라이언트(스마트폰 등)에게 값이 바뀔 때마다 알아서 밀어주는(push) 방식 — 클라이언트가 계속 물어볼 필요 없음 |
| **Peripheral / Central** | ESP32가 "주변장치"(Peripheral, 서버 역할)가 될 수도, "중앙장치"(Central, 스캐너/클라이언트 역할)가 될 수도 있음 |

## 라이브러리 선택 — NimBLE-Arduino 권장

Arduino-ESP32 코어에는 기본 내장된 `BLEDevice.h`(Bluedroid 스택 기반)도 있지만, 이 시리즈는 **NimBLE-Arduino**를 씁니다.

| 항목 | 내장 BLEDevice (Bluedroid) | NimBLE-Arduino |
|---|---|---|
| RAM 사용량 | 상대적으로 큼 | 훨씬 작음 (원래 소형 MCU용으로 설계됨) |
| 안정성/유지보수 | 코어 버전에 따라 변동 있음 | 별도 활발히 유지보수되는 프로젝트 |
| API | `BLEDevice`, `BLEServer` 등 | `NimBLEDevice`, `NimBLEServer` 등 (거의 유사한 이름 체계) |

> 참고: Arduino-ESP32 코어 최신 버전은 내부적으로 NimBLE 스택을 기본으로 채택하는 방향으로 가고 있습니다. 다만 이 문서에서는 코어 버전에 관계없이 일관된 API를 쓰기 위해 별도 라이브러리로 명시적으로 설치합니다.

## 라이브러리 설치

```ini
lib_deps =
    h2zero/NimBLE-Arduino @ ^1.4.1
```

## 테스트 도구 — 스마트폰 앱

BLE 예제를 테스트하려면 스마트폰에 BLE 스캐너 앱이 필요합니다. **nRF Connect for Mobile**(Nordic Semiconductor, iOS/Android 무료)을 권장합니다 — Service/Characteristic 구조를 그대로 보여주고, Notify 구독, 값 Write까지 GUI로 할 수 있어 개발 중 테스트에 매우 유용합니다.

## 다음

`28_BLE_GATT_SERVER_LAB.md` — 첫 GATT 서버("Hello BLE")를 만듭니다.
