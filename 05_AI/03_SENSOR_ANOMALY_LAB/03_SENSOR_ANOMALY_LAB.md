# 03. 진동/센서 이상 탐지 (Anomaly Detection)

세 예제 중 **기존 하드웨어(가변저항, GPIO6)로 바로 실습 가능**한 예제입니다. 진짜 진동을 측정하려면 가속도계(MPU6050 등)나 진동 스위치(SW-420, 노크 센서)가 필요하지만, "센서 값의 정상 패턴을 학습해서 벗어나면 이상으로 판정"하는 ML 파이프라인 자체는 어떤 아날로그/디지털 센서든 동일합니다 — 여기서는 가변저항 값을 "센서"로 취급합니다.

> 실제 진동 센서(SW-420, 노크 센서)를 갖고 계시다면, `nonos_curriculum/46_VIBRATION_KNOCK_SENSOR_LAB.md`에서 배선/코드를 먼저 확인하신 뒤 이 문서의 Step 1(데이터 수집)을 그 센서로 교체하는 걸 권장합니다 — 아래 관찰 포인트에 구체적인 교체 방법을 정리해뒀습니다.

## 개념 — Autoencoder 기반 이상 탐지

정상 데이터만으로 **Autoencoder**(입력을 압축했다가 그대로 복원하도록 학습된 신경망)를 학습시킵니다. 정상 패턴에 대해서는 복원이 잘 되지만(복원 오차가 작음), 한 번도 본 적 없는 비정상 패턴이 들어오면 복원이 잘 안 됩니다(복원 오차가 큼). 이 오차가 임계값을 넘으면 "이상"으로 판정합니다 — **비정상 데이터를 따로 수집할 필요가 없다는 게 핵심 장점**입니다 (정상 데이터만 있으면 됨).

## Step 1. 데이터 수집 (ESP-IDF, ADC 읽기)

`00_LITERT_TFLM_SETUP.md`의 프로젝트에서, 정상 상태의 센서 값을 수집해 시리얼로 CSV처럼 출력하는 코드를 작성합니다. ESP-IDF는 Arduino의 `analogRead()` 대신 `adc_oneshot` 드라이버를 씁니다.

```c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"

#define SAMPLE_WINDOW 20   // 20개 샘플을 하나의 "패턴"으로 묶음

void app_main(void) {
    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t init_cfg = { .unit_id = ADC_UNIT_1 };
    adc_oneshot_new_unit(&init_cfg, &adc_handle);

    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_5, &chan_cfg);  // GPIO6 = ADC1_CH5

    while (1) {
        for (int i = 0; i < SAMPLE_WINDOW; i++) {
            int raw;
            adc_oneshot_read(adc_handle, ADC_CHANNEL_5, &raw);
            printf("%d", raw);
            if (i < SAMPLE_WINDOW - 1) printf(",");
            vTaskDelay(pdMS_TO_TICKS(20));   // 20ms 간격 -> 20개면 400ms 윈도우
        }
        printf("\n");
    }
}
```

PC에서 시리얼 로그를 텍스트 파일로 저장합니다 (PlatformIO 시리얼 모니터 로그 저장 기능 또는 `pio device monitor > normal_data.csv`). **정상 상태**(가변저항을 평소 움직이는 범위 내에서 천천히/규칙적으로 돌리기)로 최소 수백~수천 줄을 모으세요.

## Step 2. Python으로 Autoencoder 학습 (PC에서, 오프디바이스)

