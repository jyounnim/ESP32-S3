# 02. Priority와 선점형 스케줄링

## 이 실습에서 배우는 것

FreeRTOS는 **선점형(preemptive)** 스케줄러입니다. 우선순위가 높은 Task가 Ready 상태가 되면, 낮은 우선순위 Task가 실행 중이더라도 즉시 그 자리를 빼앗아(선점) 실행됩니다. bare-metal `loop()`에는 이런 개념 자체가 없습니다.

## 핵심 개념

- 숫자가 **클수록 높은 우선순위** (FreeRTOS 기본 규칙, Zephyr와 반대이니 주의)
- 같은 우선순위의 Task끼리는 **time-slice(정해진 시간 단위로 교대)** 방식으로 번갈아 실행
- 더 높은 우선순위 Task가 Ready 상태가 되는 순간, 실행 중이던 낮은 우선순위 Task는 즉시 중단되고 CPU를 빼앗김

## 코드

```cpp
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
```

## 실행 & 확인

- `LowPriorityTask`가 카운트를 열심히 세는 동안에도, 2초마다 `HighPriorityTask`의 "preempting now!"가 정확한 타이밍에 끼어들어 출력되는지 확인
- 두 Task를 같은 코어(1)에 배치했기 때문에, 이 선점 동작은 진짜 병렬 실행이 아니라 **스케줄러가 강제로 교체**해준 것입니다

## 관찰 포인트

- `HighPriorityTask`의 우선순위를 `3`에서 `1`(LowPriorityTask와 동일)로 바꿔보세요 — 이 경우 두 Task는 time-slice로 번갈아 실행되며, 정확히 2초마다 나오던 출력 타이밍이 흐트러지는 걸 관찰할 수 있습니다
- `HighPriorityTask`의 우선순위를 `0`(LowPriorityTask보다 낮음)으로 바꾸면 어떻게 될지 예상해보고 실제로 확인해보세요
- 이 실습은 05번(우선순위 역전)의 기초가 됩니다 — 우선순위가 항상 "높은 게 무조건 좋다"는 뜻은 아니라는 걸 그 실습에서 배우게 됩니다

## 다음

03번 파일(`03_TASK_LIFECYCLE_LAB.md`)에서 Task를 런타임에 동적으로 생성/삭제하는 방법을 다룹니다.
