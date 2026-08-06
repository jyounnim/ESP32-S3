# 07. ADC로 아날로그 값 읽기

## 목표

가변저항(또는 조도센서)의 아날로그 전압을 ADC로 읽어 시리얼로 출력합니다.

## 준비물

- 가변저항(potentiometer) 1개

## 회로

- 가변저항 양 끝 → 3.3V, GND
- 가변저항 가운데(wiper) 핀 → GPIO1 (ADC1_CH0)

> ⚠️ ESP32-S3의 ADC 입력 전압 범위는 최대 3.3V입니다. 5V 신호를 직접 연결하지 마세요.
> ⚠️ Wi-Fi를 함께 사용할 계획이라면 ADC2(GPIO11~20 일부)가 아닌 **ADC1(GPIO1~10)** 채널을 쓰는 걸 권장합니다 — ADC2는 Wi-Fi 드라이버와 리소스를 공유해 값이 불안정해질 수 있습니다.

## 코드

```cpp
#include <Arduino.h>

const int ADC_PIN = 1;   // GPIO1, ADC1_CH0

void setup() {
  Serial.begin(115200);
  delay(1000);
  // ESP32-S3 기본 ADC 해상도는 12bit(0~4095)
}

void loop() {
  int raw = analogRead(ADC_PIN);
  float voltage = raw * (3.3f / 4095.0f);
  Serial.printf("raw=%d, voltage=%.2fV\n", raw, voltage);
  delay(200);
}
```

## 실행 & 확인

- 가변저항을 돌리면서 raw 값(0~4095)과 voltage 값이 따라 변하는지 확인

## 관찰 포인트

- `analogReadResolution(10)`을 `setup()`에 추가해 해상도를 10bit(0~1023)로 낮춰보고 raw 값 범위가 어떻게 바뀌는지 확인
- ESP32-S3의 ADC는 비선형성이 있어 극단값(0V 근처, 3.3V 근처)에서 오차가 커질 수 있습니다 — 멀티미터로 실측값과 비교해보세요

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 값이 항상 0 또는 4095 근처 고정 | 가변저항 배선 확인 (wiper가 아닌 양 끝 단자를 GPIO에 연결한 경우 흔함) |
| 값이 불안정하게 튐(noise) | 배선 접촉 불량, 또는 Wi-Fi 사용 중인 ADC2 채널 — ADC1(GPIO1~10)로 교체 권장 |

## 다음

`08_PWM_ADC_COMBINED_LAB.md` — PWM과 ADC를 결합해 가변저항으로 LED 밝기를 실시간 조절합니다.
