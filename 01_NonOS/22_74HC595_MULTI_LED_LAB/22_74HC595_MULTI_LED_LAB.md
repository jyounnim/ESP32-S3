# 22. 74HC595로 다중 LED 확장

## 목표

`21`과 같은 74HC595 하나로 8개의 개별 LED를 제어합니다. `04`(다중 GPIO 출력)와 같은 나이트라이더 효과를, 이번엔 GPIO 3개만으로 구현합니다.

## 준비물

- 74HC595 1개, LED 8개, 220Ω 저항 8개

## 회로

- 74HC595 배선은 `21`과 동일 (DS=GPIO4, SHCP=GPIO5, STCP=GPIO6, OE=GND, MR=VCC)
- Q0~Q7 각각 → 저항 → LED (+) → LED (–) → GND

## 코드

```cpp
#include <Arduino.h>

const int DATA_PIN = 4;
const int CLOCK_PIN = 5;
const int LATCH_PIN = 6;

void writePattern(byte pattern) {
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, pattern);
  digitalWrite(LATCH_PIN, HIGH);
}

void setup() {
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
}

void loop() {
  for (int i = 0; i < 8; i++) {
    writePattern(1 << i);
    delay(100);
  }
  for (int i = 7; i >= 0; i--) {
    writePattern(1 << i);
    delay(100);
  }
}
```

## 실행 & 확인

- LED가 한 개씩 좌→우→좌로 흐르며 켜지는지 확인 (`04`와 같은 효과이지만, 이번엔 ESP32 GPIO를 3개만 사용)

## 관찰 포인트

- `1 << i`(비트 시프트)로 "i번째 비트만 1인 값"을 만드는 방식은 8개의 출력을 하나의 바이트로 다루는 전형적인 패턴입니다
- **74HC595는 캐스케이드(직렬 연결)가 가능합니다** — 첫 번째 칩의 9번 핀(Q7', 직렬 출력)을 두 번째 칩의 DS(14번)에 연결하고 SHCP/STCP는 공유하면, GPIO 3개 그대로 16개, 24개... 얼마든지 출력을 늘릴 수 있습니다. 이 경우 `shiftOut()`을 칩 개수만큼 여러 번 호출한 뒤(뒤쪽 칩에 해당하는 데이터부터 먼저 보내야 함) 마지막에 한 번만 래치를 올립니다
- `04`(다중 GPIO 직접 제어)와 이번 실습을 비교해보면: ESP32 자체 GPIO가 남아돈다면 `04`처럼 직접 제어가 더 간단하지만, LED/출력이 많아지거나 GPIO가 부족하면 74HC595 같은 확장 칩이 필수가 됩니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 순서가 반대로 보임 | `MSBFIRST`를 `LSBFIRST`로 바꿔서 비교 (Q0~Q7 배선 순서와 시프트 방향의 조합에 따라 달라짐) |
| 특정 LED만 안 켜짐 | 해당 Q핀 배선/저항/LED 극성 확인 |

## 다음

`23_TM1637_LAB.md` — 2선으로 7-Segment 4자리를 제어하는 TM1637 모듈을 다룹니다.
