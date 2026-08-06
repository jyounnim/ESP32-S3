# 21. Producer-Consumer 종합 패턴

## 이 실습에서 배우는 것

지금까지 배운 Queue(10번), Mutex(09번), 멀티코어 배치(18번)를 하나로 묶어, 실무에서 정말 흔한 구조인 **다중 Producer - 다중 Consumer(작업 큐, work queue)** 패턴을 만들어봅니다. 여러 생산자가 작업을 큐에 넣고, 여러 소비자가 그 큐에서 각자 꺼내 처리하는 구조입니다.

## 코드

```cpp
#include <Arduino.h>

struct WorkItem {
  int producerId;
  int itemId;
};

QueueHandle_t xWorkQueue;
SemaphoreHandle_t xStatsMutex;
uint32_t totalProcessed = 0;

void ProducerTask(void *pvParameters) {
  int id = (int)(intptr_t)pvParameters;
  int itemCounter = 0;
  for (;;) {
    WorkItem item = { id, itemCounter++ };
    if (xQueueSend(xWorkQueue, &item, pdMS_TO_TICKS(100)) == pdTRUE) {
      Serial.printf("Producer%d: enqueued item %d\n", id, item.itemId);
    } else {
      Serial.printf("Producer%d: queue full, dropped item %d\n", id, item.itemId);
    }
    vTaskDelay(pdMS_TO_TICKS(300 + id * 100));   // different producers, different rates
  }
}

void ConsumerTask(void *pvParameters) {
  int id = (int)(intptr_t)pvParameters;
  WorkItem item;
  for (;;) {
    if (xQueueReceive(xWorkQueue, &item, portMAX_DELAY) == pdTRUE) {
      Serial.printf("Consumer%d: processing item %d from Producer%d\n", id, item.itemId, item.producerId);
      vTaskDelay(pdMS_TO_TICKS(200));   // simulate processing time

      xSemaphoreTake(xStatsMutex, portMAX_DELAY);
      totalProcessed++;
      xSemaphoreGive(xStatsMutex);
    }
  }
}

void StatsTask(void *pvParameters) {
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(3000));
    xSemaphoreTake(xStatsMutex, portMAX_DELAY);
    Serial.printf("=== Stats: %lu items processed so far ===\n", totalProcessed);
    xSemaphoreGive(xStatsMutex);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  xWorkQueue = xQueueCreate(10, sizeof(WorkItem));
  xStatsMutex = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(ProducerTask, "Producer1", 2048, (void *)1, 1, NULL, 1);
  xTaskCreatePinnedToCore(ProducerTask, "Producer2", 2048, (void *)2, 1, NULL, 1);
  xTaskCreatePinnedToCore(ConsumerTask, "Consumer1", 2048, (void *)1, 1, NULL, 0);
  xTaskCreatePinnedToCore(ConsumerTask, "Consumer2", 2048, (void *)2, 1, NULL, 0);
  xTaskCreatePinnedToCore(StatsTask, "StatsTask", 2048, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
```

## 설계 포인트

| 항목 | 설명 |
|---|---|
| Producer 2개를 Core 1에 | 생산 속도는 상대적으로 가벼운 작업이라 한 코어에 몰아도 무방 |
| Consumer 2개를 Core 0에 | 처리(200ms)가 상대적으로 무거운 작업이라 별도 코어로 분리해 생산 측에 영향을 덜 줌 |
| `xWorkQueue` 하나를 Consumer 2개가 공유 | 어느 Consumer든 **먼저 준비된 쪽이** 큐에서 다음 항목을 가져감 — 자동으로 부하가 분산되는 "작업 큐(Thread Pool)" 패턴 |
| `totalProcessed`를 Mutex로 보호 | 두 Consumer가 동시에 값을 증가시킬 수 있으므로 09번에서 배운 대로 상호배제 필요 |

## 실행 & 확인

- 두 Producer가 서로 다른 주기(300ms/400ms 근처)로 항목을 넣고, 두 Consumer가 나눠서 처리하는지 확인
- 3초마다 `=== Stats: N items processed so far ===`가 출력되며 값이 꾸준히 증가하는지 확인
- Producer들의 총 생산 속도와 Consumer들의 총 처리 속도를 비교해보세요 — 지금 설정(Producer 2개, 각각 300~400ms 주기 / Consumer 2개, 각각 200ms 처리)는 소비 속도가 더 빨라서 큐가 거의 안 쌓입니다

## 관찰 포인트

- `ConsumerTask`를 하나만 남기고 주석 처리해보세요 — 소비 속도가 생산 속도를 못 따라가면 `Producer%d: queue full, dropped item`이 나타나기 시작하는지 확인 (10번 실습에서 배운 큐 포화 상황의 실전 재현)
- 이 구조는 웹 서버의 요청 처리, 로그 수집·처리, 실시간 센서 데이터 처리 파이프라인 등 정말 다양한 실무 상황에 그대로 응용됩니다 — "여러 곳에서 일이 들어오고, 여러 워커가 나눠 처리하고, 처리 결과를 안전하게 집계"하는 구조는 사실상 이 실습의 변형입니다

## 다음

22번 파일(`22_FREERTOS_VS_BAREMETAL_LAB.md`)에서 지금까지 배운 내용을 bare-metal 방식과 나란히 비교하며 정리합니다.
