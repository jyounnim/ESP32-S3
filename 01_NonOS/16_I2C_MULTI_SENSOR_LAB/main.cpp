// Source: 16_I2C_MULTI_SENSOR_LAB.md
// Section: 코드

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_AHTX0 aht;
Adafruit_MPU6050 mpu;
Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();

  bool ok = true;
  if (!aht.begin())   { Serial.println("AHT20 init failed");   ok = false; }
  if (!mpu.begin())   { Serial.println("MPU6050 init failed"); ok = false; }
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { Serial.println("OLED init failed"); ok = false; }

  if (!ok) {
    Serial.println("One or more devices failed - check I2C scanner (09) first");
  }

  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  sensors_event_t a, g, mpuTemp;
  mpu.getEvent(&a, &g, &mpuTemp);

  Serial.printf("Temp:%.1fC Humidity:%.1f%% AccelZ:%.2f\n",
                temp.temperature, humidity.relative_humidity, a.acceleration.z);

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.printf("Temp: %.1f C\n", temp.temperature);
  display.printf("Humidity: %.1f %%\n", humidity.relative_humidity);
  display.printf("AccelZ: %.2f m/s^2\n", a.acceleration.z);
  display.display();

  delay(500);
}
