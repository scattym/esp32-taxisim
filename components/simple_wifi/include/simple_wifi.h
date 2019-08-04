//
// Created by sysadmin on 23/06/18.
//

#ifndef ESP_IDF_TEMPLATE_SIMPLE_WIFI_H
#define ESP_IDF_TEMPLATE_SIMPLE_WIFI_H
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
/* The examples use simple WiFi configuration that you can set via
   'make menuconfig'.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define SIMPLE_WIFI_ESP_WIFI_MODE_AP   CONFIG_ESP_WIFI_MODE_AP //TRUE:AP FALSE:STA
#define SIMPLE_WIFI_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define SIMPLE_WIFI_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define SIMPLE_WIFI_MAX_STA_CONN       CONFIG_MAX_STA_CONN
#define SIMPLE_WIFI_ESP_HOSTNAME       CONFIG_ESP_HOSTNAME

void simple_wifi_init();

#endif //ESP_IDF_TEMPLATE_SIMPLE_WIFI_H
