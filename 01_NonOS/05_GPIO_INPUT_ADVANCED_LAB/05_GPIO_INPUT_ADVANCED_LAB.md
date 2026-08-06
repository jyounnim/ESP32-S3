# 05. GPIO 입력 심화 — 롱프레스/더블클릭 감지

## 목표

버튼 하나로 **싱글클릭 / 더블클릭 / 롱프레스** 세 가지 다른 동작을 구분합니다. 리모컨, 스마트워치 등 실제 제품에서 흔히 쓰이는 입력 패턴입니다.

## 준비물

- 택트 스위치 1개 (`02`와 동일 배선, GPIO5)

## 핵심 개념

- **롱프레스**: 버튼을 누른 뒤 뗄 때까지의 시간이 임계값(예: 800ms) 이상
- **더블클릭**: 첫 클릭이 끝난 뒤 일정 시간(예: 300ms) 안에 두 번째 클릭이 들어옴
- **싱글클릭**: 더블클릭 대기 시간이 지나도 두 번째 클릭이 없었던 경우 — 즉, 싱글클릭은 "즉시" 확정되지 않고 대기 시간이 지나야 확정됩니다 (더블클릭과 구분하기 위한 필연적인 지연)

## 코드

```cpp
#include <Arduino.h>

const int BUTTON_PIN = 5;
const unsigned long DEBOUNCE_MS = 30;
const unsigned long LONG_PRESS_MS = 800;
const unsigned long DOUBLE_CLICK_WINDOW_MS = 300;

int lastReading = HIGH;
int stableState = HIGH;
unsigned long lastChangeTime = 0;
unsigned long pressStartTime = 0;
unsigned long lastReleaseTime = 0;
bool waitingForSecondClick = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.println("Ready. Try single click, double click, and long press.");
}

void loop() {
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastReading) {
    lastChangeTime = millis();
  }

  if ((millis() - lastChangeTime) > DEBOUNCE_MS) {
    if (reading != stableState) {
      stableState = reading;

      if (stableState == LOW) {
        pressStartTime = millis();               // press started
      } else {
        unsigned long heldMs = millis() - pressStartTime;   // released

        if (heldMs >= LONG_PRESS_MS) {
          Serial.println("LONG PRESS");
          waitingForSecondClick = false;
        } else if (waitingForSecondClick && (millis() - lastReleaseTime) < DOUBLE_CLICK_WINDOW_MS) {
          Serial.println("DOUBLE CLICK");
          waitingForSecondClick = false;
        } else {
          waitingForSecondClick = true;
          lastReleaseTime = millis();
        }
      }
    }
  }

  // double-click window expired without a second click -> confirm single click
  if (waitingForSecondClick && (millis() - lastReleaseTime) >= DOUBLE_CLICK_WINDOW_MS) {
    Serial.println("SINGLE CLICK");
    waitingForSecondClick = false;
  }

  lastReading = reading;
}
```

## 실행 & 확인

- 짧게 한 번 누르면 (약 300ms 후) `SINGLE CLICK`
- 빠르게 두 번 누르면 즉시 `DOUBLE CLICK`
- 길게 누르고 있다 떼면 `LONG PRESS`

## 관찰 포인트

- 싱글클릭이 즉시 판정되지 않고 `DOUBLE_CLICK_WINDOW_MS`만큼 지연되는 게 이상하게 느껴질 수 있지만, 이건 이 패턴의 구조적 특성입니다 — "이게 마지막 클릭인지 아닌지"는 그 시간이 지나봐야 알 수 있습니다. 즉각 반응이 중요한 UI라면 더블클릭을 포기하고 싱글클릭만 즉시 처리하는 설계를 택하기도 합니다
- `LONG_PRESS_MS`, `DOUBLE_CLICK_WINDOW_MS` 값을 조절하며 원하는 감도로 튜닝해보세요
- 이 상태 머신은 롱프레스 중에는 별도 "누르고 있는 동안 반복 신호"(auto-repeat)를 내지 않습니다 — 볼륨 버튼처럼 누르고 있는 동안 값이 계속 바뀌게 하려면, `stableState == LOW`인 동안 `loop()`에서 주기적으로 추가 이벤트를 발생시키는 로직을 더해야 합니다 (응용 과제)

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 더블클릭이 항상 싱글클릭 두 번으로 인식됨 | `DOUBLE_CLICK_WINDOW_MS`를 늘려보기 — 클릭 속도가 이 값보다 느리면 싱글클릭으로 판정됨 |
| 롱프레스가 안 잡힘 | `LONG_PRESS_MS`를 줄여보기, 또는 정말 그만큼 오래 누르고 있는지 확인 |

## 다음

`06_PWM_LAB.md` — PWM으로 LED 밝기를 제어하는 아날로그 출력을 다룹니다.
