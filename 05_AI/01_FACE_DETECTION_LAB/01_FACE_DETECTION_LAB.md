# 01. 얼굴 인식 (Face Detection/Recognition)

## ⚠️ 하드웨어 필요

지금 쓰시는 ESP32-S3-DevKitC-1(N16R8)에는 **카메라가 없습니다**. 아래 중 하나가 필요합니다.

- OV2640 또는 OV3660 카메라 모듈 (DVP 인터페이스로 보드에 연결)
- 또는 카메라가 내장된 ESP32-S3 보드(ESP32-S3-EYE, Freenove ESP32-S3-CAM 등)로 교체

이 문서는 카메라 모듈을 DevKitC-1에 연결했다는 가정 하에 진행합니다. 카메라가 없다면, 우선 03번(진동/센서 이상 탐지)부터 진행하시길 권장드립니다 — 그건 기존 가변저항만으로 바로 실습 가능합니다.

## 도구 선택 — TFLM이 아니라 ESP-WHO

일반적인 `esp-tflite-micro`에 포함된 `person_detection` 예제는 "사람이 있다/없다"만 구분하는 아주 단순한 250KB 모델입니다. **진짜 얼굴 인식(얼굴 위치 검출 + 등록된 얼굴 식별)**을 원하시면, Espressif가 이 목적으로 별도로 만든 프레임워크인 **ESP-WHO**를 쓰는 게 맞습니다. ESP-WHO는 내부적으로 TFLM이 아니라 **ESP-DL**(Espressif 자체 추론 엔진, S3 벡터 명령어에 최적화)을 씁니다 — "LiteRT(TFLM)만 쓰고 싶다"는 원 요청과는 다르지만, 얼굴 인식에 한해서는 이 쪽이 Espressif의 공식 권장 경로입니다.

## Step 1. ESP-WHO 클론

```bash
git clone --recursive https://github.com/espressif/esp-who.git
cd esp-who/examples/human_face_detection/lcd
```

`--recursive`가 중요합니다 — ESP-WHO는 여러 하위 컴포넌트(esp-dl 등)를 git submodule로 포함하고 있습니다.

## Step 2. 보드 지원(BSP) 확인

ESP-WHO의 최신 예제들은 **BSP(Board Support Package)** 방식으로, ESP32-S3-EYE·ESP32-S3-Korvo-2 같은 특정 평가 보드 기준으로 카메라/디스플레이 핀이 미리 정의되어 있습니다. DevKitC-1 + 별도 카메라 모듈처럼 커스텀 하드웨어라면, 사용 중인 카메라 모듈의 실제 배선(SIOD, SIOC, VSYNC, HREF, PCLK, XCLK, D0~D7 핀)에 맞춰 카메라 초기화 설정을 직접 수정해야 합니다. 이 부분은 카메라 모듈마다 배선이 다르므로, 정확한 핀맵은 구매하신 카메라 모듈의 문서를 참고하셔야 합니다.

```bash
idf.py set-target esp32s3
idf.py menuconfig
# Camera Sensor 설정에서 SIOD/SIOC/XCLK/VSYNC/HREF/PCLK/D0-D7 핀 번호를 실제 배선에 맞게 수정
```

## Step 3. 빌드 & 실행

```bash
idf.py build
idf.py -p <포트> flash monitor
```

카메라 앞에 얼굴을 비추면, 감지된 얼굴 주위에 사각형이 표시됩니다(디스플레이가 연결된 경우) 또는 시리얼 로그로 좌표가 출력됩니다(디스플레이 없이 `lcd` 대신 다른 예제 폴더 사용 시).

## Step 4. 얼굴 등록(선택) — 얼굴 인식까지 확장

`human_face_detection`은 "얼굴이 있는지"만 확인합니다. 특정 사람을 구분하는 **얼굴 인식**까지 하려면 `human_face_recognition` 예제를 대신 사용합니다.

```bash
cd esp-who/examples/human_face_recognition/lcd
```

이 예제는 보통 버튼(또는 시리얼 명령)으로 "지금 보이는 얼굴을 N번으로 등록"하는 기능이 포함되어 있어, 등록된 얼굴이 다시 나타나면 ID를 인식합니다.

## 관찰 포인트

- ESP-WHO는 내부적으로 FreeRTOS Task 기반 구조(`WhoTaskBase`)로 짜여 있습니다 — 카메라 캡처, 얼굴 검출, 인식이 각각 별도 Task로 비동기 실행되어 프레임 속도(FPS)를 높입니다. `freertos_curriculum/`에서 배운 Task 분리·Queue 패턴이 실전에서 이렇게 쓰인다는 걸 확인할 수 있는 좋은 사례입니다
- 조명이 어둡거나 얼굴이 화면 가장자리에 걸치면 검출률이 떨어집니다 — 실제 제품에 적용할 땐 카메라 각도/조명을 고려해야 합니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 카메라 초기화 실패 (`camera init failed`) | 핀맵이 실제 배선과 다름 — Step 2의 menuconfig 설정 재확인 |
| 빌드는 되는데 화면이 안 뜸 | `lcd` 없는 보드라면 `lcd` 없는 예제 변형을 찾거나, 시리얼 로그 출력 버전으로 대체 |
| PSRAM 부족 크래시 | N16R8은 8MB PSRAM이라 여유 있는 편이지만, `sdkconfig.defaults`에 `CONFIG_SPIRAM=y`가 제대로 적용됐는지 재확인 (`00_LITERT_TFLM_SETUP.md` 참고) |

## 다음

02번 파일(`02_WAKE_WORD_LAB.md`)에서 웨이크워드(음성 키워드) 감지를 다룹니다.
