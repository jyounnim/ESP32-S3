// Source: 37_SERVO_MOTOR_LAB.md
// Section: 코드

#include <Arduino.h>
#include <ESP32Servo.h>

Servo myServo;
const int SERVO_PIN = 18;

void setup() {
  Serial.begin(115200);
  delay(1000);

  myServo.attach(SERVO_PIN);   // 내부적으로 LEDC(또는 S3의 경우 MCPWM) 채널을 자동 할당
}

void loop() {
  for (int angle = 0; angle <= 180; angle += 5) {
    myServo.write(angle);
    delay(30);
  }
  delay(500);
  for (int angle = 180; angle >= 0; angle -= 5) {
    myServo.write(angle);
    delay(30);
  }
  delay(500);
}
