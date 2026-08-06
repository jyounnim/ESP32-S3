// Source: 18_MULTICORE_LAB.md
// Section: 코드

#include <Arduino.h>

const uint32_t WORK_ITERATIONS = 2000000;
volatile uint32_t coreDoneCount = 0;

uint32_t doWork() {
  volatile uint32_t x = 0;
  for (uint32_t i = 0; i < WORK_ITERATIONS; i++) {
    x += i % 7;
  }
  return x;
}

void WorkerTask(void *pvParameters) {
  doWork();
  Serial.printf("WorkerTask on core %d: done\n", xPortGetCoreID());
  coreDoneCount++;
  vTaskDelete(NULL);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // 1) Sequential - do the same total amount of work twice, one after another
  Serial.println("--- Sequential (single core) ---");
  unsigned long t0 = millis();
  doWork();
  doWork();
  unsigned long sequentialMs = millis() - t0;
  Serial.printf("Sequential time: %lu ms\n", sequentialMs);

  // 2) Parallel - split the same total work across both cores
  Serial.println("--- Parallel (dual core) ---");
  coreDoneCount = 0;
  unsigned long t1 = millis();
  xTaskCreatePinnedToCore(WorkerTask, "Worker0", 2048, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(WorkerTask, "Worker1", 2048, NULL, 1, NULL, 1);

  while (coreDoneCount < 2) {
    vTaskDelay(pdMS_TO_TICKS(10));
  }
  unsigned long parallelMs = millis() - t1;
  Serial.printf("Parallel time: %lu ms\n", parallelMs);
  Serial.printf("Speedup: %.2fx\n", (float)sequentialMs / (float)parallelMs);
}

void loop() {
  vTaskDelete(NULL);
}
