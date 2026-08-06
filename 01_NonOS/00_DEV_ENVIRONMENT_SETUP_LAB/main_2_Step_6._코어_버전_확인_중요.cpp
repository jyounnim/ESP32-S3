// Source: 00_DEV_ENVIRONMENT_SETUP_LAB.md
// Section: Step 6. 코어 버전 확인 (중요)

void setup() {
  Serial.begin(115200);
  delay(1000);
#if defined(ESP_ARDUINO_VERSION_MAJOR)
  Serial.printf("Arduino-ESP32 core: %d.%d.%d\n", ESP_ARDUINO_VERSION_MAJOR, ESP_ARDUINO_VERSION_MINOR, ESP_ARDUINO_VERSION_PATCH);
#endif
}
void loop() {}
