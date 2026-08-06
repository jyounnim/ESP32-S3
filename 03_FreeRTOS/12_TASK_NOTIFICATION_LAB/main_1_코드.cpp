// Source: 12_TASK_NOTIFICATION_LAB.md
// Section: 코드

#include <Arduino.h>

TaskHandle_t consumerHandle = NULL;

void ProducerTask(void *pvParameters) {
  uint32_t counter = 0;
  for (;;) {
    counter++;
    if (consumerHandle != NULL) {
      xTaskNotify(consumerHandle, counter, eSetValueWithOverwrite);
      Serial.printf("ProducerTask: notified value=%lu\n", counter);
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void ConsumerTask(void *pvParameters) {
  uint32_t receivedValue;
  for (;;) {
    if (xTaskNotifyWait(0, 0xFFFFFFFF, &receivedValue, portMAX_DELAY) == pdTRUE) {
      Serial.printf("ConsumerTask: received value=%lu\n", receivedValue);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  xTaskCreatePinnedToCore(ConsumerTask, "ConsumerTask", 2048, NULL, 1, &consumerHandle, 1);
  xTaskCreatePinnedToCore(ProducerTask, "ProducerTask", 2048, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
