// Source: 10_I2C_TEMP_HUMIDITY_LAB.md
// Section: 코드

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>

Adafruit_AHTX0 aht;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();

  if (!aht.begin()) {
    Serial.println("AHT20 not found - check wiring");
    while (1) delay(1000);
  }
  Serial.println("AHT20 initialized");
}

void loop() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);   // 한 번 호출로 온도/습도 둘 다 갱신됨

  Serial.printf("Temperature: %.2f C, Humidity: %.2f %%\n",
                temp.temperature, humidity.relative_humidity);
  delay(1000);
}
