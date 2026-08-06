# 46. 진동/노크 센서 — SW-420 & Knock Sensor

## 목표

`03`(외부 인터럽트)에서 배운 패턴을 실제 진동 감지에 적용합니다. SW-420(스프링식 진동 스위치)과 노크 센서(압전소자 기반)는 배선/코드가 거의 동일한 디지털 이벤트 센서입니다.

## 준비물

- SW-420 진동 센서 모듈 (보드에 LM393 비교기 내장, 감도 조절 가변저항 있음)
- Knock Sensor 모듈(압전소자 기반)

## 회로

- SW-420 OUT → GPIO5
- Knock Sensor OUT → GPIO6
- 둘 다 VCC → 3.3V, GND → GND

## 코드

```cpp
#include <Arduino.h>

const int SW420_PIN = 5;
const int KNOCK_PIN = 6;

volatile bool vibrationFlag = false;
volatile bool knockFlag = false;
volatile unsigned long lastVibrationIsr = 0;
volatile unsigned long lastKnockIsr = 0;
const unsigned long ISR_DEBOUNCE_MS = 100;

void IRAM_ATTR onVibration() {
  unsigned long now = millis();
  if (now - lastVibrationIsr > ISR_DEBOUNCE_MS) {
    vibrationFlag = true;
    lastVibrationIsr = now;
  }
}

void IRAM_ATTR onKnock() {
  unsigned long now = millis();
  if (now - lastKnockIsr > ISR_DEBOUNCE_MS) {
    knockFlag = true;
    lastKnockIsr = now;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(SW420_PIN, INPUT);
  pinMode(KNOCK_PIN, INPUT);

  attachInterrupt(digitalPinToInterrupt(SW420_PIN), onVibration, RISING);
  attachInterrupt(digitalPinToInterrupt(KNOCK_PIN), onKnock, RISING);

  Serial.println("Ready. Shake the board or knock near the sensor.");
}

void loop() {
  if (vibrationFlag) {
    vibrationFlag = false;
    Serial.println("SW-420: vibration detected");
  }
  if (knockFlag) {
    knockFlag = false;
    Serial.println("Knock sensor: knock detected");
  }
}
```

## 실행 & 확인

- 보드를 흔들면 "vibration detected"가, 노크 센서를 가볍게 두드리면 "knock detected"가 출력되는지 확인
- SW-420 모듈의 가변저항을 돌려 감도(어느 정도 진동부터 반응할지)를 조절해보세요

## 관찰 포인트

- 이 실습은 `03`(외부 인터럽트, 다중 버튼)과 회로/코드 구조가 사실상 동일합니다 — "버튼을 누른다"와 "진동/노크가 발생한다"는 물리적으로는 다른 현상이지만, ESP32 입장에서는 둘 다 "디지털 핀이 순간적으로 HIGH가 됐다"는 같은 이벤트로 처리됩니다. 센서 종류가 달라져도 인터럽트 처리 패턴 자체는 재사용된다는 걸 보여주는 사례입니다
- SW-420은 보드 위 감도 조절 가변저항으로 "얼마나 세게 흔들어야 반응할지"를 하드웨어 레벨에서 조절합니다 — 반면 노크 센서(압전소자)는 보통 세기(진폭) 정보를 아날로그로도 뽑아낼 수 있어(모듈에 따라 다름), `analogRead()`로 "얼마나 세게 두드렸는지"까지 구분하는 것도 가능합니다

### AI 진동/센서 이상 탐지 예제와의 연결

`tflm_examples/03_SENSOR_ANOMALY_LAB.md`에서는 실제 진동 센서가 없어 **가변저항으로 대체**해 Autoencoder 기반 이상 탐지 파이프라인을 구성했습니다. 이제 SW-420(또는 압전 노크 센서)을 실제로 갖고 계시니, 그 예제의 "센서 읽기" 부분을 다음과 같이 바꾸면 훨씬 현실적인 데모가 됩니다.

- **가변저항(아날로그, 연속값)** → **SW-420/노크(디지털 이벤트, 발생 여부)**로 입력 성격이 바뀝니다
- `SAMPLE_WINDOW`(예: 20개 샘플) 동안 **몇 번 이벤트가 발생했는지(카운트)**를 하나의 특징값으로 뽑아 Autoencoder에 넣는 방식으로 바꾸는 게 자연스럽습니다 — 예: "정상 상태에서는 1초에 진동이 0~1회, 비정상(기계 이상 등)일 때는 갑자기 5회 이상" 같은 패턴을 학습
- 노크 센서를 쓴다면 아날로그 세기 값까지 활용해 "진동 발생 여부 + 강도"를 함께 특징으로 사용할 수도 있습니다 (더 풍부한 입력 → 더 정교한 이상 탐지)

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 너무 민감하게(약한 흔들림에도) 반응 | SW-420의 감도 가변저항을 돌려 임계값 조정 |
| 전혀 반응 안 함 | 배선 확인, SW-420은 `RISING`이 아니라 `FALLING`으로 반응하는 모듈도 있음 — 반대로 바꿔서 시도 |

## 다음

`47_ADDITIONAL_ANALOG_SENSORS_NOTE.md` — 아날로그 센서 몇 가지를 간단히 참고 정리합니다 (별도 실습 없이 패턴 재사용).
