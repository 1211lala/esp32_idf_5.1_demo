#include "p_gpttim.h"

QueueHandle_t tim_queue = NULL;
gptimer_handle_t gptimer = NULL;

/******************************************************************************
 * 函数描述: gpttim初始化，设置定时器计数频率为1M，可自动重加载值为0，向上计数
 * 参  数1: 定时中断回调函数 bool (*gptimer_alarm_cb_t) (gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx);
 * 参  数2: 定时器计数频率
 * 参  数3: 定时器向上计数终值
 *******************************************************************************/
void gpttimer_init(gptimer_alarm_cb_t gpttim_isr_callback, uint32_t freq, uint32_t alarmValue)
{
    /* 1:配置GPT参数 */
    gptimer_config_t timer_config = {};
    timer_config.clk_src = GPTIMER_CLK_SRC_APB;
    timer_config.direction = GPTIMER_COUNT_UP;
    // timer_config.flags.intr_shared = 0;
    timer_config.resolution_hz = 1000000;
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    /* 2:配置GPT回调函数 */
    gptimer_event_callbacks_t cbs = {};
    cbs.on_alarm = gpttim_isr_callback;
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, tim_queue));

    /* 3:使能定时器 */
    ESP_ERROR_CHECK(gptimer_enable(gptimer));

    /* 3:配置闹钟事件动作 */
    gptimer_alarm_config_t alarm_config1 = {};
    alarm_config1.flags.auto_reload_on_alarm = true;
    alarm_config1.alarm_count = alarmValue;
    alarm_config1.reload_count = 0;
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config1));
    /* 3:启动定时器 */
    ESP_ERROR_CHECK(gptimer_start(gptimer));
}
