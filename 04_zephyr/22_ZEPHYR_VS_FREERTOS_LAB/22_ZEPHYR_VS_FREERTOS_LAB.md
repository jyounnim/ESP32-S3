# 22. Zephyr vs FreeRTOS 종합 비교 & 커리큘럼 정리

## 이 실습에서 배우는 것

`freertos_curriculum/`과 `zephyr_curriculum/` 두 시리즈를 나란히 거쳐오면서 확인한 차이점을 한 번에 정리합니다. 겉보기엔 "API 이름만 다른 비슷한 RTOS"처럼 보이지만, 실제로는 **설계 철학 자체가 다른 지점들**이 여러 번 발견됐습니다.

## API 대응표

| 개념 | FreeRTOS | Zephyr |
|---|---|---|
| 실행 단위 | Task | Thread |
| 생성(정적) | — (기본적으로 런타임 생성 위주) | `K_THREAD_DEFINE` |
| 생성(동적) | `xTaskCreate` | `k_thread_create` |
| 삭제 | `vTaskDelete` | 함수 `return` 또는 `k_thread_abort` |
| 우선순위 방향 | 숫자가 클수록 높음 | **숫자가 작을수록 높음** |
| 스케줄링 방식 | 항상 선점형 (시스템 전체 설정) | **스레드별로 협조적/선점형 선택 가능** |
| Binary Semaphore | `xSemaphoreCreateBinary` | `k_sem` (count 1) |
| Counting Semaphore | `xSemaphoreCreateCounting` | `k_sem` (count N) |
| Mutex | `xSemaphoreCreateMutex` | `k_mutex` (Priority Inheritance 기본 내장) |
| ISR에서 신호 주기 | `xSemaphoreGiveFromISR` (+ `portYIELD_FROM_ISR`) | `k_sem_give` (그대로, 별도 ISR 버전 없음) |
| Queue | `xQueueCreate`/`xQueueSend`/`xQueueReceive` | `k_msgq` / `k_msgq_put` / `k_msgq_get` |
| 여러 큐 동시 대기 | Queue Set | `k_poll` (더 범용적 — 세마포어·시그널까지 함께) |
| 경량 통신 | Task Notification | Poll Signal (`k_poll_signal`) |
| 비트 기반 다중 조건 | Event Group | `k_event` |
| 소프트웨어 타이머 | Software Timer (전용 Task에서 콜백 실행) | `k_timer` (**ISR 컨텍스트**에서 콜백 실행) |
| 스택 여유분 확인 | `uxTaskGetStackHighWaterMark` (word 단위) | `k_thread_stack_space_get` (**byte 단위**) |
| 짧은 공유 데이터 보호 | Critical Section (`portENTER_CRITICAL` + 스핀락) | `irq_lock`/`k_spinlock` |
| 멀티코어 모델(ESP32-S3) | SMP — 하나의 커널이 양쪽 코어 관리 | **AMP — 코어마다 별도 OS 이미지** |
| 절전 모드 | 수동 호출 필요 (Arduino 환경에서 sdkconfig 접근 불가) | `prj.conf`에서 `CONFIG_PM=y`만 켜면 자동 |
| CPU 사용률 측정 | ESP-IDF 전용 Idle/Tick Hook | `k_thread_runtime_stats_get` (이식성 있는 표준 API) |

## 근본적으로 다른 철학 3가지

### 1. 정적 구성을 선호하는 Zephyr

FreeRTOS는 "런타임에 Task를 만드는 게 기본"이지만, Zephyr는 `K_THREAD_DEFINE`처럼 **컴파일 시점에 다 정해두는 방식**을 훨씬 더 관용적으로 씁니다. 스택도 `K_THREAD_STACK_DEFINE`으로 미리 선언해야 합니다. 이건 우연이 아니라 Zephyr가 메모리가 극히 제한된 소형 칩까지 함께 지원하기 위해 "실행 전에 필요한 자원을 최대한 예측 가능하게 만들자"는 철학을 갖고 있기 때문입니다.

### 2. 스레드를 "종류"로 나누는 Zephyr

FreeRTOS는 Task 하나하나가 다 똑같은 방식(선점형)으로 스케줄링됩니다. Zephyr는 협조적/선점형이라는 **완전히 다른 두 가지 스케줄링 규칙을 스레드마다 선택**할 수 있게 했습니다(02, 04번 실습). 이건 "정말 방해받으면 안 되는 짧은 작업"과 "일반적인 작업"을 명확히 구분하고 싶을 때 강력한 도구가 됩니다.