```python
import numpy as np
import tensorflow as tf
from tensorflow import keras

# normal_data.csv: 한 줄에 20개 값 (SAMPLE_WINDOW)
data = np.loadtxt("normal_data.csv", delimiter=",")
data = data / 4095.0  # 0~1 정규화 (ADC 12bit 기준)

# 간단한 Autoencoder: 20 -> 8 -> 20
inputs = keras.Input(shape=(20,))
encoded = keras.layers.Dense(8, activation="relu")(inputs)
decoded = keras.layers.Dense(20, activation="sigmoid")(encoded)
autoencoder = keras.Model(inputs, decoded)
autoencoder.compile(optimizer="adam", loss="mse")

autoencoder.fit(data, data, epochs=50, batch_size=16, validation_split=0.1)

# 정상 데이터의 복원 오차 분포를 보고 임계값(threshold)을 정함
reconstructed = autoencoder.predict(data)
errors = np.mean((data - reconstructed) ** 2, axis=1)
print("정상 데이터 복원 오차 - 평균:", errors.mean(), "최대:", errors.max())
# 임계값 예시: 평균 + 3 * 표준편차
threshold = errors.mean() + 3 * errors.std()
print("추천 임계값:", threshold)

# INT8 양자화하여 .tflite로 변환
def representative_dataset():
    for sample in data[:100]:
        yield [sample.reshape(1, 20).astype(np.float32)]

converter = tf.lite.TFLiteConverter.from_keras_model(autoencoder)
converter.optimizations = [tf.lite.Optimize.DEFAULT]
converter.representative_dataset = representative_dataset
converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
converter.inference_input_type = tf.int8
converter.inference_output_type = tf.int8
tflite_model = converter.convert()

with open("anomaly_model.tflite", "wb") as f:
    f.write(tflite_model)
```

`threshold` 출력값을 메모해두세요 — Step 4에서 코드에 그대로 넣습니다.

## Step 3. 모델을 C 배열로 변환

```bash
xxd -i anomaly_model.tflite > anomaly_model.h
```

생성된 `anomaly_model.h`를 프로젝트의 `main/` 폴더에 넣습니다.

## Step 4. ESP32-S3에서 추론

```c
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "esp_adc/adc_oneshot.h"
#include "anomaly_model.h"

#define SAMPLE_WINDOW 20
#define ANOMALY_THRESHOLD 0.015f   // Step 2에서 구한 threshold로 교체

constexpr int kTensorArenaSize = 8 * 1024;
uint8_t tensor_arena[kTensorArenaSize];

void app_main(void) {
    // --- ADC 초기화 (Step 1과 동일) ---
    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t init_cfg = { .unit_id = ADC_UNIT_1 };
    adc_oneshot_new_unit(&init_cfg, &adc_handle);
    adc_oneshot_chan_cfg_t chan_cfg = { .bitwidth = ADC_BITWIDTH_DEFAULT, .atten = ADC_ATTEN_DB_12 };
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_5, &chan_cfg);

    // --- TFLM 초기화 ---
    const tflite::Model* model = tflite::GetModel(anomaly_model_tflite);
    tflite::MicroMutableOpResolver<3> resolver;
    resolver.AddFullyConnected();
    resolver.AddLogistic();   // sigmoid activation
    resolver.AddRelu();

    tflite::MicroInterpreter interpreter(model, resolver, tensor_arena, kTensorArenaSize);
    interpreter.AllocateTensors();

    TfLiteTensor* input = interpreter.input(0);
    TfLiteTensor* output = interpreter.output(0);

    while (1) {
        float window[SAMPLE_WINDOW];
        for (int i = 0; i < SAMPLE_WINDOW; i++) {
            int raw;
            adc_oneshot_read(adc_handle, ADC_CHANNEL_5, &raw);
            window[i] = raw / 4095.0f;
            vTaskDelay(pdMS_TO_TICKS(20));
        }

        // 입력 텐서에 채우기 (양자화 스케일 적용)
        for (int i = 0; i < SAMPLE_WINDOW; i++) {
            input->data.int8[i] = (int8_t)(window[i] / input->params.scale + input->params.zero_point);
        }

        interpreter.Invoke();

        // 복원 오차(MSE) 계산
        float mse = 0.0f;
        for (int i = 0; i < SAMPLE_WINDOW; i++) {
            float reconstructed = (output->data.int8[i] - output->params.zero_point) * output->params.scale;
            float diff = window[i] - reconstructed;
            mse += diff * diff;
        }
        mse /= SAMPLE_WINDOW;

        if (mse > ANOMALY_THRESHOLD) {
            printf("ANOMALY DETECTED! mse=%.4f\n", mse);
        } else {
            printf("normal, mse=%.4f\n", mse);
        }
    }
}
```

