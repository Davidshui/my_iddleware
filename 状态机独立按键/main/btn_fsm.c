#include "btn_fsm.h"

btn_fsm_t *btn_fsm_create(btn_get_level_t get_level, uint32_t time_ms)
{
    btn_fsm_t *btn_fsm = (btn_fsm_t *)malloc(sizeof(btn_fsm_t));

    if (btn_fsm == NULL)
    {
        return NULL;
    }

    // TODO: 初始化参数
    btn_fsm->get_level = get_level;
    btn_fsm->status = BTN_UP;
    btn_fsm->event = BTN_EVENT_NONE;
    btn_fsm->event_handle = NULL;
    btn_fsm->time_ms = time_ms;
    btn_fsm->long_press_count = 0;
    // 设计单击初始化最高，防止跑起来会有单击
    btn_fsm->double_click_count = BTN_DOUBLE_CLICK_MAX_TIME_MS + 1;

    return btn_fsm;
}

void btn_fsm_delete(btn_fsm_t *self)
{
    free(self);
}

void btn_fsm_handle(btn_fsm_t *self)
{
    switch (self->status)
    {
    case BTN_UP:
        if (self->get_level() == true)
        {
            self->status = BTN_SURE;
            self->event = BTN_EVENT_NONE;
            // self->double_click_count = 0; // 视频里的这地方讲的有问题
        }
        else
        {
            if (self->event == BTN_EVENT_CLICK)
            {
                self->event = BTN_EVENT_RELEASE;
                if (self->event_handle != NULL)
                {
                    self->event_handle(BTN_EVENT_RELEASE);
                }
            }
            else
            {
                self->event = BTN_EVENT_NONE;
            }

            self->status = BTN_UP;
            if (self->double_click_count * self->time_ms < BTN_DOUBLE_CLICK_MAX_TIME_MS)
            {
                self->double_click_count++; // 这里如果长按超过，则会发出单击事件
            }
            else if (self->double_click_count * self->time_ms == BTN_DOUBLE_CLICK_MAX_TIME_MS)
            {
                self->double_click_count++;
                self->event = BTN_EVENT_SIGNE;
                self->event_handle(BTN_EVENT_SIGNE);
            }
        }
        break;
    case BTN_SURE:
        if (self->get_level() == true)
        {
            if (self->double_click_count * self->time_ms < BTN_DOUBLE_CLICK_MAX_TIME_MS)
            {
                self->status = BTN_DOUBLE_CLICK;
                self->event = BTN_EVENT_DOUBLE;
                if (self->event_handle != NULL)
                {
                    self->event_handle(BTN_EVENT_DOUBLE);
                }
            }
            else
            {
                self->double_click_count = 0;
                self->status = BTN_DOWN;
                self->event = BTN_EVENT_PRESS;
                if (self->event_handle != NULL)
                {
                    self->event_handle(BTN_EVENT_PRESS);
                }
            }
        }
        else
        {
            self->status = BTN_UP;
            self->event = BTN_EVENT_NONE;
        }
        break;

    case BTN_DOUBLE_CLICK:
        if (self->get_level() == true)
        {
            self->status = BTN_DOUBLE_CLICK;
            self->event = BTN_EVENT_NONE;
        }
        else
        {
            // 双击事件后，不会触发单击事件
            self->double_click_count = BTN_DOUBLE_CLICK_MAX_TIME_MS + 1;
            self->status = BTN_UP;
            self->event = BTN_EVENT_RELEASE;
            if (self->event_handle != NULL)
            {
                self->event_handle(BTN_EVENT_RELEASE);
            }
        }

        break;

    case BTN_DOWN:
        self->long_press_count++;
        if (self->get_level() == true)
        {
            if (self->long_press_count * self->time_ms > BTN_LONG_PRESS_MAX_TIME_MS)
            {
                self->long_press_count = 0;
                self->status = BTN_LONG_PRESS;
                self->event = BTN_EVENT_LONG;
                if (self->event_handle != NULL)
                {
                    self->event_handle(BTN_EVENT_LONG);
                }
            }
            else
            {
                self->status = BTN_DOWN;
                self->event = BTN_EVENT_NONE;
            }
        }
        else
        {
            self->long_press_count = 0;
            self->status = BTN_UP;
            self->event = BTN_EVENT_CLICK;
            if (self->event_handle != NULL)
            {
                self->event_handle(BTN_EVENT_CLICK);
            }
        }

        break;

    case BTN_LONG_PRESS:
        if (self->get_level() == true)
        {
            self->status = BTN_LONG_PRESS;
            self->event = BTN_EVENT_NONE;
        }
        else
        {
            self->status = BTN_UP;
            self->event = BTN_EVENT_RELEASE;
            if (self->event_handle != NULL)
            {
                self->event_handle(BTN_EVENT_RELEASE);
            }
        }

        break;

    default:
        break;
    }
}

btn_event_t btn_fsm_get_event(btn_fsm_t *self)
{
    if (self == NULL)
    {
        return BTN_EVENT_NONE;
    }

    return self->event;
}

void btn_fsm_set_event_cb(btn_fsm_t *self, btn_event_handle_t event_handle)
{
    if (self == NULL)
    {
        return;
    }

    self->event_handle = event_handle;
}