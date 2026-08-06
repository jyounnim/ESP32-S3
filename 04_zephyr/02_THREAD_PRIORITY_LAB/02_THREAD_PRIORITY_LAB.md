# 02. 우선순위 체계와 협조적/선점형 스레드

## 이 실습에서 배우는 것

**Zephyr는 FreeRTOS와 정확히 반대로, 숫자가 작을수록 우선순위가 높습니다.** 그리고 더 중요한 차이가 있습니다 — Zephyr는 스레드를 **협조적(Cooperative, 우선순위가 음수)**과 **선점형(Preemptible, 우선순위가 0 이상)** 두 종류로 명확히 구분합니다. FreeRTOS(vanilla)는 이런 구분이 없이 항상 전체 시스템이 선점형으로 동작하는 것과 근본적으로 다른 설계입니다.

## 핵심 개념

| 스레드 종류 | 우선순위 범위 | 동작 |
|---|---|---|
| **협조적(Cooperative)** | 음수 (예: -16 ~ -1) | 한 번 실행되기 시작하면, **스스로 양보(yield/sleep/blocking call)하기 전까지는 절대 다른 스레드에게 CPU를 빼앗기지 않음** — 심지어 그보다 더 급한 협조적 스레드가 준비되어 있어도 마찬가지 |
| **선점형(Preemptible)** | 0 이상 | 더 높은 우선순위(협조적 스레드 포함)가 준비되는 즉시 선점당함 — FreeRTOS의 기본 동작과 유사 |

## 코드

```c
#include <zephyr/kernel.h>

#define STACK_SIZE 1024

/* ---- Part 1: two PREEMPTIBLE threads - normal preemption applies ---- */
void low_prio_preempt_entry(void *p1, void *p2, void *p3) {
    for (int i = 0; i < 5; i++) {
        printk("PreemptLow (prio 5): step %d\n", i);
        k_busy_wait(300000);   // 300ms busy-wait (no yield call, but tick interrupts still fire)
    }
    printk("PreemptLow: done\n");
}

void high_prio_preempt_entry(void *p1, void *p2, void *p3) {
    k_sleep(K_MSEC(500));      // let PreemptLow start first
    printk("PreemptHigh (prio 3): ready now - preempts PreemptLow immediately\n");
}

K_THREAD_DEFINE(low_id, STACK_SIZE, low_prio_preempt_entry, NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(high_id, STACK_SIZE, high_prio_preempt_entry, NULL, NULL, NULL, 3, 0, 0);

/* ---- Part 2: a COOPERATIVE thread that refuses to yield ---- */
void coop_low_entry(void *p1, void *p2, void *p3) {
    printk("CoopLow (prio -1): starting a long computation, will NOT yield\n");
    for (volatile long i = 0; i < 30000000; i++) { }   // long busy loop - no yield/sleep/blocking call
    printk("CoopLow: finished - only NOW does CoopHigh get a chance to run\n");
}

void coop_high_entry(void *p1, void *p2, void *p3) {
    k_sleep(K_MSEC(100));      // becomes "ready" after 100ms
    printk("CoopHigh (prio -5): this line had to wait for CoopLow to finish!\n");
}

K_THREAD_DEFINE(coop_low_id, STACK_SIZE, coop_low_entry, NULL, NULL, NULL, -1, 0, 0);
K_THREAD_DEFINE(coop_high_id, STACK_SIZE, coop_high_entry, NULL, NULL, NULL, -5, 0, 0);

int main(void) {
    return 0;
}
```

## 실행 & 확인

- **Part 1 (선점형)**: `PreemptLow`가 5단계를 출력하는 도중, 약 500ms 지점에서 `PreemptHigh (prio 3): ready now...`가 즉시 끼어드는지 확인 — 숫자가 더 작은(더 급한) 우선순위 스레드가 준비되는 즉시 선점되는, FreeRTOS와 동일한 동작입니다
- **Part 2 (협조적)**: `CoopHigh`는 100ms 뒤에 "ready" 상태가 되지만, `CoopHigh (prio -5): this line had to wait...` 로그는 그보다 훨씬 늦게(수 초 뒤, `CoopLow`가 계산을 끝낸 직후) 출력되는지 확인 — **우선순위가 더 급한데도(`-5` < `-1`) 전혀 선점하지 못했습니다**

## 관찰 포인트

- `CoopLow`의 우선순위를 `-1`에서 `2`(선점형)로만 바꿔보세요 — 나머지 코드는 그대로인데, 이번엔 `CoopHigh`가 100ms 근처에서 즉시 끼어드는 걸 확인할 수 있습니다. **딱 부호 하나(음수 vs 0 이상) 차이가 스케줄링 동작 전체를 바꿉니다**
- 이 특성 때문에 Zephyr에서 협조적 스레드를 쓸 때는 반드시 **04번 실습에서 다룰 `k_yield()`를 스스로 자주 호출**해줘야 합니다 — 그렇지 않으면 다른 모든 스레드(자신보다 우선순위가 높은 스레드 포함)가 굶주릴 수 있습니다
- FreeRTOS에도 `configUSE_PREEMPTION=0`으로 시스템 전체를 협조형으로 바꾸는 옵션은 있지만, **Task마다 개별적으로 선점형/협조형을 선택할 수는 없습니다** — Zephyr처럼 "이 스레드는 협조적으로, 저 스레드는 선점형으로" 섞어 쓸 수 있는 건 Zephyr 고유의 유연성입니다

## 다음

03번 파일(`03_THREAD_LIFECYCLE_LAB.md`)에서 스레드를 실행 중에 동적으로 생성/종료하는 방법을 다룹니다.
