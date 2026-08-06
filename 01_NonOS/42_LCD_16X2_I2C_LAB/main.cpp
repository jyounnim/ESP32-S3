// Source: 42_LCD_16X2_I2C_LAB.md
// Section: 코드

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);   // 주소, 열(칸) 수, 행 수 - 09 스캐너로 실제 주소 확인

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Hello, ESP32-S3!");
  lcd.setCursor(0, 1);
  lcd.print("16x2 LCD ready");
}

void loop() {
  lcd.setCursor(0, 1);
  lcd.printf("Uptime: %6lus", millis() / 1000);
  delay(1000);
}
