#ifndef WEBSOCKET_CLIENT_H
#define WEBSOCKET_CLIENT_H

#include "esp_err.h"

esp_err_t websocket_client_init(void);
esp_err_t websocket_client_send(const void* data, size_t len);
void websocket_client_close(void);

#endif // WEBSOCKET_CLIENT_H 