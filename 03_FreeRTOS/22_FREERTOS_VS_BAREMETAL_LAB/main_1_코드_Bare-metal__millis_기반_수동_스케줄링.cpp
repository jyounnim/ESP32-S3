// Source: 22_FREERTOS_VS_BAREMETAL_LAB.md
// Section: 코드 (Bare-metal — millis() 기반 수동 스케줄링)

#include <Arduino.h>

const int LED_PIN = 4;

unsigned long lastBlinkTime = 0;
unsigned long lastStatusTime = 0;
unsigned long lastCheckTime = 0;
bool ledState = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("Bare-metal version started");
}

void loop() {
  unsigned long now = millis();

  if (now - lastBlinkTime >= 500) {
    lastBlinkTime = now;
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
  }

  if (now - lastStatusTime >= 2000) {
    lastStatusTime = now;
    Serial.println("Bare-metal: status report");
  }

  if (now - lastCheckTime >= 100) {
    lastCheckTime = now;
    // some lightweight condition check would go here
  }

  // 문제: 위 세 가지 작업 중 하나라도 오래 걸리는 코드가 끼어들면
  // (예: 블로킹 HTTP 요청, 긴 delay()) 나머지 전부가 함께 지연된다
}
