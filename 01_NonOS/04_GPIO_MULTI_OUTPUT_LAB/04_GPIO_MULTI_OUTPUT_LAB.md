# 04. 다중 GPIO 출력 — LED 순차 제어

## 목표

여러 개의 GPIO를 배열로 관리하며 순차적으로 제어합니다. LED 8개로 "나이트 라이더" 효과(좌우로 흐르는 불빛)를 만듭니다.

## 준비물

- LED 8개, 220~330Ω 저항 8개

## 회로

- 각 LED (+) → 저항 → GPIO4, 5, 6, 7, 15, 16, 17, 18
- 각 LED (–) → GND

> 사용하시는 보드의 실크스크린을 확인해서 스트래핑 핀(0,3,45,46)과 USB 관련 핀(19,20)은 피하세요.

## 코드

```cpp
#include <Arduino.h>

const int LED_PINS[] = {4, 5, 6, 7, 15, 16, 17, 18};
const int NUM_LEDS = sizeof(LED_PINS) / sizeof(LED_PINS[0]);

void setup() {
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }
}

void loop() {
  // left to right
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(LED_PINS[i], HIGH);
    delay(80);
    digitalWrite(LED_PINS[i], LOW);
  }
  // right to left
  for (int i = NUM_LEDS - 1; i >= 0; i--) {
    digitalWrite(LED_PINS[i], HIGH);
    delay(80);
    digitalWrite(LED_PINS[i], LOW);
  }
}
```

## 실행 & 확인

- 불빛이 좌→우→좌로 흐르듯 반복되는지 확인

## 관찰 포인트

- `LED_PINS` 배열 + `NUM_LEDS`(배열 크기 자동 계산) 패턴을 쓰면, LED 개수가 바뀌어도 `for` 루프 코드는 그대로 두고 배열 값만 바꾸면 됩니다 — 핀 하나하나에 이름을 붙여 개별 처리하는 것보다 확장성이 좋습니다
- `delay(80)`을 줄이면 더 빠르게 흐르는 효과, 늘리면 더 느긋한 효과가 되는지 확인해보세요
- 응용 과제: 한 번에 2~3개의 LED가 동시에 켜지도록(꼬리 효과) 바꿔보세요 — 안쪽 `for` 루프에서 `i`, `i-1`, `i-2` 위치를 함께 켜면 됩니다
- GPIO 개수가 부족해지면 어떻게 할지는 `22_74HC595_MULTI_LED_LAB.md`에서 시프트 레지스터로 확장하는 방법을 다룹니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 특정 LED만 안 켜짐 | 해당 GPIO가 스트래핑/예약 핀인지 확인, 배선 재확인 |
| 전체적으로 어둡거나 깜빡임이 이상함 | 여러 LED를 동시에 켤 때 전류 총합이 USB 전원 공급 한계를 넘을 수 있음 — LED 개수가 많으면 외부 전원 고려 |

## 다음

`05_GPIO_INPUT_ADVANCED_LAB.md` — 롱프레스/더블클릭처럼 좀 더 복잡한 버튼 입력 패턴을 다룹니다.
