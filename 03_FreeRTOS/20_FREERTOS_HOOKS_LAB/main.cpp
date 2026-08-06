// Source: 20_FREERTOS_HOOKS_LAB.md
// Section: 코드

#include <Arduino.h>
#include "esp_freertos_hooks.h"

volatile uint32_t idleCount[2] = {0, 0};

bool IRAM_ATTR idleHookCore0() {
  idleCount[0]++;
  return true;   // call once per tick
}

bool IRAM_ATTR idleHookCore1() {
  idleCount[1]++;
  return true;
}

void ReporterTask(void *pvParameters) {
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(1000));
    Serial.printf("Idle hook calls in the last second - Core0: %lu, Core1: %lu\n", idleCount[0], idleCount[1]);
    idleCount[0] = 0;
    idleCount[1] = 0;
  }
}

void BusyTask(void *pvParameters) {
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(3000));
    Serial.println("BusyTask: starting 1s burst on core 1");
    unsigned long start = millis();
    while (millis() - start < 1000) { }
    Serial.println("BusyTask: burst done");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  esp_register_freertos_idle_hook_for_cpu(idleHookCore0, 0);
  esp_register_freertos_idle_hook_for_cpu(idleHookCore1, 1);

  xTaskCreatePinnedToCore(ReporterTask, "ReporterTask", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(BusyTask, "BusyTask", 2048, NULL, 2, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
