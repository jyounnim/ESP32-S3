# 25. NTP로 시간 동기화

## 목표

ESP32-S3는 배터리 백업 RTC가 없어, 전원을 껐다 켜면 시간 정보가 사라집니다. Wi-Fi로 NTP(Network Time Protocol) 서버에 접속해 정확한 현재 시각을 받아옵니다.

## 코드

```cpp
#include <Arduino.h>
#include <WiFi.h>
#include <time.h>

const char* WIFI_SSID = "your-ssid";
const char* WIFI_PASSWORD = "your-password";
const char* NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET_SEC = 9 * 3600;   // 한국(KST) = UTC+9
const int DAYLIGHT_OFFSET_SEC = 0;      // 한국은 서머타임 없음

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected!");

  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    Serial.println(&timeinfo, "Time synced: %Y-%m-%d %H:%M:%S");
  } else {
    Serial.println("Failed to obtain time");
  }
}

void loop() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    Serial.println(&timeinfo, "%Y-%m-%d %H:%M:%S (%A)");
  }
  delay(1000);
}
```

## 실행 & 확인

- Wi-Fi 연결 직후 "Time synced" 로그와 함께 정확한 현재 날짜/시각이 출력되는지 확인
- 이후 1초마다 시각이 정상적으로 흐르는지 확인

## 관찰 포인트

- `configTime()`은 한 번만 호출하면 되고, 그 이후로는 ESP32 내부의 소프트웨어 시계가 시간을 계속 흘려보냅니다 — `getLocalTime()`을 호출할 때마다 매번 NTP 서버에 다시 접속하는 게 아닙니다
- `GMT_OFFSET_SEC`는 지역마다 다릅니다 — 한국은 UTC+9라 `9 * 3600`(32400초)입니다. 서머타임을 쓰는 지역이라면 `DAYLIGHT_OFFSET_SEC`도 함께 설정해야 합니다
- 이 시각 정보는 `26_WIFI_SENSOR_DASHBOARD_LAB.md`처럼 센서 로그에 정확한 타임스탬프를 붙이는 데 바로 활용할 수 있습니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| `Failed to obtain time` | Wi-Fi 연결은 됐지만 NTP 서버 접속이 안 되는 경우 — 방화벽에서 UDP 123번 포트가 막혀있는지 확인 (일부 공공/회사 Wi-Fi에서 흔함) |
| 시간이 몇 시간씩 틀림 | `GMT_OFFSET_SEC` 설정 오류 — 지역에 맞는 값인지 재확인 |
| 재부팅하면 시간이 다시 틀어짐 | 정상적인 동작 — RTC 배터리가 없어 매번 재동기화가 필요. 배터리 백업이 필요하면 별도 RTC 모듈(DS3231 등) 추가 고려 |

## 다음

`26_WIFI_SENSOR_DASHBOARD_LAB.md` — 센서 값을 웹 대시보드로 보여줍니다.