### 3. 빌드 시스템이 다른 데서 오는 실질적 차이

Arduino/PlatformIO는 미리 컴파일된 라이브러리를 쓰기 때문에 `sdkconfig` 옵션을 사용자가 바꿀 수 없었습니다(`ZEPHYR_LAB.md`에서 처음 확인). Zephyr는 `west`로 항상 소스에서 직접 빌드하기 때문에 `prj.conf` 옵션을 자유롭게 켜고 끌 수 있습니다. 19번 실습(Power Management)에서 본 것처럼, 이 차이 하나로 "코드 수정 없이 Kconfig 한 줄로 자동 절전"이 가능해집니다.

## 전체 Zephyr 커리큘럼 한눈에 보기

| 번호 | 핵심 한 줄 요약 |
|---|---|
| 01 | `K_THREAD_DEFINE`은 컴파일 시점에, `k_thread_create`는 런타임에 스레드를 만든다 |
| 02 | 숫자가 작을수록 우선순위가 높고, 음수는 협조적 / 0 이상은 선점형이다 |
| 03 | 스레드는 `return`으로 스스로 끝나거나 `k_thread_abort`로 강제 종료된다 |
| 04 | 협조적 스레드는 같은 우선순위끼리도 자동 교대되지 않는다 — `k_yield()`가 필요하다 |
| 05 | 낮은 우선순위 스레드가 자원을 쥐면 높은 우선순위도 오래 기다릴 수 있다 |
| 06 | Idle Thread는 정말 할 일이 없을 때만 실행된다 |
| 07 | `k_sem_give`는 ISR에서 별도 버전 없이 그대로 호출 가능하다 |
| 08 | `k_sem`의 초기값/최댓값만 바꾸면 Counting Semaphore가 된다 |
| 09 | `k_mutex`는 Priority Inheritance가 기본 내장이다 |
| 10 | `k_msgq`는 고정 크기 데이터를 스레드 간에 전달한다 |
| 11 | `k_poll`은 여러 종류의 커널 객체를 한 번에 감시할 수 있다 |
| 12 | Poll Signal은 `k_poll`과 짝을 이루는 경량 값 전달 이벤트다 |
| 13 | `k_event`는 비트마스크로 AND/OR 다중 조건을 표현한다 |
| 14 | `k_timer`의 콜백은 스레드가 아니라 ISR 컨텍스트에서 실행된다 |
| 15 | `k_thread_stack_space_get`은 바이트 단위로 스택 여유분을 알려준다 |
| 16 | 여러 Mutex는 항상 같은 순서로 잠가야 Deadlock을 피한다 |
| 17 | ISR과 경합하는 데이터는 `k_sched_lock`이 아니라 `irq_lock`/`k_spinlock`으로 보호한다 |
| 18 | ESP32-S3의 Zephyr는 SMP가 아니라 AMP — 코어마다 별도 OS 이미지다 |
| 19 | `CONFIG_PM=y` 하나로 코드 수정 없이 자동 절전이 가능하다 |
| 20 | `k_thread_runtime_stats_get`은 이식성 있는 표준 CPU 사용률 API다 |
| 21 | `k_msgq` + `k_mutex`를 조합하면 실전 Producer-Consumer 패턴이 된다 |
| 22 | (이 파일) 두 RTOS는 핵심 개념은 비슷하지만 설계 철학은 여러 지점에서 근본적으로 다르다 |

## 마무리 과제

- `MQTT_LAB.md`, `WIFI_TASK_LAB.md` 등 기존 FreeRTOS 기반 실전 예제를 Zephyr로 포팅해보세요 — Wi-Fi/MQTT 라이브러리 자체는 다르지만(Zephyr는 `net_mgmt`, `zephyr/net/wifi_mgmt.h`, `zephyr/net/mqtt.h` 등 자체 네트워킹 스택을 씀), 이번 시리즈에서 배운 Thread/Semaphore/Queue 설계 방식은 그대로 적용할 수 있습니다
- 두 RTOS 중 어느 쪽이 "더 낫다"가 아니라, **프로젝트의 요구사항(정적 구성이 필요한가, 다양한 하드웨어 이식성이 중요한가, 팀이 이미 익숙한 생태계가 있는가)에 따라 선택하는 것**이 실무적인 결론입니다
