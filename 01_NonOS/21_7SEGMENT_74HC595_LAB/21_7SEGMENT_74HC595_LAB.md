# 21. 7-Segment 직접 구동 — 74HC595 시프트 레지스터

## 목표

7-Segment 표시기 하나를 GPIO 3개만으로 구동합니다. 74HC595는 "직렬로 데이터를 받아 병렬 8개 출력으로 내보내는" 시프트 레지스터로, GPIO 부족 문제를 해결하는 대표적인 부품입니다.

## 준비물

- 74HC595 시프트 레지스터 1개
- 7-Segment 표시기 1개 (Common Cathode 기준)
- 220Ω 저항 8개 (세그먼트 보호용)

## 회로

| 74HC595 핀 | 연결 |
|---|---|
| DS (14번, 데이터 입력) | GPIO4 |
| SHCP (11번, 시프트 클럭) | GPIO5 |
| STCP (12번, 래치 클럭) | GPIO6 |
| OE (13번, 출력 활성) | GND (항상 출력 활성) |
| MR (10번, 리셋) | VCC (리셋 비활성) |
| VCC (16번) | 3.3V |
| GND (8번) | GND |
| Q0~Q7 (15,1~7번) | 저항을 거쳐 7-Segment의 a,b,c,d,e,f,g,dp 각 핀 |

7-Segment의 공통(Common) 핀은 GND에 연결합니다 (Common Cathode 기준).

## 코드

```cpp
#include <Arduino.h>

const int DATA_PIN = 4;    // DS
const int CLOCK_PIN = 5;   // SHCP
const int LATCH_PIN = 6;   // STCP

// 비트 순서: bit0=a, bit1=b, ... bit6=g, bit7=dp (1 = 세그먼트 켜짐)
const byte DIGIT_PATTERNS[10] = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111  // 9
};

void showDigit(int digit) {
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, DIGIT_PATTERNS[digit]);
  digitalWrite(LATCH_PIN, HIGH);   // 래치를 올리는 순간 출력에 반영됨
}

void setup() {
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
}

void loop() {
  for (int i = 0; i < 10; i++) {
    showDigit(i);
    delay(500);
  }
}
```

## 실행 & 확인

- 0부터 9까지 숫자가 0.5초 간격으로 순서대로 표시되는지 확인

## 관찰 포인트

- `shiftOut()`은 내부적으로 클럭을 8번 토글하며 한 비트씩 `DATA_PIN`으로 밀어넣습니다 — GPIO 3개(Data/Clock/Latch)로 8개의 출력을 제어하는 원리가 바로 이 "직렬→병렬 변환"입니다
- `digitalWrite(LATCH_PIN, LOW)` → `shiftOut()` → `digitalWrite(LATCH_PIN, HIGH)` 순서가 중요합니다 — 래치가 LOW인 동안 데이터를 밀어넣고, HIGH로 올리는 그 순간에만 실제 출력 핀(Q0~Q7)에 반영됩니다. 이 덕분에 데이터를 밀어넣는 중간 과정이 화면에 깜빡이며 노출되지 않습니다
- **Common Anode** 표시기를 쓴다면 로직이 반대입니다 — 세그먼트가 0일 때 켜지므로, 각 패턴 값에 비트반전(`~DIGIT_PATTERNS[i] & 0xFF`)을 적용해야 합니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 숫자가 이상하게 표시됨(뒤집힌 듯) | Q0~Q7과 세그먼트 a~g,dp의 실제 배선 순서가 코드의 비트 순서와 다름 — 74HC595 데이터시트의 핀 배치와 실제 배선을 대조 |
| 아무것도 안 켜짐 | OE 핀이 GND에 제대로 연결됐는지 확인 (HIGH면 출력이 전부 비활성화됨) |
| 전체 세그먼트가 항상 켜져 있음 | Common Anode 표시기인데 Common Cathode 패턴을 그대로 쓴 경우 — 위 비트반전 적용 |

## 다음

`22_74HC595_MULTI_LED_LAB.md` — 같은 74HC595로 다중 LED를 확장 제어합니다.
