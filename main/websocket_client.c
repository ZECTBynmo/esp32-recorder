#include "websocket_client.h"
#include "esp_websocket_client.h"
#include "esp_log.h"
#include "secrets.h"
#include "esp_crt_bundle.h"

static const char *TAG = "websocket_client";
static esp_websocket_client_handle_t client;
static bool is_connected = false;

static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    // esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    
    switch (event_id) {
        case WEBSOCKET_EVENT_CONNECTED:
            ESP_LOGI(TAG, "WEBSOCKET_EVENT_CONNECTED");
            is_connected = true;
            break;
        case WEBSOCKET_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "WEBSOCKET_EVENT_DISCONNECTED");
            is_connected = false;
            break;
        case WEBSOCKET_EVENT_ERROR:
            ESP_LOGI(TAG, "WEBSOCKET_EVENT_ERROR");
            is_connected = false;
            break;
        default:
            break;
    }
}

esp_err_t websocket_client_init(void)
{
    esp_websocket_client_config_t websocket_cfg = {
        .uri = WS_URI,
        .disable_auto_reconnect = false,
        .reconnect_timeout_ms = 10000,
        .network_timeout_ms = 10000,
        .transport = WEBSOCKET_TRANSPORT_OVER_SSL,
        .skip_cert_common_name_check = true,
        .cert_pem = NULL,
        .cert_len = 0,
        .buffer_size = 2048,
        .ping_interval_sec = 10,
        .task_stack = 8192,
        .user_agent = NULL,
        .headers = NULL,
        .path = "/"
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
    if (!is_connected) {
        ESP_LOGW(TAG, "Not sending: websocket not connected");
        return ESP_ERR_NOT_CONNECTED;
    }
    
    ESP_LOGI(TAG, "Attempting to send %d bytes", len);
    
    if (esp_websocket_client_is_connected(client)) {
        int sent = esp_websocket_client_send_bin(client, data, len, pdMS_TO_TICKS(1000));
        if (sent < 0) {
            ESP_LOGE(TAG, "Failed to send data, error: %d", sent);
            return ESP_FAIL;
        }
        ESP_LOGI(TAG, "Successfully sent %d bytes", sent);
        return ESP_OK;
    }
    
    ESP_LOGW(TAG, "Not sending: websocket client reports disconnected");
    return ESP_FAIL;
}

void websocket_client_close(void)
{
    if (client) {
        esp_websocket_client_close(client, portMAX_DELAY);
        esp_websocket_client_destroy(client);
    }
}

bool websocket_client_is_connected(void)
{
    return is_connected;
} 