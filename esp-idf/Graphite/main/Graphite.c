#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"

#define TAG "Timer"

void callback(void* arg){
    static int counter ;
    if(counter <= 30){
        ESP_LOGI(TAG , "Logged : %d" , ++counter) ;
    }
    else{
        ESP_LOGI(TAG , "Timer Concluded") ;
        esp_timer_stop(*(esp_timer_handle_t*)arg) ;
    }
}

void app_main(){

    esp_timer_handle_t  timer ;
    esp_timer_create_args_t timer_args = {
        .callback = &callback,
        .arg = &timer,
        .name = "Timer"
    };

    esp_timer_create(&timer_args , &timer) ;
    esp_timer_start_periodic(timer , 1000000) ;
}
