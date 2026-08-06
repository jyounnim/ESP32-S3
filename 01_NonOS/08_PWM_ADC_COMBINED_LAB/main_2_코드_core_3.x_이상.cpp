// Source: 08_PWM_ADC_COMBINED_LAB.md
// Section: 코드 (core 3.x 이상)

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
