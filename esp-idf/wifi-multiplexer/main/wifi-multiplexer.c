#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_timer.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_http_server.h"
#include "driver/gpio.h"
#include "cJSON.h"
#include "string.h"
#include "esp_log.h"
#include "esp_system.h"
#include "sdmmc_cmd.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "sdmmc_cmd.h"
#include "esp_err.h"
#include <stdio.h>
#include "io.h"

#define AP_SSID "TestNetwork"
#define AP_PASS "12345678"
#define LED_PIN 2
#define MAX_JSON_SIZE 1024

static const char *TAG = "wifi_ap";



// remember , I have to play around w this some more
const char* html_page = "<!DOCTYPE html><html>\
<head>\
    <title>ESP32 JSON Communication</title>\
    <style>\
        body { font-family: Arial, sans-serif; margin: 20px; }\
        .container { max-width: 800px; margin: 0 auto; }\
        pre { background: #f4f4f4; padding: 10px; border-radius: 5px; }\
        button { padding: 10px 20px; margin: 10px 0; }\
        input { margin: 5px 0; padding: 5px; }\
    </style>\
</head>\
<body>\
    <div class='container'>\
        <h1>ESP32 JSON Communication</h1>\
        <div>\
            <h2>Send JSON to ESP32</h2>\
            <input type='text' id='messageInput' placeholder='Enter message'>\
            <button onclick='sendJSON()'>Send JSON</button>\
        </div>\
        <div>\
            <h2>Receive JSON from ESP32</h2>\
            <button onclick='fetchJSON()'>Fetch JSON</button>\
            <h3>Received Data:</h3>\
            <pre id='receivedJson'>No data</pre>\
        </div>\
    </div>\
    <script>\
        async function sendJSON() {\
            const message = document.getElementById('messageInput').value;\
            const data = {\
                message: message,\
                timestamp: new Date().toISOString()\
            };\
            try {\
                const response = await fetch('/api/json', {\
                    method: 'POST',\
                    headers: {\
                        'Content-Type': 'application/json'\
                    },\
                    body: JSON.stringify(data)\
                });\
                const result = await response.json();\
                alert('Server response: ' + result.status);\
            } catch (error) {\
                console.error('Error:', error);\
                alert('Error sending data');\
            }\
        }\
        async function fetchJSON() {\
            try {\
                const response = await fetch('/api/json');\
                const data = await response.json();\
                document.getElementById('receivedJson').textContent = \
                    JSON.stringify(data, null, 2);\
            } catch (error) {\
                console.error('Error:', error);\
                document.getElementById('receivedJson').textContent = \
                    'Error fetching data';\
            }\
        }\
    </script>\
</body></html>";

static esp_err_t root_handler(httpd_req_t *req)
{
    httpd_resp_send(req, html_page, strlen(html_page));
    return ESP_OK;
}

static esp_err_t post_json_handler(httpd_req_t *req)
{
    char content[MAX_JSON_SIZE];
    int ret = httpd_req_recv(req, content, sizeof(content));

    if (ret <= 0) {
        const char* error_response = "{\"status\":\"error\",\"message\":\"No data received\"}";
        httpd_resp_set_type(req, "application/json");
        httpd_resp_send(req, error_response, strlen(error_response));
        return ESP_OK;
    }

    content[ret] = '\0';

    cJSON *root = cJSON_Parse(content);
    if (root == NULL) {
        const char* error_response = "{\"status\":\"error\",\"message\":\"Invalid JSON\"}";
        httpd_resp_set_type(req, "application/json");
        httpd_resp_send(req, error_response, strlen(error_response));
        return ESP_OK;
    }

    cJSON *message = cJSON_GetObjectItem(root, "message");
    if (cJSON_IsString(message) && (message->valuestring != NULL)) {
        ESP_LOGI(TAG, "Received message: %s", message->valuestring);
    }

    writeFile(message->valuestring) ;
    cJSON_Delete(root);

    const char* success_response = "{\"status\":\"success\",\"message\":\"Data received\"}";
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, success_response, strlen(success_response));

    return ESP_OK;
}

static esp_err_t get_json_handler(httpd_req_t *req)
{
    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "name", "Test JSON");
    cJSON_AddNumberToObject(root, "uptime", esp_timer_get_time() / 1000000);
    cJSON_AddBoolToObject(root, "led_state", gpio_get_level(LED_PIN));
    char *json_string = cJSON_Print(root);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_string, strlen(json_string));

    free(json_string);
    cJSON_Delete(root);

    return ESP_OK;
}

httpd_uri_t uri_root = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = root_handler,
    .user_ctx = NULL
};

httpd_uri_t uri_get_json = {
    .uri = "/api/json",
    .method = HTTP_GET,
    .handler = get_json_handler,
    .user_ctx = NULL
};

httpd_uri_t uri_post_json = {
    .uri = "/api/json",
    .method = HTTP_POST,
    .handler = post_json_handler,
    .user_ctx = NULL
};


static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 4;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &uri_root);
        httpd_register_uri_handler(server, &uri_get_json);
        httpd_register_uri_handler(server, &uri_post_json);
        ESP_LOGI(TAG, "Server started on port: '%d'", config.server_port);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

static void wifi_init_ap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_ap_config = {
        .ap = {
            .ssid = AP_SSID,
            .ssid_len = strlen(AP_SSID),
            .password = AP_PASS,
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi AP Started with SSID: %s", AP_SSID);
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    wifi_init_ap();

    esp_log_level_set("*", ESP_LOG_INFO);
    init_sd_card();

    start_webserver();
}
