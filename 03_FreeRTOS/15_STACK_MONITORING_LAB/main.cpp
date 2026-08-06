// Source: 15_STACK_MONITORING_LAB.md
// Section: 코드

#include <Arduino.h>

TaskHandle_t lightHandle;
TaskHandle_t heavyHandle;

void LightTask(void *pvParameters) {
  for (;;) {
    int smallVar = 0;
    smallVar++;
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void recursiveWork(int depth) {
  char buffer[256];   // consumes stack on every call
  memset(buffer, 0, sizeof(buffer));
  if (depth > 0) {
    recursiveWork(depth - 1);
  }
}

void HeavyTask(void *pvParameters) {
  for (;;) {
    recursiveWork(4);   // uses noticeably more stack than LightTask
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void MonitorTask(void *pvParameters) {
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(2000));
    UBaseType_t lightFree = uxTaskGetStackHighWaterMark(lightHandle);
    UBaseType_t heavyFree = uxTaskGetStackHighWaterMark(heavyHandle);
    Serial.printf("Stack headroom (words) - LightTask: %u, HeavyTask: %u\n", lightFree, heavyFree);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  xTaskCreatePinnedToCore(LightTask, "LightTask", 2048, NULL, 1, &lightHandle, 1);
  xTaskCreatePinnedToCore(HeavyTask, "HeavyTask", 2048, NULL, 1, &heavyHandle, 1);
  xTaskCreatePinnedToCore(MonitorTask, "MonitorTask", 2048, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
