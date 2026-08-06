# 21. Producer-Consumer 종합 패턴

## 이 실습에서 배우는 것

FreeRTOS 커리큘럼 21번과 동일한 목표 — Message Queue(10번), Mutex(09번)를 조합해 다중 Producer-다중 Consumer 작업 큐를 만듭니다.

## 코드

```c
#include <zephyr/kernel.h>

#define STACK_SIZE 1024

struct work_item {
    int producer_id;
    int item_id;
};

K_MSGQ_DEFINE(work_msgq, sizeof(struct work_item), 10, 4);
K_MUTEX_DEFINE(stats_mutex);
static uint32_t total_processed = 0;

void producer_entry(void *p1, void *p2, void *p3) {
    int id = (int)(intptr_t)p1;
    int item_counter = 0;
    while (1) {
        struct work_item item = { id, item_counter++ };
        if (k_msgq_put(&work_msgq, &item, K_MSEC(100)) == 0) {
            printk("Producer%d: enqueued item %d\n", id, item.item_id);
        } else {
            printk("Producer%d: queue full, dropped item %d\n", id, item.item_id);
        }
        k_sleep(K_MSEC(300 + id * 100));
    }
}

void consumer_entry(void *p1, void *p2, void *p3) {
    int id = (int)(intptr_t)p1;
    struct work_item item;
    while (1) {
        k_msgq_get(&work_msgq, &item, K_FOREVER);
        printk("Consumer%d: processing item %d from Producer%d\n", id, item.item_id, item.producer_id);
        k_sleep(K_MSEC(200));

        k_mutex_lock(&stats_mutex, K_FOREVER);
        total_processed++;
        k_mutex_unlock(&stats_mutex);
    }
}

void stats_entry(void *p1, void *p2, void *p3) {
    while (1) {
        k_sleep(K_SECONDS(3));
        k_mutex_lock(&stats_mutex, K_FOREVER);
        printk("=== Stats: %u items processed so far ===\n", total_processed);
        k_mutex_unlock(&stats_mutex);
    }
}

K_THREAD_DEFINE(producer1_id, STACK_SIZE, producer_entry, (void *)(intptr_t)1, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(producer2_id, STACK_SIZE, producer_entry, (void *)(intptr_t)2, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(consumer1_id, STACK_SIZE, consumer_entry, (void *)(intptr_t)1, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(consumer2_id, STACK_SIZE, consumer_entry, (void *)(intptr_t)2, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(stats_id, STACK_SIZE, stats_entry, NULL, NULL, NULL, 5, 0, 0);

int main(void) {
    return 0;
}
```

## 실행 & 확인

- 두 Producer가 서로 다른 주기로 항목을 넣고, 두 Consumer가 나눠서 처리하는지 확인
- 3초마다 `=== Stats: N items processed so far ===`가 출력되며 값이 꾸준히 증가하는지 확인

## 관찰 포인트

- `Consumer` 스레드 하나를 제거해보세요 — 소비 속도가 생산 속도를 못 따라가면 `Producer%d: queue full, dropped item`이 나타나기 시작하는지 확인 (10번 실습에서 배운 큐 포화 상황의 실전 재현)
- FreeRTOS 버전과 코드 구조를 나란히 놓고 비교해보면, `xQueueSend`↔`k_msgq_put`, `xQueueReceive`↔`k_msgq_get`, `xSemaphoreTake/Give`↔`k_mutex_lock/unlock`처럼 **개념과 사용 패턴은 거의 1:1로 대응**된다는 걸 알 수 있습니다 — 두 RTOS의 핵심 IPC 개념 자체는 매우 유사하고, 차이는 주로 API 이름과 세부 디테일(우선순위 방향, 컨텍스트 제약 등)에 있습니다

## 다음

22번 파일(`22_ZEPHYR_VS_FREERTOS_LAB.md`)에서 지금까지 배운 내용을 FreeRTOS와 최종 비교하며 정리합니다.
