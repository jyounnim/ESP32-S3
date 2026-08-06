# 09. Mutex vs Binary Semaphore — Priority Inheritance

## 이 실습에서 배우는 것

Mutex(Mutual Exclusion)는 겉보기엔 Binary Semaphore와 API가 거의 같지만, **"소유자(owner)" 개념**이 있다는 결정적 차이가 있습니다. 이 소유자 개념 덕분에 FreeRTOS의 Mutex는 **Priority Inheritance(우선순위 상속)**를 지원해서, 05번 실습에서 본 우선순위 역전 문제를 완화할 수 있습니다.

## 핵심 개념

| 항목 | Binary Semaphore | Mutex |
|---|---|---|
| 생성 함수 | `xSemaphoreCreateBinary()` | `xSemaphoreCreateMutex()` |
| 생성 직후 상태 | "잠김" 상태 — 최초 1회 `xSemaphoreGive()`로 열어줘야 함 | 곧바로 "사용 가능" 상태 |
| 소유자 개념 | 없음 — 누가 Give/Take 했는지 추적 안 함 | 있음 — Take한 Task가 소유자로 기록됨 |
| Priority Inheritance | 지원 안 함 | 지원함 — 소유자가 더 높은 우선순위 Task를 기다리게 하면, 그 동안 일시적으로 소유자의 우선순위를 끌어올림 |
| 주 용도 | 이벤트/신호 전달, Task↔ISR 동기화 | 공유 자원의 상호배제(mutual exclusion) |

## 코드

05번 실습 코드에서 `xLock`의 타입만 Binary Semaphore → Mutex로 바꾼 버전입니다.

```cpp
#include <Arduino.h>

SemaphoreHandle_t xLock;   // 이번엔 Mutex - 소유자 개념과 Priority Inheritance 지원

void TaskL(void *pvParameters) {          // Priority 1 (low) - holds the resource
  for (;;) {
    xSemaphoreTake(xLock, portMAX_DELAY);
    Serial.println("TaskL: acquired the resource");
    for (int i = 0; i < 20; i++) {
      for (volatile int j = 0; j < 100000; j++) { }
      vTaskDelay(1);
    }
    Serial.println("TaskL: releasing the resource");
    xSemaphoreGive(xLock);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void TaskM(void *pvParameters) {          // Priority 2 (medium) - does NOT need the resource
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(300));
    Serial.println("TaskM: doing unrelated work (tries to preempt TaskL)");
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

  xLock = xSemaphoreCreateMutex();   // 생성 즉시 사용 가능한 상태 (Give 불필요)

  xTaskCreatePinnedToCore(TaskL, "TaskL", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskM, "TaskM", 2048, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskH, "TaskH", 2048, NULL, 3, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
```

## 실행 & 확인

- `TaskH: acquired after waiting XXX ms` 값을 05번 실습(Binary Semaphore 버전)의 결과와 비교해보세요 — Mutex 버전에서는 대기 시간이 더 짧고 일정해야 합니다
- 내부적으로 무슨 일이 일어나는지: `TaskH`가 락을 기다리기 시작하면, FreeRTOS는 현재 락을 쥐고 있는 `TaskL`의 우선순위를 일시적으로 `TaskH`와 같은 수준(3)으로 끌어올립니다. 이 상태에서는 우선순위 2인 `TaskM`이 더 이상 `TaskL`을 선점할 수 없습니다. `TaskL`이 락을 놓으면 원래 우선순위(1)로 즉시 복귀합니다

## 관찰 포인트

- Priority Inheritance는 우선순위 역전을 **완전히 없애주는 게 아니라 "제한(bound)"** 해준다는 점이 중요합니다 — `TaskH`는 여전히 `TaskL`이 자원을 다 쓸 때까지는 기다려야 하지만(그건 당연한 동기화), 적어도 `TaskM` 때문에 무한정 더 길어지는 일은 막아줍니다
- Mutex는 "소유자가 있는 잠금"이라는 의미상, **자신이 Take한 Mutex는 반드시 자기 자신이 Give해야** 합니다 — ISR에서 Mutex를 Give/Take하는 것은 지원되지 않습니다(소유자 개념이 ISR엔 맞지 않음). ISR과의 동기화에는 반드시 07번 실습처럼 Binary/Counting Semaphore를 쓰세요
- 일반적인 규칙: **"자원을 상호배제로 보호하고 싶다" → Mutex**, **"이벤트/신호를 전달하고 싶다(특히 ISR에서)" → Binary/Counting Semaphore**

## 다음

10번 파일(`10_QUEUE_BASICS_LAB.md`)에서 신호가 아닌 실제 데이터를 Task 간에 전달하는 Queue를 다룹니다.
