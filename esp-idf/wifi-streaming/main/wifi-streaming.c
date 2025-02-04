
/** INCLUDES **/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "esp_spiffs.h"
#include "nvs_flash.h"
#include <sys/stat.h>
#include <dirent.h>
#include "esp_spiffs.h"

/** DEFINES **/
#define WIFI_SUCCESS (1 << 0)
#define WIFI_FAILURE (1 << 1)
#define TCP_SUCCESS  (1 << 0)
#define TCP_FAILURE  (1 << 1)
#define MAX_FAILURES 10
#define PORT 12345
#define BUFFER_SIZE 1024

/** GLOBALS **/
static EventGroupHandle_t wifi_event_group;
static int s_retry_num = 0;
static const char *TAG = "ESP32_SERVER";

/** WiFi Event Handlers **/
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT) {
        if (event_id == WIFI_EVENT_STA_START) {
            ESP_LOGI(TAG, "Connecting to AP...");
            esp_wifi_connect();
        } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
            if (s_retry_num < MAX_FAILURES) {
                ESP_LOGI(TAG, "Reconnecting to AP...");
                esp_wifi_connect();
                s_retry_num++;
            } else {
                xEventGroupSetBits(wifi_event_group, WIFI_FAILURE);
            }
        }
    }
}

/** IP Event Handlers **/
static void ip_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "STA IP: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(wifi_event_group, WIFI_SUCCESS);
    }
}

/*Function to list files in SPIFFS, change this for SD card*/ 
void list_files(int sock) {
    struct dirent *entry;
    DIR *dir = opendir("/spiffs");

    if (!dir) {
        ESP_LOGE(TAG, "Failed to open directory.");
        return;
    }

    char file_list[512] = "FILES:\n";

    while ((entry = readdir(dir)) != NULL) {
        strcat(file_list, entry->d_name);
        strcat(file_list, "\n");
    }
    closedir(dir);

    // Send file list to client
    write(sock, file_list, strlen(file_list));
}



/** Initialize SPIFFS, change this for SD card **/
void init_spiffs() {
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPIFFS initialization failed!");
    } else {
        ESP_LOGI(TAG, "SPIFFS mounted successfully.");
    }
}

/*Check if there is any space left before adding more files, change this for SD card*/
void check_spiffs_info() {
    size_t total, used;
    esp_err_t ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE("SPIFFS", "Failed to get SPIFFS partition info.");
        return;
    }
    ESP_LOGI("SPIFFS", "Total SPIFFS space: %d bytes, Used: %d bytes", total, used);
}


/** WiFi Initialization **/
esp_err_t connect_wifi() {
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_event_handler, NULL, NULL));
    
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "Your SSID Here",
            .password = "PASSCODE",
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    ESP_LOGI(TAG, "STA initialization complete");
    
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group, WIFI_SUCCESS | WIFI_FAILURE, pdFALSE, pdFALSE, portMAX_DELAY);
    
    if (bits & WIFI_SUCCESS) {
        ESP_LOGI(TAG, "Connected to AP");
        return ESP_OK;
    } else {
        ESP_LOGI(TAG, "Failed to connect to AP");
        return ESP_FAIL;
    }
}

/** Save file from client **/
void save_file(int sock) {
    char filename[50];
    char buffer[1024];
    int length;

    // Receive filename from client
    int len = read(sock, filename, sizeof(filename) - 1);
    if (len <= 0) {
        ESP_LOGE(TAG, "Failed to receive filename.");
        return;
    }
    filename[len] = '\0';

    // Send acknowledgment to the client before receiving the file data
    write(sock, "OK", 2);

    // Prepend SPIFFS path, fix this for SD card
    char filepath[60];
    snprintf(filepath, sizeof(filepath), "/spiffs/%s", filename);

    FILE *file = fopen(filepath, "wb");
    if (!file) {
        ESP_LOGE(TAG, "Failed to open file: %s", filepath);
        return;
    }

    // Receive file data
    while ((length = read(sock, buffer, sizeof(buffer))) > 0) {
        fwrite(buffer, 1, length, file);
    }

    fclose(file);
    ESP_LOGI(TAG, "File '%s' received and saved to SPIFFS.", filepath);
}


