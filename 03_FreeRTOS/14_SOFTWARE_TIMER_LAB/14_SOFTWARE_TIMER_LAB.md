# 14. Software Timer (One-shot / Periodic)

## 이 실습에서 배우는 것

"정해진 시간마다 자동으로 뭔가 실행"하거나 "일정 시간 활동이 없으면 타임아웃 처리"하는 패턴은 매우 흔합니다. 이런 걸 직접 Task + `vTaskDelay`로 구현할 수도 있지만, FreeRTOS는 **Software Timer**라는 전용 기능을 제공합니다. 별도 Task 없이도, 콜백 함수 하나로 반복/일회성 타이밍 로직을 만들 수 있습니다.

## 핵심 개념

| 함수 | 설명 |
|---|---|
| `xTimerCreate(이름, 주기, autoReload, ID, 콜백함수)` | 타이머 생성. `autoReload=pdTRUE`면 반복(Periodic), `pdFALSE`면 한 번만(One-shot) |
| `xTimerStart(timer, 대기시간)` | 타이머 시작 |
| `xTimerReset(timer, 대기시간)` | 카운트다운을 처음부터 다시 시작 (One-shot 타이머로 "타임아웃"을 구현할 때 핵심) |
| `xTimerStop(timer, 대기시간)` | 타이머 정지 |

## 코드

```cpp
#include <Arduino.h>

TimerHandle_t xPeriodicTimer;
TimerHandle_t xTimeoutTimer;

void periodicCallback(TimerHandle_t xTimer) {
  Serial.println("PeriodicTimer: tick (every 2s)");
}

void timeoutCallback(TimerHandle_t xTimer) {
  Serial.println("TimeoutTimer: no activity for 3s - timeout!");
}

void ActivitySimulatorTask(void *pvParameters) {
  for (int i = 0; i < 3; i++) {
    vTaskDelay(pdMS_TO_TICKS(1500));
    Serial.println("ActivitySimulatorTask: activity detected, resetting timeout timer");
    xTimerReset(xTimeoutTimer, 0);
  }
  Serial.println("ActivitySimulatorTask: no more activity from now on");
  vTaskDelete(NULL);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  xPeriodicTimer = xTimerCreate("PeriodicTimer", pdMS_TO_TICKS(2000), pdTRUE, NULL, periodicCallback);
  xTimeoutTimer  = xTimerCreate("TimeoutTimer",  pdMS_TO_TICKS(3000), pdFALSE, NULL, timeoutCallback);

  xTimerStart(xPeriodicTimer, 0);
  xTimerStart(xTimeoutTimer, 0);

  xTaskCreatePinnedToCore(ActivitySimulatorTask, "ActivitySimulatorTask", 2048, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
```

## 실행 & 확인

- `PeriodicTimer`가 2초마다 계속 "tick"을 출력하는지 확인 (자동 재시작, `autoReload=pdTRUE`)
- `ActivitySimulatorTask`가 3번(1.5초 간격) "activity detected"를 출력하며 그때마다 `TimeoutTimer`를 리셋하는지 확인
- 세 번째 활동 이후로는 리셋이 더 이상 없으므로, **약 3초 뒤** `TimeoutTimer: no activity for 3s - timeout!`이 **딱 한 번만** 출력되는지 확인 (`autoReload=pdFALSE`라서 그 이후로는 다시 자동 시작되지 않음)

## 관찰 포인트

- 타이머 콜백은 별도 Task를 새로 만드는 게 아니라, FreeRTOS가 내부적으로 관리하는 **하나의 "Timer Service Task"(Daemon Task)**에서 순차적으로 실행됩니다 — 즉 모든 Software Timer 콜백이 **이 Task 하나를 공유**합니다. 그래서 콜백 안에서 `vTaskDelay`처럼 오래 대기하는 코드를 쓰면 다른 모든 타이머까지 지연되니 절대 금지입니다. 콜백은 항상 빠르게 끝나야 합니다
- 지금 사용한 "활동이 있을 때마다 리셋, 일정 시간 조용하면 타임아웃" 패턴은 실무에서 매우 흔합니다 — 화면 자동 꺼짐, 세션 만료, 통신 연결 끊김 감지 등이 전부 이 패턴의 응용입니다
- `xTimerCreate`의 4번째 인자(Timer ID)는 지금 `NULL`로 뒀지만, 여러 타이머가 콜백 함수 하나를 공유할 때 "어느 타이머에서 호출된 건지" 구분하는 용도로 활용할 수 있습니다 (콜백 안에서 `pvTimerGetTimerID(xTimer)`로 조회)

## 다음

15번 파일(`15_STACK_MONITORING_LAB.md`)에서 각 Task의 스택 사용량을 확인하고 Stack Overflow를 진단하는 방법을 다룹니다.
