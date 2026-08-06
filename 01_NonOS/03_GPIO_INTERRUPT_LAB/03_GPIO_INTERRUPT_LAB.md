# 03. 외부 인터럽트 — 다중 버튼 처리

## 목표

`loop()`에서 매번 `digitalRead`로 확인하는 폴링 방식 대신, 버튼이 눌리는 순간 즉시 반응하는 **인터럽트(Interrupt)** 방식으로 두 개의 버튼을 처리합니다.

## 준비물

- 택트 스위치 2개, LED 1개

## 회로

- Button1 → GPIO5, GND (`INPUT_PULLUP`)
- Button2 → GPIO6, GND (`INPUT_PULLUP`)
- LED → GPIO4 (`01`과 동일)

## 핵심 개념

| 요소 | 설명 |
|---|---|
| `attachInterrupt(pin, ISR함수, 모드)` | 핀 신호 변화에 인터럽트 서비스 루틴(ISR) 연결 |
| `IRAM_ATTR` | ISR을 내부 RAM에 배치 — Flash 접근이 막힌 순간에도 ISR이 실행되도록 보장 (ESP32 계열 필수) |
| `volatile` | ISR과 `loop()`가 함께 접근하는 변수에 반드시 필요 — 컴파일러 최적화로 값이 캐싱되는 것을 방지 |
| `FALLING` | HIGH→LOW로 떨어지는 순간(버튼이 눌리는 순간)에 인터럽트 발생 |

## 코드

```cpp
#include <Arduino.h>

const int BUTTON1_PIN = 5;
const int BUTTON2_PIN = 6;
const int LED_PIN = 4;

volatile bool button1Flag = false;
volatile bool button2Flag = false;
volatile unsigned long lastButton1Isr = 0;
volatile unsigned long lastButton2Isr = 0;
const unsigned long ISR_DEBOUNCE_MS = 200;

void IRAM_ATTR onButton1Press() {
  unsigned long now = millis();
  if (now - lastButton1Isr > ISR_DEBOUNCE_MS) {
    button1Flag = true;
    lastButton1Isr = now;
  }
}

void IRAM_ATTR onButton2Press() {
  unsigned long now = millis();
  if (now - lastButton2Isr > ISR_DEBOUNCE_MS) {
    button2Flag = true;
    lastButton2Isr = now;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BUTTON1_PIN), onButton1Press, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON2_PIN), onButton2Press, FALLING);

  Serial.println("Ready. Press Button1 or Button2.");
}

void loop() {
  if (button1Flag) {
    button1Flag = false;
    Serial.println("Button1 pressed - LED ON");
    digitalWrite(LED_PIN, HIGH);
  }
  if (button2Flag) {
    button2Flag = false;
    Serial.println("Button2 pressed - LED OFF");
    digitalWrite(LED_PIN, LOW);
  }
}
```

## 실행 & 확인

- Button1을 누르면 즉시 LED ON, Button2를 누르면 즉시 LED OFF가 되는지 확인
- 버튼을 누른 순간의 반응 지연이 폴링 방식(`02`)보다 체감상 더 즉각적인지 비교

## 관찰 포인트

- ISR(`onButton1Press`) 안에는 `Serial.println`이 없습니다 — ISR 안에서 시간이 걸리는 작업(시리얼 출력 등)을 하면 다른 인터럽트 처리가 지연되거나 예기치 못한 크래시로 이어질 수 있습니다. 실제 출력은 항상 `loop()`에서 플래그를 확인해 처리합니다 — 이게 인터럽트 처리의 표준 패턴입니다
- `ISR_DEBOUNCE_MS`로 ISR 내부에서 간단히 디바운싱을 처리했습니다 — `02`의 정식 상태 머신 디바운싱보다 단순하지만, "너무 짧은 간격의 중복 인터럽트 무시"라는 목적에는 충분합니다
- `IRAM_ATTR`을 빼고 컴파일해보세요 — 당장은 동작하는 것처럼 보일 수 있지만, Flash 읽기/쓰기(OTA, NVS 저장 등)가 진행 중인 순간에 인터럽트가 발생하면 크래시 위험이 생깁니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 인터럽트가 아예 발동 안 함 | `attachInterrupt`에 넘긴 핀 번호와 `pinMode` 설정 핀이 일치하는지 확인 |
| 버튼 한 번에 여러 번 반응 | `ISR_DEBOUNCE_MS`를 늘려보기 (기본 200ms) |
| 컴파일 에러 (ISR 관련) | ISR 함수는 반환값이 없고(`void`) 파라미터도 없어야 함 |

## 다음

`04_GPIO_MULTI_OUTPUT_LAB.md` — 여러 개의 LED를 동시에 제어하는 방법을 다룹니다.
