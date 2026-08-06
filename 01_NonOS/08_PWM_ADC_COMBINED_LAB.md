# 08. 종합 — 가변저항으로 LED 밝기 조절

## 목표

`06`(PWM)과 `07`(ADC)을 결합해, 가변저항 값에 따라 LED 밝기가 실시간으로 바뀌는 아날로그 컨트롤러를 구현합니다.

## 코드 (core 2.x)

```cpp
#include <Arduino.h>

const int LED_PIN = 4;
const int ADC_PIN = 1;
const int PWM_CHANNEL = 0;
const int PWM_FREQ = 5000;
const int PWM_RESOLUTION = 8;

void setup() {
  Serial.begin(115200);
  delay(1000);
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(LED_PIN, PWM_CHANNEL);
}

void loop() {
  int raw = analogRead(ADC_PIN);              // 0 ~ 4095 (12bit)
  int duty = map(raw, 0, 4095, 0, 255);        // PWM 8bit 범위로 변환
  ledcWrite(PWM_CHANNEL, duty);
  Serial.printf("adc=%d -> duty=%d\n", raw, duty);
  delay(50);
}
```

## 코드 (core 3.x 이상)

```cpp
#include <Arduino.h>

const int LED_PIN = 4;
const int ADC_PIN = 1;
const int PWM_FREQ = 5000;
const int PWM_RESOLUTION = 8;

void setup() {
  Serial.begin(115200);
  delay(1000);
  ledcAttach(LED_PIN, PWM_FREQ, PWM_RESOLUTION);
}

void loop() {
  int raw = analogRead(ADC_PIN);
  int duty = map(raw, 0, 4095, 0, 255);
  ledcWrite(LED_PIN, duty);
  Serial.printf("adc=%d -> duty=%d\n", raw, duty);
  delay(50);
}
```

## 실행 & 확인

- 가변저항을 돌리면 LED 밝기가 실시간으로 따라 바뀌는지 확인

## 관찰 포인트

- `map()`은 선형 변환만 지원합니다 — 사람 눈은 밝기를 로그 스케일로 인지하므로, `duty = pow(raw / 4095.0, 2) * 255`처럼 감마 보정을 적용하면 더 자연스러운 밝기 변화를 만들 수 있습니다 (선택 실습)
- 이 패턴(센서 입력 → 값 변환 → 출력 반영)은 이후 I2C 센서, 나침반 등에서도 반복해서 나오는 기본 골격입니다

## 체크리스트 — GPIO 섹션(01~08) 총정리

- [ ] 디지털 출력(LED 점멸), 디지털 입력(버튼) 기본 동작 확인
- [ ] 디바운싱, 인터럽트, 롱프레스/더블클릭까지 입력 처리 심화 이해
- [ ] 다중 GPIO를 배열로 관리하는 패턴 이해
- [ ] PWM으로 아날로그 같은 출력, ADC로 아날로그 입력을 다룰 수 있음

## 다음

`09_I2C_BASICS_LAB.md`부터 I2C 통신을 다룹니다.
