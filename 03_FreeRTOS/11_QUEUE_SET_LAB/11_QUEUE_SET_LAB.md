# 11. Queue Set — 여러 Queue 동시 대기

## 이 실습에서 배우는 것

`xQueueReceive(queue, ..., portMAX_DELAY)`는 **딱 하나의 큐**만 기다릴 수 있습니다. 만약 "큐 A 또는 큐 B, 둘 중 먼저 도착하는 걸 처리하고 싶다"면 어떻게 할까요? 이럴 때 쓰는 것이 **Queue Set**입니다. 여러 큐(또는 세마포어)를 하나로 묶어서, 그중 아무거나 데이터가 도착하면 즉시 깨어나는 구조를 만들 수 있습니다.

## 핵심 개념

| 함수 | 설명 |
|---|---|
| `xQueueCreateSet(전체용량)` | Queue Set 생성 — 담을 큐들의 용량 합계 이상으로 지정 |
| `xQueueAddToSet(queue, set)` | 개별 큐를 Set에 등록 |
| `xQueueSelectFromSet(set, 대기시간)` | Set에 속한 큐들 중 **데이터가 도착한 큐의 핸들**을 반환 (어느 큐인지 알려줌) |

## 코드

```cpp
#include <Arduino.h>

QueueHandle_t xTempQueue;
QueueHandle_t xHumidityQueue;
QueueSetHandle_t xSensorQueueSet;

void TempSensorTask(void *pvParameters) {
  float temp = 20.0f;
  for (;;) {
    temp += 0.3f;
    xQueueSend(xTempQueue, &temp, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(1500));
  }
}

void HumiditySensorTask(void *pvParameters) {
  float humidity = 40.0f;
  for (;;) {
    humidity += 1.0f;
    xQueueSend(xHumidityQueue, &humidity, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(900));
  }
}

void DispatcherTask(void *pvParameters) {   // one task, watching two queues at once
  for (;;) {
    QueueSetMemberHandle_t activated = xQueueSelectFromSet(xSensorQueueSet, portMAX_DELAY);

    if (activated == xTempQueue) {
      float value;
      xQueueReceive(xTempQueue, &value, 0);
      Serial.printf("DispatcherTask: [TEMP] %.1f C\n", value);
    } else if (activated == xHumidityQueue) {
      float value;
      xQueueReceive(xHumidityQueue, &value, 0);
      Serial.printf("DispatcherTask: [HUMIDITY] %.1f %%\n", value);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  xTempQueue = xQueueCreate(3, sizeof(float));
  xHumidityQueue = xQueueCreate(3, sizeof(float));

  xSensorQueueSet = xQueueCreateSet(3 + 3);   // capacity >= sum of member queue lengths
  xQueueAddToSet(xTempQueue, xSensorQueueSet);
  xQueueAddToSet(xHumidityQueue, xSensorQueueSet);

  xTaskCreatePinnedToCore(TempSensorTask, "TempSensorTask", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(HumiditySensorTask, "HumiditySensorTask", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(DispatcherTask, "DispatcherTask", 2048, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
```

## 실행 & 확인

- `DispatcherTask` 단 하나가 온도(1.5초 주기)와 습도(0.9초 주기) 두 큐를 동시에 감시하며, 어느 쪽이든 도착하는 즉시 `[TEMP]` 또는 `[HUMIDITY]` 로그를 출력하는지 확인
- 두 센서의 주기가 달라서 출력 순서가 섞여서(interleaved) 나오는지 확인

## 관찰 포인트

- `DispatcherTask`가 없다면, 두 큐를 동시에 감시하기 위해 Task를 두 개 만들거나(10번 실습 방식), 짧은 타임아웃으로 두 큐를 번갈아 폴링(polling)하는 수밖에 없습니다 — Queue Set은 Task 하나로 여러 이벤트 소스를 깔끔하게 처리할 수 있게 해줍니다
- `xQueueSelectFromSet`은 "어느 큐에서 왔는지"만 알려줄 뿐, 데이터를 자동으로 꺼내주지는 않습니다 — 반환된 핸들로 **직접 `xQueueReceive`를 한 번 더 호출**해야 실제 데이터를 얻을 수 있다는 점에 주의하세요 (그래서 코드에서 마지막 인자를 `0`으로 줬습니다 — 이미 데이터가 있다고 확신하는 상황이라 대기가 필요 없음)
- Queue Set에는 일반 Queue뿐 아니라 **Semaphore**도 등록할 수 있습니다 — "여러 종류의 이벤트(데이터 도착 + 신호 발생)를 한 Task가 동시에 기다리는" 구조에 유용합니다

> 참고: 이 기능은 `configUSE_QUEUE_SETS`가 활성화되어 있어야 사용할 수 있습니다. 표준 Arduino-ESP32 프레임워크에서는 기본적으로 활성화되어 있지만, 빌드 시 `xQueueCreateSet`이 정의되지 않았다는 에러가 나면 사용 중인 코어 버전에서 이 옵션이 꺼져 있을 수 있습니다.

## 다음

12번 파일(`12_TASK_NOTIFICATION_LAB.md`)에서 Queue/Semaphore보다 더 가벼운 통신 수단인 Task Notification을 다룹니다.
