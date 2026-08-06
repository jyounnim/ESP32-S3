# 18. MicroSD 카드 읽기/쓰기

## 목표

SPI 방식 MicroSD 카드 모듈에 파일을 쓰고 읽습니다. 로그 데이터를 SD카드에 저장하는 실무 패턴의 기초입니다.

## 준비물

- SPI MicroSD 카드 모듈, MicroSD 카드 (FAT32 포맷)

## 회로

- MOSI → GPIO11, MISO → GPIO13, SCK → GPIO12 (`17`과 동일)
- CS → GPIO10
- VCC → 3.3V (5V 전용 모듈이면 레벨시프터 필요할 수 있음, 데이터시트 확인)

## 코드

```cpp
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

const int SD_CS = 10;

void setup() {
  Serial.begin(115200);
  delay(1000);

  if (!SD.begin(SD_CS)) {
    Serial.println("SD card initialization failed");
    return;
  }
  Serial.println("SD card initialized");

  // 쓰기
  File writeFile = SD.open("/log.txt", FILE_WRITE);
  if (writeFile) {
    writeFile.println("Hello from ESP32-S3");
    writeFile.printf("Uptime: %lu ms\n", millis());
    writeFile.close();
    Serial.println("Write complete");
  } else {
    Serial.println("Failed to open file for writing");
  }

  // 읽기
  File readFile = SD.open("/log.txt");
  if (readFile) {
    Serial.println("--- File contents ---");
    while (readFile.available()) {
      Serial.write(readFile.read());
    }
    readFile.close();
  } else {
    Serial.println("Failed to open file for reading");
  }
}

void loop() {
  // 매 5초 로그 한 줄 추가
  static unsigned long lastLog = 0;
  if (millis() - lastLog > 5000) {
    lastLog = millis();
    File f = SD.open("/log.txt", FILE_APPEND);
    if (f) {
      f.printf("Log entry at %lu ms\n", millis());
      f.close();
      Serial.println("Appended log entry");
    }
  }
}
```

## 실행 & 확인

- `setup()`에서 파일 쓰기/읽기가 성공하고, 작성한 내용이 시리얼 모니터에 출력되는지 확인
- 5초마다 `loop()`에서 로그가 계속 추가되는지 확인 (SD카드를 PC에 꽂아 `log.txt` 내용으로 최종 확인 가능)

## 관찰 포인트

- `FILE_WRITE`는 파일을 열 때마다 **처음부터 덮어씁니다**. 기존 내용 뒤에 이어 쓰려면 `FILE_APPEND`를 써야 합니다 — `setup()`과 `loop()`에서 이 차이를 의도적으로 다르게 썼습니다
- SD카드 쓰기는 상대적으로 느린 연산입니다 — `loop()`에서 매번 쓰지 않고 `millis()` 기반으로 주기를 두는 패턴은, 이후 센서 로깅 예제에서도 계속 재사용됩니다
- 카드를 뺐다 다시 꽂아도 ESP32는 자동으로 재인식하지 않습니다 — 핫스왑이 필요하면 `SD.end()` 후 `SD.begin()`을 다시 호출해야 합니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| `SD card initialization failed` | 카드가 FAT32로 포맷됐는지 확인, 배선(특히 CS) 재확인, 카드를 PC에서 정상 인식하는지 확인 |
| 쓰기는 되는데 파일이 비어있음 | `close()`를 호출하지 않으면 버퍼가 플러시되지 않아 내용이 유실될 수 있음 |
| 대용량 카드(64GB+)에서 오류 | 일부 SD 라이브러리는 exFAT을 지원하지 않음 — FAT32로 포맷된 32GB 이하 카드 권장 |

## 다음

`19_SPI_TFT_LCD_LAB.md` — SPI 소형 컬러 LCD를 다룹니다.
