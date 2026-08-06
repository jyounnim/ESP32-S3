// Source: 20_SPI_NRF24L01_LAB.md
// Section: 코드 — 송신 측 (Transmitter)

#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>

#define CE_PIN 4
#define CSN_PIN 10

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "00001";

void setup() {
  Serial.begin(115200);
  delay(1000);

  if (!radio.begin()) {
    Serial.println("NRF24L01 not found - check wiring");
    while (1) delay(1000);
  }

  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);   // 실습용은 LOW로 충분, 거리 필요시 HIGH
  radio.stopListening();

  Serial.println("Transmitter ready");
}

void loop() {
  static int counter = 0;
  counter++;

  bool success = radio.write(&counter, sizeof(counter));
  Serial.printf("Sent: %d, success: %s\n", counter, success ? "yes" : "no");

  delay(1000);
}
