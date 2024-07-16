
    // TOUCH_PAD_NUM0 = 0, /*!< Touch pad channel 0 is GPIO4(ESP32) */
    // TOUCH_PAD_NUM1,     /*!< Touch pad channel 1 is GPIO0(ESP32) / GPIO1(ESP32-S2) */
    // TOUCH_PAD_NUM2, /*!< Touch pad channel 2 is GPIO2(ESP32) / GPIO2(ESP32-S2) */
    // TOUCH_PAD_NUM3, /*!< Touch pad channel 3 is GPIO15(ESP32) / GPIO3(ESP32-S2) */
    // TOUCH_PAD_NUM4, /*!< Touch pad channel 4 is GPIO13(ESP32) / GPIO4(ESP32-S2) */
    // TOUCH_PAD_NUM5, /*!< Touch pad channel 5 is GPIO12(ESP32) / GPIO5(ESP32-S2) */
    // TOUCH_PAD_NUM6, /*!< Touch pad channel 6 is GPIO14(ESP32) / GPIO6(ESP32-S2) */
    // TOUCH_PAD_NUM7, /*!< Touch pad channel 7 is GPIO27(ESP32) / GPIO7(ESP32-S2) */
    // TOUCH_PAD_NUM8, /*!< Touch pad channel 8 is GPIO33(ESP32) / GPIO8(ESP32-S2) */
    // TOUCH_PAD_NUM9, /*!< Touch pad channel 9 is GPIO32(ESP32) / GPIO9(ESP32-S2) */


#include "p_touch.h"

#define FILTERTIME 10
            static const char *TAG = "touch";

void touch_init(touch_pad_t touchNum)
{
    ESP_ERROR_CHECK(touch_pad_init());
    // 设定充放电参考电压：高参考电压，低参考电压，高参考电压衰减
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
    // 配置触摸端口
    touch_pad_config(0, 0);
    // 初始化并启动软件滤波器
    touch_pad_filter_start(FILTERTIME);
}

void touch_init_isr(touch_pad_t touchNum, intr_handler_t touch_callback)
{
    touch_pad_init();
    // 如果使用中断触发模式，应将触摸传感器FSM模式设置为“ TOUCH_FSM_MODE_TIMER”
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    // 设定充放电参考电压：高参考电压，低参考电压，高参考电压衰减
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
    // 配置触摸端口
    touch_pad_config(touchNum, 0);
    // 初始化并启动软件滤波器
    touch_pad_filter_start(FILTERTIME);
    // 设定中断限值，此时不要触摸，2/3的读取值做为限值
    uint16_t touch_value;
    touch_pad_read_filtered(touchNum, &touch_value);
    ESP_LOGI(TAG, "Touch[%d]的触发值为: %.0f", touchNum, touch_value * 0.66);
    // 设置中断阈值
    ESP_ERROR_CHECK(touch_pad_set_thresh(touchNum, touch_value * 2 / 3));
    // 注册触摸中断ISR
    touch_pad_isr_register(touch_callback, NULL);
    // 开启中断
    touch_pad_intr_enable();
}