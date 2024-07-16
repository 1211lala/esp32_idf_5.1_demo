/**
 * https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-reference/storage/fatfs.html
 * ESP-IDF 使用 FatFs 库来实现 FAT 文件系统
 *      如果使用了标志C库的文件函数 需要加上挂载文件系统的路径前缀 /sdcard/hello.c
 *      如果使用的 fatfs 的库函数则不用 /hello.c
 * 以下程序统一使用 fatfs 的函数
 *
 * 使用长文件名打开 menuconfig 将 Long filename support 打开
 *
 *
 *
    POSIX	FatFs
    "r"	    FA_READ
    "r+"	FA_READ | FA_WRITE
    "w"	    FA_CREATE_ALWAYS | FA_WRITE
    "w+"	FA_CREATE_ALWAYS | FA_WRITE | FA_READ
    "a"	    FA_OPEN_APPEND | FA_WRITE
    "a+"	FA_OPEN_APPEND | FA_WRITE | FA_READ
    "wx"	FA_CREATE_NEW | FA_WRITE
    "w+x"	FA_CREATE_NEW | FA_WRITE | FA_READ
 */

#include "sd_spi_fat.h"

#define EXAMPLE_MAX_CHAR_SIZE 64

static const char *TAG = "sdcard";
#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK 18
#define PIN_NUM_CS 4
/* 起始目录 */
#define ROOT "/sdcard"
/* 看结构体注释 */
#define BLACK_SIZE 1024 * 16
/* 最大同时打开的文件数 */
#define MAX_FILES 4
/* SPI总线的频率 单位 Khz */
#define SPI_MAX_FREQ 20000

/******************************************************************************
 * 函数描述: spi总线初始化SD卡并挂载fatfs,在初始化前一定要设置为fat32模式
 * 参  数1:
 *******************************************************************************/
int sd_init_mount_fat(void)
{
    esp_err_t ret;
    /* 挂载文件系统的参数, 如果 format_if_mount_failed = true 则在挂载fatfs失败后格式化SD卡 */
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = MAX_FILES,
        .allocation_unit_size = BLACK_SIZE,

    };

    sdmmc_card_t *card;
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.max_freq_khz = SPI_MAX_FREQ;
    /* 配置 SPI 总线引脚 */
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    /* 初始化 SPI 总线 */
    ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "SD 卡 SPI 总线初始化失败");
        return SD_ERR;
    }

    /* 设置片选引脚和额外配置 */
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;
    /* 挂载 FATFS */
    ret = esp_vfs_fat_sdspi_mount(ROOT, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                          "If you want the card to be formatted, set the CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                          "Make sure SD card lines have pull-up resistors in place.",
                     esp_err_to_name(ret));
        }
        return SD_ERR;
    }
    ESP_LOGI(TAG, "Filesystem mounted");

    /* 打印SD的相关消息 */
    sdmmc_card_print_info(stdout, card);

    return SD_OK;
}

/******************************************************************************
 * 函数描述: 检测文件是否存在
 * 参  数1: 文件路径
 * 返  回:  文件的大小/ (-X 不存在)
 *******************************************************************************/
int isexist(const char *path)
{

    FRESULT ret;
    FILINFO fno;
    ret = f_stat(path, &fno);
    if (ret == FR_OK)
    {
        return (int)fno.fsize;
    }
    else
    {
        ESP_LOGE(TAG, "%s不存在", path);
    }
    return -FR_NO_FILE;
}

/******************************************************************************
 * 函数描述: 向指定文件中写数据
 * 参  数1: 文件路径
 * 参  数2: 需要写入的数据
 * 返  回:  实际写入的数据(字节)
 *******************************************************************************/
int fatfs_write(const char *path, char *buffer, uint32_t size)
{
    UINT writeSize = 0;
    FIL fil;
    FRESULT fr;
    fr = f_open(&fil, path, FA_CREATE_ALWAYS | FA_WRITE);
    if (fr != FR_OK)
    {
        ESP_LOGE(TAG, "failed to open %s    error code(%d)", path, fr);
        return -FR_NO_FILE;
    }
    fr = f_write(&fil, (char *)buffer, size, &writeSize);
    if (fr != FR_OK)
    {
        ESP_LOGE(TAG, "failed to write %s    error code(%d)", path, fr);
        return -FR_NO_FILE;
    }
    f_close(&fil);
    return writeSize;
}

/******************************************************************************
 * 函数描述: 读取文件内容，一定要在读取成功(return > 0)后使用free(), 未成功不用在内部free
 * 参  数1: 文件路径
 * 参  数2: 缓存buffer
 * 参  数3: 所允许分配的最大字节数
 * 返  回:  实际读取的字节数(字节)
 *
// int len = fatfs_read("/main.c", &buf, 2000);
// if (len > 0)
// {
//     buf[len] = '\0';
//     printf("%s\r\n", buf);
//     free(buf);
// }
 *******************************************************************************/
int fatfs_read(const char *path, char **buffer, uint32_t max_size)
{
    UINT readSize = 0;
    FIL fil;
    FRESULT fr;
    uint32_t fileSize = isexist(path);
    if (fileSize > max_size)
    {
        ESP_LOGE(TAG, "%s文件太大, 以及超过分配的最大内存", path);
        return -1;
    }

    fr = f_open(&fil, path, FA_READ);
    if (fr != FR_OK)
    {
        ESP_LOGE(TAG, "failed to open %s    error code(%d)", path, fr);
        return -FR_NO_FILE;
    }
    *buffer = malloc(fileSize + 1);
    fr = f_read(&fil, *buffer, fileSize, &readSize);
    if (fr != FR_OK)
    {
        free(*buffer);
        ESP_LOGE(TAG, "failed to read %s    error code(%d)", path, fr);
        return -FR_NO_FILE;
    }
    f_close(&fil);
    return fileSize;
}

FRESULT fatfs_scan(char *path)
{
    FF_DIR dir;
    FILINFO fno;
    FRESULT res = f_opendir(&dir, (const TCHAR *)path);
    if (res == FR_OK)
    {
        while (1)
        {
            res = f_readdir(&dir, &fno);
            if ((res != FR_OK) || (fno.fname[0] == 0))
                break;
            if (fno.fname[0] == '.' && (fno.fname[1] == '\0' || (fno.fname[1] == '.' && fno.fname[2] == '\0')))
                continue;
            printf("%s/%-20s %ld bytes\n", path, fno.fname, fno.fsize);
            if (fno.fattrib & AM_DIR)
            {
                char sub_path[512];
                sprintf(sub_path, "%s/%s", path, fno.fname);
                fatfs_scan(sub_path); // Recursively scan sub-directory
            }
        }
        f_closedir(&dir);
        return FR_OK;
    }
    else
    {
        printf("%s open fail!\n", path);
        return FR_DISK_ERR;
    }
}
