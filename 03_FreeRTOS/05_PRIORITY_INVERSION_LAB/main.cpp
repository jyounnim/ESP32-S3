// Source: 05_PRIORITY_INVERSION_LAB.md
// Section: 코드

#include <Arduino.h>

SemaphoreHandle_t xLock;   // 일반 Binary Semaphore - 우선순위를 고려하지 않는 잠금

void TaskL(void *pvParameters) {          // Priority 1 (low) - holds the resource
  for (;;) {
    xSemaphoreTake(xLock, portMAX_DELAY);
    Serial.println("TaskL: acquired the resource");
    for (int i = 0; i < 20; i++) {
      for (volatile int j = 0; j < 100000; j++) { }  // simulate real work
      vTaskDelay(1);                                  // watchdog-safe yield
    }
    Serial.println("TaskL: releasing the resource");
    xSemaphoreGive(xLock);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void TaskM(void *pvParameters) {          // Priority 2 (medium) - does NOT need the resource
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(300));
    Serial.println("TaskM: doing unrelated work (preempts TaskL)");
    for (int i = 0; i < 5; i++) {
      for (volatile int j = 0; j < 200000; j++) { }
      vTaskDelay(1);
    }
  }
}

void TaskH(void *pvParameters) {          // Priority 3 (high) - urgently needs the resource
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(1000));
    TickType_t start = xTaskGetTickCount();
    Serial.println("TaskH: requesting the resource...");
    xSemaphoreTake(xLock, portMAX_DELAY);
    TickType_t waitedMs = (xTaskGetTickCount() - start) * portTICK_PERIOD_MS;
    Serial.printf("TaskH: acquired after waiting %lu ms\n", waitedMs);
    xSemaphoreGive(xLock);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  xLock = xSemaphoreCreateBinary();
  xSemaphoreGive(xLock);   // start "available"

  xTaskCreatePinnedToCore(TaskL, "TaskL", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskM, "TaskM", 2048, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskH, "TaskH", 2048, NULL, 3, NULL, 1);
  // 셋 다 같은 코어(1)에 배치 - 선점 효과를 명확하게 보기 위함
}

void loop() {
  vTaskDelete(NULL);
}
