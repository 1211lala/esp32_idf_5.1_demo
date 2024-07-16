#include "lvglstart.h"
#include "stdio.h"
#include "wifi_mqtt.h"
#include "datafile.h"

#define rgb_to_hex(r, g, b) ((r << 16) | (g << 8) | (b))

LV_FONT_DECLARE(font_80)
LV_FONT_DECLARE(font_40)
LV_FONT_DECLARE(font_24)
LV_FONT_DECLARE(font_14)

lv_obj_t *bgObj = NULL;
lv_obj_t *tileLeft = NULL;
lv_obj_t *tileRight = NULL;
lv_timer_t *timer;

lv_obj_t *switch0;
lv_obj_t *switch1;
lv_obj_t *switch2;
lv_obj_t *switch3;
lv_obj_t *label00;
lv_obj_t *label11;
lv_obj_t *label22;
lv_obj_t *label33;

lv_obj_t *timelabel;

struct LV_OBJ
{
    lv_obj_t *obj;
    lv_obj_t *btn;
    lv_obj_t *label;
};

struct LV_OBJ my_time;

lv_obj_t *taskObj[5];

void menu_demo(void)
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    bgObj = lv_scr_act();
    lv_obj_clear_flag(bgObj, LV_OBJ_FLAG_SCROLLABLE);

    tileLeft = lv_tileview_create(bgObj);
    lv_obj_set_style_bg_color(tileLeft, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);

    my_time.obj = lv_obj_create(tileLeft);
    lv_obj_set_size(my_time.obj, LCD_W - 1, LCD_H - 1);
    lv_obj_set_style_bg_color(my_time.obj, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_align(my_time.obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_border_width(my_time.obj, 0, LV_STATE_DEFAULT);

    my_time.label = lv_label_create(my_time.obj);
    lv_obj_set_style_text_font(my_time.obj, &font_80, LV_STATE_DEFAULT);
    lv_obj_align(my_time.label, LV_ALIGN_CENTER, 0, -100);
    lv_obj_set_style_text_color(my_time.obj, lv_color_hex(rgb_to_hex(248, 100, 135)), LV_STATE_DEFAULT);
    timer = lv_timer_create(timer_cb, 1000, NULL);

    tileRight = lv_tileview_add_tile(tileLeft, 1, 0, LV_DIR_HOR);
    lv_obj_set_style_bg_color(tileRight, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);

    timelabel = lv_label_create(tileRight);
    lv_obj_align(timelabel, LV_ALIGN_TOP_MID, -110, 20);
    lv_obj_set_style_text_font(timelabel, &font_40, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(timelabel, lv_color_hex(rgb_to_hex(248, 100, 135)), LV_STATE_DEFAULT);

    taskObj[0] = lv_obj_create(tileRight);
    lv_obj_align(taskObj[0], LV_ALIGN_TOP_MID, -110, 120);
    lv_obj_set_size(taskObj[0], 200, 150);
    lv_obj_set_style_bg_color(taskObj[0], lv_color_hex(rgb_to_hex(1, 114, 254)), LV_PART_MAIN);
    lv_obj_set_style_border_width(taskObj[0], 0, LV_STATE_DEFAULT);

    switch0 = lv_switch_create(taskObj[0]);
    lv_obj_align(switch0, LV_ALIGN_BOTTOM_RIGHT, 0, 0);

    lv_obj_t *label0 = lv_label_create(taskObj[0]);
    lv_obj_align(label0, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_text_font(label0, &font_24, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label0, lv_color_hex(rgb_to_hex(0xff, 0xff, 0xff)), LV_STATE_DEFAULT);
    lv_label_set_text_fmt(label0, "卧室灯");

    label00 = lv_label_create(taskObj[0]);
    lv_obj_align_to(label00, label0, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    lv_obj_set_style_text_font(label00, &font_14, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label00, lv_color_hex(rgb_to_hex(0xff, 0xff, 0xff)), LV_STATE_DEFAULT);
    lv_label_set_text_fmt(label00, "OFF");

    taskObj[1] = lv_obj_create(tileRight);
    lv_obj_align(taskObj[1], LV_ALIGN_TOP_MID, 110, 120);
    lv_obj_set_size(taskObj[1], 200, 150);
    lv_obj_set_style_bg_color(taskObj[1], lv_color_hex(rgb_to_hex(0, 52, 164)), LV_PART_MAIN);
    lv_obj_set_style_border_width(taskObj[1], 0, LV_STATE_DEFAULT);

    switch1 = lv_switch_create(taskObj[1]);
    lv_obj_align(switch1, LV_ALIGN_BOTTOM_RIGHT, 0, 0);

    lv_obj_t *label1 = lv_label_create(taskObj[1]);
    lv_obj_align(label1, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_text_font(label1, &font_24, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label1, lv_color_hex(rgb_to_hex(0xff, 0xff, 0xff)), LV_STATE_DEFAULT);
    lv_label_set_text_fmt(label1, "风扇");

    label11 = lv_label_create(taskObj[1]);
    lv_obj_align_to(label11, label1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    lv_obj_set_style_text_font(label11, &font_14, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label11, lv_color_hex(rgb_to_hex(0xff, 0xff, 0xff)), LV_STATE_DEFAULT);
    lv_label_set_text_fmt(label11, "OFF");

    taskObj[2] = lv_obj_create(tileRight);
    lv_obj_align(taskObj[2], LV_ALIGN_BOTTOM_MID, -110, -40);
    lv_obj_set_size(taskObj[2], 200, 150);
    lv_obj_set_style_bg_color(taskObj[2], lv_color_hex(rgb_to_hex(11, 150, 158)), LV_PART_MAIN);
    lv_obj_set_style_border_width(taskObj[2], 0, LV_STATE_DEFAULT);

    switch2 = lv_switch_create(taskObj[2]);
    lv_obj_align(switch2, LV_ALIGN_BOTTOM_RIGHT, 0, 0);

    lv_obj_t *label2 = lv_label_create(taskObj[2]);
    lv_obj_align(label2, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_text_font(label2, &font_24, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label2, lv_color_hex(rgb_to_hex(0xff, 0xff, 0xff)), LV_STATE_DEFAULT);
    lv_label_set_text_fmt(label2, "窗帘");
    label22 = lv_label_create(taskObj[2]);
    lv_obj_align_to(label22, label2, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    lv_obj_set_style_text_font(label22, &font_14, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label22, lv_color_hex(rgb_to_hex(0xff, 0xff, 0xff)), LV_STATE_DEFAULT);
    lv_label_set_text_fmt(label22, "OFF");

    taskObj[3] = lv_obj_create(tileRight);
    lv_obj_align(taskObj[3], LV_ALIGN_BOTTOM_MID, 110, -40);
    lv_obj_set_size(taskObj[3], 200, 150);
    lv_obj_set_style_bg_color(taskObj[3], lv_color_hex(rgb_to_hex(166, 63, 147)), LV_PART_MAIN);
    lv_obj_set_style_border_width(taskObj[3], 0, LV_STATE_DEFAULT);

    switch3 = lv_switch_create(taskObj[3]);
    lv_obj_align(switch3, LV_ALIGN_BOTTOM_RIGHT, 0, 0);

    lv_obj_t *label3 = lv_label_create(taskObj[3]);
    lv_obj_align(label3, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_text_font(label3, &font_24, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label3, lv_color_hex(rgb_to_hex(0xff, 0xff, 0xff)), LV_STATE_DEFAULT);
    lv_label_set_text_fmt(label3, "厕所灯");
    label33 = lv_label_create(taskObj[3]);
    lv_obj_align_to(label33, label3, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    lv_obj_set_style_text_font(label33, &font_14, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label33, lv_color_hex(rgb_to_hex(0xff, 0xff, 0xff)), LV_STATE_DEFAULT);
    lv_label_set_text_fmt(label33, "OFF");

    lv_obj_add_event_cb(switch0, switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL); /* 添加事件 */
    lv_obj_add_event_cb(switch1, switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL); /* 添加事件 */
    lv_obj_add_event_cb(switch2, switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL); /* 添加事件 */
    lv_obj_add_event_cb(switch3, switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL); /* 添加事件 */
}

void switch_event_cb(lv_event_t *e)
{

    lv_obj_t *target = lv_event_get_target(e);
    uint8_t status = lv_obj_has_state(target, LV_STATE_CHECKED);
    if (target == switch0)
    {
        if (status)
        {

            // esp_mqtt_client_publish(mqttdev.client, mqttRoomLedRead, "{\"status\":\"on\"}", 0, 0, 0);
            esp_mqtt_client_publish(mqttdev.client, mqttRoomLedRead, "on", 0, 0, 0);

            lv_label_set_text_fmt(label00, "ON");
        }
        else
        {
            // esp_mqtt_client_publish(mqttdev.client, mqttRoomLedRead, "{\"status\":\"off\"}", 0, 0, 0);
            esp_mqtt_client_publish(mqttdev.client, mqttRoomLedRead, "off", 0, 0, 0);
            lv_label_set_text_fmt(label00, "OFF");
        }
    }
    else if (target == switch1)
    {
        if (status)
        {
            esp_mqtt_client_publish(mqttdev.client, mqttFanCtrl, "{\"status\":\"on\"}", 0, 0, 0);
            lv_label_set_text_fmt(label11, "ON");
        }
        else
        {
            esp_mqtt_client_publish(mqttdev.client, mqttFanCtrl, "{\"status\":\"off\"}", 0, 0, 0);
            lv_label_set_text_fmt(label11, "OFF");
        }
    }
    else if (target == switch2)
    {
        if (status)
        {
            esp_mqtt_client_publish(mqttdev.client, mqttCurtainCtrl, "{\"status\":\"on\"}", 0, 0, 0);
            lv_label_set_text_fmt(label22, "ON");
        }
        else
        {
            esp_mqtt_client_publish(mqttdev.client, mqttCurtainCtrl, "{\"status\":\"off\"}", 0, 0, 0);
            lv_label_set_text_fmt(label22, "OFF");
        }
    }
    else if (target == switch3)
    {
        if (status)
        {
            esp_mqtt_client_publish(mqttdev.client, mqttWcLedCtrl, "{\"status\":\"on\"}", 0, 0, 0);
            lv_label_set_text_fmt(label33, "ON");
        }
        else
        {
            esp_mqtt_client_publish(mqttdev.client, mqttWcLedCtrl, "{\"status\":\"off\"}", 0, 0, 0);
            lv_label_set_text_fmt(label33, "OFF");
        }
    }
}
void timer_cb(lv_timer_t *t)
{
    if (sntpdev.isOK)
    {
        lv_label_set_text_fmt(my_time.label, "%02d:%02d", sntpdev.tmInfo.tm_hour, sntpdev.tmInfo.tm_min);
        lv_label_set_text_fmt(timelabel, "%02d:%02d:%02d", sntpdev.tmInfo.tm_hour, sntpdev.tmInfo.tm_min, sntpdev.tmInfo.tm_sec);
    }
    else
    {
        lv_label_set_text_fmt(my_time.label, "%02d:%02d", 12, 12);
        lv_label_set_text_fmt(timelabel, "%02d:%02d:%02d", 12, 12, 12);
    }
}
