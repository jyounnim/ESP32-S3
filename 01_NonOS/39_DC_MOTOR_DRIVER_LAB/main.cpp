// Source: 39_DC_MOTOR_DRIVER_LAB.md
// Section: 코드

#include <Arduino.h>

const int IN1_PIN = 4;
const int IN2_PIN = 5;
const int ENA_PIN = 6;
const int PWM_CHANNEL = 0;

void setMotor(int speed) {   // speed: -255(역방향 최대) ~ 255(정방향 최대)
  if (speed > 0) {
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);
  } else if (speed < 0) {
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, HIGH);
  } else {
    digitalWrite(IN1_PIN, LOW);   // 둘 다 LOW = 정지(자유 회전, coast)
    digitalWrite(IN2_PIN, LOW);
  }
  ledcWrite(PWM_CHANNEL, abs(speed));
}

void setup() {
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  ledcSetup(PWM_CHANNEL, 5000, 8);
  ledcAttachPin(ENA_PIN, PWM_CHANNEL);
}

void loop() {
  setMotor(200);    // 정방향, 속도 200/255
  delay(2000);

  setMotor(0);
  delay(1000);

  setMotor(-200);   // 역방향
  delay(2000);

  setMotor(0);
  delay(1000);
}
