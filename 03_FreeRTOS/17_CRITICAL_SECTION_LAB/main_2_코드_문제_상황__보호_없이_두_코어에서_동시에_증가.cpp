// Source: 17_CRITICAL_SECTION_LAB.md
// Section: 코드 (문제 상황 — 보호 없이 두 코어에서 동시에 증가)

#include <Arduino.h>

volatile int32_t sharedCounter = 0;
const int ITERATIONS = 100000;

void UnprotectedIncrementTask(void *pvParameters) {
  for (int i = 0; i < ITERATIONS; i++) {
    sharedCounter++;   // read-modify-write - NOT atomic across cores
  }
  Serial.printf("Core %d: finished (unprotected)\n", xPortGetCoreID());
  vTaskDelete(NULL);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  sharedCounter = 0;

  xTaskCreatePinnedToCore(UnprotectedIncrementTask, "Core0Task", 2048, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(UnprotectedIncrementTask, "Core1Task", 2048, NULL, 1, NULL, 1);

  vTaskDelay(pdMS_TO_TICKS(3000));   // wait for both tasks to finish
  Serial.printf("Expected: %d, Actual: %d\n", ITERATIONS * 2, sharedCounter);
}

void loop() {
  vTaskDelete(NULL);
}
