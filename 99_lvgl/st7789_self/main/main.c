#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_idf_version.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"

#include "lv_demos.h"
#include "st7789v2_spi.h"
#include "driver/gptimer.h"

void lvgl_ui(void *arg);
void lvgl_handler(void *arg);

gptimer_handle_t gptimer;

bool gptimer_callbacks(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    lv_tick_inc(1);
    return 1;
}

void app_main(void)
{

    gptimer_config_t timer_config = {};
    timer_config.clk_src = GPTIMER_CLK_SRC_APB;
    timer_config.direction = GPTIMER_COUNT_UP;
    // timer_config.flags.intr_shared = 0;
    timer_config.resolution_hz = 1000000;
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    /* 2:配置GPT回调函数 */
    gptimer_event_callbacks_t cbs = {};
    cbs.on_alarm = gptimer_callbacks;
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));
    /* 3:使能定时器 */
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    /* 3:配置闹钟事件动作 */
    gptimer_alarm_config_t alarm_config1 = {};
    alarm_config1.flags.auto_reload_on_alarm = true;
    alarm_config1.alarm_count = 1000;
    alarm_config1.reload_count = 0;
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config1));
    /* 3:启动定时器 */
    ESP_ERROR_CHECK(gptimer_start(gptimer));

    xTaskCreate(lvgl_ui, "ui", 1024 * 4, NULL, 4, NULL);
    xTaskCreate(lvgl_handler, "i", 1024 * 5, NULL, 5, NULL);

    vTaskDelete(NULL);
}

void lvgl_ui(void *arg)
{
    lv_init();
    // lv_port_indev_init();
    lv_port_disp_init();
    printf("初始化完成\n");

    // lv_obj_t *btn = lv_btn_create(lv_scr_act());
    // lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);
    // lv_obj_set_size(btn, 100, 50);


    // lv_group_t *group = lv_group_create();
    // lv_indev_set_group(indev_keypad, group);
 
    // lv_group_add_obj(group ,btn);
    lv_demo_benchmark();
    while (1)
    {
        vTaskDelay(10);
    }
}
void lvgl_handler(void *arg)
{
    while (1)
    {
        lv_task_handler();
        vTaskDelay(1);
    }
}