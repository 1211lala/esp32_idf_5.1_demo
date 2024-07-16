#include "driver/gpio.h"
#include "esp_lvgl_port.h"
#include "gpio_button.h"
#include "lvgl.h"

#define PREV_BTN 35
#define NEXT_BTN 34
#define ENTER_BTN 36

#define ACTIVE_LEVEL 0

void gpio_button_init(void)
{
    const button_config_t lv_button_config[3] = {
        {
            .type = BUTTON_TYPE_GPIO,
            .long_press_time = 1500,
            .short_press_time = 180,
            .gpio_button_config = {
                .gpio_num = PREV_BTN,
                .active_level = ACTIVE_LEVEL,
            },
        },
        {
            .type = BUTTON_TYPE_GPIO,
            .long_press_time = 1500,
            .short_press_time = 180,
            .gpio_button_config = {
                .gpio_num = NEXT_BTN,
                .active_level = ACTIVE_LEVEL,
            },
        },
        {
            .type = BUTTON_TYPE_GPIO,
            .long_press_time = 1500,
            .short_press_time = 180,
            .gpio_button_config = {
                .gpio_num = ENTER_BTN,
                .active_level = ACTIVE_LEVEL,
            },
        },
    };

    lv_disp_t *disp = lv_disp_get_default();
    const lvgl_port_nav_btns_cfg_t btns = {
        .disp = disp,
        .button_prev = &lv_button_config[0],
        .button_next = &lv_button_config[1],
        .button_enter = &lv_button_config[2],
    };
    lvgl_port_add_navigation_buttons(&btns);
    lv_group_t *g = lv_group_get_default();
    if (g == NULL)
    {
        g = lv_group_create();
        lv_group_set_default(g);
    }

    lv_indev_t *cur_drv = NULL;
    for (;;)
    {
        cur_drv = lv_indev_get_next(cur_drv);
        if (!cur_drv)
        {
            break;
        }

        if (cur_drv->driver->type == LV_INDEV_TYPE_KEYPAD)
        {
            lv_indev_set_group(cur_drv, g);
        }

        if (cur_drv->driver->type == LV_INDEV_TYPE_ENCODER)
        {
            lv_indev_set_group(cur_drv, g);
        }
    }
}