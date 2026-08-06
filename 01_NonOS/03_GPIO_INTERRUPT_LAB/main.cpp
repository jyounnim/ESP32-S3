// Source: 03_GPIO_INTERRUPT_LAB.md
// Section: 코드

#include <Arduino.h>

const int BUTTON1_PIN = 5;
const int BUTTON2_PIN = 6;
const int LED_PIN = 4;

volatile bool button1Flag = false;
volatile bool button2Flag = false;
volatile unsigned long lastButton1Isr = 0;
volatile unsigned long lastButton2Isr = 0;
const unsigned long ISR_DEBOUNCE_MS = 200;

void IRAM_ATTR onButton1Press() {
  unsigned long now = millis();
  if (now - lastButton1Isr > ISR_DEBOUNCE_MS) {
    button1Flag = true;
    lastButton1Isr = now;
  }
}

void IRAM_ATTR onButton2Press() {
  unsigned long now = millis();
  if (now - lastButton2Isr > ISR_DEBOUNCE_MS) {
    button2Flag = true;
    lastButton2Isr = now;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BUTTON1_PIN), onButton1Press, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON2_PIN), onButton2Press, FALLING);

  Serial.println("Ready. Press Button1 or Button2.");
}

void loop() {
  if (button1Flag) {
    button1Flag = false;
    Serial.println("Button1 pressed - LED ON");
    digitalWrite(LED_PIN, HIGH);
  }
  if (button2Flag) {
    button2Flag = false;
    Serial.println("Button2 pressed - LED OFF");
    digitalWrite(LED_PIN, LOW);
  }
}
