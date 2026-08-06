// Source: 08_COUNTING_SEMAPHORE_LAB.md
// Section: 코드

#include <Arduino.h>

#define POOL_SIZE 2
SemaphoreHandle_t xResourcePool;

void WorkerTask(void *pvParameters) {
  int id = (int)(intptr_t)pvParameters;
  for (;;) {
    Serial.printf("Worker%d: waiting for a free slot\n", id);
    xSemaphoreTake(xResourcePool, portMAX_DELAY);
    Serial.printf("Worker%d: acquired a slot, using resource...\n", id);
    vTaskDelay(pdMS_TO_TICKS(2000));   // simulate using the resource
    Serial.printf("Worker%d: releasing the slot\n", id);
    xSemaphoreGive(xResourcePool);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Pool of 2 slots, both available at start
  xResourcePool = xSemaphoreCreateCounting(POOL_SIZE, POOL_SIZE);

  for (int i = 1; i <= 4; i++) {
    xTaskCreatePinnedToCore(WorkerTask, "WorkerTask", 2048, (void *)(intptr_t)i, 1, NULL, 1);
  }
}

void loop() {
  vTaskDelete(NULL);
}
