#include "common.h"
#include "bspled.h"
#include "bspkey.h"
#include "p_exti.h"
#include "p_adc.h"
#include "p_uart.h"
#include "p_gpttim.h"
#include "p_i2c_sht20.h"

void sht20_task(void *pvParameters)
{
    sht20_init(100000);
    float temp = 0;
    float hum = 0;
    uint8_t flag = 0;
    while (1)
    {
        flag = 0;
        if (sht20_get_temp(&temp) != ESP_OK)
        {
            flag = 1;
        }
        if (sht20_get_hum(&hum) != ESP_OK)
        {
            flag = 1;
        }
        if (flag == 0)
        {
            ESP_LOGI("SHT20", "HUM: %.2f%% TEMP: %.2fC", hum, temp);
        }

        vTaskDelay(1000 / portTICK);
    }
}

void app_main()
{

    xTaskCreate(sht20_task, "sht20_task", 2048, NULL, 5, NULL);
}
