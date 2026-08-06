# ESP32-S3

# 00. 개발환경 셋업 — VS Code + PlatformIO (ESP32-S3)

Non-OS(bare-metal, RTOS 없이 `setup()`/`loop()` 구조) 기준 ESP32-S3 실습 전체 시리즈의 시작 파일입니다. 이 문서로 개발환경을 구축한 뒤, 목차 순서대로 진행하시면 됩니다.

## 사전 준비물

- PC (Windows / macOS / Linux)
- ESP32-S3 개발보드 (본 시리즈는 **ESP32-S3-DevKitC-1, N16R8** 기준으로 작성 — 16MB Quad Flash + 8MB Octal PSRAM)
- 데이터 전송을 지원하는 USB 케이블 (충전 전용 케이블 아님)

---

## Step 1. VS Code 설치

[code.visualstudio.com](https://code.visualstudio.com)에서 OS에 맞는 버전 설치. 이미 있다면 생략.

## Step 2. PlatformIO IDE 확장 설치

1. VS Code Extensions(`Ctrl+Shift+X`) → `PlatformIO IDE` 검색 → 설치
2. 설치 후 VS Code 재시작 (최초 로딩에 수 분 소요)
3. 좌측 Activity Bar에 PlatformIO 개미 아이콘이 보이면 설치 완료

## Step 3. 새 프로젝트 생성

1. PlatformIO Home → **New Project**
2. Board: `Espressif ESP32-S3-DevKitC-1` 검색 후 선택
3. Framework: **Arduino**
4. 생성 완료 시 `platformio.ini`, `src/main.cpp`가 자동 생성됨

## Step 4. N16R8 Flash/PSRAM 설정

기본 보드 정의는 N8(8MB Flash, PSRAM 없음) 기준입니다. N16R8(16MB Quad Flash + 8MB Octal PSRAM)을 쓰신다면 `platformio.ini`에 아래 설정을 추가하세요.

```ini
[env:esp32-s3-devkitc-1]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino
monitor_speed = 115200

; ---- N16R8 (16MB Quad Flash + 8MB Octal PSRAM) 설정 ----
board_build.flash_mode = qio
board_upload.flash_size = 16MB
board_build.partitions = default_16MB.csv
board_build.arduino.memory_type = qio_opi
build_flags =
    -DBOARD_HAS_PSRAM
```

> Flash는 Quad(`qio`), PSRAM은 Octal(`opi`)이라 `memory_type = qio_opi`를 씁니다. 다른 모듈(N16R8V, N32R8V 등)을 쓰신다면 Flash/PSRAM 구성이 다를 수 있으니 실물 각인을 확인하세요.

## Step 5. Hello World

```cpp
#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Hello, ESP32-S3!");
}

void loop() {
  Serial.println("Running...");
  delay(1000);
}
```

Build(하단 상태바 체크 아이콘) → Upload(화살표 아이콘) → Serial Monitor(플러그 아이콘)로 확인.

## Step 6. 코어 버전 확인 (중요)

`ledcAttach` 같은 최신 PWM API를 쓸지, `ledcSetup`+`ledcAttachPin` 같은 구버전 API를 쓸지는 **설치된 Arduino-ESP32 코어 버전**에 따라 갈립니다. 아래 코드로 확인하세요.

```cpp
void setup() {
  Serial.begin(115200);
  delay(1000);
#if defined(ESP_ARDUINO_VERSION_MAJOR)
  Serial.printf("Arduino-ESP32 core: %d.%d.%d\n", ESP_ARDUINO_VERSION_MAJOR, ESP_ARDUINO_VERSION_MINOR, ESP_ARDUINO_VERSION_PATCH);
#endif
}
void loop() {}
```

- **core 2.x**: PWM은 `ledcSetup(channel, freq, res)` + `ledcAttachPin(pin, channel)` + `ledcWrite(channel, duty)`
- **core 3.x 이상**: PWM은 `ledcAttach(pin, freq, res)` + `ledcWrite(pin, duty)`

이 시리즈의 PWM 관련 예제는 **core 2.x(구 API) 기준으로 작성**되어 있습니다 — 만약 3.x를 쓰고 계시다면 신버전 API로 바꿔서 진행하세요 (예제마다 두 방식을 함께 표기).

## 확인용 체크리스트

- [ ] VS Code + PlatformIO IDE 설치
- [ ] 프로젝트 생성 및 N16R8 Flash/PSRAM 설정 반영
- [ ] Hello World 빌드/업로드/시리얼 출력 확인
- [ ] 사용 중인 Arduino-ESP32 코어 버전 확인

---

## 전체 목차

### GPIO — 디지털/아날로그 입출력
| 번호 | 파일 | 내용 |
|---|---|---|
| 01 | `01_GPIO_OUTPUT_LAB.md` | LED 점멸 (디지털 출력) |
| 02 | `02_GPIO_INPUT_LAB.md` | 버튼 입력 + 디바운싱 |
| 03 | `03_GPIO_INTERRUPT_LAB.md` | 외부 인터럽트 — 다중 버튼 처리 |
| 04 | `04_GPIO_MULTI_OUTPUT_LAB.md` | 다중 GPIO 출력 — LED 순차 제어 |
| 05 | `05_GPIO_INPUT_ADVANCED_LAB.md` | 입력 심화 — 롱프레스/더블클릭 감지 |
| 06 | `06_PWM_LAB.md` | PWM으로 LED 밝기 제어 |
| 07 | `07_ADC_LAB.md` | ADC로 아날로그 값 읽기 |
| 08 | `08_PWM_ADC_COMBINED_LAB.md` | 종합 — 가변저항으로 LED 밝기 조절 |

### I2C 통신
| 번호 | 파일 | 내용 |
|---|---|---|
| 09 | `09_I2C_BASICS_LAB.md` | I2C 기초 — 버스 스캔 |
| 10 | `10_I2C_TEMP_HUMIDITY_LAB.md` | 온습도 센서 — AHT20 |
| 11 | `11_I2C_PRESSURE_LAB.md` | 기압/온도 센서 — BME280 |
| 12 | `12_I2C_OLED_TEXT_LAB.md` | OLED — SSD1306 텍스트 출력 |
| 13 | `13_I2C_OLED_GRAPHICS_LAB.md` | OLED — 그래픽/아이콘 |
| 14 | `14_I2C_COMPASS_LAB.md` | 전자 나침반 — QMC5883L |
| 15 | `15_I2C_MEMS_IMU_LAB.md` | MEMS 가속도/자이로 — MPU6050 |
| 16 | `16_I2C_MULTI_SENSOR_LAB.md` | I2C 다중 센서 동시 사용 |

### SPI 통신
| 번호 | 파일 | 내용 |
|---|---|---|
| 17 | `17_SPI_BASICS_LAB.md` | SPI 기초 — 개념과 배선 |
| 18 | `18_SPI_SDCARD_LAB.md` | MicroSD 카드 읽기/쓰기 |
| 19 | `19_SPI_TFT_LCD_LAB.md` | 소형 LCD — ST7735 |
| 20 | `20_SPI_NRF24L01_LAB.md` | NRF24L01 무선 모듈 송수신 |

### 디스플레이/출력 장치
| 번호 | 파일 | 내용 |
|---|---|---|
| 21 | `21_7SEGMENT_74HC595_LAB.md` | 7-Segment — 74HC595 직접 구동 |
| 22 | `22_74HC595_MULTI_LED_LAB.md` | 74HC595로 다중 LED 확장 |
| 23 | `23_TM1637_LAB.md` | TM1637 모듈 (2-wire 7-Segment) |

### Wi-Fi
| 번호 | 파일 | 내용 |
|---|---|---|
| 24 | `24_WIFI_SCAN_LAB.md` | Wi-Fi 스캔 — 주변 AP 목록 |
| 25 | `25_WIFI_NTP_LAB.md` | NTP로 시간 동기화 |
| 26 | `26_WIFI_SENSOR_DASHBOARD_LAB.md` | 센서 값 웹 대시보드 |

### BLE
| 번호 | 파일 | 내용 |
|---|---|---|
| 27 | `27_BLE_BASICS_LAB.md` | BLE 개념 & 라이브러리 선택 |
| 28 | `28_BLE_GATT_SERVER_LAB.md` | BLE GATT 서버 — Hello BLE |
| 29 | `29_BLE_NOTIFY_SENSOR_LAB.md` | BLE Notify — 센서 값 실시간 전송 |
| 30 | `30_BLE_SCAN_CLIENT_LAB.md` | BLE Scan/Client — 주변 기기 검색 |

### 종합 프로젝트
| 번호 | 파일 | 내용 |
|---|---|---|
| 31 | `31_PROJECT_SENSOR_OLED_LAB.md` | 센서 값 → OLED 표시 |
| 32 | `32_PROJECT_SENSOR_BLE_LAB.md` | 센서 값 → BLE 전송 + OLED 동시 표시 |
| 33 | `33_PROJECT_COMPASS_7SEGMENT_LAB.md` | 나침반 방향 → 7-Segment 표시 |

### 확장 — 추가 센서 & 액추에이터 (사용자 보유 부품 기준)
| 번호 | 파일 | 내용 |
|---|---|---|
| 34 | `34_HCSR04_ULTRASONIC_LAB.md` | 초음파 거리 센서 — HC-SR04 |
| 35 | `35_DS18B20_ONEWIRE_LAB.md` | 1-Wire 온도 센서 — DS18B20 |
| 36 | `36_RTC_DS3231_LAB.md` | RTC 모듈 — DS3231 |
| 37 | `37_SERVO_MOTOR_LAB.md` | 서보모터 제어 |
| 38 | `38_STEPPER_MOTOR_LAB.md` | 스테퍼모터 — 28BYJ-48 + ULN2003 |
| 39 | `39_DC_MOTOR_DRIVER_LAB.md` | DC 모터 드라이버 (H-bridge) |
| 40 | `40_RFID_RC522_LAB.md` | RFID 카드 리더 — RC522 |
| 41 | `41_DOT_MATRIX_MAX7219_LAB.md` | 8x8 도트매트릭스 — MAX7219 |
| 42 | `42_LCD_16X2_I2C_LAB.md` | 16x2 캐릭터 LCD (I2C) |
| 43 | `43_ADS1115_LAB.md` | 정밀 외장 ADC — ADS1115 |
| 44 | `44_BH1750_LAB.md` | 조도 센서 — BH1750 |
| 45 | `45_GAS_SENSOR_MQ135_LAB.md` | 가스 센서 — MQ-135 |
| 46 | `46_VIBRATION_KNOCK_SENSOR_LAB.md` | 진동/노크 센서 — SW-420 & Knock (AI 이상탐지 예제 연결) |
| 47 | `47_ADDITIONAL_ANALOG_SENSORS_NOTE.md` | 추가 아날로그 센서 참고 (Water Level, Soil Moisture 등) |

---

## 공통 참고사항

- 별도 명시가 없는 한 모든 예제는 `framework = arduino`, RTOS Task 분리 없이 `setup()`/`loop()` 구조입니다
- 코드의 `Serial.print`/`printf` 출력 문자열은 영어로 작성했습니다
- I2C 기본 핀: SDA=GPIO8, SCL=GPIO9 (ESP32-S3 Arduino 코어 기본값 — 다른 핀을 쓰려면 `Wire.begin(SDA, SCL)`로 재지정)
- SPI 기본 핀: MOSI=GPIO11, MISO=GPIO13, SCK=GPIO12, CS는 예제별로 지정
