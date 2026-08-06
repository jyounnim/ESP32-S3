// Source: 06_IDLE_TASK_LAB.md
// Section: 코드

#include <Arduino.h>

volatile uint32_t idleCounter = 0;

void IdleCounterTask(void *pvParameters) {   // Priority 0 - same level as the system Idle Task
  for (;;) {
    idleCounter++;
  }
}

void ReporterTask(void *pvParameters) {      // Priority 1
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(1000));
    Serial.printf("IdleCounterTask incremented %lu times in the last second\n", idleCounter);
    idleCounter = 0;
  }
}

void BusyBurstTask(void *pvParameters) {     // Priority 2 - occasionally hogs the CPU
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(3000));
    Serial.println("BusyBurstTask: starting a 1s CPU burst");
    TickType_t start = xTaskGetTickCount();
    while ((xTaskGetTickCount() - start) < pdMS_TO_TICKS(1000)) {
      // pure busy work - higher priority, so it preempts IdleCounterTask/ReporterTask
    }
    Serial.println("BusyBurstTask: burst done");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  xTaskCreatePinnedToCore(IdleCounterTask, "IdleCounterTask", 2048, NULL, 0, NULL, 1);
  xTaskCreatePinnedToCore(ReporterTask, "ReporterTask", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(BusyBurstTask, "BusyBurstTask", 2048, NULL, 2, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
