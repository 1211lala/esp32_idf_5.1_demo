/*
1.需要添加组件
my_project
    build
    main
        CMakeLists.txt
        idf_component.yml   <--
        main.c
    CMakeLists.txt

idf_component.yml里面的内容如下

## IDF Component Manager Manifest File
dependencies:
  idf: ">=5.1"
  lvgl/lvgl: ">=8.*"
  esp_lvgl_port: "^1"

2.点击下边的设置进入menuconfig，找到最下面的lvgl打开下面几个选项
true   Swap the 2 bytes of RGB565 color. Useful if the display has an 8-bit interface (e.g. SPI).
true   If true use custom malloc/free, otherwise use the built-in `lv_mem_alloc()` and `lv_mem_free()`
64     Number of the memory buffer
true   Use the standard memcpy and memset instead of LVGL's own functions
true   Benchmark your system
然后编译就好
*/

#include <stdio.h>

// esp驱动lcd需要的头文件
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"

#include "driver/spi_master.h" // 初始化spi需要的头文件

#include "esp_lvgl_port.h" // esp lvgl port组件
#include "lvgl.h"          // lvgl组件
#include "lv_demos.h"      // lvgl的demo

#define LCD_SPI_PORT SPI2_HOST // 使用的spi总线

#define LCD_CLK 18   // lcd的clk引脚
#define LCD_MOSI 17  // lcd的sda引脚
#define LCD_MISO -1 // lcd的miso引脚

#define LCD_DC 19                                 // lcd的dc引脚
#define LCD_CS 16                                 // lcd的cs引脚
#define LCD_PIXEL_CLOCK_HZ_SPI (80 * 1000 * 1000) // lcd的spi的频率40~80mhz都可以，推荐40mhz

#define LCD_RST 15 // lcd的rst引脚

#define LCD_H_RES 240                       // 宽
#define LCD_V_RES 280                       // 高
#define LCD_DRAW_BUF_HEIGHT (LCD_V_RES / 2) // 刷的buf设置为屏幕高的一半

#define LCD_SWAP_XY false
#define LCD_MIRROR_X false
#define LCD_MIRROR_Y false

#define OFFSET_X 0 // 设置屏幕偏移
#define OFFSET_Y 20 // 设置屏幕偏移

void st7789_init(void)
{
    // 创建lcd面板的io句柄和面板句柄
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_handle_t panel_handle = NULL;

    // 初始化一组spi
    const spi_bus_config_t spi_bus_config = {
        .sclk_io_num = LCD_CLK,
        .mosi_io_num = LCD_MOSI,
        .miso_io_num = LCD_MISO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = (LCD_H_RES * LCD_DRAW_BUF_HEIGHT) * sizeof(uint16_t),
    };
    spi_bus_initialize(LCD_SPI_PORT, &spi_bus_config, SPI_DMA_CH_AUTO);

    // 初始化spi的io总线
    const esp_lcd_panel_io_spi_config_t io_spi_config = {
        .dc_gpio_num = LCD_DC,
        .cs_gpio_num = LCD_CS,
        .pclk_hz = LCD_PIXEL_CLOCK_HZ_SPI,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_SPI_PORT, &io_spi_config, &io_handle);

    // 注册ST7789
    const esp_lcd_panel_dev_config_t panel_dev_config = {
        .reset_gpio_num = LCD_RST,
        .color_space = LCD_RGB_ENDIAN_RGB,
        .bits_per_pixel = 16,
    };
    esp_lcd_new_panel_st7789(io_handle, &panel_dev_config, &panel_handle);

    esp_lcd_panel_reset(panel_handle);                              // 重启ST7789
    esp_lcd_panel_init(panel_handle);                               // 初始化ST7789
    esp_lcd_panel_swap_xy(panel_handle, LCD_SWAP_XY);               // 设置反转
    esp_lcd_panel_mirror(panel_handle, LCD_MIRROR_X, LCD_MIRROR_Y); // 设置镜像
    esp_lcd_panel_invert_color(panel_handle, true);                 // 设置颜色反转
    esp_lcd_panel_set_gap(panel_handle, OFFSET_X, OFFSET_Y);        // 设置屏幕间隙，距离左侧和顶部的距离，相当于是设置偏移
    /* !< 现在还没有开启屏幕，等lvgl刷新画面的时候再开启屏幕 */

    // 初始化esp lvgl port组件
    const lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = 5,       /*!< LVGL 线程优先级 */
        .task_stack = 8192,       /*!< LVGL 线程栈大小 */
        .task_affinity = -1,      /*!< LVGL 线程在哪个核心上运行（cpu0或者cpu1）-1为没有要求 */
        .task_max_sleep_ms = 500, /*!< LVGL 线程最大睡眠时间 */
        .timer_period_ms = 5,
    };
    lvgl_port_init(&lvgl_cfg);

    // 把屏幕注册到lvgl
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,                           // lcd的io句柄
        .panel_handle = panel_handle,                     // lcd的面板句柄
        .buffer_size = (LCD_H_RES * LCD_DRAW_BUF_HEIGHT), // 需要刷的buf大小，为屏幕宽*高的一半
        .double_buffer = 0,                               // 设置为1启用双buf，实测没什么用
        .hres = LCD_H_RES,                                // lcd的宽
        .vres = LCD_V_RES,                                // lcd的高
        .monochrome = 0,                                  // 单色屏设置为1
        /* Rotation values must be same as used in esp_lcd for initial settings of the screen */
        .rotation = {
            .swap_xy = LCD_SWAP_XY,
            .mirror_x = LCD_MIRROR_X,
            .mirror_y = LCD_MIRROR_Y,
        },
        .flags = {
    /* 如果开启psram则申请buf到psram，反之申请buf到自身内存里面 */
#if CONFIG_SPIRAM
            .buff_spiram = true,
#else
            .buff_dma = true,
#endif
            // .sw_rotate = true, // 开启软件旋转屏幕
            // .swap_bytes = true, // 在发送到LCD驱动器之前交换RGB656（16位）中的字节，解决颜色不正常
        }};
    lvgl_port_add_disp(&disp_cfg);

    /* 这里设置屏幕最开始显示的画面再开启屏幕 */
    // lv_demo_benchmark();
    esp_lcd_panel_disp_on_off(panel_handle, true); // 开启屏幕
}
