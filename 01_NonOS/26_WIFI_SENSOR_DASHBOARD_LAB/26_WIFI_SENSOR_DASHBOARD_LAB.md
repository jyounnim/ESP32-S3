# 26. 센서 값 웹 대시보드

## 목표

`10`(AHT20 온습도 센서)의 값을 웹 브라우저에서 실시간으로 확인할 수 있는 대시보드를 만듭니다.

## 준비물

- AHT20 모듈 (`10`과 동일 배선)

## 코드

```cpp
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>

const char* WIFI_SSID = "your-ssid";
const char* WIFI_PASSWORD = "your-password";

WebServer server(80);
Adafruit_AHTX0 aht;

float currentTemp = 0;
float currentHumidity = 0;

const char PAGE_HTML[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html><head><meta charset="utf-8"><title>Sensor Dashboard</title>
<style>
body { font-family: sans-serif; text-align: center; margin-top: 60px; }
.value { font-size: 48px; font-weight: bold; }
</style></head>
<body>
<h1>ESP32-S3 Sensor Dashboard</h1>
<p>Temperature</p><p class="value" id="temp">--</p>
<p>Humidity</p><p class="value" id="humidity">--</p>
<script>
async function refresh() {
  const res = await fetch('/data');
  const data = await res.json();
  document.getElementById('temp').textContent = data.temp.toFixed(1) + ' C';
  document.getElementById('humidity').textContent = data.humidity.toFixed(1) + ' %';
}
refresh();
setInterval(refresh, 2000);
</script>
</body></html>
)HTML";

void handleRoot() {
  server.send(200, "text/html", PAGE_HTML);
}

void handleData() {
  String json = "{\"temp\":" + String(currentTemp, 2) + ",\"humidity\":" + String(currentHumidity, 2) + "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();

  if (!aht.begin()) {
    Serial.println("AHT20 not found");
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nDashboard: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
}

void loop() {
  server.handleClient();

  static unsigned long lastRead = 0;
  if (millis() - lastRead > 1000) {
    lastRead = millis();
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);
    currentTemp = temp.temperature;
    currentHumidity = humidity.relative_humidity;
  }
}
```

## 실행 & 확인

- 시리얼 모니터에서 확인한 IP 주소로 브라우저 접속
- 온도/습도 값이 2초마다 자동 갱신되는지 확인 (새로고침 불필요)

## 관찰 포인트

- `/data` 엔드포인트가 JSON을 반환하고, 브라우저의 자바스크립트가 `fetch()`로 그 값만 가져와 화면 텍스트를 갱신하는 구조입니다 — 페이지 전체를 새로고침하지 않아 깜빡임이 없습니다. 이 폴링 패턴은 이전 실습(`WEBSERVER_LED_LAB.md`)의 LED 상태 갱신과 동일한 원리입니다
- 센서 값 읽기(`aht.getEvent`)를 `loop()`에서 매번 하지 않고 `millis()`로 1초 주기를 둔 이유는, 센서 통신(I2C)이 `server.handleClient()`의 응답 속도에 방해가 되지 않게 하기 위함입니다
- 센서 종류를 바꾸고 싶다면(BME280, MPU6050 등) `currentTemp`/`currentHumidity` 대신 해당 센서의 값을 담는 전역 변수로 바꾸고, `/data`의 JSON 필드만 맞춰주면 됩니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 페이지는 뜨는데 값이 안 나옴 | 브라우저 개발자 도구(F12) → Network 탭에서 `/data` 요청 상태 확인 |
| 값이 항상 0 | AHT20 초기화 실패 — `09`의 I2C 스캐너로 배선 재확인 |

## 다음

`27_BLE_BASICS_LAB.md`부터 BLE를 다룹니다.
