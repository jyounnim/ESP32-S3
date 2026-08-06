# 15. 스택 사용량 모니터링과 Stack Overflow

## 이 실습에서 배우는 것

각 Task는 생성할 때 지정한 크기만큼의 스택을 독립적으로 가집니다. 너무 작게 잡으면 실제 겪으셨던 `Stack canary watchpoint triggered` 크래시(WIFI_TASK_LAB.md에서 다뤘던 문제)로 이어집니다. FreeRTOS는 실행 중에 각 Task의 스택 여유분을 확인할 수 있는 API를 제공해서, 크래시가 나기 전에 미리 위험을 감지할 수 있습니다.

## 핵심 개념

| 함수 | 설명 |
|---|---|
| `uxTaskGetStackHighWaterMark(handle)` | 해당 Task가 지금까지 실행되며 **가장 적게 남았던** 스택 여유분(word 단위)을 반환. 값이 작을수록 위험 |
| Stack High Water Mark | "만조 표시" — 스택이 가장 깊게(많이) 사용됐던 지점의 기록. 낮을수록 오버플로우에 가까웠다는 뜻 |

## 코드

```cpp
#include <Arduino.h>

TaskHandle_t lightHandle;
TaskHandle_t heavyHandle;

void LightTask(void *pvParameters) {
  for (;;) {
    int smallVar = 0;
    smallVar++;
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void recursiveWork(int depth) {
  char buffer[256];   // consumes stack on every call
  memset(buffer, 0, sizeof(buffer));
  if (depth > 0) {
    recursiveWork(depth - 1);
  }
}

void HeavyTask(void *pvParameters) {
  for (;;) {
    recursiveWork(4);   // uses noticeably more stack than LightTask
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void MonitorTask(void *pvParameters) {
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(2000));
    UBaseType_t lightFree = uxTaskGetStackHighWaterMark(lightHandle);
    UBaseType_t heavyFree = uxTaskGetStackHighWaterMark(heavyHandle);
    Serial.printf("Stack headroom (words) - LightTask: %u, HeavyTask: %u\n", lightFree, heavyFree);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  xTaskCreatePinnedToCore(LightTask, "LightTask", 2048, NULL, 1, &lightHandle, 1);
  xTaskCreatePinnedToCore(HeavyTask, "HeavyTask", 2048, NULL, 1, &heavyHandle, 1);
  xTaskCreatePinnedToCore(MonitorTask, "MonitorTask", 2048, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
```

## 실행 & 확인

- `HeavyTask`(재귀 호출 + 256바이트 버퍼)의 여유분 수치가 `LightTask`보다 눈에 띄게 작게 나오는지 확인
- 두 Task 모두 스택 크기(`2048`)는 동일하게 줬지만, **실제 사용 패턴에 따라 여유분이 크게 다르다**는 걸 숫자로 확인하는 것이 이 실습의 핵심입니다

## 관찰 포인트

- `HeavyTask`의 스택 크기를 `2048`에서 `512`로 줄여보세요 — `HeavyTask`의 헤드룸이 0에 가까워지거나, 아예 04번/이전에 겪었던 것과 같은 `Stack canary watchpoint triggered` 크래시가 재현될 수 있습니다 (확인 후 다시 2048로 복구하세요)
- `recursiveWork`의 `depth` 값을 4에서 10 정도로 늘려보고 헤드룸이 더 줄어드는지 확인해보세요 — 재귀 호출은 호출될 때마다 스택을 쌓기 때문에 스택 오버플로우의 대표적인 원인입니다
- 실무 기준: 정확한 수치는 상황마다 다르지만, 헤드룸이 너무 빠듯하면(예: 총 스택의 10~20% 미만) 예상 못한 코드 경로(예외 처리, 라이브러리 내부 호출 깊이 증가 등)에서 오버플로우가 날 위험이 있습니다 — 넉넉하게 잡고, 이 실습처럼 주기적으로 모니터링하는 습관이 안전합니다

## 다음

16번 파일(`16_DEADLOCK_LAB.md`)에서 여러 Mutex를 동시에 쓸 때 발생할 수 있는 Deadlock을 재현하고 회피하는 법을 다룹니다.
