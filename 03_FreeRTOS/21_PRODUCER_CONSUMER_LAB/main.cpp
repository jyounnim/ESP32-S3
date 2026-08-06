// Source: 21_PRODUCER_CONSUMER_LAB.md
// Section: 코드

#include <Arduino.h>

struct WorkItem {
  int producerId;
  int itemId;
};

QueueHandle_t xWorkQueue;
SemaphoreHandle_t xStatsMutex;
uint32_t totalProcessed = 0;

void ProducerTask(void *pvParameters) {
  int id = (int)(intptr_t)pvParameters;
  int itemCounter = 0;
  for (;;) {
    WorkItem item = { id, itemCounter++ };
    if (xQueueSend(xWorkQueue, &item, pdMS_TO_TICKS(100)) == pdTRUE) {
      Serial.printf("Producer%d: enqueued item %d\n", id, item.itemId);
    } else {
      Serial.printf("Producer%d: queue full, dropped item %d\n", id, item.itemId);
    }
    vTaskDelay(pdMS_TO_TICKS(300 + id * 100));   // different producers, different rates
  }
}

void ConsumerTask(void *pvParameters) {
  int id = (int)(intptr_t)pvParameters;
  WorkItem item;
  for (;;) {
    if (xQueueReceive(xWorkQueue, &item, portMAX_DELAY) == pdTRUE) {
      Serial.printf("Consumer%d: processing item %d from Producer%d\n", id, item.itemId, item.producerId);
      vTaskDelay(pdMS_TO_TICKS(200));   // simulate processing time

      xSemaphoreTake(xStatsMutex, portMAX_DELAY);
      totalProcessed++;
      xSemaphoreGive(xStatsMutex);
    }
  }
}

void StatsTask(void *pvParameters) {
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(3000));
    xSemaphoreTake(xStatsMutex, portMAX_DELAY);
    Serial.printf("=== Stats: %lu items processed so far ===\n", totalProcessed);
    xSemaphoreGive(xStatsMutex);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  xWorkQueue = xQueueCreate(10, sizeof(WorkItem));
  xStatsMutex = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(ProducerTask, "Producer1", 2048, (void *)1, 1, NULL, 1);
  xTaskCreatePinnedToCore(ProducerTask, "Producer2", 2048, (void *)2, 1, NULL, 1);
  xTaskCreatePinnedToCore(ConsumerTask, "Consumer1", 2048, (void *)1, 1, NULL, 0);
  xTaskCreatePinnedToCore(ConsumerTask, "Consumer2", 2048, (void *)2, 1, NULL, 0);
  xTaskCreatePinnedToCore(StatsTask, "StatsTask", 2048, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
