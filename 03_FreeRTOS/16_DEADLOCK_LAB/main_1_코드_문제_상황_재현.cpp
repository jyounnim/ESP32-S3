// Source: 16_DEADLOCK_LAB.md
// Section: 코드 (문제 상황 재현)

#include <Arduino.h>

SemaphoreHandle_t xMutexX;
SemaphoreHandle_t xMutexY;

void TaskA(void *pvParameters) {
  for (;;) {
    Serial.println("TaskA: taking MutexX");
    xSemaphoreTake(xMutexX, portMAX_DELAY);
    Serial.println("TaskA: got MutexX, now taking MutexY");
    vTaskDelay(pdMS_TO_TICKS(100));   // give TaskB time to grab MutexY first
    xSemaphoreTake(xMutexY, portMAX_DELAY);

    Serial.println("TaskA: got both mutexes, working...");
    xSemaphoreGive(xMutexY);
    xSemaphoreGive(xMutexX);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void TaskB(void *pvParameters) {
  for (;;) {
    Serial.println("TaskB: taking MutexY");
    xSemaphoreTake(xMutexY, portMAX_DELAY);
    Serial.println("TaskB: got MutexY, now taking MutexX");
    vTaskDelay(pdMS_TO_TICKS(100));   // give TaskA time to grab MutexX first
    xSemaphoreTake(xMutexX, portMAX_DELAY);

    Serial.println("TaskB: got both mutexes, working...");
    xSemaphoreGive(xMutexX);
    xSemaphoreGive(xMutexY);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  xMutexX = xSemaphoreCreateMutex();
  xMutexY = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(TaskA, "TaskA", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskB, "TaskB", 2048, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
