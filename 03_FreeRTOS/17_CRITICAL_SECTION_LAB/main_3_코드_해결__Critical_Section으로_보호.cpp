// Source: 17_CRITICAL_SECTION_LAB.md
// Section: 코드 (해결 — Critical Section으로 보호)

#include <Arduino.h>

volatile int32_t sharedCounter = 0;
portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
const int ITERATIONS = 100000;

void ProtectedIncrementTask(void *pvParameters) {
  for (int i = 0; i < ITERATIONS; i++) {
    portENTER_CRITICAL(&spinlock);
    sharedCounter++;
    portEXIT_CRITICAL(&spinlock);
  }
  Serial.printf("Core %d: finished (protected)\n", xPortGetCoreID());
  vTaskDelete(NULL);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  sharedCounter = 0;

  xTaskCreatePinnedToCore(ProtectedIncrementTask, "Core0Task", 2048, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(ProtectedIncrementTask, "Core1Task", 2048, NULL, 1, NULL, 1);

  vTaskDelay(pdMS_TO_TICKS(3000));
  Serial.printf("Expected: %d, Actual: %d\n", ITERATIONS * 2, sharedCounter);
}

void loop() {
  vTaskDelete(NULL);
}
