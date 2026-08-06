// Source: 12_I2C_OLED_TEXT_LAB.md
// Section: 코드

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1        // 리셋 핀 없음 (I2C 모듈은 보통 없음)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 not found - check wiring/address");
    while (1) delay(1000);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Hello, ESP32-S3!");
  display.println("SSD1306 OLED ready");
  display.display();   // 버퍼 내용을 실제 화면에 전송
}

void loop() {
  static int counter = 0;
  display.fillRect(0, 20, 128, 20, SSD1306_BLACK);   // 이전 숫자 영역만 지움
  display.setCursor(0, 20);
  display.setTextSize(2);
  display.printf("Count: %d", counter++);
  display.display();
  delay(1000);
}