## 실행 & 확인

- 평소처럼 가변저항을 움직이면 "normal"이 계속 출력되는지 확인
- 평소와 다른 패턴(예: 아주 빠르게 반복적으로 돌리기, 평소 안 쓰던 극단적인 위치에서만 움직이기)을 만들어보면 "ANOMALY DETECTED"가 뜨는지 확인

## 관찰 포인트

- 이 파이프라인은 실제 진동 센서(MPU6050 가속도계 등)로 그대로 확장할 수 있습니다 — Step 1의 ADC 읽기 부분을 I2C로 가속도 X/Y/Z 값을 읽는 코드로 바꾸고, `SAMPLE_WINDOW`를 3축 × N개 샘플로 늘리면 됩니다. Autoencoder 구조와 학습/양자화/배포 흐름은 동일합니다
- **SW-420/노크 센서로 교체하는 경우**: 이 센서들은 아날로그 연속값이 아니라 "이벤트 발생 여부"를 주는 디지털 센서입니다(`46_VIBRATION_KNOCK_SENSOR_LAB.md` 참고). Step 1의 `adc_oneshot_read()` 대신, `SAMPLE_WINDOW`(20개 슬롯) 동안 인터럽트가 몇 번 발생했는지 **카운트**해서 하나의 값으로 만드는 방식이 자연스럽습니다:
  ```c
  // 20ms마다 샘플링하는 대신, 예: 400ms 윈도우 동안의 이벤트 카운트를 특징값으로 사용
  volatile int event_count = 0;   // ISR에서 증가
  // ... 윈도우 종료 시 event_count를 window[i] 자리에 넣고 0으로 리셋
  ```
  정상 상태(평소 진동 수준)에서는 카운트가 낮고 일정하게 유지되지만, 이상 상태(충격, 고장 등)에서는 카운트가 급증하는 패턴을 Autoencoder가 학습하게 됩니다 — 아날로그 값 대신 "빈도"를 특징으로 쓴다는 점만 다르고, 나머지 Autoencoder/양자화/배포 파이프라인은 완전히 동일합니다
- 임계값(threshold)은 한 번 정하고 끝이 아니라, 실제 배포 환경에서 "정상 오차"가 얼마나 변동하는지 보고 조정해야 합니다 — 너무 낮으면 오탐이 잦고, 너무 높으면 진짜 이상을 놓칩니다
- `MicroMutableOpResolver<3>`처럼 필요한 연산(Op)만 등록하는 이유는, TFLM이 모든 연산을 다 포함하면 플래시 용량을 많이 차지하기 때문입니다 — 모델에 실제로 쓰인 레이어(FullyConnected, Logistic 등)만 선택적으로 등록해서 바이너리 크기를 줄입니다

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| `AllocateTensors() failed` | `kTensorArenaSize`가 모델이 필요로 하는 크기보다 작음 — 값을 늘려보며 재시도 |
| 항상 ANOMALY만 뜸 | 정규화(÷4095.0f) 기준이 학습 때와 다르거나, 양자화 scale/zero_point 적용이 잘못됐을 가능성 — Step 2의 정규화 방식과 Step 4가 정확히 일치하는지 확인 |
| 학습은 잘 됐는데 임베디드에서 결과가 이상함 | Python(float32)과 ESP32(int8 양자화) 사이의 정밀도 차이 — 임계값을 임베디드에서 실측한 정상 오차 기준으로 재조정 |

## 다음

세 예제를 모두 완료하셨습니다. `00_LITERT_TFLM_SETUP.md`의 체크리스트를 다시 확인해보시고, 궁금한 부분이나 막히는 지점이 있으면 언제든 말씀해주세요.
