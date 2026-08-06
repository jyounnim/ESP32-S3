// Source: 13_I2C_OLED_GRAPHICS_LAB.md
// Section: 코드

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// 16x16 1비트 비트맵 아이콘 (하트 모양), MSB 우선
static const unsigned char PROGMEM heartIcon[] = {
  0x00, 0x00, 0x0C, 0x30, 0x1E, 0x78, 0x3F, 0xFC,
  0x3F, 0xFC, 0x3F, 0xFC, 0x1F, 0xF8, 0x0F, 0xF0,
  0x07, 0xE0, 0x03, 0xC0, 0x01, 0x80, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 not found");
    while (1) delay(1000);
  }
}

void loop() {
  static int percent = 0;
  percent = (percent + 5) % 105;

  display.clearDisplay();

  // 기본 도형
  display.drawRect(0, 0, 40, 20, SSD1306_WHITE);
  display.fillCircle(60, 10, 8, SSD1306_WHITE);
  display.drawLine(80, 0, 110, 20, SSD1306_WHITE);

  // 비트맵 아이콘
  display.drawBitmap(10, 30, heartIcon, 16, 16, SSD1306_WHITE);

  // 진행바(progress bar)
  int barWidth = map(percent, 0, 100, 0, 60);
  display.drawRect(40, 40, 64, 16, SSD1306_WHITE);
  display.fillRect(42, 42, barWidth, 12, SSD1306_WHITE);
  display.setCursor(40, 58);
  display.setTextSize(1);
  display.printf("%d%%", percent > 100 ? 100 : percent);

  display.display();
  delay(200);
}
