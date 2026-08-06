# 19. Light Sleep — 저전력 모드

## 이 실습에서 배우는 것

배터리로 동작하는 기기는 전력 소모를 줄이는 게 중요합니다. ESP32-S3는 CPU 클럭을 잠시 멈추는 **Light Sleep** 모드를 제공합니다. 이 실습에서는 `vTaskDelay()`와 근본적으로 다른 점 — **"한 Task만 쉬는 것"과 "칩 전체가 쉬는 것"의 차이**를 확인합니다.

## 핵심 개념

| 함수 | 설명 |
|---|---|
| `esp_sleep_enable_timer_wakeup(마이크로초)` | 몇 마이크로초 뒤에 자동으로 깨어날지 예약 |
| `esp_light_sleep_start()` | Light Sleep 진입 — 호출 시점부터 **칩 전체(양쪽 코어 포함)가 일시정지**되고, 설정된 타이머(또는 GPIO 등 다른 wakeup source)가 울리면 깨어남 |

## 코드

```cpp
#include <Arduino.h>
#include "esp_sleep.h"

void LightSleepTask(void *pvParameters) {
  for (;;) {
    Serial.println("LightSleepTask: awake, doing some work...");
    vTaskDelay(pdMS_TO_TICKS(2000));

    Serial.println("LightSleepTask: entering light sleep for 3s");
    Serial.flush();   // make sure this message is fully sent before the chip halts

    esp_sleep_enable_timer_wakeup(3000000);   // 3,000,000 microseconds = 3s
    esp_light_sleep_start();

    Serial.println("LightSleepTask: woke up");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  xTaskCreatePinnedToCore(LightSleepTask, "LightSleepTask", 2048, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
```

## 실행 & 확인

- "awake" → 2초 대기 → "entering light sleep" → (3초간 완전 정지) → "woke up"이 계속 반복되는지 확인
- 보드에 다른 LED(전원/상태 LED 등)가 있다면, Light Sleep 구간 동안 미세하게 꺼지거나 어두워지는 게 보일 수도 있습니다 (보드에 따라 다름)

## 관찰 포인트

- **이게 핵심입니다**: 지금까지 써온 `vTaskDelay()`는 **호출한 그 Task만** Blocked 상태가 되고, 다른 Task와 다른 코어는 평소처럼 계속 동작합니다. 반면 `esp_light_sleep_start()`는 **칩 전체를 멈춥니다** — 이 순간에는 다른 Task도, 다른 코어도, 심지어 대부분의 주변장치도 함께 멈춥니다. 만약 `WIFI_TASK_LAB.md`처럼 `GpioTask`가 Core 1에서 계속 LED를 갱신하고 있었다면, Light Sleep 구간 동안 그것도 완전히 멈춥니다
- 그래서 Light Sleep은 "Task 하나가 잠깐 딴짓해도 되는" 상황이 아니라, **"지금은 온 시스템이 정말 할 일이 없다"**는 확신이 있을 때만 써야 합니다
- 참고로 FreeRTOS 본연의 기능 중에는 "모든 Task가 Blocked 상태가 되면 자동으로 Light Sleep에 들어가는" **Tickless Idle**이라는 완전 자동화 기능도 있습니다. 다만 이 기능은 `sdkconfig`의 `CONFIG_FREERTOS_USE_TICKLESS_IDLE`, `CONFIG_PM_ENABLE` 옵션이 켜져 있어야 하는데, 표준 Arduino/PlatformIO 프레임워크는 미리 컴파일된 라이브러리를 쓰기 때문에 이런 `sdkconfig` 옵션을 사용자가 직접 바꿀 수 없습니다(ESP-IDF를 직접 써야 가능). 그래서 이번 실습처럼 **직접 코드에서 명시적으로 `esp_light_sleep_start()`를 호출하는 방식**이 Arduino 환경에서는 현실적인 대안입니다

## 다음

20번 파일(`20_FREERTOS_HOOKS_LAB.md`)에서 각 코어의 유휴 시간에 자동으로 실행되는 Hook 함수를 다룹니다.
