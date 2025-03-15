#include <stdio.h>
#include <string.h>
#include <time.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"

// Wi-Fi credentials
#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "788"

// SoftAP credentials
#define AP_SSID "ESP32_AP"
#define AP_PASSWORD "12345678"

// Server Port, File write location, and buffer size for read and write
#define PORT 5000
#define MOUNT_POINT "/sdcard"
#define BUFFER_SIZE 1024  // Size of each buffer
#define MAX_FILE_SIZE 4096  // Maximum write size
// Configure these based on your Freenove board's schematic!
#define SD_CLK_PIN   GPIO_NUM_14   // CLK pin (adjust for your board)
#define SD_CMD_PIN   GPIO_NUM_15   // CMD pin
#define SD_D0_PIN    GPIO_NUM_2    // D0 pin (data line 0)
// For 4-bit mode, also define D1-D3:
#define SD_D1_PIN    GPIO_NUM_4
#define SD_D2_PIN    GPIO_NUM_12
#define SD_D3_PIN    GPIO_NUM_13


static uint8_t read_buffer[BUFFER_SIZE];  // Buffer for reading incoming data
static uint8_t write_buffer[MAX_FILE_SIZE];  // Buffer for storing file data
static size_t write_buffer_index = 0;  // Current position in the write buffer

static const char *TAG = "STREAMING";

// Function to initialize Wi-Fi in Station mode

static void wifi_init_sta() {
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Wi-Fi Station initialized. Connecting to %s...", WIFI_SSID);

    // Wait for Wi-Fi connection
    int retry_count = 0;
    while (1) {
        if (esp_wifi_connect() == ESP_OK) {
            ESP_LOGI(TAG, "Attempting to connect to Wi-Fi...");
            break;
        }
        // stall before trying to connect again
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        retry_count++;
        if (retry_count > 10) {
            ESP_LOGE(TAG, "Failed to connect to Wi-Fi");
            return;
        }
    }

    // Wait for IP address
    esp_netif_ip_info_t ip_info;
    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    while (1) {
        if (esp_netif_get_ip_info(netif, &ip_info) == ESP_OK) {
            if (ip_info.ip.addr != 0) {
                ESP_LOGI(TAG, "Connected to Wi-Fi");
                ESP_LOGI(TAG, "IP Address: " IPSTR, IP2STR(&ip_info.ip));
                break;
            }
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}


// Function to initialize Wi-Fi in SoftAP mode
static void wifi_init_softap() {
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = AP_SSID,
            .password = AP_PASSWORD,
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Wi-Fi SoftAP initialized. SSID: %s, Password: %s", AP_SSID, AP_PASSWORD);
}

static esp_err_t init_sd_card() {
    esp_err_t ret;

    // SDMMC host configuration
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    // Follow schematics for sdmmc, use 1-bit mode if wroom
    // SDMMC slot configuration (GPIO pins)
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.clk = SD_CLK_PIN;
    slot_config.cmd = SD_CMD_PIN;
    slot_config.d0 = SD_D0_PIN;
    slot_config.d1 = SD_D1_PIN;  // Only needed for 4-bit mode
    slot_config.d2 = SD_D2_PIN;  // Only needed for 4-bit mode
    slot_config.d3 = SD_D3_PIN;  // Only needed for 4-bit mode
    slot_config.width = 1;       // Use 1 bit mode if you have wroom esp32 4 bit only supported wrover
    // Mount the filesystem
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 10,
        .allocation_unit_size = 0
    };

    sdmmc_card_t *card;
    ret = esp_vfs_fat_sdmmc_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG,"Failed to mount filesystem. Make sure the SD card is formatted with FAT.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the SD card (%s).", esp_err_to_name(ret));
        }
        return ret;
    }

    ESP_LOGI(TAG, "SD card initialized and mounted at %s", MOUNT_POINT);
    return ESP_OK;
}

static void tcp_server_stream(void *pvParameters) {
    int addr_family = AF_INET;
    int ip_protocol = IPPROTO_IP;

    // Initialize the SD card once at task start
    if (init_sd_card() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SD card");
        vTaskDelete(NULL);
    }

    // Create a socket
    int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (listen_sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
    }

    // Bind the socket
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listen_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        ESP_LOGE(TAG, "Socket bind failed: errno %d", errno);
        close(listen_sock);
        vTaskDelete(NULL);
    }

    // Listen for incoming connections
    if (listen(listen_sock, 5) < 0) {
        ESP_LOGE(TAG, "Socket listen failed: errno %d", errno);
        close(listen_sock);
        vTaskDelete(NULL);
    }
    ESP_LOGI(TAG, "TCP server listening on port %d", PORT);

    while (1) {
        // Accept incoming connection
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_sock = accept(listen_sock, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_sock < 0) {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            continue;
        }
        ESP_LOGI(TAG, "New client connected");

        // Set a timeout for recv to prevent hanging
        struct timeval timeout;
        timeout.tv_sec = 5;  // 5-second timeout
        timeout.tv_usec = 0;
        setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        // Receive the filename from the client
        char filename[248];
        int len = recv(client_sock, filename, sizeof(filename) - 1, 0);
        if (len < 0) {
            ESP_LOGE(TAG, "Failed to receive filename: errno %d", errno);
            close(client_sock);
            continue;
        }
        filename[len] = '\0'; // End of Filename
        ESP_LOGI(TAG, "Requested file: %s", filename);

        // Open the requested file from the SD card
        char filepath[256]; // Adjust the buffer size as needed
        snprintf(filepath, sizeof(filepath), "/sdcard/%s", filename); // Assuming files are stored in /sdcard/
        FILE *file = fopen(filepath, "rb");
        if (!file) {
            ESP_LOGE(TAG, "Failed to open file: %s", filepath);
            const char *error_msg = "File not found";
            send(client_sock, error_msg, strlen(error_msg), 0);
            close(client_sock);
            continue;
        }

        // Send the file data to the client
        char read_buffer[BUFFER_SIZE];
        while (1) {
            int len = fread(read_buffer, 1, BUFFER_SIZE, file);
            if (len < 0) {
                ESP_LOGE(TAG, "Read from SD card failed");
                break;
            } else if (len == 0) {
                ESP_LOGI(TAG, "End of file reached");
                break;
            } else {
                int sent = send(client_sock, read_buffer, len, 0);
                if (sent < 0) {
                    if (errno == EWOULDBLOCK || errno == EAGAIN) {
                        ESP_LOGW(TAG, "Send timeout");
                        break;
                    } else {
                        ESP_LOGE(TAG, "Send failed: errno %d", errno);
                        break;
                    }
                } else if (sent == 0) {
                    ESP_LOGI(TAG, "Client disconnected");
                    break;
                } else if (sent < len) {
                    ESP_LOGW(TAG, "Partial send, %d bytes sent out of %d", sent, len);
                }
            }
        }

        // Send confirmation to client
        const char *response = "File sent successfully";
        send(client_sock, response, strlen(response), 0);

        // Clean up
        fclose(file);
        close(client_sock);
        ESP_LOGI(TAG, "Client connection closed");
    }
}

void app_main(void) {
    // standard initalization for esp32 flash storage, wifi init, etc
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize Wi-Fi
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Start Wi-Fi in Station mode although ap mode also works, simply using sta atm so i don't have to disconnected
    // and reconnect to wifi over and over.
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    wifi_init_sta();
    //wifi_init_softap();

    // Start the TCP server task
    // xTaskCreate(tcp_server_write, "tcp_server", 4096, NULL, 5, NULL);
    xTaskCreate(tcp_server_stream, "tcp_server", 4096, NULL, 5, NULL);
}
