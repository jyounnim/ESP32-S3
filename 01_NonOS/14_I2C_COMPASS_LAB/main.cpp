// Source: 14_I2C_COMPASS_LAB.md
// Section: 코드

#include <Arduino.h>
#include <Wire.h>
#include <QMC5883LCompass.h>

QMC5883LCompass compass;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();
  compass.init();
}

void loop() {
  compass.read();

  int x = compass.getX();
  int y = compass.getY();
  int z = compass.getZ();
  int azimuth = compass.getAzimuth();
  byte bearing = compass.getBearing(azimuth);

  char direction[3];
  compass.getDirection(direction, azimuth);

  Serial.printf("X:%d Y:%d Z:%d Azimuth:%d Direction:%c%c%c\n",
                x, y, z, azimuth, direction[0], direction[1], direction[2]);
  delay(250);
}
