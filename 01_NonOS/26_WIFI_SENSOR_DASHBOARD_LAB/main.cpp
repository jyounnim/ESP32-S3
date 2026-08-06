// Source: 26_WIFI_SENSOR_DASHBOARD_LAB.md
// Section: 코드

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
