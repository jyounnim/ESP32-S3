// Source: 38_STEPPER_MOTOR_LAB.md
// Section: 코드

#include <Arduino.h>
#include <Stepper.h>

const int STEPS_PER_REV = 2048;   // 28BYJ-48은 내부 기어비 때문에 한 바퀴에 약 2048스텝

// 주의: IN1, IN3, IN2, IN4 순서 (IN1,IN2,IN3,IN4가 아님 - 아래 관찰 포인트 참고)
Stepper myStepper(STEPS_PER_REV, 4, 6, 5, 7);

void setup() {
  Serial.begin(115200);
  delay(1000);
  myStepper.setSpeed(10);   // RPM (28BYJ-48은 보통 10~15 RPM 권장)
}

void loop() {
  Serial.println("Forward one revolution");
  myStepper.step(STEPS_PER_REV);
  delay(500);

  Serial.println("Backward one revolution");
  myStepper.step(-STEPS_PER_REV);
  delay(500);
}
