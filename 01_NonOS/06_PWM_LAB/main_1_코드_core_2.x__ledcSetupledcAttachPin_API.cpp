// Source: 06_PWM_LAB.md
// Section: 코드 (core 2.x — `ledcSetup`/`ledcAttachPin` API)

#include <Arduino.h>

const int LED_PIN = 4;
const int PWM_CHANNEL = 0;
const int PWM_FREQ = 5000;       // 5kHz
const int PWM_RESOLUTION = 8;    // 8bit -> duty 0~255

void setup() {
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(LED_PIN, PWM_CHANNEL);
}

void loop() {
  for (int duty = 0; duty <= 255; duty++) {
    ledcWrite(PWM_CHANNEL, duty);   // 채널 번호로 씀 (핀 번호 아님)
    delay(10);
  }
  for (int duty = 255; duty >= 0; duty--) {
    ledcWrite(PWM_CHANNEL, duty);
    delay(10);
  }
}
