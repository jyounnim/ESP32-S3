# 42. 16x2 캐릭터 LCD (I2C 백팩)

## 목표

OLED(`12`~`13`)와는 다른, 산업 현장에서 여전히 널리 쓰이는 클래식 캐릭터 LCD를 다룹니다. 원래 16x2 LCD는 배선 핀이 많지만(6~10개), I2C 백팩(PCF8574 칩 기반 어댑터)을 쓰면 `09`의 다른 I2C 장치들처럼 2선으로 제어할 수 있습니다.

## 준비물

- 16x2 캐릭터 LCD + I2C 백팩(PCF8574) 모듈 (일체형으로 판매되는 경우가 많음)

## 회로

- `09`와 동일 I2C 배선 (SDA=GPIO8, SCL=GPIO9), 주소 보통 `0x27` 또는 `0x3F`

## 라이브러리 설치

```ini
lib_deps =
    johnrickman/LiquidCrystal_I2C @ ^1.1.4
```

## 코드

```cpp
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
```

## 실행 & 확인

- 첫 줄에 "Hello, ESP32-S3!", 둘째 줄에 초기 메시지가 표시되는지 확인
- 이후 둘째 줄이 1초마다 uptime으로 갱신되는지 확인

## 관찰 포인트

- 주소가 `0x27`이 아니라 `0x3F`로 뜨는 백팩도 흔합니다 — PCF8574(주소 `0x27` 계열)와 PCF8574A(주소 `0x3F` 계열) 두 가지 칩 버전이 있기 때문입니다. `09`의 스캐너로 먼저 확인하세요
- 캐릭터 LCD는 OLED처럼 임의의 그래픽/폰트를 그리는 게 아니라, **내장된 고정 문자셋**을 씁니다 — 다만 최대 8개까지 사용자 정의 문자(`createChar()`)를 등록해서 간단한 아이콘(예: 하트, 화살표)을 만들 수 있습니다
- `lcd.setCursor(0, 1)`처럼 매번 같은 위치를 지정한 뒤 새로 쓰는 방식으로 이전 내용을 덮어씁니다 — OLED의 `fillRect`로 지우는 방식과 달리, 캐릭터 LCD는 문자 단위로 덮어써지므로 새 텍스트가 이전 텍스트보다 짧으면 뒷부분 잔상이 남을 수 있습니다(위 코드는 `%6lu`로 자리수를 고정해 이 문제를 피했습니다)

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 화면이 하얗게만 나오고 글자 없음 | 대비(contrast) 조절 나사가 백팩에 있는 경우 — 드라이버 돌려 조절 |
| `LCD not found`류 동작 안 함 | `09`의 스캐너로 실제 주소 확인 후 코드의 `0x27`을 교체 |
| 글자가 깨져 보임 | 초기화(`lcd.init()`) 이후 충분한 대기 없이 바로 쓴 경우 — `delay(100)` 정도 추가 |

## 다음

`43_ADS1115_LAB.md` — 정밀 외장 ADC를 다룹니다.
