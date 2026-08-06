// Source: 18_SPI_SDCARD_LAB.md
// Section: 코드

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

const int SD_CS = 10;

void setup() {
  Serial.begin(115200);
  delay(1000);

  if (!SD.begin(SD_CS)) {
    Serial.println("SD card initialization failed");
    return;
  }
  Serial.println("SD card initialized");

  // 쓰기
  File writeFile = SD.open("/log.txt", FILE_WRITE);
  if (writeFile) {
    writeFile.println("Hello from ESP32-S3");
    writeFile.printf("Uptime: %lu ms\n", millis());
    writeFile.close();
    Serial.println("Write complete");
  } else {
    Serial.println("Failed to open file for writing");
  }

  // 읽기
  File readFile = SD.open("/log.txt");
  if (readFile) {
    Serial.println("--- File contents ---");
    while (readFile.available()) {
      Serial.write(readFile.read());
    }
    readFile.close();
  } else {
    Serial.println("Failed to open file for reading");
  }
}

void loop() {
  // 매 5초 로그 한 줄 추가
  static unsigned long lastLog = 0;
  if (millis() - lastLog > 5000) {
    lastLog = millis();
    File f = SD.open("/log.txt", FILE_APPEND);
    if (f) {
      f.printf("Log entry at %lu ms\n", millis());
      f.close();
      Serial.println("Appended log entry");
    }
  }
}
