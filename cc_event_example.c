/*
 * @Author: HoGC
 * @Date: 2022-03-20 18:41:33
 * @Last Modified time: 2022-03-20 18:41:33
 */

#include <stdio.h>

#include "cc_event.h"

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

void event_handler(cc_event_base_t base_event, cc_event_t event){
    printf("base_event: %s   event_id: %d\n", base_event, event.id);
}

void app_main(void)
{
    printf("cc_event example!\n");

    cc_event_register_handler(WIFI_EVENT, event_handler);
    cc_event_register_handler(BIND_EVENT, event_handler);
    
    cc_event_post(WIFI_EVENT, WIFI_EVENT_CONNECT, NULL, 0);
    cc_event_post(BIND_EVENT, BIND_EVENT_SUCCESS, NULL, 0);

    cc_event_post(WIFI_EVENT, WIFI_EVENT_DISCONNECT, NULL, 0);
    cc_event_post(BIND_EVENT, BIND_EVENT_FAIL, NULL, 0);

    // 直接post，不依赖event_run，所以最先post
    cc_event_real_post(WIFI_EVENT, WIFI_EVENT_DISCONNECT, NULL, 0);  
    
    while(1){
        cc_event_run();
    }
}   
