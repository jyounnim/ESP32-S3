// Source: 41_DOT_MATRIX_MAX7219_LAB.md
// Section: 코드

#include <Arduino.h>
#include <LedControl.h>

const int DIN_PIN = 4;
const int CLK_PIN = 5;
const int CS_PIN = 6;

LedControl lc(DIN_PIN, CLK_PIN, CS_PIN, 1);   // 마지막 인자: 연결된 MAX7219 모듈 개수

// 8x8 하트 모양 패턴 (한 행 = 1바이트, 비트 1이 LED 켜짐)
byte heart[8] = {
  0b00000000,
  0b01100110,
  0b11111111,
  0b11111111,
  0b11111111,
  0b01111110,
  0b00111100,
  0b00011000
};

void setup() {
  lc.shutdown(0, false);   // 절전모드 해제(켜기)
  lc.setIntensity(0, 8);   // 밝기 0(어둡게)~15(밝게)
  lc.clearDisplay(0);
}

void loop() {
  for (int row = 0; row < 8; row++) {
    lc.setRow(0, row, heart[row]);
  }
  delay(1000);

  lc.clearDisplay(0);
  delay(500);
}
