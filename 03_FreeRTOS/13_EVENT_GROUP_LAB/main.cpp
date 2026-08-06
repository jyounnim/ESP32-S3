// Source: 13_EVENT_GROUP_LAB.md
// Section: 코드

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
