// Source: 19_LIGHT_SLEEP_LAB.md
// Section: 코드

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
