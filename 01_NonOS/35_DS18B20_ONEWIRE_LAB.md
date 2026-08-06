# 35. 1-Wire 온도 센서 — DS18B20

## 목표

I2C, SPI에 이어 **세 번째 통신 방식인 1-Wire**를 다룹니다. 이름 그대로 신호선 딱 하나로 전원(기생 전원 모드 시)과 데이터를 함께 처리하는 프로토콜입니다. 방수 캡슐형이 흔해 온도를 물/외부 환경에서 재는 용도로 인기가 많습니다.

## 준비물

- DS18B20 (방수 캡슐형 또는 TO-92 패키지)
- 4.7kΩ 저항 1개 (풀업용, 대부분 방수형 모듈에는 이미 내장)

## 회로

- VCC → 3.3V, GND → GND
- DATA → GPIO4 (+ DATA와 VCC 사이에 4.7kΩ 풀업 저항, 모듈에 내장되어 있으면 생략)

## 라이브러리 설치

```ini
lib_deps =
    paulstoffregen/OneWire @ ^2.3.8
    milesburton/DallasTemperature @ ^3.11.0
```

## 코드

```cpp
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const int ONE_WIRE_PIN = 4;

OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(115200);
  delay(1000);
  sensors.begin();

  int count = sensors.getDeviceCount();
  Serial.printf("Found %d DS18B20 device(s) on the bus\n", count);
}

void loop() {
  sensors.requestTemperatures();          // 변환 시작 (최대 750ms 소요)
  float tempC = sensors.getTempCByIndex(0);

  if (tempC == DEVICE_DISCONNECTED_C) {
    Serial.println("Sensor disconnected");
  } else {
    Serial.printf("Temperature: %.2f C\n", tempC);
  }

  delay(1000);
}
```

## 실행 & 확인

- `Found N device(s)`가 1 이상으로 나오는지 확인
- 온도 값이 출력되고, 센서를 손으로 잡으면 서서히 올라가는지 확인

## 관찰 포인트

- **1-Wire의 진짜 강점은 여러 센서를 한 선에 데이지체인처럼 매달 수 있다는 점입니다.** 각 DS18B20은 공장에서 부여된 고유 64비트 주소를 갖고 있어서, I2C처럼 주소 충돌 걱정 없이 같은 선에 수십 개를 연결할 수 있습니다. `sensors.getDeviceCount()`로 몇 개가 붙어있는지, `sensors.getAddress()`로 각각의 고유 주소를 확인할 수 있습니다
- `requestTemperatures()`를 호출하면 센서 내부에서 온도 변환이 진행되는데, 기본 해상도(12bit)에서 최대 750ms 정도 걸립니다 — 이 시간 동안 코드가 멈춰있다는 점에 주의하세요 (해상도를 낮추면 더 빨라짐: `sensors.setResolution(9)`)
- I2C/SPI는 마스터가 클럭을 제공하지만, 1-Wire는 클럭 없이 타이밍만으로 통신합니다 — 그만큼 배선 길이나 노이즈에 더 민감할 수 있어, 풀업 저항 값과 배선 품질이 중요합니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| `Found 0 device(s)` | 풀업 저항 확인(DATA-VCC 사이 4.7kΩ), 배선 확인 |
| `DEVICE_DISCONNECTED_C` | 통신 중 오류 — 배선 길이가 길다면 풀업 저항 값을 낮춰보기(예: 2.2kΩ) |
| 값이 항상 85.00 | 센서가 변환을 완료하기 전에 읽은 경우 — `requestTemperatures()` 이후 충분한 대기 시간 확인 |

## 다음

`36_RTC_DS3231_LAB.md` — 배터리로 시간을 유지하는 RTC 모듈을 다룹니다.
