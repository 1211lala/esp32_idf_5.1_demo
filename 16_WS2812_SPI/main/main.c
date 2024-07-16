#include "common.h"
#include "bspled.h"
#include "bspkey.h"
#include "p_exti.h"
#include "p_adc.h"
#include "p_uart.h"
#include "p_gpttim.h"
#include "p_i2c_sht20.h"
#include "spiffs.h"
#include "sd_spi_fat.h"
#include "ws2812_spi.h"

void ws2812_spi(void *arg)
{
    uint32_t color = 0;
    ws2812_spi_init();
    while (1)
    {
        color = 0xff0000;
        ws2812_change_line(&color);
        ws2812_push_data();
        vTaskDelay(1000 / portTICK);

        color = 0x00ff00;
        ws2812_change_select(1, &color);
        ws2812_push_data();
        vTaskDelay(1000 / portTICK);
    }
}

void app_main()
{
    led_init();
    xTaskCreate(ws2812_spi, "ws2812_spi", 1024 * 10, NULL, 5, NULL);
}
