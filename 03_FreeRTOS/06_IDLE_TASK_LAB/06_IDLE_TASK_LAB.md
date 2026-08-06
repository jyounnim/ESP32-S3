# 06. Idle Task와 CPU 유휴 시간

## 이 실습에서 배우는 것

FreeRTOS는 실행할 Task가 하나도 없는 순간에도 CPU를 완전히 놀리지 않고, 항상 **Idle Task**(우선순위 0, 시스템이 자동 생성)를 실행합니다. Idle Task는 메모리 정리 같은 배경 작업을 하며, 동시에 "지금 이 코어가 한가하다"는 걸 보여주는 지표이기도 합니다. 이 실습에서는 우선순위 0짜리 사용자 Task를 만들어 Idle Task와 같은 처지에서 CPU를 얼마나 받는지 관찰합니다.

## 핵심 개념

- 우선순위 0은 시스템의 Idle Task가 쓰는 최저 우선순위입니다 — 사용자 Task도 우선순위 0으로 만들 수 있고, 이 경우 Idle Task와 동등하게 취급되어 **time-slice로 번갈아** 실행됩니다
- 우선순위 0 Task가 얼마나 자주 실행되는지는 "다른 높은 우선순위 Task들이 지금 CPU를 얼마나 쓰고 있는가"의 반대 지표가 됩니다

## 코드

```cpp
#include <Arduino.h>

volatile uint32_t idleCounter = 0;

void IdleCounterTask(void *pvParameters) {   // Priority 0 - same level as the system Idle Task
  for (;;) {
    idleCounter++;
  }
}

void ReporterTask(void *pvParameters) {      // Priority 1
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(1000));
    Serial.printf("IdleCounterTask incremented %lu times in the last second\n", idleCounter);
    idleCounter = 0;
  }
}

void BusyBurstTask(void *pvParameters) {     // Priority 2 - occasionally hogs the CPU
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(3000));
    Serial.println("BusyBurstTask: starting a 1s CPU burst");
    TickType_t start = xTaskGetTickCount();
    while ((xTaskGetTickCount() - start) < pdMS_TO_TICKS(1000)) {
      // pure busy work - higher priority, so it preempts IdleCounterTask/ReporterTask
    }
    Serial.println("BusyBurstTask: burst done");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  xTaskCreatePinnedToCore(IdleCounterTask, "IdleCounterTask", 2048, NULL, 0, NULL, 1);
  xTaskCreatePinnedToCore(ReporterTask, "ReporterTask", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(BusyBurstTask, "BusyBurstTask", 2048, NULL, 2, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
```

## 실행 & 확인

- 평상시(BusyBurstTask가 쉬는 3초 구간)에는 `idleCounter`가 매우 큰 값으로 찍히는지 확인
- `BusyBurstTask`가 1초짜리 CPU burst를 도는 동안 보고되는 값이 확 줄어드는지(거의 0에 가까운지) 확인 — 그 1초 동안은 우선순위 2인 `BusyBurstTask`가 코어를 독점하므로, 우선순위 0인 `IdleCounterTask`는 거의 실행되지 못합니다

## 관찰 포인트

- `idleCounter`의 절대값 자체는 큰 의미가 없습니다(하드웨어/컴파일러 최적화에 따라 달라짐) — 중요한 건 **burst 구간과 평상시의 상대적인 차이**입니다
- 이 원리를 응용하면 간단한 "CPU 사용률 측정기"를 만들 수 있습니다 — 실제로 ESP-IDF는 이와 비슷한 방식(Idle Hook 카운팅)으로 코어별 사용률을 계산합니다. 20번 실습(`20_FREERTOS_HOOKS_LAB.md`)에서 정식 API로 다시 다룹니다
- `BusyBurstTask`가 1초 동안 `vTaskDelay` 없이 완전히 busy 상태인데도 04번 실습과 달리 재부팅되지 않는 이유를 생각해보세요 — burst 시간(1초)이 Task Watchdog 기본 타임아웃(약 5초)보다 훨씬 짧고, burst 사이사이(3초)에 Idle Task가 충분히 실행되기 때문입니다

## 다음

07번 파일(`07_ISR_SEMAPHORE_LAB.md`)에서 인터럽트(ISR)가 Semaphore를 통해 Task를 깨우는 방법을 다룹니다.
