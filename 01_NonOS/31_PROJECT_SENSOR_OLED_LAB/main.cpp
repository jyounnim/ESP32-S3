// Source: 31_PROJECT_SENSOR_OLED_LAB.md
// Section: 코드

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_AHTX0 aht;
Adafruit_SSD1306 display(128, 64, &Wire, -1);

void drawGauge(int x, int y, int radius, float value, float maxValue, const char* label) {
  display.drawCircle(x, y, radius, SSD1306_WHITE);
  float angle = map((long)(value * 10), 0, (long)(maxValue * 10), -90, 90) * PI / 180.0;
  int nx = x + sin(angle) * radius;
  int ny = y - cos(angle) * radius;
  display.drawLine(x, y, nx, ny, SSD1306_WHITE);
  display.setCursor(x - 10, y + radius + 4);
  display.setTextSize(1);
  display.print(label);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();

  if (!aht.begin())   Serial.println("AHT20 init failed");
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) Serial.println("OLED init failed");

  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  display.clearDisplay();

  // 상단: 숫자로 크게 표시
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.printf("%.1fC", temp.temperature);
  display.setCursor(70, 0);
  display.printf("%.0f%%", humidity.relative_humidity);

  // 하단: 반원 게이지 두 개
  drawGauge(32, 48, 14, temp.temperature, 40.0, "TEMP");
  drawGauge(96, 48, 14, humidity.relative_humidity, 100.0, "HUM");

  display.display();
  delay(500);
}
