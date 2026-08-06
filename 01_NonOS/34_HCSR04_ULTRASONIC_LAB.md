# 34. 초음파 거리 센서 — HC-SR04

## 목표

초음파를 쏘고 반사되어 돌아오는 시간을 측정해 거리를 계산합니다. 지금까지 없던 **"펄스 폭 측정"** 패턴을 `pulseIn()`으로 처음 다룹니다.

## 준비물

- HC-SR04 초음파 센서

## 회로

- VCC → 5V(모듈에 따라 3.3V 지원 여부 다름, 데이터시트 확인), GND → GND
- TRIG → GPIO5
- ECHO → GPIO6

> ⚠️ HC-SR04는 ECHO 핀이 5V 로직인 모듈이 많습니다. ESP32-S3의 GPIO는 3.3V 내성이라 5V를 직접 연결하면 손상 위험이 있습니다 — ECHO 라인에 저항 분압(예: 1kΩ+2kΩ)이나 레벨시프터를 거는 걸 권장합니다. 3.3V 호환 모듈이라면 직결 가능합니다.

## 코드

```cpp
#include <Arduino.h>

const int TRIG_PIN = 5;
const int ECHO_PIN = 6;

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  // 10us 펄스를 TRIG에 보내 측정 시작
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // ECHO가 HIGH로 유지되는 시간(us) = 초음파가 왕복한 시간
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);   // 30ms 타임아웃 (약 5m 초과 시 0 반환)

  if (duration == 0) {
    Serial.println("Out of range");
  } else {
    float distanceCm = duration * 0.0343 / 2.0;   // 음속 343m/s -> 0.0343cm/us, 왕복이라 /2
    Serial.printf("Distance: %.1f cm\n", distanceCm);
  }

  delay(200);
}
```

## 실행 & 확인

- 센서 앞에 손이나 물체를 가까이/멀리 움직이며 거리 값이 따라 변하는지 확인

## 관찰 포인트

- `pulseIn()`은 지정한 핀이 HIGH(또는 LOW)로 유지되는 시간을 마이크로초 단위로 재는 함수입니다 — **내부적으로 `pulseIn()`이 반환할 때까지 코드 실행이 멈춥니다(blocking)**. 최대 30ms 정도라 대부분의 경우 문제없지만, 이 시리즈에서 나중에 다룰 FreeRTOS Task 구조에서는 이런 blocking 호출을 어느 Task에 둘지 신중히 설계해야 합니다
- 거리 계산 공식(`duration * 0.0343 / 2.0`)의 `/2`는 초음파가 물체까지 갔다가 반사되어 돌아오는 **왕복** 시간이기 때문입니다
- 타임아웃(`30000`)을 지정하지 않으면, 물체가 감지 범위 밖에 있을 때 `pulseIn()`이 계속 대기하며 멈출 수 있습니다 — 항상 타임아웃을 지정하는 걸 권장합니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 항상 "Out of range" | 배선(TRIG/ECHO가 바뀐 경우 흔함) 확인, 전원 전압 확인 |
| 거리 값이 불안정하게 튐 | 딱딱하지 않은 표면(옷, 스펀지 등)은 초음파가 잘 반사되지 않음 — 딱딱한 평면으로 테스트 |
| 근거리(수 cm 이내)에서 오작동 | HC-SR04는 보통 최소 감지거리(2cm 근처)가 있음 — 데이터시트 확인 |

## 다음

`35_DS18B20_ONEWIRE_LAB.md` — 1-Wire 프로토콜을 쓰는 방수 온도 센서를 다룹니다.
