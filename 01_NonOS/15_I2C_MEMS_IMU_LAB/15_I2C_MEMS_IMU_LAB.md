# 15. MEMS 가속도/자이로 센서 — MPU6050

## 목표

6축 MEMS 센서 MPU6050으로 가속도(기울기/움직임)와 각속도(회전)를 측정합니다.

## 준비물

- MPU6050 모듈 (I2C 주소 보통 `0x68`)

## 회로

- `09`와 동일 배선

## 라이브러리 설치

```ini
lib_deps =
    adafruit/Adafruit MPU6050 @ ^2.2.6
```

## 코드

```cpp
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();

  if (!mpu.begin()) {
    Serial.println("MPU6050 not found - check wiring");
    while (1) delay(1000);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  Serial.println("MPU6050 initialized");
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);   // 가속도/자이로/온도 한 번에 갱신

  Serial.printf("Accel(m/s^2) X:%.2f Y:%.2f Z:%.2f | Gyro(rad/s) X:%.2f Y:%.2f Z:%.2f | Temp:%.1fC\n",
    a.acceleration.x, a.acceleration.y, a.acceleration.z,
    g.gyro.x, g.gyro.y, g.gyro.z,
    temp.temperature);

  delay(100);
}
```

## 실행 & 확인

- 센서를 가만히 두면 한 축(보통 Z)의 가속도가 약 9.8 m/s²(중력가속도) 근처로 찍히는지 확인
- 센서를 기울이거나 회전시키면 값이 따라 변하는지 확인

## 관찰 포인트

- `setAccelerometerRange`/`setGyroRange`는 측정 가능한 범위를 정합니다 — 범위를 넓히면(예: 16G) 더 큰 움직임도 놓치지 않지만 정밀도(분해능)는 낮아집니다. 반대로 범위를 좁히면(2G) 미세한 움직임을 더 정밀하게 잡아내지만 큰 움직임에서는 값이 잘립니다(clipping)
- 가속도 값이 정지 상태에서도 정확히 9.8이 아니라 살짝 오차가 있는 게 정상입니다 — 정밀한 용도라면 정지 상태 값을 기준으로 오프셋을 빼는 캘리브레이션이 필요합니다
- 이 센서는 "진동 감지"의 기초가 됩니다 — 일정 주기로 가속도 값을 여러 개 모아 변화폭(분산)을 계산하면 간단한 진동/충격 감지기를 만들 수 있습니다 (응용 과제)

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| `MPU6050 not found` | `09`의 스캐너로 `0x68`(또는 `0x69`, AD0 핀 상태에 따라 다름) 확인 |
| 가만히 둬도 값이 계속 흔들림(noise) | `setFilterBandwidth`로 더 낮은 대역폭(예: `MPU6050_BAND_5_HZ`) 적용 시 노이즈가 줄어듦 |

## 다음

`16_I2C_MULTI_SENSOR_LAB.md` — 여러 I2C 센서를 동시에 연결할 때의 주소 충돌 문제를 다룹니다.
