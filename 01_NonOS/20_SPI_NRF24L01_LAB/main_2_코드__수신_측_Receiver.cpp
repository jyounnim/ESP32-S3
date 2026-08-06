// Source: 20_SPI_NRF24L01_LAB.md
// Section: 코드 — 수신 측 (Receiver)

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

  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();

  Serial.println("Receiver ready");
}

void loop() {
  if (radio.available()) {
    int received;
    radio.read(&received, sizeof(received));
    Serial.printf("Received: %d\n", received);
  }
}
