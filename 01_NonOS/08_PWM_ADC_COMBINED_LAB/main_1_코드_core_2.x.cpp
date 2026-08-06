// Source: 08_PWM_ADC_COMBINED_LAB.md
// Section: 코드 (core 2.x)

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
