#ifndef WEBSOCKET_CLIENT_H
#define WEBSOCKET_CLIENT_H

#include "esp_err.h"
#include <stddef.h>
#include <stdbool.h>

#define ESP_ERR_NOT_CONNECTED 0x102

esp_err_t websocket_client_init(void);
esp_err_t websocket_client_send(const void* data, size_t len);
void websocket_client_close(void);
bool websocket_client_is_connected(void);

#endif // WEBSOCKET_CLIENT_H