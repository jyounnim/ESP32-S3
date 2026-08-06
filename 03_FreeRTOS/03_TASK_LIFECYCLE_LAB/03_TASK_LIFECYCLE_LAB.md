# 03. Task 동적 생성/삭제

## 이 실습에서 배우는 것

Task는 `setup()`에서 미리 다 만들어 둘 필요가 없습니다. 실행 중에 필요할 때 생성하고, 다 쓰면 삭제해서 메모리를 회수할 수 있습니다. 이는 고정된 함수 목록만 존재하는 bare-metal 구조와의 큰 차이점입니다.

## 핵심 개념

| 함수 | 설명 |
|---|---|
| `vTaskDelete(NULL)` | 자기 자신을 삭제 (Task 함수 안에서 호출) |
| `vTaskDelete(handle)` | 다른 Task를 핸들로 지정해서 삭제 |
| `xTaskCreate(...)` 반환값 | Task 생성 성공 시 `pdPASS`, 실패 시 `pdFAIL` (메모리 부족 등) |

## 코드

```cpp
#include <Arduino.h>

TaskHandle_t workerHandle = NULL;

void WorkerTask(void *pvParameters) {
  int jobId = (int)(intptr_t)pvParameters;
  Serial.printf("WorkerTask #%d: started\n", jobId);

  for (int i = 0; i < 5; i++) {
    Serial.printf("WorkerTask #%d: working... (%d/5)\n", jobId, i + 1);
    vTaskDelay(pdMS_TO_TICKS(500));
  }

  Serial.printf("WorkerTask #%d: done, deleting self\n", jobId);
  workerHandle = NULL;
  vTaskDelete(NULL);   // 자기 자신을 삭제 (여기서 함수가 끝나지 않고 즉시 종료됨)
}

void ManagerTask(void *pvParameters) {
  int jobCounter = 0;
  for (;;) {
    if (workerHandle == NULL) {
      jobCounter++;
      Serial.printf("ManagerTask: spawning WorkerTask #%d\n", jobCounter);
      BaseType_t result = xTaskCreate(
        WorkerTask, "WorkerTask", 2048,
        (void *)(intptr_t)jobCounter, 1, &workerHandle
      );
      if (result != pdPASS) {
        Serial.println("ManagerTask: failed to create WorkerTask (out of memory?)");
      }
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  xTaskCreatePinnedToCore(ManagerTask, "ManagerTask", 2048, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
```

## 실행 & 확인

- `ManagerTask`가 1초마다 `workerHandle`이 비어있는지 확인하고, 비어있으면 새 `WorkerTask`를 생성
- `WorkerTask`는 5번 일하고 스스로 삭제 → 다음 주기에 `ManagerTask`가 새 Worker를 또 생성하는 패턴이 반복되는지 확인

## 관찰 포인트

- `pvParameters`로 정수(`jobId`)를 전달할 때 `(void *)(intptr_t)jobCounter`처럼 포인터로 캐스팅해서 넘기는 방식을 확인하세요 — 실제 메모리 주소가 아니라 정수 값 자체를 포인터 타입에 "욱여넣는" 흔한 트릭입니다. 구조체 등 더 큰 데이터를 넘기려면 힙에 할당하거나 static 변수를 사용해야 합니다
- Task를 계속 생성/삭제하다 보면 힙 단편화가 누적될 수 있습니다 — `ESP.getFreeHeap()`을 주기적으로 출력해서 장시간 동작 시 메모리가 줄어드는지 관찰해보세요 (응용 과제)
- `vTaskDelete(NULL)`은 호출 즉시 Task가 종료되므로, 그 아래에 작성한 코드는 절대 실행되지 않습니다 — 삭제 전에 필요한 정리(cleanup) 작업은 반드시 `vTaskDelete` 호출 이전에 끝내야 합니다

## 다음

04번 파일(`04_TASK_WATCHDOG_LAB.md`)에서 Task가 왜 반드시 주기적으로 yield해야 하는지, 그렇지 않으면 어떤 일이 벌어지는지 다룹니다.
