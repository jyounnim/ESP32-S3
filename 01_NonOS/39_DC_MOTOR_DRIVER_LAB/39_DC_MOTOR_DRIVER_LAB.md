# 39. DC 모터 드라이버 (H-bridge)

## 목표

`37`(서보), `38`(스테퍼)에 이어 세 번째 액추에이터 유형인 일반 DC모터를 다룹니다. ESP32의 GPIO는 모터에 직접 충분한 전류를 공급할 수 없어, **모터 드라이버(H-bridge)**를 거쳐 제어합니다.

## 준비물

- DC 모터 1개, L298N류 모터 드라이버 모듈(또는 유사 Motor Driver Shield)
- 모터용 별도 전원(모터 사양에 맞는 전압, 보통 6~12V)

## 회로

- IN1 → GPIO4, IN2 → GPIO5 (회전 방향 결정)
- ENA → GPIO6 (PWM으로 속도 제어)
- 모터 드라이버의 모터 출력(OUT1/OUT2) → DC 모터
- 모터 드라이버의 전원 입력 → 별도 모터 전원(+, GND), **로직 GND는 ESP32 GND와 반드시 공통 연결**

## 코드

```cpp
#include <Arduino.h>

const int IN1_PIN = 4;
const int IN2_PIN = 5;
const int ENA_PIN = 6;
const int PWM_CHANNEL = 0;

void setMotor(int speed) {   // speed: -255(역방향 최대) ~ 255(정방향 최대)
  if (speed > 0) {
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);
  } else if (speed < 0) {
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, HIGH);
  } else {
    digitalWrite(IN1_PIN, LOW);   // 둘 다 LOW = 정지(자유 회전, coast)
    digitalWrite(IN2_PIN, LOW);
  }
  ledcWrite(PWM_CHANNEL, abs(speed));
}

void setup() {
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  ledcSetup(PWM_CHANNEL, 5000, 8);
  ledcAttachPin(ENA_PIN, PWM_CHANNEL);
}

void loop() {
  setMotor(200);    // 정방향, 속도 200/255
  delay(2000);

  setMotor(0);
  delay(1000);

  setMotor(-200);   // 역방향
  delay(2000);

  setMotor(0);
  delay(1000);
}
```

## 실행 & 확인

- 모터가 정방향으로 2초 회전 → 정지 → 역방향으로 2초 회전 → 정지를 반복하는지 확인

## 관찰 포인트

- **IN1/IN2 조합으로 방향을, ENA의 PWM duty로 속도를 결정**하는 게 H-bridge 드라이버의 표준 제어 방식입니다 — `06`에서 배운 PWM이 이번엔 "밝기"가 아니라 "모터 속도"를 표현하는 데 쓰였습니다
- IN1=LOW, IN2=LOW(또는 둘 다 HIGH)는 모터를 정지시키지만 방식이 다릅니다 — 대부분의 드라이버에서 "둘 다 LOW"는 자유 회전 정지(coast, 관성으로 천천히 멈춤), "둘 다 HIGH"는 급정지(brake)에 가깝게 동작합니다. 정확한 동작은 드라이버 칩(L298N, TB6612FNG 등)마다 다르니 데이터시트 확인이 필요합니다
- **모터 전원과 ESP32 전원을 분리**한 이유: 모터는 회전 시작/정지 시 큰 전류 스파이크와 역기전력을 만들어낼 수 있어, ESP32와 같은 전원을 쓰면 ESP32가 리셋되거나 오동작할 위험이 있습니다. GND만 공통으로 연결(신호 기준점 공유)하고 전원 라인은 분리하는 게 표준적인 안전 설계입니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 모터가 안 돎 | 모터 전원 연결 확인, ENA(PWM)가 실제로 신호를 받고 있는지 확인 |
| ESP32가 모터 동작 중 리셋됨 | 전원 분리 미흡 — 모터 전원과 로직 전원을 반드시 분리 |
| 방향이 반대로 동작 | IN1/IN2 배선이 바뀐 경우 — 코드에서 `speed > 0`/`speed < 0` 조건의 HIGH/LOW를 서로 바꿔서 보정 가능 |

## 다음

`40_RFID_RC522_LAB.md` — SPI 기반 RFID 카드 리더를 다룹니다.
