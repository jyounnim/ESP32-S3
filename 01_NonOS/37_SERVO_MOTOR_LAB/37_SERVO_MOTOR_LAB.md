# 37. 서보모터 제어

## 목표

`06`에서 PWM으로 LED 밝기를 조절했다면, 이번엔 같은 PWM 개념을 **각도 제어**에 씁니다. 서보모터는 duty cycle이 아니라 **펄스 폭(1~2ms)**으로 목표 각도가 정해지는, LED PWM과는 다른 해석 방식을 씁니다.

## 준비물

- SG90 등 소형 서보모터 1개

## 회로

- 서보 신호선(주황/노랑) → GPIO18
- 서보 전원(빨강) → 5V (SG90 기준. USB 전원만으로 여러 개 구동 시 전류 부족 가능 — 별도 5V 전원 권장)
- 서보 GND(갈색/검정) → GND (ESP32 GND와 반드시 공통)

## 라이브러리 설치

ESP32는 표준 Arduino `Servo.h`와 호환되지 않아 전용 라이브러리가 필요합니다.

```ini
lib_deps =
    madhephaestus/ESP32Servo @ ^3.0.5
```

## 코드

```cpp
#include <Arduino.h>
#include <ESP32Servo.h>

Servo myServo;
const int SERVO_PIN = 18;

void setup() {
  Serial.begin(115200);
  delay(1000);

  myServo.attach(SERVO_PIN);   // 내부적으로 LEDC(또는 S3의 경우 MCPWM) 채널을 자동 할당
}

void loop() {
  for (int angle = 0; angle <= 180; angle += 5) {
    myServo.write(angle);
    delay(30);
  }
  delay(500);
  for (int angle = 180; angle >= 0; angle -= 5) {
    myServo.write(angle);
    delay(30);
  }
  delay(500);
}
```

## 실행 & 확인

- 서보가 0도→180도→0도로 부드럽게 왕복하는지 확인

## 관찰 포인트

- `06`의 LED PWM은 "duty가 클수록 밝다"는 단순한 관계였지만, 서보는 **펄스가 HIGH로 유지되는 절대 시간(보통 500~2500µs)**이 각도를 결정합니다 — 같은 "PWM"이라는 이름이지만 신호를 해석하는 방식이 완전히 다릅니다
- `ESP32Servo` 라이브러리는 ESP32-S3에서 **MCPWM**(모터 제어 전용 하드웨어)을 활용해 더 정밀한 타이밍을 제공합니다 — 일반 LEDC 기반 PWM보다 서보 제어에 특화되어 있습니다
- `myServo.attach(pin, min, max)`처럼 최소/최대 펄스폭(µs)을 직접 지정할 수도 있습니다 — 서보 모델마다 실제 0도/180도에 대응하는 펄스폭이 조금씩 달라, 정밀한 제어가 필요하면 데이터시트 값으로 보정하는 게 좋습니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 서보가 떨리기만 하고 안 움직임 | 전원 부족(USB 하나로 여러 서보/다른 부하와 공유) — 별도 5V 전원 공급 권장 |
| 특정 각도 근처에서 멈춤 | `min`/`max` 펄스폭 설정이 실제 서보 사양과 안 맞음 |
| 컴파일 에러 (`Servo.h` 관련) | 표준 Arduino `Servo` 라이브러리가 자동으로 잡힌 경우 — `#include <ESP32Servo.h>`로 명시했는지 확인 |

## 다음

`38_STEPPER_MOTOR_LAB.md` — 스텝 시퀀스로 제어하는 스테퍼모터를 다룹니다.
