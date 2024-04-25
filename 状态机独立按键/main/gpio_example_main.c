/* GPIO Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "btn_fsm.h"


bool get_level(void)
{
    return !gpio_get_level(0);
}

void btn_event_handle(btn_event_t event)
{
    switch (event)
    {
    case BTN_EVENT_PRESS:
        printf("event: 按下事件\n");
        break;
    case BTN_EVENT_RELEASE:
        printf("event: 松开事件\n");
        break;
    case BTN_EVENT_LONG:
        printf("event: 长按事件\n");
        break;
    case BTN_EVENT_CLICK:
        printf("event: 点击事件\n");
        break;
    case BTN_EVENT_SIGNE:
        printf("event: 单击事件\n");
        break;
    case BTN_EVENT_DOUBLE:
        printf("event: 双击事件\n");
        break;
    
    default:
        break;
    }
}

void app_main(void)
{
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = 1ULL << 0;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    btn_fsm_t* btn_fsm = btn_fsm_create(get_level, 20);
    btn_fsm_set_event_cb(btn_fsm, btn_event_handle);
    
    while(1) {
        btn_fsm_handle(btn_fsm);
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}
