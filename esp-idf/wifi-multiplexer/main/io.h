#ifndef DEBUG
#define IO_H

#include "esp_http_server.h"
#include "esp_err.h"

#define MAX_FILENAME_LENGTH 256
#define MAX_PATH_LENGTH 512

// File types enum
typedef enum {
    FILE_TYPE_JPEG = 0,
    FILE_TYPE_PNG,
    FILE_TYPE_TXT,
    FILE_TYPE_PDF,
    FILE_TYPE_MP4,
    FILE_TYPE_UNKNOWN
} FileType;

// Error codes
typedef enum {
    EXIT_SUCCESSFUL = 0,
    METADATA_ERROR,
    FILE_OPEN_ERROR,
    FILE_SIZE_ERROR,
    MEMORY_ERROR,
    RECEIVE_ERROR,
    WRITE_ERROR,
    READ_ERROR,
    JSON_PARSE_ERROR
} IO_ERROR;

// File metadata structure
typedef struct {
    char filename[MAX_FILENAME_LENGTH];
    size_t filesize;
    FileType type;
    char path[MAX_PATH_LENGTH];
} FileMetadata;

// Function declarations
IO_ERROR writeFile(const char* path, const uint8_t* data, size_t len, FileType type);
IO_ERROR readFile(const char* path, uint8_t** data, size_t* len);
IO_ERROR handle_file_upload(httpd_req_t *req, char* filename, size_t content_len);
IO_ERROR parse_file_metadata(const char* json_str, FileMetadata* metadata);
esp_err_t init_sd_card(void);

#endif // IO_H
