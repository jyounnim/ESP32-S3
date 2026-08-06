# 00. LiteRT(TFLM) 개발환경 셋업 — ESP32-S3

Google이 TensorFlow Lite를 **LiteRT**로 리브랜딩했지만, 마이크로컨트롤러용 런타임 자체는 여전히 **TFLM(TensorFlow Lite Micro / LiteRT for Microcontrollers)**이라는 이름으로 불립니다. ESP32-S3에서는 Espressif가 공식 포팅한 **`esp-tflite-micro`**를 씁니다 (내부적으로 S3의 벡터 명령어를 쓰는 `esp-nn` 가속 커널 포함).

## ⚠️ 먼저 알아두실 것 — 프레임워크 변경

지금까지의 모든 실습(GPIO, Wi-Fi, MQTT, OTA, FreeRTOS 등)은 `framework = arduino`로 진행했습니다. 하지만 `esp-tflite-micro`는 **ESP-IDF 전용 컴포넌트**로 배포되고 있어서, PlatformIO의 Arduino 프레임워크와 억지로 조합하면 `array.h`를 못 찾는 등의 컴파일 에러가 흔하게 보고됩니다(공식 GitHub 이슈로 확인).

**해결책**: 이번 AI 예제들만 새 PlatformIO 프로젝트를 만들고 `framework = espidf`로 진행합니다. VS Code + PlatformIO라는 개발 환경 자체는 동일하고, 내부 빌드 시스템만 순정 ESP-IDF를 씁니다.

> 참고로 이건 `ZEPHYR_LAB.md`에서 겪었던 것과 비슷한 유형의 이슈입니다 — Arduino는 미리 컴파일된 라이브러리를 쓰다 보니 특정 컴포넌트 생태계와 안 맞는 경우가 있고, 그럴 때는 그 생태계가 공식 지원하는 빌드 시스템으로 옮기는 게 가장 확실합니다.

## Step 1. 새 프로젝트 생성 (ESP-IDF 프레임워크)

1. PlatformIO Home → New Project
2. Board: `Espressif ESP32-S3-DevKitC-1` 선택
3. **Framework: `Espressif IoT Development Framework`** 선택 (Arduino 아님)
4. 생성 완료 후 `platformio.ini` 확인:

```ini
[env:esp32-s3-devkitc-1]
platform = espressif32
board = esp32-s3-devkitc-1
framework = espidf
monitor_speed = 115200
```

## Step 2. N16R8 Flash/PSRAM 설정 (ESP-IDF 방식)

기존 Arduino 프로젝트에서는 `board_build.*` 옵션으로 설정했지만, ESP-IDF 프로젝트는 **`sdkconfig.defaults` 파일**로 설정합니다. Arduino와 달리 ESP-IDF는 소스에서 직접 빌드하기 때문에 `sdkconfig`를 완전히 자유롭게 커스터마이징할 수 있습니다.

프로젝트 루트에 `sdkconfig.defaults` 파일을 만들고 아래 내용을 추가합니다.

```
CONFIG_ESPTOOLPY_FLASHSIZE_16MB=y
CONFIG_ESPTOOLPY_FLASHMODE_QIO=y
CONFIG_SPIRAM=y
CONFIG_SPIRAM_MODE_OCT=y
CONFIG_SPIRAM_SPEED_80M=y
CONFIG_PARTITION_TABLE_CUSTOM=y
```

이 값은 N16R8(16MB Quad Flash + 8MB Octal PSRAM)에 맞춘 설정입니다 — 이전에 `TROUBLESHOOTING.md`에서 확인한 `qio_opi` 구성(Flash는 Quad, PSRAM은 Octal)과 동일한 내용을 ESP-IDF 방식으로 표현한 것입니다.

## Step 3. esp-tflite-micro 의존성 추가 (IDF Component Manager)

ESP-IDF는 `idf_component.yml`이라는 파일로 외부 컴포넌트를 선언합니다. `main/idf_component.yml` 파일을 만듭니다.

```yaml
dependencies:
  espressif/esp-tflite-micro: "^1.3.3"
```

PlatformIO에서 빌드하면 IDF Component Manager가 자동으로 이 의존성을 받아옵니다 (`managed_components/` 폴더에 다운로드됨).

## Step 4. Hello World로 환경 검증

`esp-tflite-micro`에는 사인 함수를 근사하는 아주 작은 모델로 환경이 제대로 됐는지 확인하는 `hello_world` 예제가 포함되어 있습니다. 직접 코드를 옮겨 적기보다, 아래 명령으로 예제 자체를 받아서 그대로 빌드해보는 걸 권장합니다.

```bash
# managed_components 안의 예제를 프로젝트로 복사
cp -r managed_components/espressif__esp-tflite-micro/examples/hello_world/main/* main/
```

빌드 후 시리얼 모니터에 사인파 예측값이 500ms마다 출력되면 환경 구축이 끝난 것입니다.

## 확인용 체크리스트

- [ ] PlatformIO 프로젝트를 `framework = espidf`로 생성
- [ ] `sdkconfig.defaults`에 N16R8 Flash/PSRAM 설정 반영
- [ ] `main/idf_component.yml`에 `espressif/esp-tflite-micro` 의존성 추가
- [ ] `hello_world` 예제 빌드/업로드 성공, 사인파 값 출력 확인

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| `Failed to resolve component 'tflite-lib'` | `idf_component.yml` 위치나 문법 오류 — `main/` 폴더 안에 정확히 있는지 확인 |
| PSRAM이 인식 안 됨 | `sdkconfig.defaults`가 실제로 적용됐는지 확인 — `.pio/build/.../sdkconfig` 파일을 열어 `CONFIG_SPIRAM=y`가 있는지 확인. 없다면 `pio run -t clean` 후 재빌드 |
| `array.h` 관련 컴파일 에러 | Arduino 프레임워크로 시도하고 계신 건 아닌지 확인 — 반드시 `framework = espidf`여야 합니다 |

---

환경 구축이 끝나면 아래 순서로 진행합니다.

1. `01_FACE_DETECTION_LAB.md` — 얼굴 인식 (카메라 모듈 필요)
2. `02_WAKE_WORD_LAB.md` — 웨이크워드 감지 (I2S 마이크 필요)
3. `03_SENSOR_ANOMALY_LAB.md` — 진동/센서 이상 탐지 (기존 가변저항으로 실습 가능)
