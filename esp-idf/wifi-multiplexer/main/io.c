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
#include <stddef.h>
#include <stdio.h>
#include "esp_http_server.h"
#include "cJSON.h"
#include "io.h"

// Define pins for SPI
#define PIN_NUM_MISO  19
#define PIN_NUM_MOSI  23
#define PIN_NUM_CLK   18
#define PIN_NUM_CS    5
#define FILE_WRITE_TAG "Writing"
#define MAX_FILE_SIZE (1024 * 1024)  // 1MB max file size
#define CHUNK_SIZE 4096              // 4KB chunks for reading/writing
#define MAX_JSON_SIZE 1024

static const char *TAG = "io_handler";
static bool sd_card_mounted = false;
static const char* mount_point = "/sdcard";

IO_ERROR writeFile(const char* path, const uint8_t* data, size_t len, FileType type) {
    if (path == NULL || data == NULL) {
        ESP_LOGE(TAG, "Invalid parameters for file writing");
        return METADATA_ERROR;
    }

    char full_path[256];
    snprintf(full_path, sizeof(full_path), "/sdcard/%s", path);
    ESP_LOGI(TAG, "Writing file to: %s", full_path);

    FILE* file = fopen(full_path, "wb");
    if (file == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing: %s", full_path);
        return FILE_OPEN_ERROR;
    }

    size_t written = fwrite(data, 1, len, file);
    fclose(file);

    if (written != len) {
        ESP_LOGE(TAG, "Failed to write complete file. Written: %d, Expected: %d", written, len);
        return WRITE_ERROR;
    }

    ESP_LOGI(TAG, "Successfully written %d bytes to file: %s", written, full_path);
    return EXIT_SUCCESSFUL;
}

// Function to read file from SD card
IO_ERROR readFile(const char* path, uint8_t** data, size_t* len) {
    if (path == NULL || data == NULL || len == NULL) {
        ESP_LOGE(TAG, "Invalid parameters for file reading");
        return METADATA_ERROR;
    }

    char full_path[256];
    snprintf(full_path, sizeof(full_path), "/sdcard/%s", path);
    ESP_LOGI(TAG, "Reading file from: %s", full_path);

    FILE* file = fopen(full_path, "rb");
    if (file == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading: %s", full_path);
        return FILE_OPEN_ERROR;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    *len = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory for file data
    *data = malloc(*len);
    if (*data == NULL) {
        fclose(file);
        ESP_LOGE(TAG, "Failed to allocate memory for file data");
        return MEMORY_ERROR;
    }

    // Read file data
    size_t read_len = fread(*data, 1, *len, file);
    fclose(file);

    if (read_len != *len) {
        free(*data);
        *data = NULL;
        *len = 0;
        ESP_LOGE(TAG, "Failed to read complete file");
        return READ_ERROR;
    }

    ESP_LOGI(TAG, "Successfully read %d bytes from file: %s", read_len, full_path);
    return EXIT_SUCCESSFUL;
}

// Function to handle file upload from HTTP request
// Function to parse file metadata from JSON


IO_ERROR parse_file_metadata(const char* json_str, FileMetadata* metadata) {
    if (json_str == NULL || metadata == NULL) {
        return METADATA_ERROR;
    }

    cJSON *root = cJSON_Parse(json_str);
    if (root == NULL) {
        return JSON_PARSE_ERROR;
    }

    cJSON *filename = cJSON_GetObjectItem(root, "fileName");
    cJSON *filesize = cJSON_GetObjectItem(root, "fileSize");
    cJSON *filetype = cJSON_GetObjectItem(root, "fileType");

    if (!filename || !filesize || !filetype) {
        cJSON_Delete(root);
        return METADATA_ERROR;
    }

    strncpy(metadata->filename, filename->valuestring, MAX_FILENAME_LENGTH - 1);
    metadata->filesize = filesize->valueint;
    metadata->type = filetype->valueint;

    cJSON_Delete(root);
    return EXIT_SUCCESSFUL;
}


esp_err_t init_sd_card(void) {
    esp_err_t ret;
    ESP_LOGI(TAG, "Initializing SD card...");

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
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return ret;
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
    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. Check SD card is inserted and formatted.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s)", esp_err_to_name(ret));
        }
        return ret;
    }

    // Verify we can write to the card
    char test_path[64];
    snprintf(test_path, sizeof(test_path), "%s/test.txt", mount_point);
    FILE* test_file = fopen(test_path, "w");
    if (test_file == NULL) {
        ESP_LOGE(TAG, "Failed to create test file. Check SD card is not write-protected");
        esp_vfs_fat_sdmmc_unmount();
        return ESP_FAIL;
    }
    fclose(test_file);
    unlink(test_path);  // Clean up test file

    sd_card_mounted = true;
    ESP_LOGI(TAG, "SD card mounted successfully at %s", mount_point);
    sdmmc_card_print_info(stdout, card);
    return ESP_OK;
}

