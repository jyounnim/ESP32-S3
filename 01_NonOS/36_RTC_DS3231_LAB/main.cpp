// Source: 36_RTC_DS3231_LAB.md
// Section: 코드

#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();

  if (!rtc.begin()) {
    Serial.println("DS3231 not found - check wiring");
    while (1) delay(1000);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power - setting time to compile time");
    // 코드를 컴파일한 시점의 PC 시간으로 최초 1회 설정
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  DateTime now = rtc.now();

  Serial.printf("%04d-%02d-%02d %02d:%02d:%02d\n",
    now.year(), now.month(), now.day(),
    now.hour(), now.minute(), now.second());

  Serial.printf("Temperature (built-in sensor): %.2f C\n", rtc.getTemperature());

  delay(1000);
}
