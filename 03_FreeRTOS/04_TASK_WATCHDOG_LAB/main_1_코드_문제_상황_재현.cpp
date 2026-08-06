// Source: 04_TASK_WATCHDOG_LAB.md
// Section: 코드 (문제 상황 재현)

#include <Arduino.h>

void GreedyTask(void *pvParameters) {
  Serial.println("GreedyTask: started, hogging the CPU with no yield...");
  uint32_t counter = 0;
  for (;;) {
    counter++;   // yield 없는 순수 계산 반복 - Idle Task가 실행될 기회를 주지 않음
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  xTaskCreatePinnedToCore(GreedyTask, "GreedyTask", 2048, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
