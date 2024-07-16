#ifndef _SPIFFS_H_
#define _SPIFFS_H_


#include "common.h"

#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_system.h"
#include "esp_spiffs.h"
#include "dirent.h"

extern esp_vfs_spiffs_conf_t conf;

void fs_mount(void);
void fs_scan(void);
int isExist(const char *path);
int fs_read(const char *path, char **buffer, uint32_t max_size);
int fs_write(const char *path, const char *buffer, uint32_t size);
#endif

