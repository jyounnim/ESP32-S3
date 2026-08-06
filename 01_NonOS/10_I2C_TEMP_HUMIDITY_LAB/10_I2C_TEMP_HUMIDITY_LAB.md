# 10. 온습도 센서 — AHT20

## 목표

I2C 온습도 센서 AHT20(또는 호환 AHT21)에서 온도와 습도를 읽습니다.

## 준비물

- AHT20 모듈 (I2C 주소 보통 `0x38`)

## 회로

- `09`와 동일 (SDA=GPIO8, SCL=GPIO9)
- 먼저 `09_I2C_BASICS_LAB.md`의 스캐너로 `0x38`이 뜨는지 확인하고 진행하세요

## 라이브러리 설치

`platformio.ini`에 추가:

```ini
lib_deps =
    adafruit/Adafruit AHTX0 @ ^2.0.5
```

## 코드

```cpp
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>

Adafruit_AHTX0 aht;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();

  if (!aht.begin()) {
    Serial.println("AHT20 not found - check wiring");
    while (1) delay(1000);
  }
  Serial.println("AHT20 initialized");
}

void loop() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);   // 한 번 호출로 온도/습도 둘 다 갱신됨

  Serial.printf("Temperature: %.2f C, Humidity: %.2f %%\n",
                temp.temperature, humidity.relative_humidity);
  delay(1000);
}
```

## 실행 & 확인

- 1초마다 온도(°C)와 습도(%)가 출력되는지 확인
- 손으로 센서를 감싸쥐거나 입김을 불어 값이 변하는지 확인

## 관찰 포인트

- `aht.getEvent(&humidity, &temp)` 하나의 호출로 두 값이 동시에 갱신됩니다 — 이 라이브러리는 내부적으로 Adafruit의 통일된 센서 인터페이스(`sensors_event_t`)를 쓰기 때문에, 다른 Adafruit 센서 라이브러리(BME280 등)와 코드 패턴이 유사합니다
- `aht.begin()`이 실패하면 `while(1)`로 멈추게 했습니다 — 센서 없이 코드가 계속 도는 것보다, 초기화 실패를 명확히 알리고 멈추는 게 디버깅에 유리합니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| `AHT20 not found` | `09`의 스캐너로 `0x38` 주소가 뜨는지 먼저 확인 |
| 값이 이상하게 고정됨 | 센서 초기화 직후 첫 몇 번의 읽기는 불안정할 수 있음 — `delay(1000)` 이후 안정화 확인 |

## 다음

`11_I2C_PRESSURE_LAB.md` — 기압까지 함께 측정하는 BME280을 다룹니다.
