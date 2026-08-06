# 20. ESP-IDF 코어별 Idle/Tick Hook

## 이 실습에서 배우는 것

Vanilla(순정) FreeRTOS는 `vApplicationIdleHook()`/`vApplicationTickHook()`라는 **전역 함수 하나**로 Idle/Tick 이벤트에 코드를 끼워 넣습니다. 그런데 이건 원래 **단일 코어**를 전제로 설계된 기능이라, 듀얼코어 ESP32-S3에서는 두 코어가 같은 훅을 공유하게 되어 어느 코어에서 호출된 건지 구분하기 어렵습니다. ESP-IDF는 이 한계를 보완하기 위해 **코어별로 독립적인 Hook을 등록하는 전용 API**를 추가로 제공합니다.

## 핵심 개념

| 함수 | 설명 |
|---|---|
| `esp_register_freertos_idle_hook_for_cpu(콜백, 코어번호)` | 지정한 코어의 Idle Task가 실행될 때마다 콜백 호출 |
| `esp_register_freertos_tick_hook_for_cpu(콜백, 코어번호)` | 지정한 코어의 매 Tick 인터럽트마다 콜백 호출 |
| 콜백 반환값 | `true`면 "매 tick마다 한 번만" 호출, `false`면 "가능한 한 계속 반복" 호출 |

> ⚠️ Idle Hook 콜백 안에서는 **절대 블로킹 함수를 호출하면 안 됩니다** (`vTaskDelay`, `Serial.print`처럼 시간이 걸리는 호출 금지). Idle Task 자체의 역할(메모리 정리 등)을 방해할 수 있습니다. 그래서 아래 예제는 콜백 안에서는 카운터만 증가시키고, 실제 출력은 별도 Task에서 합니다.

## 코드

```cpp
#include <Arduino.h>
#include "esp_freertos_hooks.h"

volatile uint32_t idleCount[2] = {0, 0};

bool IRAM_ATTR idleHookCore0() {
  idleCount[0]++;
  return true;   // call once per tick
}

bool IRAM_ATTR idleHookCore1() {
  idleCount[1]++;
  return true;
}

void ReporterTask(void *pvParameters) {
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(1000));
    Serial.printf("Idle hook calls in the last second - Core0: %lu, Core1: %lu\n", idleCount[0], idleCount[1]);
    idleCount[0] = 0;
    idleCount[1] = 0;
  }
}

void BusyTask(void *pvParameters) {
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(3000));
    Serial.println("BusyTask: starting 1s burst on core 1");
    unsigned long start = millis();
    while (millis() - start < 1000) { }
    Serial.println("BusyTask: burst done");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  esp_register_freertos_idle_hook_for_cpu(idleHookCore0, 0);
  esp_register_freertos_idle_hook_for_cpu(idleHookCore1, 1);

  xTaskCreatePinnedToCore(ReporterTask, "ReporterTask", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(BusyTask, "BusyTask", 2048, NULL, 2, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
```

## 실행 & 확인

- 평상시에는 `Core0`와 `Core1`의 idle hook 호출 횟수가 둘 다 비슷하게 큰 값으로 나오는지 확인
- `BusyTask`가 1초짜리 CPU burst를 도는 동안(Core 1에서 실행), `Core1` 값만 뚝 떨어지고 `Core0` 값은 영향받지 않는지 확인 — 이게 바로 "코어별로 독립적인 Hook"이라는 의미입니다

## 관찰 포인트

- 06번 실습에서는 우선순위 0짜리 사용자 Task로 비슷한 걸 흉내 냈다면, 이번엔 **공식 API로 진짜 Idle Task 자체의 실행 빈도**를 코어별로 정확히 측정한 것입니다
- 이 패턴을 확장하면 간단한 코어별 CPU 사용률 모니터(사용률 = 1 - (idle 횟수 / 최대 가능 횟수))를 직접 만들 수 있습니다
- Tick Hook(`esp_register_freertos_tick_hook_for_cpu`)도 같은 방식으로 등록할 수 있습니다 — Idle Hook은 "정말 할 일이 없을 때만" 호출되지만, Tick Hook은 시스템이 바쁘든 한가하든 **매 tick마다 무조건** 호출된다는 차이가 있습니다. 정확한 주기적 타이밍이 필요하다면 Tick Hook, "유휴 시간 활용"이 목적이면 Idle Hook을 씁니다 (다만 대부분의 경우 14번 실습의 Software Timer가 더 다루기 쉬운 대안입니다)

## 다음

21번 파일(`21_PRODUCER_CONSUMER_LAB.md`)에서 지금까지 배운 요소들을 조합해 다중 Producer-Consumer 패턴을 구현합니다.
