# 05. 우선순위 역전(Priority Inversion) 재현

## 이 실습에서 배우는 것

"우선순위가 높으면 무조건 먼저, 빨리 실행된다"는 직관이 항상 맞지는 않습니다. **공유 자원을 낮은 우선순위 Task가 붙잡고 있는 상황**에서는, 높은 우선순위 Task가 그 자원을 기다리며 예상보다 훨씬 오래 지연될 수 있습니다. 이를 **우선순위 역전**이라 부르며, 02번 실습에서 배운 "선점형 스케줄링"의 어두운 면입니다.

## 시나리오 (H/M/L 패턴)

- **TaskL** (우선순위 1, 낮음): 공유 자원(Lock)을 차지하고 작업
- **TaskM** (우선순위 2, 중간): 공유 자원과 **무관한** 작업이지만, 우선순위가 TaskL보다 높아서 TaskL을 계속 선점
- **TaskH** (우선순위 3, 높음): 공유 자원이 필요한데, TaskL이 붙잡고 있어서 대기 → 그런데 TaskL은 TaskM에게 계속 선점당해서 자원을 좀처럼 놓지 못함 → 결과적으로 **가장 우선순위가 높은 TaskH가 가장 오래 기다리는** 역설적인 상황 발생

## 코드

```cpp
#include <Arduino.h>

SemaphoreHandle_t xLock;   // 일반 Binary Semaphore - 우선순위를 고려하지 않는 잠금

void TaskL(void *pvParameters) {          // Priority 1 (low) - holds the resource
  for (;;) {
    xSemaphoreTake(xLock, portMAX_DELAY);
    Serial.println("TaskL: acquired the resource");
    for (int i = 0; i < 20; i++) {
      for (volatile int j = 0; j < 100000; j++) { }  // simulate real work
      vTaskDelay(1);                                  // watchdog-safe yield
    }
    Serial.println("TaskL: releasing the resource");
    xSemaphoreGive(xLock);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void TaskM(void *pvParameters) {          // Priority 2 (medium) - does NOT need the resource
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(300));
    Serial.println("TaskM: doing unrelated work (preempts TaskL)");
    for (int i = 0; i < 5; i++) {
      for (volatile int j = 0; j < 200000; j++) { }
      vTaskDelay(1);
    }
  }
}

void TaskH(void *pvParameters) {          // Priority 3 (high) - urgently needs the resource
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(1000));
    TickType_t start = xTaskGetTickCount();
    Serial.println("TaskH: requesting the resource...");
    xSemaphoreTake(xLock, portMAX_DELAY);
    TickType_t waitedMs = (xTaskGetTickCount() - start) * portTICK_PERIOD_MS;
    Serial.printf("TaskH: acquired after waiting %lu ms\n", waitedMs);
    xSemaphoreGive(xLock);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  xLock = xSemaphoreCreateBinary();
  xSemaphoreGive(xLock);   // start "available"

  xTaskCreatePinnedToCore(TaskL, "TaskL", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskM, "TaskM", 2048, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskH, "TaskH", 2048, NULL, 3, NULL, 1);
  // 셋 다 같은 코어(1)에 배치 - 선점 효과를 명확하게 보기 위함
}

void loop() {
  vTaskDelete(NULL);
}
```

## 실행 & 확인

- `TaskH: acquired after waiting XXX ms` 로그의 대기 시간을 확인하세요 — TaskM이 계속 끼어들면서 TaskL이 자원을 오래 붙잡고 있어, TaskH가 우선순위 1위임에도 수백ms 이상 대기하는 걸 볼 수 있습니다
- 비교를 위해 `TaskM` 생성 코드를 잠시 주석 처리하고 다시 실행해보세요 — TaskM의 방해가 없으면 TaskH의 대기 시간이 훨씬 짧고 일정해지는 걸 확인할 수 있습니다

## 관찰 포인트

- 지금 사용한 `xSemaphoreCreateBinary()`는 "누가 자원을 갖고 있는지"에 대한 개념이 없는 단순 신호 장치입니다 — 그래서 스케줄러가 "TaskH가 기다리고 있으니 TaskL을 잠깐 승격시켜주자" 같은 배려를 해줄 수 없습니다
- 이 문제를 해결하는 표준적인 방법이 **Mutex의 우선순위 상속(Priority Inheritance)**입니다 — TaskL이 락을 쥔 동안 일시적으로 TaskH와 같은 우선순위로 격상시켜, TaskM이 끼어들지 못하게 막는 방식입니다. 09번 실습(`09_MUTEX_LAB.md`)에서 `xSemaphoreCreateMutex()`로 바꿔서 이 문제가 실제로 완화되는지 직접 비교합니다
- 실무에서는 "공유 자원을 오래 붙잡는 로직"을 최대한 짧게 만드는 것도 우선순위 역전을 줄이는 근본적인 방법입니다

## 다음

06번 파일(`06_IDLE_TASK_LAB.md`)에서 아무 Task도 할 일이 없을 때 스케줄러가 무엇을 실행하는지 다룹니다.
