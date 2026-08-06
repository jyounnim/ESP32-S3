# 02. GPIO 입력 — 버튼 입력 + 디바운싱

## 목표

버튼 입력을 읽고, 클릭 횟수를 정확히 세기 위한 소프트웨어 디바운싱을 적용합니다.

## 준비물

- 택트 스위치(버튼) 1개, `01`의 LED 회로

## 회로

- 버튼 한쪽 다리 → GPIO5
- 버튼 다른쪽 다리 → GND
- `INPUT_PULLUP` 사용 — 외부 저항 불필요

## 코드 (기본 — 누르는 동안만 반응)

```cpp
#include <Arduino.h>

const int LED_PIN = 4;
const int BUTTON_PIN = 5;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);   // 안 누르면 HIGH, 누르면 LOW
}

void loop() {
  bool pressed = (digitalRead(BUTTON_PIN) == LOW);
  digitalWrite(LED_PIN, pressed ? HIGH : LOW);
}
```

## 코드 (디바운싱 — 클릭 횟수 카운트)

물리 버튼은 눌리는 순간 신호가 짧게 여러 번 튀는(chattering) 현상이 있어, 정확한 클릭 횟수를 세려면 디바운싱이 필요합니다.

```cpp
#include <Arduino.h>

const int LED_PIN = 4;
const int BUTTON_PIN = 5;
const unsigned long DEBOUNCE_MS = 30;

int lastStableState = HIGH;
int lastReading = HIGH;
unsigned long lastChangeTime = 0;
int clickCount = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastReading) {
    lastChangeTime = millis();
  }

  if ((millis() - lastChangeTime) > DEBOUNCE_MS) {
    if (reading != lastStableState) {
      lastStableState = reading;
      if (lastStableState == LOW) {          // 눌리는 순간(falling edge)만 카운트
        clickCount++;
        Serial.printf("Click count: %d\n", clickCount);
      }
      digitalWrite(LED_PIN, lastStableState == LOW ? HIGH : LOW);
    }
  }

  lastReading = reading;
}
```

## 실행 & 확인

- 버튼을 여러 번 눌러 시리얼 모니터에 클릭 횟수가 정확히 증가하는지 확인

## 관찰 포인트

- `DEBOUNCE_MS`를 0으로 낮춰 채터링 현상을 직접 재현해보세요
- 여기서 쓴 `lastReading`(raw 감지용)과 `lastStableState`(확정 상태) 두 변수 분리 패턴은 이후 실습에서도 계속 재사용됩니다 — 하나의 변수로 겸하면 토글이 안 되는 버그가 생기니 주의하세요

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 버튼을 눌러도 반응 없음 | `INPUT_PULLUP` 사용 시 버튼 반대쪽이 GND에 연결되어 있는지 확인 |
| 클릭이 여러 번 카운트됨 | 디바운싱 미적용 — 위 심화 코드 적용 |

## 다음

`03_GPIO_INTERRUPT_LAB.md` — 폴링 대신 인터럽트로 버튼을 처리하는 방법을 다룹니다.
