# 11. 기압/온도/습도 센서 — BME280

## 목표

온도·습도뿐 아니라 기압, 그리고 기압 기반 추정 고도까지 측정하는 BME280을 다룹니다.

## 준비물

- BME280 모듈 (I2C 주소 보통 `0x76` 또는 `0x77`)

## 회로

- `09`와 동일 배선

## 라이브러리 설치

```ini
lib_deps =
    adafruit/Adafruit BME280 Library @ ^2.2.4
```

## 코드

```cpp
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();

  bool found = bme.begin(0x76) || bme.begin(0x77);   // 모듈마다 주소가 다를 수 있음
  if (!found) {
    Serial.println("BME280 not found - check wiring/address");
    while (1) delay(1000);
  }
  Serial.println("BME280 initialized");
}

void loop() {
  Serial.printf("Temp: %.2f C, Pressure: %.2f hPa, Humidity: %.2f %%, Altitude: %.2f m\n",
    bme.readTemperature(),
    bme.readPressure() / 100.0F,
    bme.readHumidity(),
    bme.readAltitude(SEALEVELPRESSURE_HPA));
  delay(1000);
}
```

## 실행 & 확인

- 온도/기압/습도/추정고도가 1초마다 출력되는지 확인

## 관찰 포인트

- `bme.begin(0x76) || bme.begin(0x77)`처럼 두 주소를 순서대로 시도하는 패턴을 확인하세요 — 같은 센서라도 모듈 제조사에 따라 주소가 다른 경우가 흔해서, 이렇게 방어적으로 작성하면 재사용성이 좋아집니다
- 고도는 GPS처럼 절대적인 값이 아니라 **현재 기압을 해수면 기압(`SEALEVELPRESSURE_HPA`)과 비교해 추정한 값**입니다 — 날씨(기압 변화)에 따라 같은 위치에서도 값이 달라질 수 있습니다. 정확도가 중요하다면 그 지역의 실시간 해수면 기압으로 보정해야 합니다
- AHT20(`10`)과 BME280을 동시에 연결하면 `16_I2C_MULTI_SENSOR_LAB.md`의 다중 센서 실습으로 바로 이어집니다 — 두 센서의 I2C 주소가 겹치지 않는지 미리 확인해두세요

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| `BME280 not found` | `09`의 스캐너로 실제 주소(`0x76`/`0x77`) 확인 |
| 습도 값이 항상 0 | 저가형 클론 중 BMP280(습도 센서 없음)을 BME280으로 착각해 구매한 경우 — 모듈 각인 확인 |

## 다음

`12_I2C_OLED_TEXT_LAB.md` — SSD1306 OLED에 텍스트를 출력합니다.
