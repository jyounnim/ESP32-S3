# 19. 소형 컬러 LCD — ST7735

## 목표

SPI 방식 소형 컬러 TFT LCD(ST7735 컨트롤러)에 텍스트와 도형을 컬러로 표시합니다. I2C의 SSD1306(흑백)과 달리 컬러 표현이 가능합니다.

## 준비물

- ST7735 TFT LCD 모듈 (보통 0.96"~1.8", 128x160 또는 80x160)

## 회로

- SCK → GPIO12, MOSI → GPIO11 (`17`과 동일)
- CS → GPIO10
- DC(Data/Command) → GPIO17
- RST(Reset) → GPIO18
- VCC → 3.3V, GND → GND
- (백라이트 핀이 있는 모듈은 3.3V 또는 별도 PWM 제어)

## 라이브러리 설치

```ini
lib_deps =
    adafruit/Adafruit ST7735 and ST7789 Library @ ^1.10.3
    adafruit/Adafruit GFX Library @ ^1.11.11
```

## 코드

```cpp
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
```

## 실행 & 확인

- 첫 화면에 텍스트가 표시되는지 확인
- 빨간 원이 화면을 좌→우로 흐르며 반복되는지 확인

## 관찰 포인트

- `initR()`의 인자(`INITR_BLACKTAB`, `INITR_GREENTAB`, `INITR_REDTAB` 등)는 패널 뒷면의 탭 색상 스티커와 관련된 초기화 옵션입니다 — 화면이 어긋나 보이거나(오프셋), 색이 이상하면 이 옵션을 바꿔보는 게 가장 흔한 해결책입니다
- SSD1306(I2C, `12`~`13`)과 비교하면: SPI가 하드웨어적으로 더 빠르기 때문에, 픽셀 수가 많고 색상 정보(RGB565, 픽셀당 2바이트)까지 전송해야 하는 컬러 디스플레이는 대부분 SPI를 씁니다. I2C로는 컬러 LCD의 데이터 전송 속도를 감당하기 어렵습니다
- `fillRect`로 이전 프레임의 특정 영역만 지우는 패턴은 `13`(OLED 그래픽)에서 배운 것과 동일합니다 — 디스플레이 종류가 달라져도 "부분 갱신으로 깜빡임 줄이기" 원칙은 재사용됩니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 화면이 하얗게/까맣게만 나옴 | 배선(특히 DC, RST) 확인, `initR()` 옵션을 다른 탭 색상으로 변경 시도 |
| 화면이 일부만 표시되거나 밀려 보임 | 패널 해상도(128x160 vs 80x160)와 초기화 옵션 불일치 |
| 색이 반전되어 보임 | 일부 모듈은 `tft.invertDisplay(true)` 필요 |

## 다음

`20_SPI_NRF24L01_LAB.md` — NRF24L01 무선 모듈로 두 ESP32 간 데이터를 주고받습니다.
