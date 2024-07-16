#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

#define ST7789_VERSION "0.0.4"
// spi init
// =================================
#define LCD_SPI_HOST SPI2_HOST // 使用的spi总线

#define LCD_PIN_MOSI 6  // lcd的sda引脚
#define LCD_PIN_MISO -1 // lcd的miso引脚
#define LCD_PIN_CLK 7   // lcd的clk引脚

#define LCD_PIN_DC 4 // lcd的dc引脚
#define LCD_PIN_CS -1 // lcd的cs引脚（可不配置cs脚）

#define LCD_PIXEL_CLOCK_HZ_SPI (80 * 100 * 1000) // lcd的spi的频率40~80mhz都可以，推荐40mhz

// =================================

#define LCD_PIN_RST -1 // lcd的rst引脚（可不配置rst引脚）

#define LCD_H_RES 240 // Horizontal Resolution，水平分辨率
#define LCD_V_RES 320 // Vertical Resolution, 垂直分辨率

#define LCD_RGB 0
#define LCD_BGR 1
#define LCD_COLOR LCD_RGB

#define BITS_PER_PIXEL 16 // RGB565: 16bti RGB666: 24bti