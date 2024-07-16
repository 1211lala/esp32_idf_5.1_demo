#ifndef _SD_SPI_FAT_H
#define _SD_SPI_FAT_H

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "dirent.h"

#define SD_OK   0
#define SD_ERR  -1


int sd_init_mount_fat(void);
int fatfs_write(const char *path, char *data, uint32_t size);
int fatfs_read(const char *path, char **buffer, uint32_t max_size);
int isexist(const char *path);
FRESULT fatfs_scan(char *path);
#endif