// Source: 47_ADDITIONAL_ANALOG_SENSORS_NOTE.md
// Section: 공통 패턴 (07번 재사용)

#include <Arduino.h>

const int SENSOR_PIN = 1;   // 실제 사용하는 센서로 교체

void setup() {
  Serial.begin(115200);
  delay(1000);
}

void loop() {
  int raw = analogRead(SENSOR_PIN);
  float voltage = raw * (3.3f / 4095.0f);
  Serial.printf("raw=%d, voltage=%.2fV\n", raw, voltage);
  delay(200);
}
