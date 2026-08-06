// Source: 16_DEADLOCK_LAB.md
// Section: 코드 (해결 — 잠금 순서 통일)

void TaskB(void *pvParameters) {
  for (;;) {
    Serial.println("TaskB: taking MutexX");
    xSemaphoreTake(xMutexX, portMAX_DELAY);   // TaskA와 동일하게 X를 먼저
    Serial.println("TaskB: got MutexX, now taking MutexY");
    vTaskDelay(pdMS_TO_TICKS(100));
    xSemaphoreTake(xMutexY, portMAX_DELAY);

    Serial.println("TaskB: got both mutexes, working...");
    xSemaphoreGive(xMutexY);
    xSemaphoreGive(xMutexX);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
