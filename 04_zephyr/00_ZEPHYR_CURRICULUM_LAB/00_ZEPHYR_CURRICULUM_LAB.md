# Zephyr RTOS 커리큘럼 개요

`freertos_curriculum/`의 22개 실습과 동일한 주제를, 이번엔 Zephyr RTOS의 API와 사상에 맞춰 다시 구성한 시리즈입니다. 단순히 함수 이름만 바꾼 것이 아니라, **Zephyr만의 설계(협조적/선점형 스레드 구분, 통합된 k_poll, k_event 등)가 FreeRTOS와 실제로 어떻게 다른지** 확인하는 데 초점을 맞췄습니다.

## 사전 준비물

- `ZEPHYR_LAB.md`에서 다룬 west + Zephyr SDK 환경 구축이 먼저 되어 있어야 합니다 (PlatformIO 아님)
- 대부분 `printk()`만으로 확인 가능하고, 하드웨어 배선이 필요한 실습(07번)은 별도 명시
- 코드의 출력 문자열은 영어로 작성되어 있습니다
- 빌드: `west build -b esp32s3_devkitc/esp32s3/procpu` (보드 타겟 이름은 사용 중인 Zephyr 버전에 따라 다를 수 있으니 `west boards | grep esp32s3`로 확인)

## FreeRTOS와의 첫 번째 큰 차이 — 우선순위 숫자

**Zephyr는 숫자가 작을수록 우선순위가 높습니다** (FreeRTOS와 정반대). 게다가 **음수 우선순위는 "협조적(cooperative)" 스레드**, **0 이상은 "선점형(preemptible)" 스레드**로 아예 종류가 나뉩니다. 이건 FreeRTOS에는 없는 Zephyr만의 핵심 개념이라, 커리큘럼 초반부(02, 04번)에서 집중적으로 다룹니다.

## 목차

| 번호 | 파일 | 주제 | FreeRTOS 커리큘럼 대응 |
|---|---|---|---|
| 01 | `01_THREAD_CREATION_LAB.md` | Thread 생성 기초 (K_THREAD_DEFINE / k_thread_create) | 01 |
| 02 | `02_THREAD_PRIORITY_LAB.md` | 우선순위 체계와 협조적/선점형 스레드 | 02 |
| 03 | `03_THREAD_LIFECYCLE_LAB.md` | Thread 동적 생성/종료 | 03 |
| 04 | `04_COOPERATIVE_YIELD_LAB.md` | 협조적 스레드와 k_yield — 반드시 양보해야 하는 이유 | 04 |
| 05 | `05_PRIORITY_INVERSION_LAB.md` | 우선순위 역전 재현 | 05 |
| 06 | `06_IDLE_THREAD_LAB.md` | Idle Thread와 CPU 유휴 시간 | 06 |
| 07 | `07_ISR_SEMAPHORE_LAB.md` | 인터럽트(ISR) + k_sem | 07 |
| 08 | `08_COUNTING_SEMAPHORE_LAB.md` | Counting Semaphore | 08 |
| 09 | `09_MUTEX_LAB.md` | k_mutex vs k_sem, Priority Inheritance | 09 |
| 10 | `10_MSGQ_BASICS_LAB.md` | Message Queue 기본 (k_msgq) | 10 |
| 11 | `11_K_POLL_LAB.md` | k_poll — 여러 커널 객체 동시 대기 | 11 (Queue Set) |
| 12 | `12_POLL_SIGNAL_LAB.md` | Poll Signal — 경량 이벤트 | 12 (Task Notification) |
| 13 | `13_K_EVENT_LAB.md` | k_event — 다중 조건 대기 | 13 (Event Group) |
| 14 | `14_K_TIMER_LAB.md` | k_timer (One-shot / Periodic) | 14 |
| 15 | `15_STACK_MONITORING_LAB.md` | 스택 사용량 모니터링 | 15 |
| 16 | `16_DEADLOCK_LAB.md` | Deadlock 재현과 회피 | 16 |
| 17 | `17_CRITICAL_SECTION_LAB.md` | irq_lock / k_sched_lock / k_spinlock | 17 |
| 18 | `18_MULTICORE_REALITY_LAB.md` | ESP32-S3에서의 멀티코어 — AMP vs SMP | 18 |
| 19 | `19_POWER_MANAGEMENT_LAB.md` | Zephyr Power Management (prj.conf) | 19 |
| 20 | `20_RUNTIME_STATS_LAB.md` | Thread Runtime Stats — 정식 CPU 사용률 API | 20 |
| 21 | `21_PRODUCER_CONSUMER_LAB.md` | Producer-Consumer 종합 패턴 | 21 |
| 22 | `22_ZEPHYR_VS_FREERTOS_LAB.md` | Zephyr vs FreeRTOS 종합 비교 | 22 |

## 학습 흐름

- **01~06**: Thread 자체의 특성 — 특히 Zephyr 고유의 협조적/선점형 구분
- **07~13**: 동기화/통신 수단 — k_sem/k_mutex/k_msgq는 FreeRTOS와 유사하지만, k_poll·k_event는 Zephyr가 더 통합적으로 설계한 부분
- **14~17**: 타이밍 제어와 자원 보호
- **18~20**: ESP32-S3에서 Zephyr가 실제로 어떻게 다르게 동작하는지 (특히 18번 — 멀티코어 모델 자체가 FreeRTOS/ESP-IDF와 다릅니다)
- **21~22**: 종합 응용 및 FreeRTOS 커리큘럼과의 최종 비교
