/*
 * @Author: HoGC
 * @Date: 2022-03-20 18:41:37
 * @Last Modified time: 2022-03-20 18:41:37
 */

#include "cc_event.h"

#include <string.h>

typedef struct _evevt_ctx{
    struct _evevt_ctx *next;
    cc_event_base_t base_event;
    cc_event_t event;
}_evevt_ctx_t;

typedef struct _evevt_handler_ctx{
    struct _evevt_handler_ctx *next;
    cc_event_handler_t handler;
}_evevt_handler_ctx_t;

typedef struct _evevt_base_ctx{
    struct _evevt_base_ctx *next;
    cc_event_base_t base_event;
    _evevt_handler_ctx_t *handler_head;
}_evevt_base_ctx_t;

_evevt_base_ctx_t *s_base_ctx_head = NULL;
_evevt_ctx_t *s_event_ctx_head = NULL;

bool cc_event_register_handler(cc_event_base_t base_event, cc_event_handler_t handler){
    
    _evevt_base_ctx_t *base_node = NULL;
    _evevt_handler_ctx_t *handler_node = NULL;

    if(!s_base_ctx_head){
        base_node = (_evevt_base_ctx_t *)CC_EVENT_MALLOC(sizeof(_evevt_base_ctx_t));
        if (!base_node) {
            return false;
        }
        base_node->base_event = base_event;
        base_node->handler_head = NULL;

        s_base_ctx_head = base_node;
        base_node->next = NULL;
    }else{
        _evevt_base_ctx_t *base_node_p = NULL;
        for (base_node_p = s_base_ctx_head; base_node_p != NULL; base_node_p = base_node_p->next) {
            if (base_node_p->base_event == base_event) {
                base_node = base_node_p;
                break;
            }
        }

        if(!base_node){
            base_node = (_evevt_base_ctx_t *)CC_EVENT_MALLOC(sizeof(_evevt_base_ctx_t));
            if (!base_node) {
                return false;
            } 
            base_node->base_event = base_event;
            base_node->handler_head = NULL;

            base_node->next = s_base_ctx_head;
            s_base_ctx_head = base_node;
        }
    }

    handler_node = (_evevt_handler_ctx_t *)CC_EVENT_MALLOC(sizeof(_evevt_handler_ctx_t));
    if (!handler_node) {
        return false;
    }

    handler_node->handler = handler;

    if(!base_node->handler_head){
        base_node->handler_head = handler_node;
        handler_node->next = NULL;
    }else{
        handler_node->next = base_node->handler_head;
        base_node->handler_head = handler_node;
    }

    return true;
}

bool cc_event_unregister_handler(cc_event_base_t base_event, cc_event_handler_t handler){

    _evevt_base_ctx_t *base_node = NULL;

    if(!s_base_ctx_head){
        return false;
    }

    _evevt_base_ctx_t *base_node_p = NULL;
    for (base_node_p = s_base_ctx_head; base_node_p != NULL; base_node_p = base_node_p->next) {
        if (base_node_p->base_event == base_event) {
            base_node = base_node_p;
            break;
        }
    }

    if(!base_node){
        return false;
    }

    _evevt_handler_ctx_t *handler_node_p = NULL;
    _evevt_handler_ctx_t *handler_node_unregister_p = NULL;

    handler_node_p = base_node->handler_head;
    if(handler_node_p){
        if(handler_node_p->handler == handler){
            handler_node_unregister_p = handler_node_p;
            if(handler_node_p->next){
                base_node->handler_head = handler_node_p->next;
            }else{
                base_node->handler_head = NULL;
            }
        }else{
            while(handler_node_p->next){
                if(handler_node_p->next->handler == handler){
                    handler_node_unregister_p = handler_node_p->next;
                    if(handler_node_p->next->next){
                        handler_node_p->next = handler_node_p->next->next;
                    }else{
                        handler_node_p->next = NULL;
                    }
                }
            }
        }
    }

    if(!handler_node_unregister_p){
        return false;
    }
    
    CC_EVENT_FREE(handler_node_unregister_p);

    return true;
}

bool cc_event_post(cc_event_base_t base_event, int32_t event_id, void* event_data, size_t event_data_len){

    _evevt_ctx_t *event_node = NULL;

    if(!s_base_ctx_head){
        return false;
    }

    event_node = (_evevt_ctx_t *)CC_EVENT_MALLOC(sizeof(_evevt_ctx_t));
    if (!event_node) {
        return false;
    } 

    event_node->base_event = base_event;
    event_node->event.id = event_id;
    
    if(event_data_len > 0){
        event_node->event.data = CC_EVENT_MALLOC(event_data_len);
        if (!event_node->event.data) {
            CC_EVENT_FREE(event_node);
            return false;
        } 
        memcpy(event_node->event.data, event_data, event_data_len);
        event_node->event.data_len = event_data_len;
    }else{
        event_node->event.data = NULL;
        event_node->event.data_len = 0;
    }

    if(!s_event_ctx_head){
        s_event_ctx_head = event_node;
        event_node->next = NULL;
    }else{
        event_node->next = s_event_ctx_head;
        s_event_ctx_head = event_node;
    }

    return true;
}

bool cc_event_real_post(cc_event_base_t base_event, int32_t event_id, void* event_data, size_t event_data_len){

    bool ret = false;
    cc_event_t post_event = {0};

    if(!s_base_ctx_head){
        return false;
    }

    post_event.id = event_id;
    post_event.data = event_data;
    post_event.data_len = event_data_len;

    if(s_base_ctx_head){
        _evevt_base_ctx_t *base_node_p = NULL;
        for (base_node_p = s_base_ctx_head; base_node_p != NULL; base_node_p = base_node_p->next) {
            if (base_node_p->base_event == base_event) {
                _evevt_handler_ctx_t *handler_node_p = NULL;
                for (handler_node_p = base_node_p->handler_head; handler_node_p != NULL; handler_node_p = handler_node_p->next) {
                    if(handler_node_p->handler){
                        handler_node_p->handler(base_node_p->base_event, post_event);
                        ret = true;
                    }   
                }
            }
        }
    }

    return ret;
}

void cc_event_run(void){

    _evevt_ctx_t *event_node_p = NULL;
    _evevt_ctx_t *event_node_post = NULL;

    while(s_event_ctx_head){
        event_node_p = s_event_ctx_head;
        if(event_node_p->next){
            while(event_node_p->next->next){
                event_node_p = event_node_p->next;
            }
            event_node_post = event_node_p->next;
            event_node_p->next = NULL;
        }else{
            event_node_post = event_node_p;
            s_event_ctx_head = NULL;
        }

        if(s_base_ctx_head){
            _evevt_base_ctx_t *base_node_p = NULL;
            for (base_node_p = s_base_ctx_head; base_node_p != NULL; base_node_p = base_node_p->next) {
                if(base_node_p->base_event == event_node_post->base_event){
                    _evevt_handler_ctx_t *handler_node_p = NULL;
                    for (handler_node_p = base_node_p->handler_head; handler_node_p != NULL; handler_node_p = handler_node_p->next) {
                        if(handler_node_p->handler){
                            handler_node_p->handler(base_node_p->base_event, event_node_post->event);
                        }
                    }
                }
            
            }
        }

        CC_EVENT_FREE(event_node_post->event.data);
        CC_EVENT_FREE(event_node_post);
    }
}