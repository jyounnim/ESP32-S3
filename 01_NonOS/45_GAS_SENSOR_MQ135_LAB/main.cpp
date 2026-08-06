// Source: 45_GAS_SENSOR_MQ135_LAB.md
// Section: 코드

#include <Arduino.h>

const int MQ135_PIN = 1;
const unsigned long WARMUP_MS = 20000;   // 최소 20초, 제대로 하려면 24~48시간 예열 권장(아래 참고)

unsigned long startTime;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Warming up MQ-135 sensor...");
  startTime = millis();
}

void loop() {
  unsigned long elapsed = millis() - startTime;

  if (elapsed < WARMUP_MS) {
    Serial.printf("Warming up... %lus remaining\n", (WARMUP_MS - elapsed) / 1000);
    delay(1000);
    return;
  }

  int raw = analogRead(MQ135_PIN);
  float voltage = raw * (3.3f / 4095.0f);
  Serial.printf("raw=%d  voltage=%.2fV  (higher = more gas detected, relative value)\n", raw, voltage);
  delay(1000);
}
