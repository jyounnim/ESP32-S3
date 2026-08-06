// Source: 17_SPI_BASICS_LAB.md
// Section: 코드 — SPI 초기화 템플릿

#include <Arduino.h>
#include <SPI.h>

const int CS_PIN = 10;

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);   // 평소엔 HIGH(비활성) 유지, 통신 시작할 때만 LOW로

  SPI.begin();   // 기본 핀(MOSI=11, MISO=13, SCK=12) 사용
  Serial.println("SPI initialized");
}

void loop() {
  // 통신 예시: CS를 LOW로 내려 장치를 선택한 뒤 데이터 교환, 끝나면 다시 HIGH
  digitalWrite(CS_PIN, LOW);
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  byte response = SPI.transfer(0x00);   // 더미 바이트 전송, 응답 수신
  SPI.endTransaction();
  digitalWrite(CS_PIN, HIGH);

  Serial.printf("Response: 0x%02X\n", response);
  delay(1000);
}
