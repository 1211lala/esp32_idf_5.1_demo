#include "ws2812_spi.h"

#define WS_NUM 3
#define WS_PIN 13
#define WS_SPI_PORT VSPI_HOST

#define TH 0xF8 // 11111000
#define TL 0XE0 // 11100000

#define USE_CONTINUOUS_SEND

uint8_t ws_buf[WS_NUM][24];

static spi_device_handle_t spi;

/******************************************************************************
 * 函数描述: ws2812的spi引脚初始化
 *******************************************************************************/
void ws2812_spi_init(void)
{
    spi_bus_config_t buscfg = {};
    buscfg.mosi_io_num = WS_PIN;
    buscfg.miso_io_num = -1;
    buscfg.sclk_io_num = -1;
    buscfg.quadhd_io_num = -1;
    buscfg.quadwp_io_num = -1;
    buscfg.max_transfer_sz = SOC_SPI_MAXIMUM_BUFFER_SIZE;
    
    spi_device_interface_config_t devcfg = {};
    devcfg.clock_source = SPI_CLK_SRC_DEFAULT;
    devcfg.clock_speed_hz = 8 * 1000 * 1000;
    devcfg.spics_io_num = -1;
    devcfg.queue_size = 6;
    devcfg.mode = 0;

    ESP_ERROR_CHECK(spi_bus_initialize(WS_SPI_PORT, &buscfg, SPI_DMA_CH_AUTO));
    ESP_ERROR_CHECK(spi_bus_add_device(WS_SPI_PORT, &devcfg, &spi));
}

void spi_send_data(const uint8_t *data, int16_t len)
{
    esp_err_t ret;
    spi_transaction_t t;
    if (len == 0)
        return;
    memset(&t, 0, sizeof(t));
    t.length = len * 8;
    t.tx_buffer = data;
    t.user = (void *)1;
    ret = spi_device_polling_transmit(spi, &t);
    assert(ret == ESP_OK);
}

void grb_to_rgb(uint32_t *color)
{
    uint8_t r, g, b;
    r = *color >> 8;
    g = *color >> 16;
    b = *color;
    *color = (r << 16) | (g << 8) | (b << 0);
}

#ifdef USE_CONTINUOUS_SEND
void ws2812_set_color(uint32_t *color, uint16_t size)
{
    for (uint32_t i = 0; i < WS_NUM; i++)
    {
        grb_to_rgb(&color[i]);
        uint32_t mask = 0x800000;
        for (uint32_t j = 0; j < 24; j++)
        {
            ws_buf[i][j] = (color[i] & mask) ? TH : TL;

            mask = mask >> 1;
        }
    }
}
#else
void ws2812_set_color(uint32_t *color, uint16_t size)
{
    for (int i = 0; i < size; i++)
    {
        grb_to_rgb(color[i]);
        uint32_t mask = 0x800000;
        uint8_t data[24];
        for (int j = 0; j < 24; j++)
        {
            data[j] = (mask & color[i]) ? TH : TL;
            mask >>= 1;
        }
        spi_send_data(data, 24);
    }
}
#endif

void ws2812_change_select(uint16_t wsNum, uint32_t *color)
{
    if (wsNum >= WS_NUM)
        return;
    grb_to_rgb(color);
    uint32_t mask = 0x800000;
    for (uint32_t j = 0; j < 24; j++)
    {
        ws_buf[wsNum][j] = (*color & mask) ? TH : TL;

        mask = mask >> 1;
    }
}

void ws2812_change_line(uint32_t *color)
{
    grb_to_rgb(color);
    for (uint32_t i = 0; i < WS_NUM; i++)
    {
        
        uint32_t mask = 0x800000;
        for (uint32_t j = 0; j < 24; j++)
        {
            ws_buf[i][j] = (*color & mask) ? TH : TL;

            mask = mask >> 1;
        }
    }
}

void ws2812_push_data(void)
{
    spi_send_data(ws_buf, WS_NUM * 24);
}
