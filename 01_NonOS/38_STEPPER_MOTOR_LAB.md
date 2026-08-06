# 38. 스테퍼모터 — 28BYJ-48 + ULN2003

## 목표

서보(`37`)가 "각도 하나를 지정"하는 방식이라면, 스테퍼모터는 **정해진 스텝 시퀀스를 순서대로 밟아가며 회전**하는 완전히 다른 제어 방식입니다. 정밀한 위치/회전수 제어가 필요한 프린터, CNC 등에서 흔히 쓰입니다.

## 준비물

- 28BYJ-48 스테퍼모터 + ULN2003 드라이버 보드 (한 세트로 판매되는 경우가 많음)

## 회로

- ULN2003의 IN1~IN4 → GPIO4, GPIO5, GPIO6, GPIO7
- ULN2003 전원(+) → 5V, GND → GND
- 모터 커넥터는 ULN2003 보드에 직결

## 라이브러리

Arduino 표준 내장 `Stepper` 라이브러리를 그대로 씁니다 (별도 설치 불필요).

## 코드

```cpp
#include <Arduino.h>
#include <Stepper.h>

const int STEPS_PER_REV = 2048;   // 28BYJ-48은 내부 기어비 때문에 한 바퀴에 약 2048스텝

// 주의: IN1, IN3, IN2, IN4 순서 (IN1,IN2,IN3,IN4가 아님 - 아래 관찰 포인트 참고)
Stepper myStepper(STEPS_PER_REV, 4, 6, 5, 7);

void setup() {
  Serial.begin(115200);
  delay(1000);
  myStepper.setSpeed(10);   // RPM (28BYJ-48은 보통 10~15 RPM 권장)
}

void loop() {
  Serial.println("Forward one revolution");
  myStepper.step(STEPS_PER_REV);
  delay(500);

  Serial.println("Backward one revolution");
  myStepper.step(-STEPS_PER_REV);
  delay(500);
}
```

## 실행 & 확인

- 모터가 정확히 한 바퀴 회전한 뒤(육안으로 표시점을 붙여 확인 권장) 반대 방향으로 다시 한 바퀴 도는지 확인

## 관찰 포인트

- **핀 순서가 `IN1, IN3, IN2, IN4`인 이유**: `Stepper` 라이브러리는 두 개의 코일(coil A, coil B)을 번갈아 여자화(energize)하는 순서를 가정하는데, ULN2003 보드의 IN1~IN4 물리적 배치와 실제 코일 쌍의 연결 순서가 다릅니다. 이 순서를 맞추지 않으면 모터가 떨리기만 하고 제대로 돌지 않거나 반대로 도는 등 이상 동작을 합니다 — 스테퍼모터 튜토리얼에서 반복적으로 나오는 흔한 함정입니다
- 서보는 "0~180도" 안에서만 움직일 수 있지만, 스테퍼는 `step()`에 계속 큰 값을 줘서 **여러 바퀴 연속 회전**도 가능합니다 — 회전 범위 제한이 없는 액추에이터가 필요할 때 스테퍼를 선택하는 이유입니다
- `setSpeed()`의 RPM을 너무 높이면 모터가 힘을 못 내고 헛돌 수 있습니다(step을 놓침, 이른바 "steps lost") — 28BYJ-48은 저속·고토크 특성이라 무리하게 빠른 속도를 주지 않는 게 안전합니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 모터가 떨리기만 함 | IN 핀 순서 확인 (`IN1,IN3,IN2,IN4`), 배선 확인 |
| 회전은 하는데 힘이 약함 | 전원 공급 부족 — ULN2003 보드에 별도 5V 전원 공급 확인 |
| 정확한 바퀴 수가 안 맞음 | `STEPS_PER_REV`(2048)가 실제 모터 개체별로 미세하게(약 2038~2048) 다를 수 있음 — 정밀도가 중요하면 실측으로 보정 |

## 다음

`39_DC_MOTOR_DRIVER_LAB.md` — DC모터를 H-bridge 드라이버로 제어합니다.
