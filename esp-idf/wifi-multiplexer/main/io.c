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
#include "cJSON.h"

// Define pins for SPI
#define PIN_NUM_MISO  19
#define PIN_NUM_MOSI  23
#define PIN_NUM_CLK   18
#define PIN_NUM_CS    5
#define FILE_WRITE_TAG "Writing"
#define FILE_TYPES 5 // Number of different file types




typedef enum IO_ERROR{
    EXIT_SUCCESSFUL       , // Successful Event
    SPECIFICATION_FAILURE , // Failed to specify file type
    SIZE_BOUND            , // Exceeded file size
    METADATA_ERROR        , // Metadata error
}IO_ERROR;


// File type definitions


typedef enum FILE_TYPE {
    JPEG                  ,
    TXT                   ,
    PNG                   ,
    MP4                   ,
    PDF                   ,

}FILE_TYPE;

typedef struct {
  FILE_TYPE        extension     ;
  char*            name          ;
  char*            path          ;
  double           created_on    ;
  double           updated_on    ;
}FileMetaData;


IO_ERROR jpeg_handler( FileMetaData* file_details  ) ;
IO_ERROR txt_handler(  FileMetaData* file_details  ) ;
IO_ERROR png_handler(  FileMetaData* file_details  ) ;
IO_ERROR mp4_handler(  FileMetaData* file_details  ) ;
IO_ERROR pdf_handler(  FileMetaData* file_details  ) ;

typedef IO_ERROR ( *callback_arr )( FileMetaData* );

static callback_arr read_callback_array[FILE_TYPES] = {
    &jpeg_handler ,
    &txt_handler  ,
    &png_handler  ,
    &mp4_handler  ,
    &pdf_handler  ,
};

void writeFile(FileMetaData* file_details )
{
    // Check if file_details-> is non null , if it is -> save to a root for now just throw an error
    if(file_details == NULL){
        ESP_LOGE("SD", "Failed to write due to unspecified file_path") ;
    }
    ESP_LOGI("SD"  , "Attempting to write to %s" , file_details->name);
    FILE* file = fopen( file_details->path , "w") ;
    if ( file == NULL ) {
        ESP_LOGE("SD", "Failed to open %s .txt for writing, null ", file_details->name);
    } else {
        // Get file type
        fprintf( file , "SD card is Working\n");
        fprintf( file , "SD card is Working\n");
        fprintf( file , "Testing , Testing , 1 , 2 , 3\n");
        fclose( file );
        ESP_LOGI("SD", "Successfully written to test.txt.");
    }
}


// Is this still needed ?

void readFile( FileMetaData* file_details )
{
    if( file_details == NULL )
    {
        ESP_LOGE("SD" , "Failed to access file metadata") ;
    }

    if( file_details->extension == TXT)
    {
        ESP_LOGI("SD" , "ATTEMPTING FILE TYPE : %d" , ( int ) file_details->extension) ;
        read_callback_array[file_details->extension](file_details) ;
    }else{
        ESP_LOGE("SD" , "Unknown filetype : %d " , ( int ) file_details->extension );
    }
}


IO_ERROR jpeg_handler( FileMetaData* file_details )
{
    ESP_LOGI("SD" , "Attempting to write to a jpg file" ) ;
    if( file_details == NULL )
    {
        return METADATA_ERROR ;
    }
    return EXIT_SUCCESSFUL ;
}

IO_ERROR txt_handler( FileMetaData* file_details )
{
    ESP_LOGE("SD" , "Attempting to write to a txt file" ) ;
    if( file_details == NULL )
    {
        return METADATA_ERROR ;
    }
    FILE* file = fopen( file_details->path , "r");
    if (file == NULL) {
        ESP_LOGE("SD", "Failed to open test.txt for reading.");
    } else {
        ESP_LOGI("SD", "Reading from test.txt...");
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), file )) {
            printf("%s", buffer);
        }
        fclose( file );
        ESP_LOGI("SD", "File read successfully. Length of file " );
    }
    return EXIT_SUCCESSFUL ;
}

IO_ERROR png_handler( FileMetaData* file_details )
{
    ESP_LOGI("SD" , "Attempting to write to a png file" ) ;
    if( file_details == NULL )
    {
        return METADATA_ERROR ;
    }
    return EXIT_SUCCESSFUL ;
}

IO_ERROR mp4_handler( FileMetaData* file_details )
{
    ESP_LOGI("SD" , "Attempting to write to a mp4 file" ) ;
    if( file_details == NULL )
    {
        return METADATA_ERROR ;
    }
    return EXIT_SUCCESSFUL ;
}

IO_ERROR pdf_handler( FileMetaData* file_details )
{
    ESP_LOGI("SD" , "Attempting to write to a pdf file" ) ;
    if( file_details == NULL )
    {
        return METADATA_ERROR ;
    }
    return EXIT_SUCCESSFUL ;
}

void init_sd_card()
{
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
    // FILE* test_file = fopen("/sdcard/test.txt", "w");

}
