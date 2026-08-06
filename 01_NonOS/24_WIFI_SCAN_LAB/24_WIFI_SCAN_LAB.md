# 24. Wi-Fi 스캔 — 주변 AP 목록

## 목표

연결 없이, 주변에 어떤 Wi-Fi 네트워크가 있는지 스캔해서 SSID/신호세기/보안방식을 출력합니다.

## 코드

```cpp
#include <Arduino.h>
#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();   // 스캔 전 기존 연결 상태 정리
  delay(100);
}

void loop() {
  Serial.println("Scanning...");
  int n = WiFi.scanNetworks();
  Serial.printf("Found %d networks\n", n);

  for (int i = 0; i < n; i++) {
    Serial.printf("%2d: %-24s  %4d dBm  ch%2d  %s\n",
      i + 1,
      WiFi.SSID(i).c_str(),
      WiFi.RSSI(i),
      WiFi.channel(i),
      WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "OPEN" : "encrypted");
  }

  WiFi.scanDelete();   // 스캔 결과 메모리 해제
  Serial.println();
  delay(5000);
}
```

## 실행 & 확인

- 5초마다 주변 Wi-Fi 목록이 SSID, 신호세기(dBm), 채널, 보안방식과 함께 출력되는지 확인

## 관찰 포인트

- `WiFi.RSSI()` 값은 음수이고, **0에 가까울수록(예: -40) 신호가 강하고, 음수가 클수록(예: -90) 약합니다**
- `WiFi.scanDelete()`를 호출하지 않으면 스캔 결과가 메모리에 계속 쌓여 힙 공간을 낭비합니다 — 다음 스캔 전에 반드시 정리하는 습관을 들이세요
- 이 스캔 결과를 활용하면 "가장 신호가 강한 AP 자동 선택" 같은 로직도 만들 수 있습니다 — `WiFi.RSSI(i)` 값을 비교해서 가장 큰(0에 가까운) 인덱스를 찾으면 됩니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 스캔 결과가 0개 | 5GHz 전용 네트워크만 있는 환경(ESP32-S3는 2.4GHz만 지원), 또는 안테나/배선 확인 |
| 스캔이 매번 오래 걸림 | 정상 — 전체 채널을 스캔하는 데 보통 수백 ms~2초 정도 걸림 |

## 다음

`25_WIFI_NTP_LAB.md` — NTP로 실제 시간을 동기화합니다.
