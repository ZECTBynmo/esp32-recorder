#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "wifi_connect.h"
#include "audio_recorder.h"
#include "websocket_client.h"

static const char *TAG = "main";

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();

    // Initialize WebSocket client
    ESP_ERROR_CHECK(websocket_client_init());

    // Initialize and start audio recording
    ESP_ERROR_CHECK(init_audio_recorder());
    start_audio_recording();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
