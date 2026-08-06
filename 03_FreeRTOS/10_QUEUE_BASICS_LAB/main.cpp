// Source: 10_QUEUE_BASICS_LAB.md
// Section: 코드

#include <Arduino.h>

struct SensorData {
  int id;
  float value;
};

QueueHandle_t xDataQueue;

void SensorTask(void *pvParameters) {     // Sender
  int counter = 0;
  for (;;) {
    SensorData data;
    data.id = counter++;
    data.value = 20.0f + (counter % 10) * 0.5f;   // simulate a changing sensor reading

    if (xQueueSend(xDataQueue, &data, pdMS_TO_TICKS(100)) == pdTRUE) {
      Serial.printf("SensorTask: sent id=%d value=%.1f\n", data.id, data.value);
    } else {
      Serial.println("SensorTask: queue full, send failed");
    }
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void ProcessTask(void *pvParameters) {    // Receiver
  SensorData received;
  for (;;) {
    if (xQueueReceive(xDataQueue, &received, portMAX_DELAY) == pdTRUE) {
      Serial.printf("ProcessTask: received id=%d value=%.1f\n", received.id, received.value);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  xDataQueue = xQueueCreate(5, sizeof(SensorData));   // holds up to 5 SensorData items

  xTaskCreatePinnedToCore(SensorTask, "SensorTask", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(ProcessTask, "ProcessTask", 2048, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
