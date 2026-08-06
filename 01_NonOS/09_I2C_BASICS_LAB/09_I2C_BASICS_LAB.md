# 09. I2C 기초 — 버스 스캔(Bus Scanner)

## 목표

I2C 배선이 제대로 됐는지, 연결된 센서의 실제 주소가 무엇인지 확인하는 가장 기본적인 진단 도구를 만듭니다. 이후 I2C 센서 실습(10~16번)을 시작하기 전에 항상 먼저 돌려보는 걸 권장합니다.

## 준비물

- I2C 장치 1개 이상 (센서, OLED 등 — 없어도 배선 문제 진단용으로 실행 가능)

## 회로 (공통)

ESP32-S3 Arduino 코어의 기본 I2C 핀은 **SDA=GPIO8, SCL=GPIO9**입니다.

- 장치 SDA → GPIO8
- 장치 SCL → GPIO9
- 장치 VCC → 3.3V, GND → GND

> 대부분의 I2C 브레이크아웃 보드는 SDA/SCL 라인에 풀업 저항이 이미 내장되어 있습니다. 없는 경우 4.7kΩ 풀업 저항을 SDA, SCL에 각각 추가하세요.

## 코드

```cpp
#include <Arduino.h>
#include <Wire.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();   // 기본 핀: SDA=8, SCL=9
  Serial.println("I2C Scanner starting...");
}

void loop() {
  int deviceCount = 0;

  for (uint8_t address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();

    if (error == 0) {
      Serial.printf("Found device at 0x%02X\n", address);
      deviceCount++;
    }
  }

  Serial.printf("Total: %d device(s)\n\n", deviceCount);
  delay(3000);
}
```

## 실행 & 확인

- 연결한 장치의 I2C 주소가 시리얼 모니터에 출력되는지 확인 (예: OLED SSD1306은 보통 `0x3C`, AHT20은 `0x38`, BME280은 `0x76` 또는 `0x77`)

## 관찰 포인트

- 이 스캐너는 이후 모든 I2C 실습에서 "배선은 맞는데 라이브러리가 왜 안 되지?"를 진단할 때 가장 먼저 돌려볼 도구입니다 — 주소가 안 뜨면 라이브러리 문제가 아니라 배선/전원 문제입니다
- 같은 주소를 쓰는 장치 두 개를 동시에 연결하면 충돌이 납니다 — `16_I2C_MULTI_SENSOR_LAB.md`에서 이 상황을 다룹니다
- 다른 핀을 쓰고 싶다면 `Wire.begin(sda, scl)`처럼 인자를 넘기면 됩니다 (예: `Wire.begin(4, 5)`)

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 아무 장치도 안 뜸 | SDA/SCL 배선 확인, 전원(VCC/GND) 확인, 풀업 저항 확인 |
| 예상과 다른 주소가 뜸 | 일부 센서는 핀 점퍼/저항으로 주소를 바꿀 수 있음 — 데이터시트 확인 |
| 여러 개의 알 수 없는 주소가 뜸 | 배선 접촉 불량으로 인한 노이즈 — 점퍼선을 다시 꽂아보기 |

## 다음

`10_I2C_TEMP_HUMIDITY_LAB.md` — AHT20 온습도 센서를 다룹니다.
