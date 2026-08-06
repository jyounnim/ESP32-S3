// Source: 40_RFID_RC522_LAB.md
// Section: 코드

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 17

MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  delay(1000);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Ready. Tap an RFID card/tag.");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  Serial.print("UID:");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.printf(" %02X", mfrc522.uid.uidByte[i]);
  }
  Serial.println();

  // 카드 종류도 함께 확인 가능
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.printf("Card type: %s\n", mfrc522.PICC_GetTypeName(piccType));

  mfrc522.PICC_HaltA();   // 다음 카드를 받을 준비
}
