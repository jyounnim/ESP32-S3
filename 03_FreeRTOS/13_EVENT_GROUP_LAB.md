# 13. Event Group — 다중 조건 대기

## 이 실습에서 배우는 것

"A 작업도 끝나고, B 작업도 끝나고, C 작업도 끝나야 다음 단계로 진행"처럼 **여러 조건을 동시에 기다려야** 할 때가 있습니다. Queue나 Semaphore 하나로는 표현하기 번거로운 이런 상황을, Event Group은 **비트 플래그 여러 개**로 깔끔하게 표현합니다.

## 핵심 개념

| 함수 | 설명 |
|---|---|
| `xEventGroupCreate()` | Event Group 생성 (내부적으로 24비트까지 사용 가능) |
| `xEventGroupSetBits(group, bits)` | 특정 비트를 켬 ("이 작업 완료됨") |
| `xEventGroupWaitBits(group, bitsToWaitFor, clearOnExit, waitForAll, 대기시간)` | 지정한 비트(들)가 켜질 때까지 대기 |

## 코드

```cpp
#include <Arduino.h>

#define WIFI_READY_BIT     (1 << 0)
#define SENSOR_READY_BIT   (1 << 1)
#define STORAGE_READY_BIT  (1 << 2)
#define ALL_READY_BITS     (WIFI_READY_BIT | SENSOR_READY_BIT | STORAGE_READY_BIT)

EventGroupHandle_t xSystemEvents;

void WifiInitTask(void *pvParameters) {
  Serial.println("WifiInitTask: initializing...");
  vTaskDelay(pdMS_TO_TICKS(1500));
  Serial.println("WifiInitTask: ready");
  xEventGroupSetBits(xSystemEvents, WIFI_READY_BIT);
  vTaskDelete(NULL);
}

void SensorInitTask(void *pvParameters) {
  Serial.println("SensorInitTask: initializing...");
  vTaskDelay(pdMS_TO_TICKS(800));
  Serial.println("SensorInitTask: ready");
  xEventGroupSetBits(xSystemEvents, SENSOR_READY_BIT);
  vTaskDelete(NULL);
}

void StorageInitTask(void *pvParameters) {
  Serial.println("StorageInitTask: initializing...");
  vTaskDelay(pdMS_TO_TICKS(2200));
  Serial.println("StorageInitTask: ready");
  xEventGroupSetBits(xSystemEvents, STORAGE_READY_BIT);
  vTaskDelete(NULL);
}

void MainTask(void *pvParameters) {
  Serial.println("MainTask: waiting for all subsystems...");
  xEventGroupWaitBits(
    xSystemEvents,
    ALL_READY_BITS,
    pdFALSE,   // don't clear the bits after returning
    pdTRUE,    // wait for ALL bits (AND condition)
    portMAX_DELAY
  );
  Serial.println("MainTask: all subsystems ready! starting main application...");
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(5000));
    Serial.println("MainTask: running normally");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  xSystemEvents = xEventGroupCreate();

  xTaskCreatePinnedToCore(MainTask, "MainTask", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(WifiInitTask, "WifiInitTask", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(SensorInitTask, "SensorInitTask", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(StorageInitTask, "StorageInitTask", 2048, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
```

## 실행 & 확인

- 세 초기화 Task가 각자 다른 시간(1.5초/0.8초/2.2초)에 끝나는데도, `MainTask`는 **가장 늦게 끝나는 StorageInitTask(2.2초)를 기준으로** "all subsystems ready" 메시지를 출력하는지 확인
- 이후 `MainTask`가 5초마다 정상 동작 로그를 출력하는지 확인

## 관찰 포인트

- `xEventGroupWaitBits`의 4번째 인자를 `pdFALSE`로 바꿔보세요 (AND → OR 조건) — 이 경우 세 비트 중 **아무거나 하나만** 켜져도 즉시 통과하므로, `SensorInitTask`(가장 빠른 0.8초)가 끝나자마자 `MainTask`가 진행되는 걸 확인할 수 있습니다
- 3번째 인자(`clearOnExit`)를 `pdTRUE`로 바꾸면, 대기가 풀리는 순간 해당 비트들이 자동으로 꺼집니다 — "한 번만 체크하고 리셋"하는 이벤트(예: 버튼 눌림 감지)에 유용하고, 지금처럼 "한 번 켜지면 계속 켜진 상태 유지"가 필요한 상태 플래그에는 `pdFALSE`가 맞습니다
- Event Group은 Task 여러 개가 서로 다른 시점에 "각자 할 일을 마쳤다"고 보고하고, 하나의 Task가 "전부 모이면 시작"하는 **barrier(장벽) 패턴**에 특히 잘 어울립니다 — 실제로 이 예제가 그 패턴의 전형적인 예시입니다 (여러 서브시스템 초기화 후 메인 로직 시작)

## 다음

14번 파일(`14_SOFTWARE_TIMER_LAB.md`)에서 정해진 시간마다(또는 한 번만) 자동으로 실행되는 Software Timer를 다룹니다.
