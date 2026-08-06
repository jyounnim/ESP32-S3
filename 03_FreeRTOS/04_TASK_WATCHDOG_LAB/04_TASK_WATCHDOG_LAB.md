# 04. Task Watchdog — Task가 반드시 yield해야 하는 이유

## 이 실습에서 배우는 것

RTOS의 각 코어는 할 일이 없을 때 **Idle Task**를 실행합니다. ESP32-S3는 기본적으로 이 Idle Task를 감시하는 **Task Watchdog Timer(TWDT)**가 켜져 있어서, 어떤 Task가 CPU를 독점해서 Idle Task가 정해진 시간(기본 약 5초) 동안 한 번도 실행되지 못하면 시스템 오류로 간주하고 **강제로 재부팅**시킵니다. 이 실습에서는 그 상황을 일부러 재현해봅니다.

> ⚠️ 아래 코드는 의도적으로 보드를 재부팅시키는 코드입니다. 예상된 동작이니 당황하지 마세요.

## 핵심 개념

- `vTaskDelay()`, `xQueueReceive()`(대기), `xSemaphoreTake()`(대기) 등은 호출하는 동안 **다른 Task(Idle Task 포함)에게 CPU를 양보**합니다
- 반대로 `for(;;) { 계산만 반복 }`처럼 양보 없는 무한루프는 그 Task가 할당된 코어를 완전히 독점합니다
- 독점된 코어의 Idle Task가 오래 굶으면 Task Watchdog이 발동 → `Task watchdog got triggered (IDLE0/IDLE1)` 에러와 함께 재부팅

## 코드 (문제 상황 재현)

```cpp
#include <Arduino.h>

void GreedyTask(void *pvParameters) {
  Serial.println("GreedyTask: started, hogging the CPU with no yield...");
  uint32_t counter = 0;
  for (;;) {
    counter++;   // yield 없는 순수 계산 반복 - Idle Task가 실행될 기회를 주지 않음
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  xTaskCreatePinnedToCore(GreedyTask, "GreedyTask", 2048, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
```

### 실행 & 확인

- 업로드 후 몇 초 안에 시리얼 모니터에 `E (xxxx) task_wdt: Task watchdog got triggered. The following tasks/users did not reset the watchdog in time: IDLE1 ...`같은 로그와 함께 보드가 재부팅되는지 확인
- 이건 버그가 아니라 **Task Watchdog이 정상적으로 자기 역할(먹통 상태 감지)을 한 것**입니다

## 코드 (해결 — 주기적으로 yield)

```cpp
#include <Arduino.h>

void PoliteTask(void *pvParameters) {
  Serial.println("PoliteTask: started, yielding periodically...");
  uint32_t counter = 0;
  for (;;) {
    counter++;
    if (counter % 1000000 == 0) {
      Serial.printf("PoliteTask: still alive (counter=%lu)\n", counter);
      vTaskDelay(1);   // 최소한의 yield - Idle Task에게 실행 기회를 줌
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  xTaskCreatePinnedToCore(PoliteTask, "PoliteTask", 2048, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
```

### 실행 & 확인

- 이번에는 재부팅 없이 `PoliteTask: still alive`가 계속 출력되는지 확인 — 같은 방식으로 계산을 반복하지만, 주기적으로 `vTaskDelay(1)`을 호출해 Idle Task에게 실행 기회를 주기 때문에 watchdog이 만족합니다

## 관찰 포인트

- ESP32-S3 Arduino의 `delay()`는 내부적으로 `vTaskDelay()`를 호출하도록 구현되어 있어서, 클래식 AVR Arduino의 `delay()`(순수 busy-wait)와 달리 **이미 스케줄러 친화적**입니다. 즉 `for(;;) { delay(10); }`처럼 `delay()`가 루프 안에 있다면 이번 실습의 문제가 발생하지 않습니다 — 문제는 어떤 형태로든 **양보하는 호출이 아예 없는** 순수 계산 루프일 때만 발생합니다
- `vTaskDelay(1)`처럼 아주 짧은 delay도 watchdog을 만족시키기에 충분합니다 — 중요한 건 시간의 길이가 아니라 "양보라는 행위 자체가 있었는가"입니다
- 02번 실습(`LowPriorityTask`)에서도 `vTaskDelay(1)`을 주기적으로 넣어준 이유가 바로 이것입니다 — 지금 이 실습으로 그 코드의 의미가 명확해졌을 것입니다

## 다음

05번 파일(`05_PRIORITY_INVERSION_LAB.md`)에서 우선순위가 항상 성능에 유리하게만 작동하지는 않는 상황(우선순위 역전)을 다룹니다.
