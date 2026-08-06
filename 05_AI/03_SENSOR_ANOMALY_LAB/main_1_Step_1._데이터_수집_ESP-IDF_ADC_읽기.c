// Source: 03_SENSOR_ANOMALY_LAB.md
// Section: Step 1. 데이터 수집 (ESP-IDF, ADC 읽기)

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
