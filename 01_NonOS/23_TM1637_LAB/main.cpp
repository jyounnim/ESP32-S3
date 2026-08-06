// Source: 23_TM1637_LAB.md
// Section: 코드

#include <Arduino.h>
#include <TM1637Display.h>

#define CLK_PIN 5
#define DIO_PIN 4

TM1637Display display(CLK_PIN, DIO_PIN);

void setup() {
  display.setBrightness(0x0f);   // 0x00(어둡게) ~ 0x0f(밝게)
}

void loop() {
  static int counter = 0;

  display.showNumberDec(counter);   // 숫자를 4자리로 표시 (앞자리 0은 자동 생략)
  counter = (counter + 1) % 10000;
  delay(200);
}
