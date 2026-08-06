// Source: 02_TASK_PRIORITY_LAB.md
// Section: 코드

#include <Arduino.h>

void LowPriorityTask(void *pvParameters) {
  uint32_t count = 0;
  for (;;) {
    count++;
    if (count % 500000 == 0) {
      Serial.printf("LowPriorityTask: still working (count=%lu)\n", count);
    }
    // 의도적으로 vTaskDelay 없이 계속 도는 CPU-bound 작업 시뮬레이션
    // (04번 실습에서 이 방식의 위험성을 다룹니다 - 여기서는 짧게만 실행)
    if (count > 3000000) {
      vTaskDelay(1);  // watchdog 방지를 위한 최소한의 yield
      count = 0;
    }
  }
}

void HighPriorityTask(void *pvParameters) {
  for (;;) {
    Serial.println("HighPriorityTask: preempting now!");
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  xTaskCreatePinnedToCore(LowPriorityTask, "LowPriorityTask", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(HighPriorityTask, "HighPriorityTask", 2048, NULL, 3, NULL, 1);
  // 두 Task를 일부러 같은 코어(1)에 몰아서, 우선순위 효과가 뚜렷하게 보이도록 함
}

void loop() {
  vTaskDelete(NULL);
}
