# FreeRTOS 커리큘럼 개요

RTOS의 일반적인 특성과 FreeRTOS(그리고 ESP32-S3에서의 구현체인 ESP-IDF FreeRTOS)만의 특징을 순서대로 익히는 22개 실습 시리즈입니다. 각 실습은 독립된 `.md` 파일로 구성되어 있고, 번호 순서대로 진행하면 개념이 이어지도록 설계했습니다.

## 공통 사항

- 개발환경: 기존에 구성한 VS Code + PlatformIO + ESP32-S3
- 대부분의 실습은 **별도 배선 없이 시리얼 모니터만으로 확인 가능**합니다 (LED/버튼이 필요한 실습은 해당 파일에 명시)
- 코드의 `Serial.print`/`printf` 출력 문자열은 영어로 작성되어 있습니다
- `platformio.ini`는 기존 프로젝트 설정을 그대로 사용하면 됩니다 (`framework = arduino`)

## 목차

| 번호 | 파일 | 주제 |
|---|---|---|
| 01 | `01_TASK_CREATION_LAB.md` | Task 생성 기초 (xTaskCreate / xTaskCreatePinnedToCore) |
| 02 | `02_TASK_PRIORITY_LAB.md` | Priority와 선점형 스케줄링 |
| 03 | `03_TASK_LIFECYCLE_LAB.md` | Task 동적 생성/삭제 |
| 04 | `04_TASK_WATCHDOG_LAB.md` | Task Watchdog — Task가 반드시 yield해야 하는 이유 |
| 05 | `05_PRIORITY_INVERSION_LAB.md` | 우선순위 역전(Priority Inversion) 재현 |
| 06 | `06_IDLE_TASK_LAB.md` | Idle Task와 CPU 유휴 시간 |
| 07 | `07_ISR_SEMAPHORE_LAB.md` | 인터럽트(ISR) + Binary Semaphore |
| 08 | `08_COUNTING_SEMAPHORE_LAB.md` | Counting Semaphore — 자원 풀 관리 |
| 09 | `09_MUTEX_LAB.md` | Mutex vs Binary Semaphore, Priority Inheritance |
| 10 | `10_QUEUE_BASICS_LAB.md` | Queue 기본 — Task 간 데이터 전달 |
| 11 | `11_QUEUE_SET_LAB.md` | Queue Set — 여러 Queue 동시 대기 |
| 12 | `12_TASK_NOTIFICATION_LAB.md` | Task Notification — 경량 통신 수단 |
| 13 | `13_EVENT_GROUP_LAB.md` | Event Group — 다중 조건 대기 |
| 14 | `14_SOFTWARE_TIMER_LAB.md` | Software Timer (One-shot / Periodic) |
| 15 | `15_STACK_MONITORING_LAB.md` | 스택 사용량 모니터링과 Stack Overflow |
| 16 | `16_DEADLOCK_LAB.md` | Deadlock 재현과 회피 |
| 17 | `17_CRITICAL_SECTION_LAB.md` | Critical Section — 듀얼코어 스핀락 |
| 18 | `18_MULTICORE_LAB.md` | 듀얼코어 Task 분산 |
| 19 | `19_LIGHT_SLEEP_LAB.md` | Light Sleep — 저전력 모드 |
| 20 | `20_FREERTOS_HOOKS_LAB.md` | ESP-IDF 코어별 Idle/Tick Hook |
| 21 | `21_PRODUCER_CONSUMER_LAB.md` | Producer-Consumer 종합 패턴 |
| 22 | `22_FREERTOS_VS_BAREMETAL_LAB.md` | FreeRTOS vs Bare-metal 종합 비교 |

## 학습 흐름

- **01~06**: Task 자체의 특성 (생성, 우선순위, 스케줄링, 생명주기)
- **07~13**: Task 간 동기화/통신 수단 (Semaphore, Mutex, Queue, Notification, Event Group)
- **14~17**: 타이밍 제어와 자원 보호 (Timer, 스택, Deadlock, Critical Section)
- **18~20**: ESP32-S3(듀얼코어) 특화 기능 (멀티코어, 절전, Hook)
- **21~22**: 종합 응용 및 정리
