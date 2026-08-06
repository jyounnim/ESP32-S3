# 45. 가스 센서 — MQ-135 (공기질)

## 목표

MQ 시리즈 가스 센서(반도체식 가스 센서)의 대표로 MQ-135(공기질/암모니아/벤젠 등 복합가스)를 다룹니다. 이 시리즈는 MQ-3(알코올), MQ-7(일산화탄소) 등 감지 대상 가스만 다를 뿐 **회로 구성과 코드는 사실상 동일**합니다.

## 준비물

- MQ-135 모듈 (보통 LM393 비교기가 함께 실장되어 있어 디지털/아날로그 출력 둘 다 나옴)

## 회로

- VCC → 5V (MQ 시리즈는 내부 히터 때문에 보통 5V 필요, 모듈 사양 확인)
- AOUT(아날로그 출력) → GPIO1 (ADC1_CH0)
- GND → GND

## 코드

```cpp
#include <Arduino.h>

const int MQ135_PIN = 1;
const unsigned long WARMUP_MS = 20000;   // 최소 20초, 제대로 하려면 24~48시간 예열 권장(아래 참고)

unsigned long startTime;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Warming up MQ-135 sensor...");
  startTime = millis();
}

void loop() {
  unsigned long elapsed = millis() - startTime;

  if (elapsed < WARMUP_MS) {
    Serial.printf("Warming up... %lus remaining\n", (WARMUP_MS - elapsed) / 1000);
    delay(1000);
    return;
  }

  int raw = analogRead(MQ135_PIN);
  float voltage = raw * (3.3f / 4095.0f);
  Serial.printf("raw=%d  voltage=%.2fV  (higher = more gas detected, relative value)\n", raw, voltage);
  delay(1000);
}
```

## 실행 & 확인

- 최소 20초 예열 후 raw 값이 출력되는지 확인
- 알코올 소독제, 라이터 가스(직접 점화하지 말고 근처에 흘려보내는 정도로) 등을 가까이 대면 raw 값이 올라가는지 확인

## 관찰 포인트

- **정확한 PPM(농도) 값을 얻으려면 별도 캘리브레이션이 필요합니다.** MQ 시리즈는 저항 변화를 이용한 반도체식 센서라, 깨끗한 공기에서의 기준 저항(R0)을 먼저 구하고, 그 비율(Rs/R0)을 데이터시트의 로그 그래프에 대입해야 실제 PPM을 계산할 수 있습니다. 이 실습은 "상대적으로 값이 오르내리는 것"까지만 확인하고, 정밀 PPM 계산은 `MQUnifiedsensor` 같은 전용 라이브러리(캘리브레이션 절차 포함)를 참고하시는 걸 권장합니다
- **예열(warm-up)이 매우 중요합니다.** MQ 시리즈는 내부 히터가 안정된 온도에 도달해야 값이 신뢰할 만해지는데, 데이터시트 기준으로는 초기 사용 전 **24~48시간의 번인(burn-in)**을 권장하는 경우도 많습니다. 이 실습의 20초는 "값이 나오기 시작하는 최소 시간"일 뿐, 정밀 측정용 예열이 아닙니다
- **다른 MQ 센서로 교체**: MQ-3(알코올), MQ-7(일산화탄소) 등은 배선과 코드가 이 실습과 완전히 동일하고, 감지하는 가스 종류와 캘리브레이션 곡선만 다릅니다 — `MQ135_PIN`을 실제 연결한 핀으로 바꾸고 변수명만 취향껏 바꾸면 그대로 재사용 가능합니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 값이 항상 최대/최소 고정 | 전원(5V) 공급 확인, 예열 시간 부족 |
| 값이 너무 민감하게 튐 | 정상적인 특성 — MQ 센서는 원래 노이즈가 있는 편, 이동평균으로 스무딩 권장 |

## 다음

`46_VIBRATION_KNOCK_SENSOR_LAB.md` — 진동/노크 센서를 다룹니다.
