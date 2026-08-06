// Source: 21_7SEGMENT_74HC595_LAB.md
// Section: 코드

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
