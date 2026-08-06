# 07. 인터럽트(ISR) + Binary Semaphore

## 이 실습에서 배우는 것

RTOS에서 인터럽트(ISR)는 "매우 급하지만 아주 짧게 처리해야 하는" 특수한 컨텍스트입니다. ISR 안에서 `Serial.print`나 무거운 로직을 직접 실행하면 안 되고, 대신 **"이벤트가 발생했다"는 신호만 Task에게 넘긴 뒤 즉시 빠져나오는** 패턴이 표준입니다. Binary Semaphore가 이 역할에 가장 흔히 쓰입니다.

## 준비물

- 택트 스위치(버튼) 1개 — 한쪽 핀을 GPIO5, 다른 쪽을 GND에 연결 (`GPIO_LAB.md`와 동일한 배선)

## 핵심 개념

| 함수 | 설명 |
|---|---|
| `attachInterrupt(pin, ISR함수, 모드)` | 특정 핀의 신호 변화에 ISR을 연결 |
| `IRAM_ATTR` | ISR 함수를 Flash가 아닌 내부 RAM에 배치 — Flash 접근이 잠시 막히는 상황에서도 ISR이 실행되도록 보장 (ESP32 계열 필수) |
| `xSemaphoreGiveFromISR(sem, &woken)` | ISR 전용 Give 함수 — 일반 `xSemaphoreGive`는 ISR 안에서 쓰면 안 됨 |
| `portYIELD_FROM_ISR(woken)` | ISR 종료 직후, 깨어난 Task의 우선순위가 더 높다면 즉시 컨텍스트 전환 |

## 코드

```cpp
#include <Arduino.h>

const int BUTTON_PIN = 5;
SemaphoreHandle_t xButtonSemaphore;

void IRAM_ATTR buttonISR() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR(xButtonSemaphore, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void ButtonHandlerTask(void *pvParameters) {
  for (;;) {
    if (xSemaphoreTake(xButtonSemaphore, portMAX_DELAY) == pdTRUE) {
      Serial.println("ButtonHandlerTask: interrupt signal received, handling button press");
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  xButtonSemaphore = xSemaphoreCreateBinary();
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);

  xTaskCreatePinnedToCore(ButtonHandlerTask, "ButtonHandlerTask", 2048, NULL, 2, NULL, 1);

  Serial.println("Ready. Press the button connected to GPIO5.");
}

void loop() {
  vTaskDelete(NULL);
}
```

## 실행 & 확인

- 버튼을 누를 때마다 `ButtonHandlerTask: interrupt signal received...`가 출력되는지 확인
- 버튼을 누른 즉시(체감상 딜레이 없이) 반응하는지 확인 — ISR이 즉시 Task를 깨우기 때문입니다

## 관찰 포인트

- ISR(`buttonISR`) 안에는 `Serial.print`가 전혀 없습니다 — ISR 안에서 시리얼 출력처럼 시간이 걸리는 작업을 하면 다른 인터럽트 처리가 지연되거나 예기치 못한 크래시로 이어질 수 있습니다. 실제 출력은 항상 `ButtonHandlerTask`(일반 Task)에서 이루어집니다
- `IRAM_ATTR`을 빼고 컴파일해서 업로드해보세요 — 대부분의 경우 당장은 동작하는 것처럼 보일 수 있지만, Flash 읽기/쓰기가 진행 중인 순간(OTA, NVS 쓰기 등)에 인터럽트가 발생하면 크래시할 위험이 생깁니다. ESP32 계열에서는 ISR에 `IRAM_ATTR`을 붙이는 것이 사실상 필수 관례입니다
- 버튼을 아주 빠르게 여러 번 누르거나 기계적 채터링이 있으면, Binary Semaphore가 "이미 Give된 상태"라 추가 Give는 무시되고 Task가 한 번만 깨어날 수 있습니다 — 실무에서는 여기에 디바운싱 로직을 추가로 결합합니다 (`GPIO_LAB.md`의 디바운싱 참고)

## 다음

08번 파일(`08_COUNTING_SEMAPHORE_LAB.md`)에서 여러 개의 자원을 관리하는 Counting Semaphore를 다룹니다.
