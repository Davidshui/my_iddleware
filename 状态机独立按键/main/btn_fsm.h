#ifndef BTN_FSM_H
#define BTN_FSM_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define BTN_LONG_PRESS_MAX_TIME_MS 500
#define BTN_DOUBLE_CLICK_MAX_TIME_MS 500

    typedef enum _btn_status_t
    {
        BTN_UP = 0,       // 传入的状态，松开的时候
        BTN_SURE,         // 派生的状态，松开到按下的时候，用于消抖
        BTN_DOWN,         // 传入的状态，按下的时候
        BTN_LONG_PRESS,   // 派生的状态，在按下超过时间的时候进入该状态
        BTN_DOUBLE_CLICK, // 派生的状态，在松开小于一定时间内进入该状态
    } btn_status_t;

    typedef enum _btn_event_t
    {
        BTN_EVENT_NONE = 0,
        BTN_EVENT_PRESS,   // 按下事件
        BTN_EVENT_RELEASE, // 松开事件
        BTN_EVENT_LONG,    // 长按事件
        BTN_EVENT_CLICK,   // 点击事件，相对长按
        BTN_EVENT_SIGNE,   // 单击事件，相对双击
        BTN_EVENT_DOUBLE,  // 双击事件
    } btn_event_t;

    typedef bool (*btn_get_level_t)(void);
    typedef void (*btn_event_handle_t)(btn_event_t event);

    typedef struct _btn_fsm_t
    {
        uint32_t time_ms;
        uint32_t long_press_count;
        uint32_t double_click_count;
        btn_get_level_t get_level;
        btn_status_t status;
        btn_event_t event;
        btn_event_handle_t event_handle;
    } btn_fsm_t;

    btn_fsm_t *btn_fsm_create(btn_get_level_t get_level, uint32_t time_ms);
    void btn_fsm_delete(btn_fsm_t *self);
    void btn_fsm_handle(btn_fsm_t *self);
    btn_event_t btn_fsm_get_event(btn_fsm_t *self);
    void btn_fsm_set_event_cb(btn_fsm_t *self, btn_event_handle_t event_handle);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // BTN_FSM_H