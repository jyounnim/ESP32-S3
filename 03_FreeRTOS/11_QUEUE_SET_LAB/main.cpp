// Source: 11_QUEUE_SET_LAB.md
// Section: 코드

#include <Arduino.h>

QueueHandle_t xTempQueue;
QueueHandle_t xHumidityQueue;
QueueSetHandle_t xSensorQueueSet;

void TempSensorTask(void *pvParameters) {
  float temp = 20.0f;
  for (;;) {
    temp += 0.3f;
    xQueueSend(xTempQueue, &temp, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(1500));
  }
}

void HumiditySensorTask(void *pvParameters) {
  float humidity = 40.0f;
  for (;;) {
    humidity += 1.0f;
    xQueueSend(xHumidityQueue, &humidity, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(900));
  }
}

void DispatcherTask(void *pvParameters) {   // one task, watching two queues at once
  for (;;) {
    QueueSetMemberHandle_t activated = xQueueSelectFromSet(xSensorQueueSet, portMAX_DELAY);

    if (activated == xTempQueue) {
      float value;
      xQueueReceive(xTempQueue, &value, 0);
      Serial.printf("DispatcherTask: [TEMP] %.1f C\n", value);
    } else if (activated == xHumidityQueue) {
      float value;
      xQueueReceive(xHumidityQueue, &value, 0);
      Serial.printf("DispatcherTask: [HUMIDITY] %.1f %%\n", value);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  xTempQueue = xQueueCreate(3, sizeof(float));
  xHumidityQueue = xQueueCreate(3, sizeof(float));

  xSensorQueueSet = xQueueCreateSet(3 + 3);   // capacity >= sum of member queue lengths
  xQueueAddToSet(xTempQueue, xSensorQueueSet);
  xQueueAddToSet(xHumidityQueue, xSensorQueueSet);

  xTaskCreatePinnedToCore(TempSensorTask, "TempSensorTask", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(HumiditySensorTask, "HumiditySensorTask", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(DispatcherTask, "DispatcherTask", 2048, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
