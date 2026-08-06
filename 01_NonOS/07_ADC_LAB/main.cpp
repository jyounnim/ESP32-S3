// Source: 07_ADC_LAB.md
// Section: 코드

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
