# cc_event
## 使用C编写的嵌入式事件通知库    
* 支持事件分组
* 支持自定义参数
* 支持多线程
* 方便移植

### 例程
```
cc_event example!
base_event: WIFI_EVENT   event_id: 1   event_data: WIFI_EVENT_DISCONNECT
base_event: WIFI_EVENT   event_id: 0   event_data: WIFI_EVENT_CONNECT
base_event: BIND_EVENT   event_id: 0   event_data: NULL
base_event: WIFI_EVENT   event_id: 1   event_data: NULL
base_event: BIND_EVENT   event_id: 1   event_data: NULL
```
```
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

void event_handler(cc_event_base_t base_event, cc_event_t event){
    if(event.data_len == 0){
        printf("base_event: %s   event_id: %d   event_data: %s\n", base_event, event.id, "NULL");
    }else{
        printf("base_event: %s   event_id: %d   event_data: %.*s\n", base_event, event.id, event.data_len, event.data);
    }
}

void __lock(void){
    //lock();
}
void __unlock(void){
    //unlock();
}

void app_main(void)
{
    printf("cc_event example!\n");

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
        cc_event_run();
    }
}
```
