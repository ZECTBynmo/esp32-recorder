#include "websocket_client.h"
#include "esp_websocket_client.h"
#include "esp_log.h"
#include "secrets.h"

static const char *TAG = "websocket_client";
static esp_websocket_client_handle_t client;

static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    // esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    
    switch (event_id) {
        case WEBSOCKET_EVENT_CONNECTED:
            ESP_LOGI(TAG, "WEBSOCKET_EVENT_CONNECTED");
            break;
        case WEBSOCKET_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "WEBSOCKET_EVENT_DISCONNECTED");
            break;
        case WEBSOCKET_EVENT_ERROR:
            ESP_LOGI(TAG, "WEBSOCKET_EVENT_ERROR");
            break;
        default:
            break;
    }
}

esp_err_t websocket_client_init(void)
{
    esp_websocket_client_config_t websocket_cfg = {
        .uri = WS_URI,
    };

    client = esp_websocket_client_init(&websocket_cfg);
    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to initialize websocket client");
        return ESP_FAIL;
    }

    ESP_ERROR_CHECK(esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, NULL));
    
    esp_err_t ret = esp_websocket_client_start(client);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start websocket client");
        return ret;
    }

    return ESP_OK;
}

esp_err_t websocket_client_send(const void* data, size_t len)
{
    if (esp_websocket_client_is_connected(client)) {
        return esp_websocket_client_send_bin(client, data, len, portMAX_DELAY);
    }
    return ESP_FAIL;
}

void websocket_client_close(void)
{
    if (client) {
        esp_websocket_client_close(client, portMAX_DELAY);
        esp_websocket_client_destroy(client);
    }
} 