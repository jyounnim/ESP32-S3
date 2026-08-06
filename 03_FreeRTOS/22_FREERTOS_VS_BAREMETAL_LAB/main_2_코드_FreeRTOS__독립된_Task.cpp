// Source: 22_FREERTOS_VS_BAREMETAL_LAB.md
// Section: 코드 (FreeRTOS — 독립된 Task)

#include <Arduino.h>

const int LED_PIN = 4;

void BlinkTask(void *pvParameters) {
  bool ledState = false;
  for (;;) {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void StatusTask(void *pvParameters) {
  for (;;) {
    Serial.println("FreeRTOS: status report");
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void CheckTask(void *pvParameters) {
  for (;;) {
    // some lightweight condition check would go here
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("FreeRTOS version started");

  xTaskCreatePinnedToCore(BlinkTask, "BlinkTask", 2048, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(StatusTask, "StatusTask", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(CheckTask, "CheckTask", 2048, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
