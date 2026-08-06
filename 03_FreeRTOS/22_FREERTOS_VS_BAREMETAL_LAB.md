# 22. FreeRTOS vs Bare-metal 종합 비교 & 커리큘럼 정리

## 이 실습에서 배우는 것

같은 요구사항("LED 500ms마다 점멸 + 2초마다 상태 출력 + 100ms마다 어떤 조건 확인")을 **bare-metal 방식**과 **FreeRTOS 방식** 두 가지로 각각 구현하고 나란히 비교합니다. 01번부터 21번까지 배운 내용이 왜 필요했는지 이 마지막 실습에서 한 번에 정리됩니다.

## 코드 (Bare-metal — millis() 기반 수동 스케줄링)

```cpp
#include <Arduino.h>

const int LED_PIN = 4;

unsigned long lastBlinkTime = 0;
unsigned long lastStatusTime = 0;
unsigned long lastCheckTime = 0;
bool ledState = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("Bare-metal version started");
}

void loop() {
  unsigned long now = millis();

  if (now - lastBlinkTime >= 500) {
    lastBlinkTime = now;
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
  }

  if (now - lastStatusTime >= 2000) {
    lastStatusTime = now;
    Serial.println("Bare-metal: status report");
  }

  if (now - lastCheckTime >= 100) {
    lastCheckTime = now;
    // some lightweight condition check would go here
  }

  // 문제: 위 세 가지 작업 중 하나라도 오래 걸리는 코드가 끼어들면
  // (예: 블로킹 HTTP 요청, 긴 delay()) 나머지 전부가 함께 지연된다
}
```

## 코드 (FreeRTOS — 독립된 Task)

```cpp
#include <Arduino.h>

const int LED_PIN = 4;

void BlinkTask(void *pvParameters) {
  bool ledState = false;
  for (;;) {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void StatusTask(void *pvParameters) {
  for (;;) {
    Serial.println("FreeRTOS: status report");
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void CheckTask(void *pvParameters) {
  for (;;) {
    // some lightweight condition check would go here
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("FreeRTOS version started");

  xTaskCreatePinnedToCore(BlinkTask, "BlinkTask", 2048, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(StatusTask, "StatusTask", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(CheckTask, "CheckTask", 2048, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
```

## 실행 & 비교

- 두 버전 모두 겉보기 동작(LED 점멸 + 상태 출력)은 동일하게 보입니다
- 차이는 **"만약 그중 하나가 오래 걸리는 작업을 포함하게 되면"** 드러납니다. 예를 들어 `StatusTask`(또는 bare-metal의 상태 출력 블록)에 `WIFI_TASK_LAB.md`에서처럼 HTTP 요청을 추가한다고 상상해보세요:
  - **Bare-metal**: HTTP 요청이 blocking이라면(진짜 non-blocking 상태 머신으로 직접 구현하지 않는 한) `loop()` 전체가 멈추고, 그동안 LED 점멸도 멈춥니다
  - **FreeRTOS**: `StatusTask` 하나만 멈추고, `BlinkTask`는 (다른 우선순위/코어 배치에 따라) 영향 없이 계속 동작합니다 — `WIFI_TASK_LAB.md`에서 직접 확인했던 바로 그 차이입니다

## 비교 정리표

| 항목 | Bare-metal (`millis()` 방식) | FreeRTOS |
|---|---|---|
| 작업 분리 | 하나의 `loop()` 안에 모두 뒤섞임 | 독립된 Task로 분리 (01) |
| 우선순위 | 없음 — 코드에 적힌 순서가 곧 처리 순서 | Task별 우선순위, 선점형 스케줄링 (02) |
| 동적 확장 | 함수 목록이 컴파일 시점에 고정 | Task를 실행 중 생성/삭제 가능 (03) |
| 응답성 보장 | 없음 — 감시 장치 없이 조용히 멈출 수 있음 | Task Watchdog이 멈춤을 감지 (04) |
| 자원 경합 | 특별한 장치 없음, 개발자가 알아서 조율 | Semaphore/Mutex/Critical Section (07, 09, 17) |
| 데이터 전달 | 전역 변수 + 수동 플래그 | Queue, Task Notification (10, 12) |
| 복합 조건 대기 | 여러 `if`문 조합, 직접 구현 | Event Group (13) |
| 주기적 작업 | `millis()` 비교를 매번 직접 작성 | Software Timer (14) |
| 메모리 안전성 | 스택 개념 자체가 희미함(전체가 하나의 스택 공유) | Task별 독립 스택 + 모니터링 (15) |
| 병렬 처리 | 사실상 불가능(단일 흐름) | 듀얼코어 분산 실행 (18) |
| 전력 관리 | 직접 `delay()`로 흉내만 가능 | Light Sleep, Idle Hook (19, 20) |

