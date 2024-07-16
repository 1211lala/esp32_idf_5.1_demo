idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES lv_port_disp
                    REQUIRES lv_port_indev
                    REQUIRES lvgl
                    REQUIRES st7789v2_spi
                    REQUIRES keyboard
                    )
