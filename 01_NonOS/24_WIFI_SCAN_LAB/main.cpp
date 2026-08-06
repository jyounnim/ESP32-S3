// Source: 24_WIFI_SCAN_LAB.md
// Section: 코드

#include <Arduino.h>
#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();   // 스캔 전 기존 연결 상태 정리
  delay(100);
}

void loop() {
  Serial.println("Scanning...");
  int n = WiFi.scanNetworks();
  Serial.printf("Found %d networks\n", n);

  for (int i = 0; i < n; i++) {
    Serial.printf("%2d: %-24s  %4d dBm  ch%2d  %s\n",
      i + 1,
      WiFi.SSID(i).c_str(),
      WiFi.RSSI(i),
      WiFi.channel(i),
      WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "OPEN" : "encrypted");
  }

  WiFi.scanDelete();   // 스캔 결과 메모리 해제
  Serial.println();
  delay(5000);
}
