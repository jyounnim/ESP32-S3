// Source: 03_SENSOR_ANOMALY_LAB.md
// Section: Step 4. ESP32-S3에서 추론

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
