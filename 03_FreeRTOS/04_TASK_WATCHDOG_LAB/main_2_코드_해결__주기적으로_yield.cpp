// Source: 04_TASK_WATCHDOG_LAB.md
// Section: 코드 (해결 — 주기적으로 yield)

#include <Arduino.h>

void PoliteTask(void *pvParameters) {
  Serial.println("PoliteTask: started, yielding periodically...");
  uint32_t counter = 0;
  for (;;) {
    counter++;
    if (counter % 1000000 == 0) {
      Serial.printf("PoliteTask: still alive (counter=%lu)\n", counter);
      vTaskDelay(1);   // 최소한의 yield - Idle Task에게 실행 기회를 줌
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  xTaskCreatePinnedToCore(PoliteTask, "PoliteTask", 2048, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
