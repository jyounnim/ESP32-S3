# 31. 종합 프로젝트 — 센서 값 → OLED 표시

## 목표

`10`(AHT20 온습도 센서) + `12`/`13`(SSD1306 OLED)을 결합해, 센서 값을 보기 좋게 레이아웃한 미니 계기판을 만듭니다.

## 준비물

- AHT20 모듈, SSD1306 OLED (둘 다 I2C, `16`처럼 병렬 연결)

## 코드

```cpp
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
```

## 실행 & 확인

- 상단에 온도/습도 숫자, 하단에 반원 게이지 바늘이 값에 따라 움직이는지 확인

## 관찰 포인트

- `drawGauge()` 함수로 게이지 그리기 로직을 재사용 가능하게 분리했습니다 — 온도용, 습도용으로 같은 함수를 값과 라벨만 바꿔 두 번 호출합니다. 이후 다른 센서를 추가하고 싶으면 이 함수를 그대로 재사용하면 됩니다
- `map()`은 정수 연산이라 소수점 값을 그대로 넣으면 정밀도가 떨어집니다 — `value * 10`처럼 스케일을 올린 뒤 계산하는 트릭으로 소수점 첫째자리까지의 정밀도를 유지했습니다
- 이 프로젝트에 `06`(PWM)을 추가해서, 온도가 임계값을 넘으면 LED가 켜지는 경고 기능도 쉽게 덧붙일 수 있습니다 (응용 과제)

## 다음

`32_PROJECT_SENSOR_BLE_LAB.md` — 같은 센서 값을 OLED와 BLE로 동시에 내보냅니다.
