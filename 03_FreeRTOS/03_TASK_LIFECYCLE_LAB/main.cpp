// Source: 03_TASK_LIFECYCLE_LAB.md
// Section: 코드

#include <Arduino.h>

TaskHandle_t workerHandle = NULL;

void WorkerTask(void *pvParameters) {
  int jobId = (int)(intptr_t)pvParameters;
  Serial.printf("WorkerTask #%d: started\n", jobId);

  for (int i = 0; i < 5; i++) {
    Serial.printf("WorkerTask #%d: working... (%d/5)\n", jobId, i + 1);
    vTaskDelay(pdMS_TO_TICKS(500));
  }

  Serial.printf("WorkerTask #%d: done, deleting self\n", jobId);
  workerHandle = NULL;
  vTaskDelete(NULL);   // 자기 자신을 삭제 (여기서 함수가 끝나지 않고 즉시 종료됨)
}

void ManagerTask(void *pvParameters) {
  int jobCounter = 0;
  for (;;) {
    if (workerHandle == NULL) {
      jobCounter++;
      Serial.printf("ManagerTask: spawning WorkerTask #%d\n", jobCounter);
      BaseType_t result = xTaskCreate(
        WorkerTask, "WorkerTask", 2048,
        (void *)(intptr_t)jobCounter, 1, &workerHandle
      );
      if (result != pdPASS) {
        Serial.println("ManagerTask: failed to create WorkerTask (out of memory?)");
      }
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  xTaskCreatePinnedToCore(ManagerTask, "ManagerTask", 2048, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
