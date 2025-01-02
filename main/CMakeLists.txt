idf_component_register(SRCS "recorder.c" "wifi_connect.c" "websocket_client.c" "audio_recorder.c"
                    INCLUDE_DIRS "."
                    REQUIRES nvs_flash esp_wifi esp_websocket_client driver esp_adc)
