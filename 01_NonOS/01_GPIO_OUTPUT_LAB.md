# 01. GPIO 출력 — LED 점멸

## 목표

디지털 출력으로 LED를 켜고 끄는 가장 기본적인 GPIO 제어를 익힙니다.

## 준비물

- LED 1개, 220~330Ω 저항 1개

## 회로

- LED (+) 다리 → 저항 → GPIO4
- LED (–) 다리 → GND

> 보드에 온보드 RGB LED가 있다면 그걸로도 실습 가능하지만, WS2812 프로토콜 기반이라 `digitalWrite`가 아닌 `Adafruit_NeoPixel` 같은 별도 라이브러리가 필요합니다. 이 실습은 외부 단색 LED 기준입니다.

## 코드

```cpp
#include <Arduino.h>

const int LED_PIN = 4;

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);
}
```

## 실행 & 확인

- Build → Upload
- LED가 0.5초 간격으로 점멸하는지 확인

## 관찰 포인트

- `delay()` 값을 바꿔가며 점멸 속도가 달라지는지 확인
- `digitalWrite` 대신 `digitalToggle(LED_PIN)`(arduino-esp32 코어 지원 시)으로 바꿔 코드를 더 간결하게 만들어보세요

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| LED가 항상 켜져 있거나 꺼져 있음 | 극성(+/-) 확인, 저항 없이 직결 시 손상 가능성 |
| 특정 GPIO가 동작하지 않음 | 스트래핑 핀(0,3,45,46), USB 관련 핀(19,20) 등은 용도 제한 — 보드 핀맵 확인 |

## 다음

`02_GPIO_INPUT_LAB.md` — 버튼 입력과 디바운싱을 다룹니다.
