#include "st7789v2_spi.h"

// #define user_cs
static const int portTICK = 10;
static spi_device_handle_t spi;
struct ST7789V2 lcd = {
	.sckPin = 18,
	.sdaPin = 17,
	.resPin = 15,
	.dcPin = 19,
	.csPin = 16,
	.blkPin = 21,
	.spi = SPI3_HOST,
	.freq = 80000000,
	.width = 240,
	.heigh = 280,
	.horizonta = 0,
	.spiSize = 19200,		// 这个是值spi一次传输的最大数据量
};

void lcd_send_bytes(const uint8_t *data, int16_t len, uint8_t dc)
{
#ifdef user_cs
	st7789v2_cs_on();
#endif

	if (dc)
		st7789v2_dc_data();
	else if (!dc)
		st7789v2_dc_cmd();

	esp_err_t ret;
	spi_transaction_t t;
	if (len == 0)
		return;
	memset(&t, 0, sizeof(t));
	t.length = len * 8;
	t.tx_buffer = data;
	t.user = (void *)1;
	ret = spi_device_transmit(spi, &t);
	// ret = spi_device_polling_transmit(spi, &t);
	assert(ret == ESP_OK);

#ifdef user_cs
	st7789v2_cs_off();
#endif
}

void lcd_send_byte(const uint8_t data, uint8_t dc)
{
#ifdef user_cs
	st7789v2_cs_on();
#endif

	if (dc)
		st7789v2_dc_data();
	else if (!dc)
		st7789v2_dc_cmd();

	esp_err_t ret;
	spi_transaction_t t;
	memset(&t, 0, sizeof(t));
	t.length = 1 * 8;
	t.tx_buffer = &data;
	t.user = (void *)1;
	ret = spi_device_polling_transmit(spi, &t);
	assert(ret == ESP_OK);

#ifdef user_cs
	st7789v2_cs_off();
#endif
}
/******************************************************************************
 * 函数描述:
 *******************************************************************************/
void st7789_init(void)
{
	gpio_config_t pin_cfg = {};

	pin_cfg.intr_type = GPIO_INTR_DISABLE;
	pin_cfg.mode = GPIO_MODE_OUTPUT;
	pin_cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
	pin_cfg.pull_up_en = GPIO_PULLUP_ENABLE;
	pin_cfg.pin_bit_mask = (1ULL << lcd.resPin) | (1ULL << lcd.dcPin) | (1ULL << lcd.csPin) | (1ULL << lcd.blkPin);
	gpio_config(&pin_cfg);

	spi_bus_config_t buscfg = {};
	buscfg.mosi_io_num = lcd.sdaPin;
	buscfg.miso_io_num = -1;
	buscfg.sclk_io_num = lcd.sckPin;
	buscfg.quadhd_io_num = -1;
	buscfg.quadwp_io_num = -1;
	buscfg.max_transfer_sz = lcd.spiSize;

	spi_device_interface_config_t devcfg = {};
	devcfg.clock_source = SPI_CLK_SRC_DEFAULT;
	devcfg.clock_speed_hz = lcd.freq;
	devcfg.spics_io_num = lcd.csPin;
	// devcfg.spics_io_num = -1;
	devcfg.queue_size = 4;
	devcfg.mode = 0;
	devcfg.flags = SPI_DEVICE_NO_DUMMY;

       	ESP_ERROR_CHECK(spi_bus_initialize(lcd.spi, &buscfg, SPI_DMA_CH_AUTO));
	ESP_ERROR_CHECK(spi_bus_add_device(lcd.spi, &devcfg, &spi));

	st7789v2_res_on();
	vTaskDelay(10 / portTICK);
	st7789v2_res_off();
	vTaskDelay(10 / portTICK);
	st7789v2_blk_on();
	vTaskDelay(10 / portTICK);

	//************* Start Initial Sequence **********//
	lcd_send_byte(0x11, CMD);	// Sleep out
	vTaskDelay(120 / portTICK); // Delay 120ms
	//************* Start Initial Sequence **********//
	lcd_send_byte(0x36, CMD);
	if (lcd.horizonta == 0)
		lcd_send_byte(0x00, DATA);
	else if (lcd.horizonta == 1)
		lcd_send_byte(0xC0, DATA);
	else if (lcd.horizonta == 2)
		lcd_send_byte(0x70, DATA);
	else
		lcd_send_byte(0xA0, DATA);

	lcd_send_byte(0x3A, CMD);
	lcd_send_byte(0x05, DATA);

	lcd_send_byte(0xB2, CMD);
	lcd_send_byte(0x0C, DATA);
	lcd_send_byte(0x0C, DATA);
	lcd_send_byte(0x00, DATA);
	lcd_send_byte(0x33, DATA);
	lcd_send_byte(0x33, DATA);

	lcd_send_byte(0xB7, CMD);
	lcd_send_byte(0x35, DATA);

	lcd_send_byte(0xBB, CMD);
	lcd_send_byte(0x32, DATA); // Vcom=1.35V

	lcd_send_byte(0xC2, CMD);
	lcd_send_byte(0x01, DATA);

	lcd_send_byte(0xC3, CMD);
	lcd_send_byte(0x15, DATA); // GVDD=4.8V  颜色深度

	lcd_send_byte(0xC4, CMD);
	lcd_send_byte(0x20, DATA); // VDV, 0x20:0v

	lcd_send_byte(0xC6, CMD);
	lcd_send_byte(0x0F, DATA); // 0x0F:60Hz

	lcd_send_byte(0xD0, CMD);
	lcd_send_byte(0xA4, DATA);
	lcd_send_byte(0xA1, DATA);

	lcd_send_byte(0xE0, CMD);
	lcd_send_byte(0xD0, DATA);
	lcd_send_byte(0x08, DATA);
	lcd_send_byte(0x0E, DATA);
	lcd_send_byte(0x09, DATA);
	lcd_send_byte(0x09, DATA);
	lcd_send_byte(0x05, DATA);
	lcd_send_byte(0x31, DATA);
	lcd_send_byte(0x33, DATA);
	lcd_send_byte(0x48, DATA);
	lcd_send_byte(0x17, DATA);
	lcd_send_byte(0x14, DATA);
	lcd_send_byte(0x15, DATA);
	lcd_send_byte(0x31, DATA);
	lcd_send_byte(0x34, DATA);

	lcd_send_byte(0xE1, CMD);
	lcd_send_byte(0xD0, DATA);
	lcd_send_byte(0x08, DATA);
	lcd_send_byte(0x0E, DATA);
	lcd_send_byte(0x09, DATA);
	lcd_send_byte(0x09, DATA);
	lcd_send_byte(0x15, DATA);
	lcd_send_byte(0x31, DATA);
	lcd_send_byte(0x33, DATA);
	lcd_send_byte(0x48, DATA);
	lcd_send_byte(0x17, DATA);
	lcd_send_byte(0x14, DATA);
	lcd_send_byte(0x15, DATA);
	lcd_send_byte(0x31, DATA);
	lcd_send_byte(0x34, DATA);
	lcd_send_byte(0x21, CMD);

	lcd_send_byte(0x29, CMD);
}

