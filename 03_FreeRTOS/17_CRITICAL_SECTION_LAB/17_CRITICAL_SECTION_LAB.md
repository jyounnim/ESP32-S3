# 17. Critical Section — 듀얼코어 스핀락

## 이 실습에서 배우는 것

09번 실습에서 배운 Mutex는 "Take → 작업 → Give" 흐름에 Task가 잠시 Blocked 상태로 대기할 수 있다는 전제가 깔려 있습니다. 하지만 아주 짧은 코드(변수 하나 증가시키기 등)를 보호할 때는 Mutex는 오히려 무겁습니다. 이런 아주 짧은 구간은 **Critical Section**으로 보호합니다. 특히 ESP32-S3는 **듀얼코어**라서, 단순히 인터럽트만 끄는 방식(단일 코어 FreeRTOS의 전통적인 방식)으로는 부족하고 **스핀락(spinlock)**이 필요합니다.

## 핵심 개념

```cpp
portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;

portENTER_CRITICAL(&spinlock);
// 아주 짧은 보호 구간
portEXIT_CRITICAL(&spinlock);
```

- 단일 코어 FreeRTOS는 `taskENTER_CRITICAL()`처럼 인자가 필요 없습니다 — 인터럽트만 꺼도 "동시 접근"이 불가능하기 때문입니다
- 하지만 듀얼코어에서는 **다른 코어가 여전히 실행 중**일 수 있어서, 인터럽트를 끄는 것만으로는 부족합니다 — 그래서 ESP32의 Critical Section API는 `portMUX_TYPE` 스핀락을 인자로 요구합니다
- 스핀락은 진짜로 "짧게 걸고 바로 푸는" 용도입니다 — 그 안에서 `vTaskDelay`나 다른 블로킹 호출을 하면 안 됩니다

## 코드 (문제 상황 — 보호 없이 두 코어에서 동시에 증가)

```cpp
#include <Arduino.h>

volatile int32_t sharedCounter = 0;
const int ITERATIONS = 100000;

void UnprotectedIncrementTask(void *pvParameters) {
  for (int i = 0; i < ITERATIONS; i++) {
    sharedCounter++;   // read-modify-write - NOT atomic across cores
  }
  Serial.printf("Core %d: finished (unprotected)\n", xPortGetCoreID());
  vTaskDelete(NULL);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  sharedCounter = 0;

  xTaskCreatePinnedToCore(UnprotectedIncrementTask, "Core0Task", 2048, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(UnprotectedIncrementTask, "Core1Task", 2048, NULL, 1, NULL, 1);

  vTaskDelay(pdMS_TO_TICKS(3000));   // wait for both tasks to finish
  Serial.printf("Expected: %d, Actual: %d\n", ITERATIONS * 2, sharedCounter);
}

void loop() {
  vTaskDelete(NULL);
}
```

### 실행 & 확인

- `Expected`(200000)와 `Actual` 값을 비교해보세요 — 대부분 **Actual이 더 작게** 나옵니다. 두 코어가 동시에 `sharedCounter`를 읽고, 각자 +1한 값을 다시 쓰는 과정에서 서로의 결과를 덮어써버리는 경우(Race Condition)가 발생하기 때문입니다

## 코드 (해결 — Critical Section으로 보호)

```cpp
#include <Arduino.h>

volatile int32_t sharedCounter = 0;
portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
const int ITERATIONS = 100000;

void ProtectedIncrementTask(void *pvParameters) {
  for (int i = 0; i < ITERATIONS; i++) {
    portENTER_CRITICAL(&spinlock);
    sharedCounter++;
    portEXIT_CRITICAL(&spinlock);
  }
  Serial.printf("Core %d: finished (protected)\n", xPortGetCoreID());
  vTaskDelete(NULL);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  sharedCounter = 0;

  xTaskCreatePinnedToCore(ProtectedIncrementTask, "Core0Task", 2048, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(ProtectedIncrementTask, "Core1Task", 2048, NULL, 1, NULL, 1);

  vTaskDelay(pdMS_TO_TICKS(3000));
  Serial.printf("Expected: %d, Actual: %d\n", ITERATIONS * 2, sharedCounter);
}

void loop() {
  vTaskDelete(NULL);
}
```

### 실행 & 확인

- 이번엔 `Expected`와 `Actual`이 정확히 일치하는지 확인 — `portENTER_CRITICAL`/`portEXIT_CRITICAL`이 "읽기-수정-쓰기"를 원자적(atomic)으로 만들어줘서, 어느 코어도 중간에 끼어들 수 없습니다

## 관찰 포인트

- Critical Section과 Mutex는 목적이 비슷해 보이지만 무게가 다릅니다 — Critical Section은 "몇 줄짜리 아주 짧은 코드"용이고, Mutex는 "네트워크 요청, 파일 접근처럼 시간이 걸릴 수 있는 코드"용입니다. 오래 걸리는 작업을 Critical Section으로 감싸면 다른 코어 전체가 그 시간만큼 멈춰버리는 심각한 성능 문제가 생깁니다
- 같은 스핀락(`spinlock`)을 여러 변수 보호에 재사용할 수도 있지만, 그러면 서로 무관한 변수에 대한 접근까지 불필요하게 직렬화됩니다 — 보호하려는 데이터 단위별로 스핀락을 따로 두는 것이 일반적입니다
- `volatile`을 `sharedCounter` 선언에 붙인 이유를 생각해보세요 — 컴파일러가 최적화 과정에서 "이 변수는 다른 코어/인터럽트에 의해 예고 없이 바뀔 수 있다"는 걸 알아야, 값을 레지스터에 캐싱한 채 방치하지 않고 매번 실제 메모리를 읽고 씁니다

## 다음

18번 파일(`18_MULTICORE_LAB.md`)에서 ESP32-S3의 듀얼코어를 본격적으로 활용해 작업을 분산시키는 방법을 다룹니다.
