# 41. 8x8 도트매트릭스 — MAX7219

## 목표

`21`~`23`의 7-Segment/TM1637과는 또 다른 디스플레이 형태인 8x8 LED 도트매트릭스를 다룹니다. MAX7219 컨트롤러가 64개의 LED를 3개의 신호선만으로 제어할 수 있게 해줍니다.

## 준비물

- MAX7219 8x8 도트매트릭스 모듈

## 회로

- DIN → GPIO4
- CLK → GPIO5
- CS → GPIO6
- VCC → 5V(모듈에 따라 3.3V도 가능, 밝기가 약해질 수 있음), GND → GND

## 라이브러리 설치

```ini
lib_deps =
    wayoda/LedControl @ ^1.0.6
```

## 코드

```cpp
#include <Arduino.h>
#include <LedControl.h>

const int DIN_PIN = 4;
const int CLK_PIN = 5;
const int CS_PIN = 6;

LedControl lc(DIN_PIN, CLK_PIN, CS_PIN, 1);   // 마지막 인자: 연결된 MAX7219 모듈 개수

// 8x8 하트 모양 패턴 (한 행 = 1바이트, 비트 1이 LED 켜짐)
byte heart[8] = {
  0b00000000,
  0b01100110,
  0b11111111,
  0b11111111,
  0b11111111,
  0b01111110,
  0b00111100,
  0b00011000
};

void setup() {
  lc.shutdown(0, false);   // 절전모드 해제(켜기)
  lc.setIntensity(0, 8);   // 밝기 0(어둡게)~15(밝게)
  lc.clearDisplay(0);
}

void loop() {
  for (int row = 0; row < 8; row++) {
    lc.setRow(0, row, heart[row]);
  }
  delay(1000);

  lc.clearDisplay(0);
  delay(500);
}
```

## 실행 & 확인

- 하트 모양이 1초간 표시됐다 0.5초간 꺼지는 것을 반복하는지 확인

## 관찰 포인트

- `setRow(장치번호, 행, 바이트패턴)`으로 한 행(8개 LED)을 한 번에 제어합니다 — `21`의 7-Segment가 세그먼트 하나하나를 비트로 다뤘던 것과 같은 원리를 2차원(8x8)으로 확장한 것입니다
- MAX7219도 **캐스케이드(직렬 연결)**가 가능합니다 — `LedControl` 생성자의 마지막 인자(모듈 개수)를 늘리고, `setRow`의 첫 인자(장치 번호, 0부터 시작)로 어느 모듈에 그릴지 지정하면 여러 개의 8x8을 이어붙여 큰 디스플레이(예: 텍스트 스크롤 전광판)를 만들 수 있습니다
- 텍스트를 스크롤하고 싶다면, 알파벳별 8x8(또는 5x8) 폰트 비트맵 데이터가 필요합니다 — `LedControl`이나 `MD_MAX72XX` 같은 라이브러리의 예제에서 이런 폰트 데이터를 참고할 수 있습니다 (응용 과제)

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 아무것도 안 켜짐 | `lc.shutdown(0, false)`를 빼먹지 않았는지 확인 — 기본값이 절전(꺼짐) 상태 |
| 패턴이 뒤집혀 보임 | 모듈의 물리적 방향(주로 IC 방향)이 반대로 설치된 경우 — 모듈을 180도 돌리거나 좌표 계산을 반전 |
| 여러 모듈 중 일부만 동작 | 캐스케이드 배선(모듈 간 DOUT→다음 모듈 DIN) 확인 |

## 다음

`42_LCD_16X2_I2C_LAB.md` — 클래식 16x2 캐릭터 LCD를 다룹니다.
