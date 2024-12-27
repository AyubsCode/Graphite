
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "esp_system.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"
#include "nvs_flash.h"
#include "esp_gatt_common_api.h"

// Define the custom service and characteristic UUIDs
#define EXAMPLE_SERVICE_UUID        0x00FF  // Custom service UUID
#define EXAMPLE_CHAR_UUID           0xFF01  // Custom characteristic UUID

// GATT service and characteristic handles
static uint16_t example_service_handle = 0;
static uint16_t example_char_handle = 0;

// Define the characteristic UUID structure
static esp_gatt_srvc_id_t example_service = {
    .is_primary = true,
    .id = {
        .inst_id = 0,
        .uuid = {
            .len = ESP_UUID_LEN_16,
            .uuid.uuid16 = EXAMPLE_SERVICE_UUID,
        },
    },
};

// Define the characteristic descriptor
static esp_gatt_char_descr_elem_t gatt_char_desc = {
    .uuid = {
        .len = ESP_UUID_LEN_16,
        .uuid.uuid16 = EXAMPLE_CHAR_UUID,
    },
    .len = ESP_UUID_LEN_16,
    .value = {0},
};

// BLE callback functions
static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_gap_ble_cb_param_t *param) {
    // Handle BLE events
}

static void esp_gatts_cb(esp_gatts_cb_event_t event, esp_gatts_cb_param_t *param) {
    // Handle GATT events
}

static void start_advertising() {
    esp_ble_adv_data_t adv_data = {
        .set_scan_rsp = false,
        .include_name = true,
        .include_txpower = true,
        .min_interval = 0x20,
        .max_interval = 0x40,
        .service_uuid = {EXAMPLE_SERVICE_UUID},
    };
    ESP_ERROR_CHECK(esp_ble_gap_start_advertising(&adv_data));
}

// Initialize BLE
static void ble_init() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    esp_bt_controller_init();
    esp_bt_controller_enable(ESP_BT_MODE_BLE);

    ESP_ERROR_CHECK(esp_ble_gatts_register_callback(esp_gatts_cb));
    ESP_ERROR_CHECK(esp_ble_gap_register_callback(esp_gap_cb));

    ESP_ERROR_CHECK(esp_ble_gatts_app_register(0));

    // Start BLE advertising
    start_advertising();
}

void app_main() {
    ble_init();
}
