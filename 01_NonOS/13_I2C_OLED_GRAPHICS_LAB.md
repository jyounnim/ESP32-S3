# 13. OLED 디스플레이 — 그래픽/아이콘

## 목표

`12`에서 텍스트만 다뤘다면, 이번엔 도형(사각형/원/선)과 비트맵 아이콘, 진행바(progress bar)를 그립니다.

## 준비물

- `12`와 동일

## 코드

```cpp
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
```

## 실행 & 확인

- 좌상단에 사각형/원/선이 그려지는지 확인
- 하트 아이콘이 표시되는지 확인
- 진행바가 0%→100%를 반복하며 채워졌다 비워지는지 확인

## 관찰 포인트

- 비트맵은 `PROGMEM`(Flash 메모리)에 저장했습니다 — 이렇게 하지 않으면 아이콘 데이터가 RAM을 차지합니다. 아이콘 개수가 많아질수록 이 차이가 중요해집니다
- 1비트 비트맵은 바이트 하나가 가로 8픽셀을 표현합니다 — 16x16 아이콘이면 가로 16픽셀 = 2바이트, 세로 16줄이라 총 32바이트가 필요합니다. 아이콘 크기를 바꾸려면 이 바이트 배열도 새로 계산해서 만들어야 합니다 (온라인 "이미지 to 비트맵 배열" 변환 도구를 쓰면 편합니다)
- 진행바는 `08`(PWM+ADC)에서 만든 "가변저항 값 읽기"와 결합하면, 실제 센서 값을 시각화하는 UI로 바로 확장할 수 있습니다 — `31_PROJECT_SENSOR_OLED_LAB.md`에서 이 결합을 다룹니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 비트맵이 깨져 보임 | 바이트 배열의 행(row) 계산이 맞지 않음 — 아이콘 크기와 배열 크기가 정확히 일치하는지 확인 |
| 진행바가 테두리를 넘어감 | `fillRect`의 폭 계산 시 테두리(`drawRect`) 두께만큼 여백을 안 뒀을 수 있음 |

## 다음

`14_I2C_COMPASS_LAB.md` — 전자 나침반 QMC5883L을 다룹니다.
