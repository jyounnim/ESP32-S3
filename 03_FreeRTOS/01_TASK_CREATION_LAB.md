# 01. Task 생성 기초 — xTaskCreate / xTaskCreatePinnedToCore

## 이 실습에서 배우는 것

Bare-metal(`setup()`/`loop()` 하나)과 달리, RTOS는 여러 작업을 **독립된 Task**로 나눠 "동시에" 실행합니다. 여기서 "동시에"는 실제 병렬(듀얼코어라면 진짜 병렬)이거나, 스케줄러가 매우 빠르게 번갈아 실행해서 병렬처럼 보이는 것(단일 코어에 여러 Task가 있을 때)입니다.

## 핵심 개념

| 함수 | 설명 |
|---|---|
| `xTaskCreate(...)` | Task를 생성하되, 실행할 코어는 스케줄러가 자동으로 결정 |
| `xTaskCreatePinnedToCore(...)` | Task를 특정 코어(0 또는 1)에 고정 — ESP32-S3(듀얼코어)에서만 의미 있는 옵션 |
| 파라미터 순서 | 함수, 이름, 스택 크기(word 단위), 파라미터, 우선순위, 핸들, (코어) |

## 코드

```cpp
#include <Arduino.h>

void TaskA(void *pvParameters) {
  for (;;) {
    Serial.printf("TaskA running on core %d\n", xPortGetCoreID());
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void TaskB(void *pvParameters) {
  for (;;) {
    Serial.printf("TaskB running on core %d\n", xPortGetCoreID());
    vTaskDelay(pdMS_TO_TICKS(700));
  }
}

void TaskC(void *pvParameters) {
  for (;;) {
    Serial.printf("TaskC running on core %d\n", xPortGetCoreID());
    vTaskDelay(pdMS_TO_TICKS(400));
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // 코어 미지정 - 스케줄러가 자동 배치
  xTaskCreate(TaskA, "TaskA", 2048, NULL, 1, NULL);

  // 코어 명시적 고정
  xTaskCreatePinnedToCore(TaskB, "TaskB", 2048, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(TaskC, "TaskC", 2048, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);   // 기본 loop Task는 더 이상 사용하지 않음
}
```

## 실행 & 확인

- 세 Task가 각자 다른 주기(1000ms / 700ms / 400ms)로 독립적으로 출력되는지 확인
- `core %d` 값을 보고 `TaskA`는 스케줄러가 어느 코어에 배치했는지, `TaskB`/`TaskC`는 지정한 코어(0/1)와 일치하는지 확인

## 관찰 포인트

- `xTaskCreate`로 만든 `TaskA`가 매번 같은 코어에서 실행되는지, 아니면 바뀌는지 여러 번 재부팅하며 관찰해보세요 (ESP32 Arduino 환경에서는 보통 한 번 배치되면 고정되는 경향이 있지만, 이는 스케줄러 구현에 따른 것이지 보장된 동작은 아닙니다)
- 세 Task 모두 우선순위가 `1`로 동일합니다 — 동일 우선순위 Task들이 어떻게 번갈아 실행되는지는 02번 실습에서 다룹니다
- 스택 크기(`2048`)를 극단적으로 줄여보고(`256` 등) 어떤 문제가 발생하는지 관찰해보세요 — 15번 실습(스택 모니터링)에서 정식으로 다룹니다

## 다음

02번 파일(`02_TASK_PRIORITY_LAB.md`)에서 우선순위에 따른 선점형 스케줄링을 다룹니다.
