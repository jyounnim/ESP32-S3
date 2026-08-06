# 40. RFID 카드 리더 — RC522

## 목표

SPI 통신(`17`~`20`)을 응용해, RFID 카드/태그를 인식하고 고유 UID를 읽습니다. 출입 통제, 태그 기반 인식 시스템의 기초입니다.

## 준비물

- MFRC522(RC522) RFID 리더 모듈, RFID 카드 또는 태그 1개 이상

## 회로

- SCK → GPIO12, MOSI → GPIO11, MISO → GPIO13 (`17`과 동일)
- SDA(=SS/CS) → GPIO10
- RST → GPIO17
- VCC → **3.3V** (RC522는 3.3V 전용 — 5V 연결 시 손상)

## 라이브러리 설치

```ini
lib_deps =
    miguelbalboa/MFRC522 @ ^1.4.11
```

## 코드

```cpp
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 17

MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  delay(1000);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Ready. Tap an RFID card/tag.");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  Serial.print("UID:");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.printf(" %02X", mfrc522.uid.uidByte[i]);
  }
  Serial.println();

  // 카드 종류도 함께 확인 가능
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.printf("Card type: %s\n", mfrc522.PICC_GetTypeName(piccType));

  mfrc522.PICC_HaltA();   // 다음 카드를 받을 준비
}
```

## 실행 & 확인

- 카드를 리더에 가까이 대면 UID(예: `04 A1 B2 C3`)와 카드 종류가 출력되는지 확인
- 서로 다른 카드/태그를 대보면 UID가 각각 다르게 나오는지 확인

## 관찰 포인트

- `PICC_IsNewCardPresent()`와 `PICC_ReadCardSerial()`을 매 `loop()`마다 확인하는 폴링 구조입니다 — 카드가 없으면 즉시 `return`해서 다음 루프로 넘어갑니다
- UID는 카드마다 고유하지만 **암호화되지 않은 평문**입니다 — 실제 출입 통제 시스템에 쓰려면 UID만으로 인증하지 말고, MIFARE Classic의 섹터 인증 기능(`PCD_Authenticate`)이나 애초에 더 안전한 인증 방식을 함께 고려해야 합니다 (UID 복제가 비교적 쉽다는 게 알려진 한계)
- 이 실습에서 읽은 UID를 문자열로 저장해뒀다가, `40`을 마친 UID 목록과 비교하는 간단한 "등록된 카드만 인식" 로직을 만들어보는 것도 좋은 응용 과제입니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 카드를 대도 반응 없음 | SPI 배선 확인(특히 RST), 전원이 3.3V인지 재확인(5V 연결 시 손상됐을 수 있음) |
| 간헐적으로만 인식 | 카드와 리더 안테나 사이 거리/각도 조정 (보통 몇 cm 이내에서만 인식) |
| UID가 매번 다르게 나옴 | 여러 카드를 동시에 리더 근처에 두지 않았는지 확인 |

## 다음

`41_DOT_MATRIX_MAX7219_LAB.md` — MAX7219로 구동하는 8x8 도트매트릭스를 다룹니다.
