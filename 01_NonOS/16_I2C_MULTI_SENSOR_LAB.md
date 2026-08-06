# 16. I2C 다중 센서 동시 사용

## 목표

I2C는 하나의 버스(SDA/SCL 두 선)에 여러 장치를 동시에 연결할 수 있는 게 핵심 장점입니다. AHT20(온습도) + MPU6050(가속도) + SSD1306(OLED)을 동시에 연결해서 한 화면에 표시합니다.

## 준비물

- AHT20 (`0x38`), MPU6050 (`0x68`), SSD1306 OLED (`0x3C`) — 세 장치의 주소가 서로 겹치지 않는 조합

## 회로

- 세 장치 모두 SDA=GPIO8, SCL=GPIO9에 **병렬로** 연결 (같은 두 선에 전부 매달림)
- VCC/GND도 각각 3.3V/GND에 병렬 연결

## 라이브러리 설치

```ini
lib_deps =
    adafruit/Adafruit AHTX0 @ ^2.0.5
    adafruit/Adafruit MPU6050 @ ^2.2.6
    adafruit/Adafruit SSD1306 @ ^2.5.13
    adafruit/Adafruit GFX Library @ ^1.11.11
```

## 코드

```cpp
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_AHTX0 aht;
Adafruit_MPU6050 mpu;
Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();

  bool ok = true;
  if (!aht.begin())   { Serial.println("AHT20 init failed");   ok = false; }
  if (!mpu.begin())   { Serial.println("MPU6050 init failed"); ok = false; }
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { Serial.println("OLED init failed"); ok = false; }

  if (!ok) {
    Serial.println("One or more devices failed - check I2C scanner (09) first");
  }

  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  sensors_event_t a, g, mpuTemp;
  mpu.getEvent(&a, &g, &mpuTemp);

  Serial.printf("Temp:%.1fC Humidity:%.1f%% AccelZ:%.2f\n",
                temp.temperature, humidity.relative_humidity, a.acceleration.z);

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.printf("Temp: %.1f C\n", temp.temperature);
  display.printf("Humidity: %.1f %%\n", humidity.relative_humidity);
  display.printf("AccelZ: %.2f m/s^2\n", a.acceleration.z);
  display.display();

  delay(500);
}
```

## 실행 & 확인

- 온도/습도/Z축 가속도가 OLED와 시리얼 모니터 양쪽에 표시되는지 확인
- 센서 중 하나만 배선을 빼보고, 나머지는 정상 동작하면서 해당 센서만 초기화 실패 메시지가 뜨는지 확인

## 관찰 포인트 — 주소가 겹치면 어떻게 하나

지금 조합은 우연히 주소가 안 겹쳤지만, 예를 들어 **같은 종류의 센서 두 개**(MPU6050 두 개 등)를 쓰면 기본 주소가 똑같아서 충돌합니다. 해결 방법은 크게 세 가지입니다.

| 방법 | 설명 |
|---|---|
| **주소 변경 핀 활용** | MPU6050은 `AD0` 핀을 3.3V로 올리면 주소가 `0x68`→`0x69`로 바뀝니다. 데이터시트에서 주소 변경 핀 유무를 확인하세요 |
| **I2C 멀티플렉서(TCA9548A)** | 주소 변경이 불가능한 동일 센서 여러 개를 쓸 때, 멀티플렉서 하나로 8개의 독립된 I2C 채널을 만들어 각 채널에 같은 주소의 센서를 하나씩 연결 |
| **두 번째 I2C 버스 사용** | ESP32-S3는 `Wire`와 `Wire1` 두 개의 독립된 I2C 버스를 지원 — `Wire1.begin(sda2, scl2)`로 물리적으로 다른 버스에 분산 |

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 특정 센서만 초기화 실패 | `09`의 스캐너로 그 센서의 주소가 실제로 뜨는지 먼저 확인 — 나머지 센서와의 배선 간섭보다는 해당 센서만의 배선 문제인 경우가 많음 |
| 전체적으로 느려짐 | 여러 장치를 동시에 폴링하면 루프 주기가 길어질 수 있음 — 꼭 필요한 주기로 `delay` 조정 |
| 간헐적으로 통신 실패 | 여러 장치를 병렬 연결할 때 풀업 저항이 너무 많거나 너무 적어 신호 품질이 나빠질 수 있음 — 보통 버스 전체에 4.7kΩ 풀업 1쌍이면 충분 |

## 다음

`17_SPI_BASICS_LAB.md`부터 SPI 통신을 다룹니다.
