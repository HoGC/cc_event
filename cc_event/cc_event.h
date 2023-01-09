/*
 * @Author: HoGC
 * @Date: 2022-03-20 18:41:41
 * @Last Modified time: 2023-01-09 22:30:44
 */

#ifndef __CC_EVENT_H__
#define __CC_EVENT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define CC_EVENT_DECLARE_BASE(base) extern cc_event_base_t base
#define CC_EVENT_DEFINE_BASE(base) cc_event_base_t base = #base

typedef struct {
    int32_t id;
    void* data;
    size_t data_len;
}cc_event_t;

typedef struct {
    void *(*malloc)(size_t size);
    void (*free)(void *ptr);
    void (*lock)(void);
    void (*unlock)(void);
}cc_event_hooks;

typedef const char* cc_event_base_t;

typedef void (*cc_event_handler_t)(cc_event_base_t base_event, cc_event_t event);

bool cc_event_register_handler(cc_event_base_t base_event, cc_event_handler_t handler);
bool cc_event_unregister_handler(cc_event_base_t base_event, cc_event_handler_t handler);

bool cc_event_post(cc_event_base_t base_event, int32_t event_id, void* event_data, size_t event_data_len);
bool cc_event_real_post(cc_event_base_t base_event, int32_t event_id, void* event_data, size_t event_data_len);

bool cc_event_set_hooks(cc_event_hooks hooks);

void cc_event_run(void);

#ifdef __cplusplus
}
#endif

#endif  //__CC_EVENT_H__