idf_component_register(SRCS "recorder.c" "wifi_connect.c"
                    INCLUDE_DIRS "."
                    REQUIRES nvs_flash esp_wifi)
