// Source: 34_HCSR04_ULTRASONIC_LAB.md
// Section: 코드

#include <Arduino.h>

const int TRIG_PIN = 5;
const int ECHO_PIN = 6;

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  // 10us 펄스를 TRIG에 보내 측정 시작
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // ECHO가 HIGH로 유지되는 시간(us) = 초음파가 왕복한 시간
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);   // 30ms 타임아웃 (약 5m 초과 시 0 반환)

  if (duration == 0) {
    Serial.println("Out of range");
  } else {
    float distanceCm = duration * 0.0343 / 2.0;   // 음속 343m/s -> 0.0343cm/us, 왕복이라 /2
    Serial.printf("Distance: %.1f cm\n", distanceCm);
  }

  delay(200);
}
