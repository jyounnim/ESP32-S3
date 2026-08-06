# 33. 종합 프로젝트 — 나침반 방향 → 7-Segment 표시

## 목표

`14`(QMC5883L 전자 나침반) + `23`(TM1637 7-Segment)을 결합해, 방위각(0~359도)을 실시간으로 4자리 숫자 표시기에 보여주는 간이 디지털 나침반을 만듭니다.

## 준비물

- QMC5883L 모듈, TM1637 4자리 7-Segment 모듈

## 회로

- QMC5883L: SDA=GPIO8, SCL=GPIO9 (`09`/`14`와 동일)
- TM1637: CLK=GPIO5, DIO=GPIO4 (`23`과 동일 — I2C 핀과 겹치지 않음)

## 코드

```cpp
#include <Arduino.h>
#include <Wire.h>
#include <QMC5883LCompass.h>
#include <TM1637Display.h>

#define CLK_PIN 5
#define DIO_PIN 4

QMC5883LCompass compass;
TM1637Display display(CLK_PIN, DIO_PIN);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();

  compass.init();
  // 14번 실습에서 얻은 캘리브레이션 값이 있다면 여기에 추가:
  // compass.setCalibration(xmin, xmax, ymin, ymax, zmin, zmax);

  display.setBrightness(0x0f);
}

void loop() {
  compass.read();
  int azimuth = compass.getAzimuth();
  if (azimuth < 0) azimuth += 360;   // 항상 0~359 범위로 정규화

  display.showNumberDec(azimuth, false);   // 방위각을 그대로 4자리 숫자로 표시

  char direction[3];
  compass.getDirection(direction, azimuth);
  Serial.printf("Azimuth: %d deg (%c%c%c)\n", azimuth, direction[0], direction[1], direction[2]);

  delay(200);
}
```

## 실행 & 확인

- 나침반을 천천히 돌리면 TM1637에 표시되는 숫자(0~359)가 방위각에 맞춰 실시간으로 바뀌는지 확인
- 시리얼 모니터로 숫자와 함께 문자 방위(N, NE, E...)도 함께 확인

## 관찰 포인트

- 서로 다른 통신 방식(I2C: 나침반, 그리고 TM1637 자체 2-wire 프로토콜)의 장치를 같은 스케치 안에서 문제없이 함께 쓸 수 있다는 걸 보여주는 예제입니다 — 각 라이브러리가 자기 핀만 신경 쓰고 서로 간섭하지 않기 때문입니다
- 정확한 방위 표시를 위해서는 `14`에서 다룬 캘리브레이션이 특히 중요합니다 — 캘리브레이션 없이는 숫자가 계속 바뀌어도 실제 방위와 오차가 클 수 있습니다
- 응용 과제: 숫자 대신 `showNumberDecEx()`로 콜론이나 특수 문자를 활용해 "N", "E", "S", "W" 같은 방위 기호를 4자리 세그먼트 조합으로 직접 표현해보세요 (완전한 문자 표현은 세그먼트 폰트를 직접 설계해야 함)

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 숫자가 안 뜸 | TM1637 배선 확인, `23`의 단독 실습으로 먼저 검증 |
| 방위값이 널뛰기함 | 나침반 캘리브레이션 미적용, 주변 금속/자성체 간섭 (`14` 참고) |

---

## 커리큘럼 전체 완료 체크리스트

- [ ] GPIO 8개(01~08) — 출력, 입력, 인터럽트, 다중 제어, PWM, ADC
- [ ] I2C 8개(09~16) — 스캔, 온습도, 기압, OLED 텍스트/그래픽, 나침반, MEMS, 다중센서
- [ ] SPI 4개(17~20) — 기초, SD카드, TFT LCD, NRF24L01
- [ ] 디스플레이 3개(21~23) — 74HC595 7세그먼트/다중LED, TM1637
- [ ] Wi-Fi 3개(24~26) — 스캔, NTP, 웹 대시보드
- [ ] BLE 4개(27~30) — 개념, GATT 서버, Notify, 스캔/클라이언트
- [ ] 종합 3개(31~33) — 센서→OLED, 센서→BLE, 나침반→7세그먼트

수고하셨습니다 — 여기까지가 기본 33개 실습입니다. `34_HCSR04_ULTRASONIC_LAB.md`부터는 보유하신 부품(초음파, 1-Wire 온도, RTC, 모터류, RFID, 추가 디스플레이/센서)을 반영한 확장 실습이 이어집니다.
