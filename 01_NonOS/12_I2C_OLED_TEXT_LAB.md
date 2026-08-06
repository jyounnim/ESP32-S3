# 12. OLED 디스플레이 — SSD1306 텍스트 출력

## 목표

0.96인치급 128x64 SSD1306 OLED에 텍스트를 출력합니다.

## 준비물

- SSD1306 OLED 모듈 (I2C 주소 보통 `0x3C`)

## 회로

- `09`와 동일 배선

## 라이브러리 설치

```ini
lib_deps =
    adafruit/Adafruit SSD1306 @ ^2.5.13
    adafruit/Adafruit GFX Library @ ^1.11.11
```

## 코드

```cpp
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
```

## 실행 & 확인

- 첫 두 줄의 텍스트가 표시되는지 확인
- 1초마다 카운터 숫자가 갱신되는지 확인

## 관찰 포인트

- `display.display()`를 호출해야만 실제 화면에 반영됩니다 — `println`/`printf`는 메모리상의 버퍼에만 그리고, `display()`가 그 버퍼를 화면으로 전송(플러시)합니다. 이 호출을 빼먹으면 아무것도 안 보입니다
- 매번 `clearDisplay()`로 전체를 지우지 않고 `fillRect`로 **바뀐 영역만** 검게 채운 뒤 다시 그리는 방식을 썼습니다 — 전체 화면을 매번 다시 그리면 텍스트가 깜빡이는 게 눈에 띄기 때문에, 실무에서는 이렇게 부분 갱신을 많이 씁니다
- `setTextSize(1)`, `setTextSize(2)`처럼 글자 크기를 구간별로 다르게 줄 수 있습니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| `SSD1306 not found` | `09`의 스캐너로 주소 확인 (대부분 `0x3C`, 일부 모듈은 `0x3D`) |
| 화면이 하얗게만 나옴 | 잘못된 컨트롤러 크기 설정 — `SCREEN_WIDTH`/`SCREEN_HEIGHT`가 실제 모듈(128x64 또는 128x32)과 일치하는지 확인 |
| 텍스트가 겹쳐 보임 | `fillRect`로 지우는 영역이 텍스트 영역보다 좁음 — 좌표/크기 재확인 |

## 다음

`13_I2C_OLED_GRAPHICS_LAB.md` — OLED에 도형과 아이콘을 그립니다.
