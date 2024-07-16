#include "common.h"
#include "bspled.h"
#include "bspkey.h"
#include "p_exti.h"
#include "p_adc.h"
#include "p_uart.h"
#include "p_gpttim.h"
#include "p_i2c_sht20.h"
#include "p_wifi_sta.h"
#include "spiffs.h"
#include "cJSON.h"
#include "st7789.h"
#include "led_brightness.h"
#include "gpio_button.h"
#include "lv_demos.h"

TaskHandle_t wifi_handle = NULL;

void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    static int s_retry_num = 0;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }

    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;

        ESP_LOGI("WIFI", "STA-IP: " IPSTR, IP2STR(&event->ip_info.ip));
        ESP_LOGI("WIFI", "STA-GATEWAY: " IPSTR, IP2STR(&event->ip_info.gw));
        ESP_LOGI("WIFI", "STA-SUBNET: " IPSTR, IP2STR(&event->ip_info.netmask));

        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < 10)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI("WIFI", "连接失败,开始重新连接...");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGE("WIFI", "连接失败!!!");
    }
}

void task_led(void *arg)
{
    while (1)
    {
        vTaskDelay(500 / portTICK);
        led_blink();
    }
}

void btn1_callback(lv_event_t *e)
{
    ESP_LOGE("btn1", "btn_callback");
}

void btn2_callback(lv_event_t *e)
{
    ESP_LOGE("btn2", "btn_callback");
}
void app_main()
{
    led_init();
    st7789_init();
    gpio_button_init();
    display_brightness_init();
    display_brightness_set(50);
    lv_demo_benchmark();
    
    // lv_obj_t *obj1 = lv_btn_create(lv_scr_act());
    // lv_obj_set_size(obj1, 60, 30);
    // lv_obj_align(obj1, LV_ALIGN_CENTER, -40, 0);
    // // lv_obj_set_style_bg_color(obj, lv_color_hex(0x1b1b1b), LV_STATE_DEFAULT );
    // lv_obj_set_style_bg_color(obj1, lv_color_hex(0xff0000), LV_STATE_CHECKED);
    // lv_obj_add_event_cb(obj1, btn1_callback, LV_EVENT_CLICKED, NULL);

    // lv_obj_t *obj2 = lv_btn_create(lv_scr_act());
    // lv_obj_set_size(obj2, 60, 30);
    // lv_obj_align(obj2, LV_ALIGN_CENTER, 40, 0);
    // // lv_obj_set_style_bg_color(obj, lv_color_hex(0x1b1b1b), LV_STATE_DEFAULT );
    // lv_obj_set_style_bg_color(obj2, lv_color_hex(0xff0000), LV_STATE_CHECKED);
    // lv_obj_add_event_cb(obj2, btn2_callback, LV_EVENT_CLICKED, NULL);

    // wifi_sta_init(&wp, wifi_event_handler);
    xTaskCreate(task_led, "task_led", 1024 * 4, NULL, 5, &wifi_handle);
}
