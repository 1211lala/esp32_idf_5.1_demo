#ifndef _WS2812_SPI_H_
#define _WS2812_SPI_H_


#include "common.h"
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "dirent.h"


void ws2812_spi_init(void);
void ws2812_set_color(uint32_t *color, uint16_t size);
void ws2812_push_data(void);
void ws2812_change_select(uint16_t wsNum, uint32_t *color);
void ws2812_change_line(uint32_t *color);
#endif

