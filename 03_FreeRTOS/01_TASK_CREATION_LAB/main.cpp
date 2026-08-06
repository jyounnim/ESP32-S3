// Source: 01_TASK_CREATION_LAB.md
// Section: 코드

#include <Arduino.h>

void TaskA(void *pvParameters) {
  for (;;) {
    Serial.printf("TaskA running on core %d\n", xPortGetCoreID());
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void TaskB(void *pvParameters) {
  for (;;) {
    Serial.printf("TaskB running on core %d\n", xPortGetCoreID());
    vTaskDelay(pdMS_TO_TICKS(700));
  }
}

void TaskC(void *pvParameters) {
  for (;;) {
    Serial.printf("TaskC running on core %d\n", xPortGetCoreID());
    vTaskDelay(pdMS_TO_TICKS(400));
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // 코어 미지정 - 스케줄러가 자동 배치
  xTaskCreate(TaskA, "TaskA", 2048, NULL, 1, NULL);

  // 코어 명시적 고정
  xTaskCreatePinnedToCore(TaskB, "TaskB", 2048, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(TaskC, "TaskC", 2048, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);   // 기본 loop Task는 더 이상 사용하지 않음
}
