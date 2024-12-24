#include <stdio.h>

#include "freeRTOS/FreeRTOS.h"
#include "freeRTOS/task.h"

#include "esp_log.h"


void  printTask(){

    char* ourTaskName = pcTaskGetName(NULL) ;
    ESP_LOGI(ourTaskName , "Hello starting up!") ;
}


void app_main(void){
    printTask() ;
}
