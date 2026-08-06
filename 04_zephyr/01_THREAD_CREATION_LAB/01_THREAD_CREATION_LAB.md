# 01. Thread 생성 기초 — K_THREAD_DEFINE / k_thread_create

## 이 실습에서 배우는 것

Zephyr에서 Task에 해당하는 개념은 **Thread**입니다. FreeRTOS의 `xTaskCreate`가 실행 중(런타임)에 Task를 만드는 방식이 기본이라면, Zephyr는 **컴파일 시점에 정적으로 스레드를 정의하는 `K_THREAD_DEFINE`**을 훨씬 더 관용적으로 씁니다. 이 차이 자체가 Zephyr의 설계 철학(정적 구성을 선호)을 보여줍니다.

## 핵심 개념

| 방식 | 설명 |
|---|---|
| `K_THREAD_DEFINE(id, 스택크기, 함수, p1, p2, p3, 우선순위, 옵션, 시작지연)` | 컴파일 시점에 스레드 정의 — `main()` 시작 전부터 스케줄링 대상 |
| `k_thread_create(&data, stack, 스택크기, 함수, p1, p2, p3, 우선순위, 옵션, 시작지연)` | 런타임에 동적으로 스레드 생성 (FreeRTOS의 `xTaskCreate`와 유사) |
| 스택 크기 단위 | **바이트** 단위입니다 — FreeRTOS는 word(4바이트) 단위였다는 점과 다릅니다 |

## 코드

```c
#include <zephyr/kernel.h>

#define STACK_SIZE 1024

void thread_a_entry(void *p1, void *p2, void *p3) {
    while (1) {
        printk("ThreadA running\n");
        k_sleep(K_MSEC(1000));
    }
}

void thread_b_entry(void *p1, void *p2, void *p3) {
    while (1) {
        printk("ThreadB running\n");
        k_sleep(K_MSEC(700));
    }
}

// Static, compile-time thread definitions
K_THREAD_DEFINE(thread_a_id, STACK_SIZE, thread_a_entry, NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(thread_b_id, STACK_SIZE, thread_b_entry, NULL, NULL, NULL, 5, 0, 0);

int main(void) {
    printk("main: started, threads are already running\n");
    return 0;
}
```

## 실행 & 확인

- `west build -b esp32s3_devkitc/esp32s3/procpu` → `west flash` → `west espressif monitor`
- `ThreadA`(1초 간격)와 `ThreadB`(0.7초 간격)가 각자 독립적으로 출력되는지 확인
- **관찰 포인트**: `main: started...` 로그가 `ThreadA`/`ThreadB`의 첫 출력보다 늦게 나올 수도 있습니다 — `K_THREAD_DEFINE`으로 만든 스레드는 `main()`이 실행되기 전부터 이미 스케줄링 대상이기 때문입니다 (FreeRTOS의 `setup()`이 `loopTask`라는 Task 하나로 실행되는 것과 근본적으로 다른 구조입니다)

## 관찰 포인트

- 마지막 인자(시작 지연, delay)를 `0` 대신 `2000`으로 바꿔보세요 — 해당 스레드가 2초 뒤에야 스케줄링 대상이 되는지 확인할 수 있습니다
- `K_THREAD_DEFINE`은 스레드 개수가 코드 작성 시점에 고정되어 있을 때(가장 흔한 경우) 적합합니다. 실행 중 개수가 변하는 상황(작업 큐 워커 등)이라면 03번 실습에서 다루는 `k_thread_create`가 필요합니다

## 다음

02번 파일(`02_THREAD_PRIORITY_LAB.md`)에서 Zephyr의 우선순위 체계와 협조적/선점형 스레드 구분을 다룹니다.
