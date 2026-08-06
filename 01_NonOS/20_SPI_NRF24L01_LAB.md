# 20. NRF24L01 무선 모듈 송수신

## 목표

2.4GHz 무선 모듈 NRF24L01로 두 개의 ESP32-S3 보드 간에 데이터를 무선으로 주고받습니다. Wi-Fi/BLE보다 훨씬 단순하고 전력 소모가 적은 근거리 무선 통신입니다.

## 준비물

- **ESP32-S3 보드 2개 + NRF24L01 모듈 2개** (송신용 1세트, 수신용 1세트)

## 회로 (양쪽 보드 동일)

- SCK → GPIO12, MOSI → GPIO11, MISO → GPIO13
- CE → GPIO4
- CSN → GPIO10
- VCC → **3.3V** (5V 연결 시 모듈 손상 — NRF24L01은 3.3V 전용), GND → GND

> ⚠️ NRF24L01은 순간 전류 소모가 커서 ESP32의 3.3V 핀에서 바로 공급하면 불안정할 수 있습니다. 동작이 불안정하면 VCC-GND 사이에 10~100µF 커패시터를 추가해보세요.

## 라이브러리 설치

```ini
lib_deps =
    nrf24/RF24 @ ^1.4.9
```

## 코드 — 송신 측 (Transmitter)

```cpp
#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>

#define CE_PIN 4
#define CSN_PIN 10

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "00001";

void setup() {
  Serial.begin(115200);
  delay(1000);

  if (!radio.begin()) {
    Serial.println("NRF24L01 not found - check wiring");
    while (1) delay(1000);
  }

  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);   // 실습용은 LOW로 충분, 거리 필요시 HIGH
  radio.stopListening();

  Serial.println("Transmitter ready");
}

void loop() {
  static int counter = 0;
  counter++;

  bool success = radio.write(&counter, sizeof(counter));
  Serial.printf("Sent: %d, success: %s\n", counter, success ? "yes" : "no");

  delay(1000);
}
```

## 코드 — 수신 측 (Receiver)

```cpp
#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>

#define CE_PIN 4
#define CSN_PIN 10

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "00001";

void setup() {
  Serial.begin(115200);
  delay(1000);

  if (!radio.begin()) {
    Serial.println("NRF24L01 not found - check wiring");
    while (1) delay(1000);
  }

  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();

  Serial.println("Receiver ready");
}

void loop() {
  if (radio.available()) {
    int received;
    radio.read(&received, sizeof(received));
    Serial.printf("Received: %d\n", received);
  }
}
```

## 실행 & 확인

- 송신 측 보드에 첫 번째 코드, 수신 측 보드에 두 번째 코드를 각각 업로드
- 송신 측이 1초마다 카운터를 보내고, 수신 측이 같은 값을 받는지 양쪽 시리얼 모니터로 확인

## 관찰 포인트

- `address`(`"00001"`)는 두 보드가 서로를 찾는 "채널 주소"입니다 — 양쪽 코드에서 반드시 동일해야 합니다
- `radio.write()`의 반환값(`success`)으로 실제 상대방이 수신을 확인(ACK)했는지 알 수 있습니다 — 상대 보드의 전원이 꺼져 있으면 `false`가 나옵니다
- `setPALevel`로 송신 출력을 조절합니다 — `RF24_PA_LOW`는 근거리(같은 방 안), `RF24_PA_HIGH`/`RF24_PA_MAX`는 더 먼 거리지만 전력 소모가 늘어납니다
- Wi-Fi/BLE와 달리 NRF24L01은 페어링이나 프로토콜 스택 오버헤드가 거의 없어서, 초저지연·초저전력이 필요한 간단한 원격 제어(RC카, 무선 센서 등)에 여전히 많이 쓰입니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| `NRF24L01 not found` | SPI 배선 확인, CE/CSN 핀 확인, 전원(3.3V 안정성) 확인 |
| `success: no`만 계속 나옴 | 수신 측 전원 확인, `address` 값이 양쪽에서 동일한지 확인, 두 모듈 간 거리를 좁혀서 재시도 |
| 가끔씩만 수신됨 | 전원 불안정 (커패시터 추가), 또는 2.4GHz Wi-Fi 채널과의 간섭 |

## 다음

`21_7SEGMENT_74HC595_LAB.md`부터 디스플레이/출력 장치를 다룹니다.
