#ifndef WIFI_CONNECT_H
#define WIFI_CONNECT_H

#include "esp_wifi.h"
#include "esp_event.h"

#define MAXIMUM_RETRY 5

void wifi_init_sta(void);

#endif // WIFI_CONNECT_H 