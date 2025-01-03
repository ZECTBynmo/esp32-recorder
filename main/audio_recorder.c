#include "audio_recorder.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/gpio.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "websocket_client.h"

static const char *TAG = "audio_recorder";
static adc_oneshot_unit_handle_t adc1_handle;
static adc_cali_handle_t adc_cali_handle = NULL;

static QueueHandle_t audio_queue;
static int16_t audio_buffer[AUDIO_BUFFER_SIZE];
static size_t buffer_index = 0;
static esp_timer_handle_t sampling_timer;

static void adc_init(void) {
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };

    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_6, &config));

    adc_cali_handle_t handle = NULL;
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };
    
    ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&cali_config, &handle));
    adc_cali_handle = handle;
}

static void IRAM_ATTR timer_callback(void* arg) {
    int adc_raw;
    if (adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &adc_raw) == ESP_OK) {
        audio_buffer[buffer_index++] = adc_raw;
        
        if (buffer_index >= AUDIO_BUFFER_SIZE) {
            buffer_index = 0;
            xQueueSendFromISR(audio_queue, audio_buffer, NULL);
        }
    }
}

static void audio_recording_task(void *pvParameters) {
    int16_t *received_buffer = heap_caps_malloc(AUDIO_BUFFER_SIZE * sizeof(int16_t), MALLOC_CAP_8BIT);
    if (received_buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate receive buffer");
        vTaskDelete(NULL);
        return;
    }
    
    while (1) {
        if (xQueueReceive(audio_queue, received_buffer, portMAX_DELAY) == pdTRUE) {
            // Send audio data over WebSocket
            if (!websocket_client_is_connected()) {
                ESP_LOGW(TAG, "Websocket not connected, dropping audio data");
                continue;
            }

            esp_err_t err = websocket_client_send(received_buffer, AUDIO_BUFFER_SIZE * sizeof(int16_t));
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to send audio data over WebSocket");
            }
        }
        vTaskDelay(1);
    }
}

esp_err_t init_audio_recorder(void) {
    adc_init();
    
    audio_queue = xQueueCreate(3, sizeof(int16_t) * AUDIO_BUFFER_SIZE);
    if (audio_queue == NULL) {
        return ESP_ERR_NO_MEM;
    }

    const esp_timer_create_args_t timer_args = {
        .callback = timer_callback,
        .name = "audio_sampler"
    };
    
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &sampling_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(sampling_timer, 1000000 / AUDIO_SAMPLE_RATE));
    
    ESP_LOGI(TAG, "ADC initialized for audio recording");
    return ESP_OK;
}

void start_audio_recording(void) {
    xTaskCreate(audio_recording_task, 
                "audio_recording_task", 
                4096, 
                NULL, 
                5, 
                NULL);
    ESP_LOGI(TAG, "Audio recording task started");
} 