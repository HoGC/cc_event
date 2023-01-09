/*
 * @Author: HoGC
 * @Date: 2022-03-20 18:41:33
 * @Last Modified time: 2023-01-09 22:31:07
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "cc_event.h"

#define STR(str) #str

enum{
    WIFI_EVENT_CONNECT = 0,
    WIFI_EVENT_DISCONNECT,
};

enum{
    BIND_EVENT_SUCCESS = 0,
    BIND_EVENT_FAIL,
};

CC_EVENT_DEFINE_BASE(WIFI_EVENT);
CC_EVENT_DEFINE_BASE(BIND_EVENT);

static SemaphoreHandle_t s_semaphore = NULL;

void event_handler(cc_event_base_t base_event, cc_event_t event){
    if(event.data_len == 0){
        printf("base_event: %s   event_id: %d   event_data: %s\n", base_event, event.id, "NULL");
    }else{
        printf("base_event: %s   event_id: %d   event_data: %.*s\n", base_event, event.id, event.data_len, event.data);
    }
}

void __lock(void){
    if(s_semaphore){
        xSemaphoreTake(s_semaphore, portMAX_DELAY);
    }
}
void __unlock(void){
    if(s_semaphore){
        xSemaphoreGive(s_semaphore);
    }
}


void app_main(void)
{
    printf("cc_event example!\n");

    s_semaphore = xSemaphoreCreateMutex();

    cc_event_hooks event_hooks = {
        .malloc = malloc,
        .free = free,
        .lock = __lock,
        .unlock = __unlock
    };
    cc_event_set_hooks(event_hooks);

    cc_event_register_handler(WIFI_EVENT, event_handler);
    cc_event_register_handler(BIND_EVENT, event_handler);

    
    cc_event_post(WIFI_EVENT, WIFI_EVENT_CONNECT, STR(WIFI_EVENT_CONNECT), strlen(STR(WIFI_EVENT_CONNECT)));
    cc_event_post(BIND_EVENT, BIND_EVENT_SUCCESS, NULL, 0);

    cc_event_post(WIFI_EVENT, WIFI_EVENT_DISCONNECT, NULL, 0);
    cc_event_post(BIND_EVENT, BIND_EVENT_FAIL, NULL, 0);

    // 直接post，不依赖event_run，所以最先post
    cc_event_real_post(WIFI_EVENT, WIFI_EVENT_DISCONNECT, STR(WIFI_EVENT_DISCONNECT), strlen(STR(WIFI_EVENT_DISCONNECT)));  
    
    while(1){
        vTaskDelay(10 / portTICK_PERIOD_MS);
        cc_event_run();
    }
}