// Source: 15_I2C_MEMS_IMU_LAB.md
// Section: 코드

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();

  if (!mpu.begin()) {
    Serial.println("MPU6050 not found - check wiring");
    while (1) delay(1000);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  Serial.println("MPU6050 initialized");
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);   // 가속도/자이로/온도 한 번에 갱신

  Serial.printf("Accel(m/s^2) X:%.2f Y:%.2f Z:%.2f | Gyro(rad/s) X:%.2f Y:%.2f Z:%.2f | Temp:%.1fC\n",
    a.acceleration.x, a.acceleration.y, a.acceleration.z,
    g.gyro.x, g.gyro.y, g.gyro.z,
    temp.temperature);

  delay(100);
}
