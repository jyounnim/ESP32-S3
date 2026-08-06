# 36. RTC 모듈 — DS3231

## 목표

`25`(NTP)는 Wi-Fi가 있어야만 시간을 알 수 있었습니다. DS3231은 작은 배터리(CR2032)로 전원이 나가도 시간을 계속 유지하는 **RTC(Real-Time Clock)** 칩이라, Wi-Fi 없이도 항상 정확한 시각을 갖습니다.

## 준비물

- DS3231 RTC 모듈 (보통 CR2032 배터리 홀더 내장)

## 회로

- `09`와 동일 I2C 배선 (SDA=GPIO8, SCL=GPIO9), 주소 보통 `0x68`

## 라이브러리 설치

```ini
lib_deps =
    adafruit/RTClib @ ^2.1.4
```

## 코드

```cpp
#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();

  if (!rtc.begin()) {
    Serial.println("DS3231 not found - check wiring");
    while (1) delay(1000);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power - setting time to compile time");
    // 코드를 컴파일한 시점의 PC 시간으로 최초 1회 설정
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  DateTime now = rtc.now();

  Serial.printf("%04d-%02d-%02d %02d:%02d:%02d\n",
    now.year(), now.month(), now.day(),
    now.hour(), now.minute(), now.second());

  Serial.printf("Temperature (built-in sensor): %.2f C\n", rtc.getTemperature());

  delay(1000);
}
```

## 실행 & 확인

- 최초 업로드 시 "RTC lost power..." 메시지와 함께 컴파일 시점의 시간으로 설정되는지 확인
- 이후 1초마다 시간이 정확히 흐르는지 확인
- **보드 전원을 완전히 뽑았다 다시 연결해도** (배터리가 꽂혀 있다면) 시간이 유지되는지 확인 — 이게 `25`(NTP)와의 핵심 차이입니다

## 관찰 포인트

- `rtc.lostPower()`는 배터리가 없거나 방전되어 시간이 초기화됐는지 확인합니다 — 이 체크 없이 매번 `rtc.adjust()`를 호출하면, 업로드할 때마다 "컴파일 시점"으로 시간이 리셋되어 버립니다(실제로는 그 이후 시간이 흘렀는데도). 최초 1회만 설정하는 이 패턴이 중요합니다
- DS3231은 **내장 온도 센서**로 스스로 온도 보정을 하는 고정밀 RTC입니다 — 덤으로 `getTemperature()`로 대략적인 주변 온도도 얻을 수 있습니다 (정밀 센서는 아니므로 참고용)
- 실무에서는 `25`(NTP)와 이 RTC를 함께 씁니다 — Wi-Fi가 연결되면 NTP로 RTC를 주기적으로 보정하고, Wi-Fi가 끊긴 동안은 RTC가 자체적으로 시간을 유지하는 하이브리드 구조가 일반적입니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| `DS3231 not found` | `09`의 스캐너로 `0x68` 확인 |
| 매번 리셋됨(전원 뽑으면 시간 초기화) | 배터리(CR2032) 미장착 또는 방전 확인 |
| 시간이 조금씩 느려지거나 빨라짐 | DS3231은 원래 오차가 매우 작은 편(연간 몇 분 이내)이지만, 완벽한 정밀도가 필요하면 주기적으로 NTP 보정 권장 |

## 다음

`37_SERVO_MOTOR_LAB.md`부터 모터/액추에이터를 다룹니다.
