// Source: 33_PROJECT_COMPASS_7SEGMENT_LAB.md
// Section: 코드

#include <Arduino.h>
#include <Wire.h>
#include <QMC5883LCompass.h>
#include <TM1637Display.h>

#define CLK_PIN 5
#define DIO_PIN 4

QMC5883LCompass compass;
TM1637Display display(CLK_PIN, DIO_PIN);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();

  compass.init();
  // 14번 실습에서 얻은 캘리브레이션 값이 있다면 여기에 추가:
  // compass.setCalibration(xmin, xmax, ymin, ymax, zmin, zmax);

  display.setBrightness(0x0f);
}

void loop() {
  compass.read();
  int azimuth = compass.getAzimuth();
  if (azimuth < 0) azimuth += 360;   // 항상 0~359 범위로 정규화

  display.showNumberDec(azimuth, false);   // 방위각을 그대로 4자리 숫자로 표시

  char direction[3];
  compass.getDirection(direction, azimuth);
  Serial.printf("Azimuth: %d deg (%c%c%c)\n", azimuth, direction[0], direction[1], direction[2]);

  delay(200);
}
