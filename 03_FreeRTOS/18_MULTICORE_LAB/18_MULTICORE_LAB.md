# 18. 듀얼코어 Task 분산

## 이 실습에서 배우는 것

지금까지의 실습 대부분은 여러 Task를 **같은 코어(1)**에 몰아서, 스케줄링(선점, 우선순위)의 효과를 명확하게 관찰하는 데 집중했습니다. 이번엔 반대로, **서로 다른 코어**에 작업을 나눠서 진짜 병렬 처리로 성능을 끌어올리는 사례를 확인합니다. 이건 단일 코어 MCU에서는 아예 불가능한, ESP32-S3(듀얼코어)만의 특징입니다.

## 핵심 개념

- 같은 코어의 Task들은 아무리 많아도 결국 **번갈아가며(시분할)** 실행되는 것이지, 진짜 동시 실행이 아닙니다
- 서로 다른 코어에 배치된 Task는 **물리적으로 동시에** 실행됩니다 — CPU 연산이 많은 독립적인 작업이라면, 두 코어에 나누는 것만으로 이론상 최대 2배 빨라질 수 있습니다

## 코드

```cpp
#include <Arduino.h>

const uint32_t WORK_ITERATIONS = 2000000;
volatile uint32_t coreDoneCount = 0;

uint32_t doWork() {
  volatile uint32_t x = 0;
  for (uint32_t i = 0; i < WORK_ITERATIONS; i++) {
    x += i % 7;
  }
  return x;
}

void WorkerTask(void *pvParameters) {
  doWork();
  Serial.printf("WorkerTask on core %d: done\n", xPortGetCoreID());
  coreDoneCount++;
  vTaskDelete(NULL);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // 1) Sequential - do the same total amount of work twice, one after another
  Serial.println("--- Sequential (single core) ---");
  unsigned long t0 = millis();
  doWork();
  doWork();
  unsigned long sequentialMs = millis() - t0;
  Serial.printf("Sequential time: %lu ms\n", sequentialMs);

  // 2) Parallel - split the same total work across both cores
  Serial.println("--- Parallel (dual core) ---");
  coreDoneCount = 0;
  unsigned long t1 = millis();
  xTaskCreatePinnedToCore(WorkerTask, "Worker0", 2048, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(WorkerTask, "Worker1", 2048, NULL, 1, NULL, 1);

  while (coreDoneCount < 2) {
    vTaskDelay(pdMS_TO_TICKS(10));
  }
  unsigned long parallelMs = millis() - t1;
  Serial.printf("Parallel time: %lu ms\n", parallelMs);
  Serial.printf("Speedup: %.2fx\n", (float)sequentialMs / (float)parallelMs);
}

void loop() {
  vTaskDelete(NULL);
}
```

## 실행 & 확인

- `Sequential time`과 `Parallel time`을 비교해보세요 — 같은 양의 총 작업인데도 `Parallel time`이 `Sequential time`의 절반에 가깝게 나오는지 확인
- `Speedup` 값이 `2.00x`에 가까운지 확인 (이상적인 병렬화라면 정확히 2배, 실제로는 Task 생성 오버헤드 등으로 약간 못 미침)

## 관찰 포인트

- `WORK_ITERATIONS`가 너무 크면 `doWork()` 한 번에 몇 초씩 걸려서 04번 실습에서 배운 Task Watchdog이 발동할 수 있습니다 — 값을 조절해가며 안전한 범위에서 실험해보세요 (대략 1초 이내로 끝나는 크기를 권장)
- 모든 작업이 이렇게 깔끔하게 2배 빨라지는 건 아닙니다 — 두 코어가 **같은 데이터를 공유하며 계속 동기화**해야 하는 작업이라면, 17번 실습에서 본 것처럼 오히려 Critical Section/Mutex 대기 시간 때문에 병렬화 효과가 줄어들 수 있습니다. 지금 예제는 두 Task가 서로 전혀 간섭하지 않는(독립적인) 작업이라 이상적인 결과가 나온 것입니다
- Wi-Fi/BLE 스택은 ESP-IDF 내부적으로 주로 Core 0을 많이 사용합니다 — 그래서 `WIFI_TASK_LAB.md`에서 GPIO 작업을 Core 1에 고정했던 설계가, 바로 이 실습에서 배운 "무거운 작업은 분리된 코어에" 원칙의 실전 적용이었습니다

## 다음

19번 파일(`19_LIGHT_SLEEP_LAB.md`)에서 배터리로 동작하는 기기를 위한 저전력 모드를 다룹니다.
