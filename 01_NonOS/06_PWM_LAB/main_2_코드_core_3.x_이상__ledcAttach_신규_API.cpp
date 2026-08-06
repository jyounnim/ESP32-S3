// Source: 06_PWM_LAB.md
// Section: 코드 (core 3.x 이상 — `ledcAttach` 신규 API)

#include <Arduino.h>

const int LED_PIN = 4;
const int PWM_FREQ = 5000;
const int PWM_RESOLUTION = 8;

void setup() {
  ledcAttach(LED_PIN, PWM_FREQ, PWM_RESOLUTION);   // 채널 관리 불필요, 핀 번호로 바로 사용
}

void loop() {
  for (int duty = 0; duty <= 255; duty++) {
    ledcWrite(LED_PIN, duty);   // 핀 번호로 씀
    delay(10);
  }
  for (int duty = 255; duty >= 0; duty--) {
    ledcWrite(LED_PIN, duty);
    delay(10);
  }
}
