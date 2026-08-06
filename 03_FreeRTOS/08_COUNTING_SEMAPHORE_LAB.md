# 08. Counting Semaphore — 자원 풀(Pool) 관리

## 이 실습에서 배우는 것

Binary Semaphore는 "0 아니면 1"이라 자원이 딱 하나뿐인 상황(또는 단순 신호)에만 맞습니다. 만약 **동시에 N개까지는 허용**하는 자원 풀(예: 동시 접속 슬롯, 동시 사용 가능한 버퍼 개수)을 관리하고 싶다면 **Counting Semaphore**를 씁니다.

## 핵심 개념

```cpp
SemaphoreHandle_t sem = xSemaphoreCreateCounting(maxCount, initialCount);
```

- `maxCount`: 세마포어가 가질 수 있는 최대값(=동시에 허용할 자원 개수)
- `initialCount`: 시작 시점에 사용 가능한 자원 개수
- `xSemaphoreTake`/`xSemaphoreGive`는 Binary Semaphore와 동일하게 사용하지만, 내부 카운트가 0이 될 때까지는 여러 Task가 동시에 Take에 성공할 수 있습니다

## 코드

```cpp
#include <Arduino.h>

#define POOL_SIZE 2
SemaphoreHandle_t xResourcePool;

void WorkerTask(void *pvParameters) {
  int id = (int)(intptr_t)pvParameters;
  for (;;) {
    Serial.printf("Worker%d: waiting for a free slot\n", id);
    xSemaphoreTake(xResourcePool, portMAX_DELAY);
    Serial.printf("Worker%d: acquired a slot, using resource...\n", id);
    vTaskDelay(pdMS_TO_TICKS(2000));   // simulate using the resource
    Serial.printf("Worker%d: releasing the slot\n", id);
    xSemaphoreGive(xResourcePool);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Pool of 2 slots, both available at start
  xResourcePool = xSemaphoreCreateCounting(POOL_SIZE, POOL_SIZE);

  for (int i = 1; i <= 4; i++) {
    xTaskCreatePinnedToCore(WorkerTask, "WorkerTask", 2048, (void *)(intptr_t)i, 1, NULL, 1);
  }
}

void loop() {
  vTaskDelete(NULL);
}
```

## 실행 & 확인

- Worker 4개가 동시에 자원을 요청하지만, **한 번에 2개까지만** "acquired a slot"이 되는지 확인
- 나머지 2개는 "waiting for a free slot"에 머물다가, 먼저 쓰던 Worker가 "releasing the slot" 한 직후에야 자원을 획득하는지 확인

## 관찰 포인트

- `POOL_SIZE`를 1로 바꾸면 사실상 Binary Semaphore(=Mutex와 유사한 상호배제)와 동일하게 동작합니다 — Counting Semaphore는 Binary Semaphore를 포함하는 더 일반화된 개념입니다
- `POOL_SIZE`를 4(Worker 개수와 동일)로 늘리면 아무도 대기하지 않고 모두 즉시 자원을 획득하는지 확인해보세요
- 실무 활용 예: DB 커넥션 풀, 동시에 열 수 있는 파일 핸들 개수 제한, 동시 HTTP 요청 개수 제한 등

## 다음

09번 파일(`09_MUTEX_LAB.md`)에서 Mutex와 Binary Semaphore의 차이, 그리고 05번에서 본 우선순위 역전을 완화하는 방법을 다룹니다.
