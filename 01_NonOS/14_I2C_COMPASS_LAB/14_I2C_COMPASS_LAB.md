# 14. 전자 나침반 — QMC5883L

## 목표

3축 지자기 센서 QMC5883L로 방위각(Azimuth)과 16방위(N, NE, E...)를 구합니다.

## 준비물

- QMC5883L 모듈 (GY-271 보드 등, I2C 주소 보통 `0x0D`)

## 회로

- `09`와 동일 배선 (SDA=GPIO8, SCL=GPIO9)

## 라이브러리 설치

```ini
lib_deps =
    mprograms/QMC5883LCompass @ ^1.1.1
```

## 코드

```cpp
#include <Arduino.h>
#include <Wire.h>
#include <QMC5883LCompass.h>

QMC5883LCompass compass;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();
  compass.init();
}

void loop() {
  compass.read();

  int x = compass.getX();
  int y = compass.getY();
  int z = compass.getZ();
  int azimuth = compass.getAzimuth();
  byte bearing = compass.getBearing(azimuth);

  char direction[3];
  compass.getDirection(direction, azimuth);

  Serial.printf("X:%d Y:%d Z:%d Azimuth:%d Direction:%c%c%c\n",
                x, y, z, azimuth, direction[0], direction[1], direction[2]);
  delay(250);
}
```

## 실행 & 확인

- 센서를 수평으로 놓고 천천히 돌리면서 `Azimuth`(0~359도)와 `Direction`(N, NE, E...)이 따라 바뀌는지 확인

## 관찰 포인트

- **정확한 방위를 위해서는 캘리브레이션이 거의 필수입니다.** 지자기 센서는 개별 소자 편차와 주변 금속/자석의 영향을 받아, 캘리브레이션 없이는 오차가 클 수 있습니다. 라이브러리 예제 폴더의 `calibration.ino`를 먼저 업로드해서 센서를 여러 방향으로 돌려가며 최소/최대값을 수집한 뒤, 출력된 `setCalibration(...)` 코드를 본 코드의 `compass.init()` 다음 줄에 추가하세요
- 센서 근처에 스피커, 모터, 큰 철제 물체가 있으면 지자기장이 왜곡되어 방위가 틀어질 수 있습니다 — 실제 배치할 위치에서 캘리브레이션하는 게 정확도에 중요합니다
- `getBearing(azimuth)`는 0~15 사이의 16방위 인덱스를 반환하고, `getDirection()`이 그걸 "NNE" 같은 문자로 바꿔줍니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| 모든 값이 0 | 일부 GY-87 등 복합 모듈은 이 라이브러리와 호환이 안 맞는 경우가 보고됨 — `09`의 스캐너로 실제 주소를 먼저 확인하고, 안 되면 `MechaQMC5883` 같은 대체 라이브러리 시도 |
| 방위가 실제와 계속 다름 | 캘리브레이션 미적용, 또는 주변 금속/자성체 간섭 |
| 값이 튐(noise) | `setSmoothing()`으로 이동평균 스무딩 적용 가능 (라이브러리 문서 참고) |

## 다음

`15_I2C_MEMS_IMU_LAB.md` — MEMS 가속도/자이로 센서 MPU6050을 다룹니다.
