#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_server.h"

#define WIFI_SSID "ESP32-Stream"
#define WIFI_PASS "12345678"

static const char *TAG = "WiFi_AP";

// Function to start Wi-Fi in Access Point mode
static void wifi_init_softap(void) {
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = WIFI_SSID,
            .ssid_len = strlen(WIFI_SSID),
            .password = WIFI_PASS,
            .max_connection = 4,  // Max devices
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };

    if (strlen(WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi AP started with SSID: %s", WIFI_SSID);
}

// HTTP GET handler for streaming data
static esp_err_t stream_get_handler(httpd_req_t *req) {
    char *data = "Real-time data from ESP32\n";
    httpd_resp_send(req, data, strlen(data));
    return ESP_OK;
}

// Start HTTP server
static httpd_handle_t start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t stream_data = {
            .uri       = "/stream",
            .method    = HTTP_GET,
            .handler   = stream_get_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &stream_data);
        ESP_LOGI(TAG, "Web server started");
    } else {
        ESP_LOGE(TAG, "Failed to start web server");
    }

    return server;
}

void app_main(void) {
    // Initialize NVS
    ESP_ERROR_CHECK(nvs_flash_init());

    // Initialize Wi-Fi in Access Point mode
    wifi_init_softap();

    // Start the web server
    start_webserver();
}
