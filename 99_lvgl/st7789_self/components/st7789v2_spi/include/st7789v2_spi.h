#ifndef _ST7789V2_SPI_H_
#define _ST7789V2_SPI_H_

#include <string.h>
#include "stdio.h"
#include <driver/spi_master.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define CMD 0
#define DATA 1

#define st7789v2_res_on() gpio_set_level(lcd.resPin, false)
#define st7789v2_res_off() gpio_set_level(lcd.resPin, true)

#define st7789v2_dc_cmd() gpio_set_level(lcd.dcPin, false)
#define st7789v2_dc_data() gpio_set_level(lcd.dcPin, true)

#define st7789v2_cs_on() gpio_set_level(lcd.resPin, false)
#define st7789v2_cs_off() gpio_set_level(lcd.resPin, true)

#define st7789v2_blk_on() gpio_set_level(lcd.blkPin, true)
#define st7789v2_blk_off() gpio_set_level(lcd.blkPin, false)

struct ST7789V2
{
    int8_t sckPin;
    int8_t sdaPin;
    int8_t resPin;
    int8_t dcPin;
    int8_t csPin;
    int8_t blkPin;
    uint32_t freq;
    spi_host_device_t spi;
    uint16_t width;
    uint16_t heigh;
    uint8_t horizonta;
    uint32_t spiSize;
};

extern struct ST7789V2 lcd;
void st7789_init(void);
void lcd_send_byte(const uint8_t data, uint8_t dc);
void lcd_send_bytes(const uint8_t *data, int16_t len, uint8_t dc);
void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void lcd_fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t *color);
void lcd_send_int16s(const uint16_t *data, int16_t len, uint8_t dc);
#endif
