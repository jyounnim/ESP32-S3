// Source: 07_ISR_SEMAPHORE_LAB.md
// Section: 코드

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