IO_ERROR handle_file_upload(httpd_req_t *req, char* filename, size_t content_len) {
    if (!sd_card_mounted) {
        ESP_LOGE(TAG, "SD card not mounted");
        return FILE_OPEN_ERROR;
    }

    if (content_len > MAX_FILE_SIZE) {
        ESP_LOGE(TAG, "File too large: %d bytes", content_len);
        return FILE_SIZE_ERROR;
    }

    char filepath[512];
    snprintf(filepath, sizeof(filepath), "%s/%s", mount_point, filename);
    ESP_LOGI(TAG, "Attempting to create file at: %s", filepath);

    // Check if directory exists
    char dir_path[512];
    strncpy(dir_path, filepath, sizeof(dir_path));
    char* last_slash = strrchr(dir_path, '/');
    if (last_slash) {
        *last_slash = '\0';
        // Try to create directory if it doesn't exist
        struct stat st;
        if (stat(dir_path, &st) != 0) {
            if (mkdir(dir_path, 0755) != 0) {
                ESP_LOGE(TAG, "Failed to create directory: %s", dir_path);
                return FILE_OPEN_ERROR;
            }
        }
    }

    // Open file for writing
    FILE* file = fopen(filepath, "wb");
    if (!file) {
        ESP_LOGE(TAG, "Failed to create file: %s", filepath );
        return FILE_OPEN_ERROR;
    }

    uint8_t *chunk = malloc(CHUNK_SIZE);
    if (!chunk) {
        fclose(file);
        ESP_LOGE(TAG, "Failed to allocate memory for chunk");
        return MEMORY_ERROR;
    }

    size_t remaining = content_len;
    size_t received = 0;

    while (remaining > 0) {
        size_t chunk_size = (remaining < CHUNK_SIZE) ? remaining : CHUNK_SIZE;
        // Attempt to parse this as json first
        int ret = httpd_req_recv(req, (char*)chunk, chunk_size);

        if (ret <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                continue;  // Retry on timeout
            }
            free(chunk);
            fclose(file);
            unlink(filepath);  // Clean up partial file
            ESP_LOGE(TAG, "File receive failed");
            return RECEIVE_ERROR;
        }

        size_t written = fwrite(chunk, 1, ret, file);
        if (written != ret) {
            free(chunk);
            fclose(file);
            unlink(filepath);  // Clean up partial file
            ESP_LOGE(TAG, "File write failed: %d bytes written, expected %d", written, ret);
            return WRITE_ERROR;
        }

        remaining -= ret;
        received += ret;

        ESP_LOGI(TAG, "Upload progress: %d%%", (int)((received * 100) / content_len));
    }

    free(chunk);
    fclose(file);

    // Verify file was created
    struct stat st;
    if (stat(filepath, &st) != 0) {
        ESP_LOGE(TAG, "Failed to verify file creation");
        return FILE_OPEN_ERROR;
    }

    ESP_LOGI(TAG, "File uploaded successfully: %s (size: %d bytes)", filepath, (int)st.st_size);
    return EXIT_SUCCESSFUL;
}
// IO_ERROR handle_file_upload(httpd_req_t *req, char* filename, size_t content_len) {
//     if (!sd_card_mounted) {
//         ESP_LOGE(TAG, "SD card not mounted");
//         return FILE_OPEN_ERROR;
//     }
//
//     if (content_len > MAX_FILE_SIZE) {
//         ESP_LOGE(TAG, "File too large: %d bytes", content_len);
//         return FILE_SIZE_ERROR;
//     }
//
//     // Create full path
//     char filepath[512];
//     snprintf(filepath, sizeof(filepath), "%s/%s", mount_point, filename);
//     ESP_LOGI(TAG, "Attempting to create file at: %s", filepath);
//
//     // Check if directory exists
//     char dir_path[512];
//     strncpy(dir_path, filepath, sizeof(dir_path));
//     char* last_slash = strrchr(dir_path, '/');
//     if (last_slash) {
//         *last_slash = '\0';
//         // Try to create directory if it doesn't exist
//         struct stat st;
//         if (stat(dir_path, &st) != 0) {
//             if (mkdir(dir_path, 0755) != 0) {
//                 ESP_LOGE(TAG, "Failed to create directory: %s", dir_path);
//                 return FILE_OPEN_ERROR;
//             }
//         }
//     }
//
//     // Open file for writing
//     FILE* file = fopen(filepath, "wb");
//     if (!file) {
//         ESP_LOGE(TAG, "Failed to create file: %s", filepath );
//         return FILE_OPEN_ERROR;
//     }
//
//     uint8_t *chunk = malloc(CHUNK_SIZE);
//     if (!chunk) {
//         fclose(file);
//         ESP_LOGE(TAG, "Failed to allocate memory for chunk");
//         return MEMORY_ERROR;
//     }
//
//     size_t remaining = content_len;
//     size_t received = 0;
//
//     while (remaining > 0) {
//         size_t chunk_size = (remaining < CHUNK_SIZE) ? remaining : CHUNK_SIZE;
//         int ret = httpd_req_recv(req, (char*)chunk, chunk_size);
//
//         if (ret <= 0) {
//             if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
//                 continue;  // Retry on timeout
//             }
//             free(chunk);
//             fclose(file);
//             unlink(filepath);  // Clean up partial file
//             ESP_LOGE(TAG, "File receive failed");
//             return RECEIVE_ERROR;
//         }
//
//         size_t written = fwrite(chunk, 1, ret, file);
//         if (written != ret) {
//             free(chunk);
//             fclose(file);
//             unlink(filepath);  // Clean up partial file
//             ESP_LOGE(TAG, "File write failed: %d bytes written, expected %d", written, ret);
//             return WRITE_ERROR;
//         }
//
//         remaining -= ret;
//         received += ret;
//
//         ESP_LOGI(TAG, "Upload progress: %d%%", (int)((received * 100) / content_len));
//     }
//
//     free(chunk);
//     fclose(file);
//
//     // Verify file was created
//     struct stat st;
//     if (stat(filepath, &st) != 0) {
//         ESP_LOGE(TAG, "Failed to verify file creation");
//         return FILE_OPEN_ERROR;
//     }
//
//     ESP_LOGI(TAG, "File uploaded successfully: %s (size: %d bytes)", filepath, (int)st.st_size);
//     return EXIT_SUCCESSFUL;
// }
