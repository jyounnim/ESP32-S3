// Source: 19_SPI_TFT_LCD_LAB.md
// Section: 코드

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

#define TFT_CS  10
#define TFT_DC  17
#define TFT_RST 18

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(115200);
  delay(1000);

  tft.initR(INITR_BLACKTAB);   // 대부분의 1.8" 모듈에 맞는 초기화 옵션 (모듈에 따라 INITR_GREENTAB 등으로 조정 필요할 수 있음)
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  tft.println("Hello, ESP32-S3!");
  tft.println("ST7735 TFT ready");
}

void loop() {
  static int x = 0;
  tft.fillRect(0, 30, tft.width(), 20, ST77XX_BLACK);   // 이전 프레임 지우기
  tft.fillCircle(x, 40, 8, ST77XX_RED);

  x += 4;
  if (x > tft.width()) x = 0;

  delay(30);
}
