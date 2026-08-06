# 10. Queue 기본 — Task 간 데이터 전달

## 이 실습에서 배우는 것

Semaphore가 "신호"만 전달한다면, Queue는 **실제 데이터**를 Task 간에 안전하게 전달합니다. FIFO(선입선출) 구조라 보낸 순서대로 받는 쪽에 전달되며, 내부적으로 여러 Task가 동시에 접근해도 안전하도록 보호되어 있습니다.

## 핵심 개념

| 함수 | 설명 |
|---|---|
| `xQueueCreate(개수, 항목크기)` | 큐 생성 — 몇 개까지 담을 수 있는지, 항목 하나의 크기가 몇 바이트인지 지정 |
| `xQueueSend(queue, &data, 대기시간)` | 데이터를 큐에 추가 (큐가 가득 찼으면 지정한 시간만큼 대기) |
| `xQueueReceive(queue, &buffer, 대기시간)` | 큐에서 데이터를 꺼내옴 (비어있으면 지정한 시간만큼 대기) |

## 코드

```cpp
#include <Arduino.h>

struct SensorData {
  int id;
  float value;
};

QueueHandle_t xDataQueue;

void SensorTask(void *pvParameters) {     // Sender
  int counter = 0;
  for (;;) {
    SensorData data;
    data.id = counter++;
    data.value = 20.0f + (counter % 10) * 0.5f;   // simulate a changing sensor reading

    if (xQueueSend(xDataQueue, &data, pdMS_TO_TICKS(100)) == pdTRUE) {
      Serial.printf("SensorTask: sent id=%d value=%.1f\n", data.id, data.value);
    } else {
      Serial.println("SensorTask: queue full, send failed");
    }
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void ProcessTask(void *pvParameters) {    // Receiver
  SensorData received;
  for (;;) {
    if (xQueueReceive(xDataQueue, &received, portMAX_DELAY) == pdTRUE) {
      Serial.printf("ProcessTask: received id=%d value=%.1f\n", received.id, received.value);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  xDataQueue = xQueueCreate(5, sizeof(SensorData));   // holds up to 5 SensorData items

  xTaskCreatePinnedToCore(SensorTask, "SensorTask", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(ProcessTask, "ProcessTask", 2048, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
```

## 실행 & 확인

- `id`와 `value`가 순서대로 정확히 전달되는지 확인 (Sender가 보낸 순서 = Receiver가 받는 순서)

## 관찰 포인트

- 단순 `int`가 아니라 **구조체(`SensorData`)** 전체를 큐에 담아 보냈습니다 — Queue는 지정한 크기만큼의 바이트를 그대로 복사해서 전달하므로, 구조체·배열 등 원하는 형태의 데이터를 자유롭게 담을 수 있습니다
- `ProcessTask`가 없다고 가정하고 `SensorTask`만 계속 돌린다면, 큐가 가득 찬 뒤(5개) `xQueueSend`가 100ms 동안 자리가 나길 기다리다 실패(`send failed`)하는 걸 볼 수 있습니다 — `ProcessTask` 생성 코드를 잠시 주석 처리하고 확인해보세요
- 포인터(`&data`)를 담는 방식(`xQueueSend(queue, &pointer, ...)`)도 가능하지만, 그 포인터가 가리키는 메모리가 살아있는 동안만 유효하다는 점에 주의해야 합니다 — 지역 변수의 주소를 넘기면 위험할 수 있어, 값 자체를 복사해서 넘기는 지금 방식이 더 안전합니다

## 다음

11번 파일(`11_QUEUE_SET_LAB.md`)에서 여러 개의 Queue를 하나의 Task가 동시에 감시하는 방법을 다룹니다.
