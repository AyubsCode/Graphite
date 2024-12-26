
#include <stdio.h>
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_device.h"
#include "esp_bt_main.h"
#include "esp_spp_api.h"

// Tag for logging
static const char *TAG = "BT_SPP_Server";

// Bluetooth SPP Callback function
static void spp_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
    switch (event) {
        case ESP_SPP_SRV_OPEN_EVT:
            ESP_LOGI(TAG, "Bluetooth connection opened");
            break;
        case ESP_SPP_CLOSE_EVT:
            ESP_LOGI(TAG, "Bluetooth connection closed");
            break;
        case ESP_SPP_DATA_IND_EVT:
            ESP_LOGI(TAG, "Data received: %s", param->data_ind.data);
            // Echo received data back to the client
            esp_spp_write(param->data_ind.handle, param->data_ind.len, param->data_ind.data);
            break;
        default:
            break;
    }
}

// Function to initialize Bluetooth
void bt_init(void) {
    esp_err_t ret;

    // Initialize the Bluetooth controller
    ret = esp_bt_controller_init();
    if (ret) {
        ESP_LOGE(TAG, "Bluetooth controller initialization failed: %s", esp_err_to_name(ret));
        return;
    }

    // Enable Bluetooth Classic mode
    ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT);
    if (ret) {
        ESP_LOGE(TAG, "Bluetooth controller enable failed: %s", esp_err_to_name(ret));
        return;
    }

    // Initialize the Bluetooth stack (Bluedroid)
    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(TAG, "Bluedroid stack initialization failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(TAG, "Bluedroid stack enable failed: %s", esp_err_to_name(ret));
        return;
    }

    // Register the SPP callback
    esp_spp_register_callback(spp_callback);

    // Initialize the SPP module in callback mode
    ret = esp_spp_init(ESP_SPP_MODE_CB);
    if (ret) {
        ESP_LOGE(TAG, "SPP initialization failed: %s", esp_err_to_name(ret));
        return;
    }

    // Start the Bluetooth SPP server
    ret = esp_spp_start_srv(ESP_SPP_SEC_NONE, 0, "ESP32_SPP_SERVER");
    if (ret) {
        ESP_LOGE(TAG, "SPP server start failed: %s", esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(TAG, "Bluetooth SPP server started");
}

// Main function
void app_main(void) {
    ESP_LOGI(TAG, "Starting Bluetooth SPP server");

    // Initialize Bluetooth
    bt_init();

    // Main loop
    while (1) {
        // Keep the server running
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