/** Send file to client **/
void retrieve_file(int sock) {
    char filename[50];
    char filepath[60];
    char buffer[BUFFER_SIZE];
    FILE *file;
    int len;

    // Receive filename
    len = read(sock, filename, sizeof(filename) - 1);
    if (len <= 0) {
        ESP_LOGE("ESP32_SERVER", "Failed to receive filename.");
        return;
    }
    filename[len] = '\0';

    // Construct full file path
    snprintf(filepath, sizeof(filepath), "/spiffs/%s", filename);

    // Open file for reading
    file = fopen(filepath, "rb");
    if (!file) {
        ESP_LOGE("ESP32_SERVER", "File %s not found.", filepath);
        write(sock, "FILE_NOT_FOUND", strlen("FILE_NOT_FOUND"));
        return;
    }

    // Send file data
    ESP_LOGI("ESP32_SERVER", "Sending file data...");
    while ((len = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        write(sock, buffer, len);
    }

    fclose(file);
    ESP_LOGI("ESP32_SERVER", "File '%s' sent successfully.", filename);
}



/*remove file from server*/
void delete_file(int sock) {
    char filename[50];
    char filepath[60];

    // Receive filename from client
    int len = read(sock, filename, sizeof(filename) - 1);
    if (len <= 0) {
        ESP_LOGE(TAG, "Failed to receive filename.");
        return;
    }
    filename[len] = '\0';

    // Construct full SPIFFS file path,fix this for SD card
    snprintf(filepath, sizeof(filepath), "/spiffs/%s", filename);

    // Check if file exists before attempting to delete
    struct stat st;
    if (stat(filepath, &st) != 0) {
        ESP_LOGE(TAG, "File %s not found.", filepath);
        write(sock, "FILE_NOT_FOUND\n", strlen("FILE_NOT_FOUND\n"));
        return;
    }

    // Delete the file
    if (unlink(filepath) == 0) {
        ESP_LOGI(TAG, "File '%s' deleted successfully.", filepath);
        write(sock, "FILE_DELETED\n", strlen("FILE_DELETED\n"));
    } else {
        ESP_LOGE(TAG, "Failed to delete file %s. Possible permission issue.", filepath);
        write(sock, "DELETE_FAILED\n", strlen("DELETE_FAILED\n"));
    }
}



/** Handle client commands */
void handle_client(int client_sock) {
    char command[20];

    while (1) {
        memset(command, 0, sizeof(command));
        int len = recv(client_sock, command, sizeof(command) - 1, 0);
        if (len <= 0) {
            ESP_LOGI(TAG, "Client disconnected.");
            break;
        }

        command[len] = '\0';
        ESP_LOGI(TAG, "Received command: %s", command);

        // Use strncmp to match commands even if '\n' is received
       if (strcmp(command, "UPLOAD") == 0) {
            save_file(client_sock);
        }
        else if (strcmp(command, "DOWNLOAD") == 0) {
            retrieve_file(client_sock);
        }
        else if (strcmp(command, "LIST") == 0) {
            list_files(client_sock);
        }
        else if (strcmp(command, "DELETE") == 0) { 
            delete_file(client_sock);
        }
        else if (strcmp(command, "EXIT") == 0) {
            ESP_LOGI("SERVER", "Client disconnected.");
            break;
        }
    }

    close(client_sock);
}

/** Start TCP Server */
void start_tcp_server() {
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        ESP_LOGE(TAG, "Socket creation failed");
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        ESP_LOGE(TAG, "Socket bind failed");
        close(server_sock);
        return;
    }

    if (listen(server_sock, 3) < 0) {
        ESP_LOGE(TAG, "Socket listen failed");
        close(server_sock);
        return;
    }

    ESP_LOGI(TAG, "Server listening on port %d...", PORT);

    while (1) {
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        if (client_sock < 0) {
            ESP_LOGE(TAG, "Failed to accept client connection");
            continue;
        }
        ESP_LOGI(TAG, "Client connected");

        // Handle client properly
        handle_client(client_sock);
    }
}

/** Main Application */
void app_main() {
    
    // Initialize NVS
    ESP_LOGI(TAG, "Initializing NVS...");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition appears corrupted, erasing and reinitializing...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "NVS initialized successfully.");

    // Initialize SPIFFS
    ESP_LOGI(TAG, "Initializing SPIFFS...");
    init_spiffs();

    // Connect to WiFi
    ESP_LOGI(TAG, "Connecting to WiFi...");
    if (connect_wifi() == ESP_OK) {
        ESP_LOGI(TAG, "WiFi connected successfully.");
        start_tcp_server();
    } 

    else {
        ESP_LOGE(TAG, "WiFi connection failed, exiting...");
    }
}