## 전체 커리큘럼 한눈에 보기

| 번호 | 핵심 한 줄 요약 |
|---|---|
| 01 | Task는 `xTaskCreate`로 만들고, 코어를 지정하려면 `xTaskCreatePinnedToCore` |
| 02 | 우선순위가 높은 Task는 언제든 낮은 Task를 선점한다 |
| 03 | Task는 실행 중에 동적으로 생성/삭제할 수 있다 |
| 04 | yield 없는 무한루프는 Task Watchdog에 걸려 재부팅된다 |
| 05 | 낮은 우선순위 Task가 자원을 쥐면, 높은 우선순위 Task도 오래 기다릴 수 있다 (우선순위 역전) |
| 06 | Idle Task는 정말 할 일이 없을 때만 실행된다 |
| 07 | ISR은 짧게, 실제 처리는 Semaphore로 깨운 Task에서 |
| 08 | Counting Semaphore는 "동시에 N개까지" 허용하는 자원 풀 관리 |
| 09 | Mutex는 소유자 개념과 Priority Inheritance로 우선순위 역전을 완화한다 |
| 10 | Queue는 Task 간에 실제 데이터(신호가 아니라)를 전달한다 |
| 11 | Queue Set은 여러 Queue를 한 Task가 동시에 감시하게 해준다 |
| 12 | Task Notification은 Queue/Semaphore보다 가벼운 내장 통신 슬롯이다 |
| 13 | Event Group은 여러 조건(AND/OR)을 동시에 기다리게 해준다 |
| 14 | Software Timer는 별도 Task 없이 주기적/일회성 작업을 실행한다 |
| 15 | `uxTaskGetStackHighWaterMark`로 스택 여유분을 미리 확인할 수 있다 |
| 16 | 여러 Mutex는 항상 같은 순서로 잠가야 Deadlock을 피한다 |
| 17 | 듀얼코어에서 아주 짧은 공유 데이터는 스핀락 기반 Critical Section으로 보호한다 |
| 18 | 독립적인 무거운 연산은 두 코어에 나누면 최대 2배 빨라진다 |
| 19 | Light Sleep은 Task 하나가 아니라 칩 전체를 멈춘다 |
| 20 | ESP-IDF는 코어별로 독립된 Idle/Tick Hook을 제공한다 |
| 21 | Queue + Mutex + 멀티코어를 조합하면 실전 Producer-Consumer 패턴이 된다 |
| 22 | (이 파일) 같은 기능도 FreeRTOS로 짜면 확장성과 견고함이 근본적으로 달라진다 |

## 관찰 포인트 (마무리 과제)

- 지금까지 만든 여러 실습(GPIO, PWM/ADC, Wi-Fi, MQTT, OTA)을 다시 보면서, 각 파일이 이 22개 개념 중 어떤 것을 실전에 응용한 것인지 스스로 짚어보세요 — 예를 들어 `WIFI_TASK_LAB.md`는 01(Task), 09(Mutex, Serial 보호), 18(코어 분산)의 조합입니다
- 앞으로 새로운 기능을 추가할 때 "이걸 bare-metal로 짤까, Task로 분리할까?"를 판단하는 기준은 결국 **"이 작업이 오래 걸리거나 대기할 수 있는가?"**입니다. 그렇다면 별도 Task + 적절한 동기화 수단(Queue/Semaphore/Mutex)으로 분리하는 것이 이 커리큘럼 전체의 결론입니다
