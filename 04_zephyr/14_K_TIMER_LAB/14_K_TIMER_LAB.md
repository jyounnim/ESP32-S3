# 14. k_timer (One-shot / Periodic)

## 이 실습에서 배우는 것

Zephyr의 `k_timer`는 FreeRTOS의 Software Timer와 목적은 같지만, **콜백이 실행되는 컨텍스트가 근본적으로 다릅니다.** FreeRTOS는 전용 "Timer Service Task"(스레드 컨텍스트)에서 콜백을 실행하지만, **Zephyr는 시스템 클럭 인터럽트 핸들러, 즉 ISR 컨텍스트에서 콜백을 실행**합니다. 이 차이를 모르고 FreeRTOS 스타일로 코드를 짜면 크래시로 이어질 수 있습니다.

## 핵심 개념

| 함수 | 설명 |
|---|---|
| `K_TIMER_DEFINE(이름, 만료콜백, 정지콜백)` | 타이머를 정적으로 정의 |
| `k_timer_start(&timer, duration, period)` | 타이머 시작. `period`가 `K_NO_WAIT`(0)이면 One-shot, 그 외 값이면 Periodic |
| 만료 콜백을 다시 호출 | 이미 실행 중인 타이머에 `k_timer_start`를 또 호출하면, FreeRTOS의 `xTimerReset`처럼 다시 처음부터 카운트다운 |

> ⚠️ **만료 콜백은 ISR 컨텍스트에서 실행됩니다.** 그 안에서 `k_sleep`, `k_mutex_lock`처럼 블로킹되는 커널 API를 호출하면 안 됩니다. `printk`처럼 ISR에서 안전한 함수만 사용하세요.

## 코드

```c
#include <zephyr/kernel.h>

void periodic_expiry(struct k_timer *timer_id) {
    printk("PeriodicTimer: tick (every 2s) [running in ISR context]\n");
}

void timeout_expiry(struct k_timer *timer_id) {
    printk("TimeoutTimer: no activity for 3s - timeout! [running in ISR context]\n");
}

K_TIMER_DEFINE(periodic_timer, periodic_expiry, NULL);
K_TIMER_DEFINE(timeout_timer, timeout_expiry, NULL);

void activity_simulator_entry(void *p1, void *p2, void *p3) {
    for (int i = 0; i < 3; i++) {
        k_sleep(K_MSEC(1500));
        printk("ActivitySimulatorThread: activity detected, resetting timeout timer\n");
        k_timer_start(&timeout_timer, K_MSEC(3000), K_NO_WAIT);   // restart = reset
    }
    printk("ActivitySimulatorThread: no more activity from now on\n");
}

K_THREAD_DEFINE(activity_id, 1024, activity_simulator_entry, NULL, NULL, NULL, 5, 0, 0);

int main(void) {
    k_timer_start(&periodic_timer, K_MSEC(2000), K_MSEC(2000));   // period > 0 -> periodic
    k_timer_start(&timeout_timer, K_MSEC(3000), K_NO_WAIT);        // period = 0 -> one-shot
    return 0;
}
```

## 실행 & 확인

- `PeriodicTimer`가 2초마다 계속 "tick"을 출력하는지 확인
- `ActivitySimulatorThread`가 3번(1.5초 간격) 활동을 보고하며 `TimeoutTimer`를 리셋하는지 확인
- 마지막 활동 이후 **약 3초 뒤** `TimeoutTimer: no activity for 3s - timeout!`이 딱 한 번 출력되는지 확인 (One-shot이라 그 이후로는 자동 재시작되지 않음)

## 관찰 포인트

- FreeRTOS 커리큘럼의 14번 실습과 비교해보세요 — 그때는 "모든 Software Timer 콜백이 Timer Service Task 하나를 공유하니 콜백을 짧게 유지하라"고 배웠습니다. Zephyr는 아예 **ISR 컨텍스트**라서 제약이 더 강합니다: 블로킹 호출 자체가 금지되고, 우선순위 개념도 적용되지 않으며(인터럽트는 스레드 우선순위 체계 바깥에 있음), 가능한 한 즉시 끝나야 합니다
- 콜백에서 무거운 작업이 필요하다면, ISR 패턴(07번 실습)과 똑같이 **콜백에서는 신호만 주고(Semaphore/Poll Signal), 실제 처리는 별도 스레드에서** 하는 구조로 만들어야 합니다
- `k_timer_status_get(&timer)`로 타이머가 마지막 확인 이후 몇 번 만료됐는지 폴링 방식으로 확인할 수도 있습니다 — 콜백 없이 "가끔 확인만 하면 되는" 상황에 유용합니다

## 다음

15번 파일(`15_STACK_MONITORING_LAB.md`)에서 스레드 스택 사용량을 모니터링하는 방법을 다룹니다.
