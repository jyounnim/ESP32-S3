// Source: 14_SOFTWARE_TIMER_LAB.md
// Section: 코드

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
