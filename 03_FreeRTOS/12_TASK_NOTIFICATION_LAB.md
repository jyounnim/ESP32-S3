# 12. Task Notification — 경량 통신 수단

## 이 실습에서 배우는 것

모든 FreeRTOS Task는 태어날 때부터 **32비트 알림 값(notification value)**을 하나씩 내장하고 있습니다. Queue나 Semaphore처럼 별도 객체를 생성할 필요 없이, 이 내장된 슬롯 하나로 간단한 신호나 값 전달을 할 수 있습니다. 별도 커널 객체가 없어서 **속도가 더 빠르고 메모리도 덜 씁니다** — FreeRTOS가 공식적으로 "Semaphore/Queue보다 최대 몇 배 빠르다"고 소개하는 기능입니다.

## 핵심 개념

| 함수 | 설명 |
|---|---|
| `xTaskNotify(handle, value, action)` | 특정 Task에게 값을 전달 (action에 따라 덮어쓰기/증가 등 동작이 다름) |
| `xTaskNotifyWait(clearOnEntry, clearOnExit, &value, 대기시간)` | 알림이 올 때까지 대기하고 값을 받아옴 |
| `xTaskNotifyGive(handle)` / `ulTaskNotifyTake(clear, 대기시간)` | 값 없이 "신호"만 주고받는 간단한 버전 — Binary Semaphore의 경량 대체재 |

## 코드

```cpp
#include <Arduino.h>

TaskHandle_t consumerHandle = NULL;

void ProducerTask(void *pvParameters) {
  uint32_t counter = 0;
  for (;;) {
    counter++;
    if (consumerHandle != NULL) {
      xTaskNotify(consumerHandle, counter, eSetValueWithOverwrite);
      Serial.printf("ProducerTask: notified value=%lu\n", counter);
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void ConsumerTask(void *pvParameters) {
  uint32_t receivedValue;
  for (;;) {
    if (xTaskNotifyWait(0, 0xFFFFFFFF, &receivedValue, portMAX_DELAY) == pdTRUE) {
      Serial.printf("ConsumerTask: received value=%lu\n", receivedValue);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  xTaskCreatePinnedToCore(ConsumerTask, "ConsumerTask", 2048, NULL, 1, &consumerHandle, 1);
  xTaskCreatePinnedToCore(ProducerTask, "ProducerTask", 2048, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
```

## 실행 & 확인

- `ProducerTask`가 1초마다 보낸 `counter` 값을 `ConsumerTask`가 그대로 받는지 확인

## 관찰 포인트

- `eSetValueWithOverwrite`는 "이전 값을 덮어쓰기" 모드입니다 — `ConsumerTask`가 아직 이전 값을 처리하지 않았는데 새 값이 또 오면, 이전 값은 사라지고 최신 값으로 대체됩니다. Queue였다면 처리 못한 값들이 큐에 쌓였겠지만, Notification은 슬롯이 하나뿐이라 이런 차이가 생깁니다
- **값 없이 신호만** 필요하다면(07번 실습의 ISR 패턴처럼) 아래처럼 더 간단하게 쓸 수 있습니다:
  ```cpp
  // 신호를 보내는 쪽
  xTaskNotifyGive(consumerHandle);

  // 받는 쪽
  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
  ```
  이 조합은 07번 실습의 `xSemaphoreGiveFromISR`/`xSemaphoreTake`를 대체할 수 있고, 실제로 ISR에서도 `vTaskNotifyGiveFromISR()`로 사용 가능합니다
- **주의할 점**: Task Notification은 Task 하나당 슬롯이 딱 하나뿐입니다 — 여러 발신자가 한 Task에게 동시에 알림을 보내면 값이 서로 덮어써질 수 있어, "여러 곳에서 오는 서로 다른 데이터를 순서대로 모두 받아야 하는" 상황에는 적합하지 않습니다. 그럴 땐 Queue(10번 실습)를 쓰는 게 맞습니다

## 다음

13번 파일(`13_EVENT_GROUP_LAB.md`)에서 여러 조건이 동시에 만족될 때까지 기다리는 Event Group을 다룹니다.
