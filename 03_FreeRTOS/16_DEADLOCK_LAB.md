# 16. Deadlock 재현과 회피

## 이 실습에서 배우는 것

Mutex 두 개 이상을 서로 다른 순서로 잠그면, 두 Task가 서로 상대방이 쥔 자원을 기다리며 **영원히 멈추는 상태(Deadlock)**에 빠질 수 있습니다. 이 실습에서는 일부러 Deadlock을 재현해보고, 가장 기본적인 회피 방법(잠금 순서 통일)을 적용해봅니다.

> ⚠️ 아래 첫 번째 코드는 의도적으로 시스템을 멈추게 만드는 코드입니다. 크래시나 재부팅은 아니고, 로그 출력이 그냥 멈춥니다(Task들이 Blocked 상태로 영원히 대기하기 때문에 워치독도 발동하지 않습니다).

## 코드 (문제 상황 재현)

```cpp
#include <Arduino.h>

SemaphoreHandle_t xMutexX;
SemaphoreHandle_t xMutexY;

void TaskA(void *pvParameters) {
  for (;;) {
    Serial.println("TaskA: taking MutexX");
    xSemaphoreTake(xMutexX, portMAX_DELAY);
    Serial.println("TaskA: got MutexX, now taking MutexY");
    vTaskDelay(pdMS_TO_TICKS(100));   // give TaskB time to grab MutexY first
    xSemaphoreTake(xMutexY, portMAX_DELAY);

    Serial.println("TaskA: got both mutexes, working...");
    xSemaphoreGive(xMutexY);
    xSemaphoreGive(xMutexX);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void TaskB(void *pvParameters) {
  for (;;) {
    Serial.println("TaskB: taking MutexY");
    xSemaphoreTake(xMutexY, portMAX_DELAY);
    Serial.println("TaskB: got MutexY, now taking MutexX");
    vTaskDelay(pdMS_TO_TICKS(100));   // give TaskA time to grab MutexX first
    xSemaphoreTake(xMutexX, portMAX_DELAY);

    Serial.println("TaskB: got both mutexes, working...");
    xSemaphoreGive(xMutexX);
    xSemaphoreGive(xMutexY);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  xMutexX = xSemaphoreCreateMutex();
  xMutexY = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(TaskA, "TaskA", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskB, "TaskB", 2048, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
```

### 실행 & 확인

- 로그가 아래 패턴에서 그대로 멈추는지 확인:
  ```
  TaskA: taking MutexX
  TaskB: taking MutexY
  TaskA: got MutexX, now taking MutexY
  TaskB: got MutexY, now taking MutexX
  (여기서 영원히 정지)
  ```
- `TaskA`는 `MutexX`를 쥔 채 `MutexY`를 기다리고, `TaskB`는 `MutexY`를 쥔 채 `MutexX`를 기다립니다 — 서로가 서로를 기다리는 **순환 대기(circular wait)** 상태로, 둘 다 영원히 풀리지 않습니다

## 코드 (해결 — 잠금 순서 통일)

`TaskB`가 `MutexY`를 먼저 잡는 게 문제의 원인입니다. **모든 Task가 항상 같은 순서로 잠그도록** 통일하면 순환 대기 자체가 성립할 수 없습니다.

```cpp
void TaskB(void *pvParameters) {
  for (;;) {
    Serial.println("TaskB: taking MutexX");
    xSemaphoreTake(xMutexX, portMAX_DELAY);   // TaskA와 동일하게 X를 먼저
    Serial.println("TaskB: got MutexX, now taking MutexY");
    vTaskDelay(pdMS_TO_TICKS(100));
    xSemaphoreTake(xMutexY, portMAX_DELAY);

    Serial.println("TaskB: got both mutexes, working...");
    xSemaphoreGive(xMutexY);
    xSemaphoreGive(xMutexX);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
```

### 실행 & 확인

- 이제 두 Task 모두 "got both mutexes, working..."까지 정상적으로 도달하고, 무한 반복되는지 확인 — Deadlock 없이 각자 자기 차례를 기다렸다가 순서대로 진행됩니다

## 관찰 포인트

- 이번 실습에서 쓴 회피 전략(**Lock Ordering, 잠금 순서 통일**)은 가장 기본적이면서도 실무에서 가장 많이 쓰이는 Deadlock 예방법입니다 — 프로젝트 전체에서 "Mutex를 여러 개 동시에 잡아야 한다면 항상 A → B → C 순서로"라는 규칙을 문서화해두는 식으로 적용합니다
- 다른 회피 방법으로는 **타임아웃 사용**이 있습니다 — `xSemaphoreTake(xMutexY, pdMS_TO_TICKS(500))`처럼 무한 대기(`portMAX_DELAY`) 대신 제한 시간을 주면, 자원을 못 얻었을 때 실패로 처리하고 이미 쥔 자원(`xMutexX`)을 풀어준 뒤 재시도하는 방식으로 Deadlock을 회피할 수 있습니다. 다만 이 방법은 순서를 통일하는 것보다 코드가 복잡해집니다
- 09번 실습에서 배운 Priority Inheritance는 **우선순위 역전**은 완화해주지만 **Deadlock은 막아주지 못한다**는 점을 기억하세요 — 완전히 다른 문제입니다

## 다음

17번 파일(`17_CRITICAL_SECTION_LAB.md`)에서 듀얼코어 환경에서 아주 짧은 공유 데이터를 보호하는 Critical Section을 다룹니다.