void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	if (lcd.horizonta == 0 || lcd.horizonta == 1)
	{
		y1 += 20;
		y2 += 20;
	}
	else if (lcd.horizonta == 2 || lcd.horizonta == 3)
	{
		x1 += 20;
		x2 += 20;
	}
	uint8_t datax1[2];
	uint8_t datax2[2];
	uint8_t datay1[2];
	uint8_t datay2[2];
	datax1[0] = x1 >> 8;
	datax1[1] = x1 & 0xff;
	datax2[0] = x2 >> 8;
	datax2[1] = x2 & 0xff;
	datay1[0] = y1 >> 8;
	datay1[1] = y1 & 0xff;
	datay2[0] = y2 >> 8;
	datay2[1] = y2 & 0xff;

	lcd_send_byte(0x2a, CMD);
	lcd_send_bytes(datax1, 2, DATA);
	lcd_send_bytes(datax2, 2, DATA);
	lcd_send_byte(0x2b, CMD);
	lcd_send_bytes(datay1, 2, DATA);
	lcd_send_bytes(datay2, 2, DATA);
	lcd_send_byte(0x2c, CMD);
}

// void lcd_fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t *color)
// {
// 	LCD_Address_Set(xsta, ysta, xend - 1, yend - 1);

// 	uint32_t size = (xend - xsta) * (yend - ysta) * 2;

// 	if (size == 0)
// 	{
// 		lcd_buf[0] = *color >> 8;
// 		lcd_buf[1] = *color;
// 		lcd_send_bytes(lcd_buf, 2, DATA);
// 	}
// 	else
// 	{
// 		if (buffSize >= size)
// 		{
// 			for (uint32_t i = 0; i < size / 2; i++)
// 			{
// 				lcd_buf[i * 2] = *color >> 8;
// 				lcd_buf[i * 2 + 1] = *color;
// 			}
// 			lcd_send_bytes(lcd_buf, size, DATA);
// 		}
// 		else
// 		{
// 			for (uint16_t i = 0; i < buffSize / 2; i++)
// 			{
// 				lcd_buf[i * 2] = *color >> 8;
// 				lcd_buf[i * 2 + 1] = *color;
// 			}
// 			uint32_t minsize = size % buffSize;

// 			for (uint16_t j = 0; j < size / buffSize; j++)
// 			{
// 				lcd_send_bytes(lcd_buf, buffSize, DATA);
// 			}
// 			lcd_send_bytes(lcd_buf, minsize, DATA);
// 		}
// 	}
// }

