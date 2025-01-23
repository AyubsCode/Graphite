#include "stdio.h"
#include "string.h"
#include "driver/gpio.h"
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

// Define pins for SPI
#define PIN_NUM_MISO  19
#define PIN_NUM_MOSI  23
#define PIN_NUM_CLK   18
#define PIN_NUM_CS    5


void writeFile(char* path){
    FILE* test_file = fopen(path , "w") ;
    if (test_file == NULL) {
        ESP_LOGE("SD", "Failed to open test.txt for writing.");
    } else {
        fprintf(test_file, "SD card is Works\n");
        fprintf(test_file, "SD card is Works\n");
        fprintf(test_file, "This is the new functions line\n");
        fclose(test_file);
        ESP_LOGI("SD", "Successfully written to test.txt.");
    }
}

void readFile(){
    FILE* test_file = fopen("/sdcard/test.txt", "w");
    test_file = fopen("/sdcard/test.txt", "r");
    if (test_file == NULL) {
        ESP_LOGE("SD", "Failed to open test.txt for reading.");
    } else {
        ESP_LOGI("SD", "Reading from test.txt...");
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), test_file)) {
            printf("%s", buffer);
        }
        fclose(test_file);
        ESP_LOGI("SD", "File read successfully.");
    }
}


void init_sd_card() {
    esp_err_t ret;
    ESP_LOGI("SD", "Initializing SD card...");

    // Configure SPI bus
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };

    ret = spi_bus_initialize(HSPI_HOST, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK) {
        ESP_LOGE("SD", "Failed to initialize bus.");
        return;
    }

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;

    // Mount configuration
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    sdmmc_card_t* card;
    ret = esp_vfs_fat_sdspi_mount("/sdcard", &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE("SD", "Failed to mount filesystem.");
        } else {
            ESP_LOGE("SD", "Failed to initialize the card (%s)", esp_err_to_name(ret));
        }
        return;
    }

    ESP_LOGI("SD", "SD card mounted successfully");
    sdmmc_card_print_info(stdout, card);
    FILE* test_file = fopen("/sdcard/test.txt", "w");
    writeFile("/sdcard/test.txt") ;
    test_file = fopen("/sdcard/test.txt", "r");
    if (test_file == NULL) {
        ESP_LOGE("SD", "Failed to open test.txt for reading.");
    } else {
        ESP_LOGI("SD", "Reading from test.txt...");
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), test_file)) {
            printf("%s", buffer);
        }
        fclose(test_file);
        ESP_LOGI("SD", "File read successfully.");
    }
}


void app_main(void) {
    esp_log_level_set("*", ESP_LOG_INFO);
    init_sd_card();
}
