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
#include "io.h"


void app_main(void) {
    esp_log_level_set("*", ESP_LOG_INFO);
    init_sd_card();
}
