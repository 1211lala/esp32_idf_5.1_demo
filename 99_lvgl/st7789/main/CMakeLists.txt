idf_component_register(SRCS  
                        "main.c" 
                        "bspled.c" 
                        "bspkey.c" 
                        "p_adc.c"
                        "p_exti.c" 
                        "p_uart.c" 
                        "p_gpttim.c"
                        "p_i2c_sht20.c"
                        "p_wifi_sta.c"
                        "spiffs.c"
                        "st7789.c"
                        "led_brightness.c"
                        "gpio_button.c"
                       INCLUDE_DIRS ".")

# 将 ../spiffs_data 的内容以 spiffs or fatfs or littlefs 的格式烧录进 storage 分区内

# spiffs_create_partition_image(storage ../fs_data FLASH_IN_PROJECT)
# fatfs_create_partition_image(storage ../fs_data FLASH_IN_PROJECT)
# littlefs_create_partition_image(storage ../fs_data FLASH_IN_PROJECT)