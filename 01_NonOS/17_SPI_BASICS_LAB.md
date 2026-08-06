# 17. SPI 기초 — 개념과 배선

## 목표

I2C와는 다른 통신 방식인 SPI의 기본 개념과 배선을 익힙니다. 이후 SD카드(18), TFT LCD(19), NRF24L01(20) 실습의 공통 기반입니다.

## I2C vs SPI

| 항목 | I2C | SPI |
|---|---|---|
| 선 개수 | 2선 (SDA, SCL) | 4선 이상 (MOSI, MISO, SCK, CS) |
| 속도 | 보통 100k~400kHz | 수 MHz~수십 MHz (훨씬 빠름) |
| 여러 장치 연결 | 같은 두 선을 공유, 주소로 구분 | 장치마다 별도의 CS(Chip Select) 선 필요 |
| 대표 용도 | 센서(저속, 짧은 거리) | 디스플레이, SD카드, 무선모듈(고속, 대용량 데이터) |

## 핵심 신호선

| 신호 | 역할 |
|---|---|
| **MOSI** (Master Out Slave In) | 마스터(ESP32)→슬레이브(장치)로 데이터 전송 |
| **MISO** (Master In Slave Out) | 슬레이브→마스터로 데이터 전송 |
| **SCK** (Serial Clock) | 마스터가 생성하는 클럭 신호 |
| **CS/SS** (Chip Select) | "지금부터 이 장치와 통신한다"를 선택하는 신호 — 장치마다 별도 GPIO 필요 |

## ESP32-S3 기본 SPI 핀 (Arduino 코어)

| 신호 | GPIO |
|---|---|
| MOSI | 11 |
| MISO | 13 |
| SCK | 12 |
| CS | 예제마다 임의의 GPIO 지정 (예: GPIO10) |

## 코드 — SPI 초기화 템플릿

```cpp
#include <Arduino.h>
#include <SPI.h>

const int CS_PIN = 10;

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);   // 평소엔 HIGH(비활성) 유지, 통신 시작할 때만 LOW로

  SPI.begin();   // 기본 핀(MOSI=11, MISO=13, SCK=12) 사용
  Serial.println("SPI initialized");
}

void loop() {
  // 통신 예시: CS를 LOW로 내려 장치를 선택한 뒤 데이터 교환, 끝나면 다시 HIGH
  digitalWrite(CS_PIN, LOW);
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  byte response = SPI.transfer(0x00);   // 더미 바이트 전송, 응답 수신
  SPI.endTransaction();
  digitalWrite(CS_PIN, HIGH);

  Serial.printf("Response: 0x%02X\n", response);
  delay(1000);
}
```

## 관찰 포인트

- 여러 SPI 장치를 한 버스에 연결할 때는 **MOSI/MISO/SCK는 공유하고, CS만 장치별로 다른 GPIO**를 씁니다 — I2C가 주소로 장치를 구분한다면, SPI는 CS 신호로 "지금 통신할 장치"를 물리적으로 선택합니다
- `SPISettings(속도, 비트순서, 모드)`에서 모드(0~3)는 클럭 극성/위상 조합입니다 — 장치 데이터시트에 명시된 모드와 다르면 통신이 안 되거나 깨진 데이터가 옵니다. 대부분의 모듈은 `SPI_MODE0`을 씁니다
- CS를 평소 HIGH로 유지하다가 통신 순간에만 LOW로 내리는 패턴은 SPI 코드 어디서나 반복되는 관용구입니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 응답이 항상 0x00 또는 0xFF | 배선(MOSI/MISO가 반대로 연결된 경우 흔함), CS 타이밍 확인 |
| 간헐적으로 통신 실패 | SPI 클럭 속도가 배선 길이에 비해 너무 빠름 — 속도를 낮춰서(예: 1MHz) 재시도 |

## 다음

`18_SPI_SDCARD_LAB.md` — MicroSD 카드 읽기/쓰기를 다룹니다.
