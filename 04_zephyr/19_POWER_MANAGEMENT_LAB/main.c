// Source: 19_POWER_MANAGEMENT_LAB.md
// Section: 코드

#include <zephyr/kernel.h>

void periodic_entry(void *p1, void *p2, void *p3) {
    while (1) {
        printk("PeriodicThread: awake, doing some work...\n");
        k_sleep(K_SECONDS(3));   // 그냥 평소처럼 sleep - PM 관련 코드 없음
    }
}

K_THREAD_DEFINE(periodic_id, 1024, periodic_entry, NULL, NULL, NULL, 5, 0, 0);

int main(void) {
    return 0;
}
