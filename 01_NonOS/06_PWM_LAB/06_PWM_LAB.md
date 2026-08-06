# 06. PWM으로 LED 밝기 제어

## 목표

디지털 ON/OFF만 가능한 `digitalWrite`와 달리, PWM(duty cycle 조절)으로 LED 밝기를 아날로그처럼 연속적으로 제어합니다.

## 준비물

- `01`과 동일한 LED 회로 (GPIO4)

## 코드 (core 2.x — `ledcSetup`/`ledcAttachPin` API)

```cpp
#include <Arduino.h>

const int LED_PIN = 4;
const int PWM_CHANNEL = 0;
const int PWM_FREQ = 5000;       // 5kHz
const int PWM_RESOLUTION = 8;    // 8bit -> duty 0~255

void setup() {
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(LED_PIN, PWM_CHANNEL);
}

void loop() {
  for (int duty = 0; duty <= 255; duty++) {
    ledcWrite(PWM_CHANNEL, duty);   // 채널 번호로 씀 (핀 번호 아님)
    delay(10);
  }
  for (int duty = 255; duty >= 0; duty--) {
    ledcWrite(PWM_CHANNEL, duty);
    delay(10);
  }
}
```

## 코드 (core 3.x 이상 — `ledcAttach` 신규 API)

`00_DEV_ENVIRONMENT_SETUP_LAB.md`에서 확인한 코어 버전이 3.x 이상이라면 아래처럼 더 단순하게 작성할 수 있습니다.

```cpp
#include <Arduino.h>

const int LED_PIN = 4;
const int PWM_FREQ = 5000;
const int PWM_RESOLUTION = 8;

void setup() {
  ledcAttach(LED_PIN, PWM_FREQ, PWM_RESOLUTION);   // 채널 관리 불필요, 핀 번호로 바로 사용
}

void loop() {
  for (int duty = 0; duty <= 255; duty++) {
    ledcWrite(LED_PIN, duty);   // 핀 번호로 씀
    delay(10);
  }
  for (int duty = 255; duty >= 0; duty--) {
    ledcWrite(LED_PIN, duty);
    delay(10);
  }
}
```

## 실행 & 확인

- LED가 서서히 밝아졌다 어두워지는 것(fade in/out)을 반복하는지 확인

## 코드 설명

| 구성 | 설명 |
|---|---|
| `PWM_CHANNEL` (2.x) | ESP32는 하드웨어 PWM 채널이 여러 개(보통 0~7 또는 그 이상) — 채널 하나를 지정된 핀에 연결하는 구조 |
| `PWM_RESOLUTION = 8` | duty 값의 범위가 0~255(2^8-1). 값을 늘리면(예: 12bit → 0~4095) 더 세밀한 밝기 제어 가능 |
| `ledcWrite(...)` | 지정한 duty 값으로 PWM 출력 갱신 |

## 관찰 포인트

- `PWM_RESOLUTION`을 12로 바꾸고 duty 범위를 0~4095로 수정해 더 부드러운 페이드를 만들어보세요
- `PWM_FREQ`를 50Hz 정도로 낮춰서 눈에 보이는 깜빡임(flicker)이 생기는지 관찰해보세요

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| `'ledcSetup' was not declared` | core 3.x를 사용 중 — 위 두 번째(신규 API) 코드로 교체 |
| `'ledcAttach' was not declared` | core 2.x를 사용 중 — 위 첫 번째(구 API) 코드 사용 |
| LED가 항상 최대/최소 밝기 | `PWM_RESOLUTION`과 duty 값 범위 불일치 (예: 8bit인데 duty를 4095로 넣음) |

## 다음

`07_ADC_LAB.md` — ADC로 아날로그 입력을 읽는 방법을 다룹니다.
