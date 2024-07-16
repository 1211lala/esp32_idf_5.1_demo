#ifndef LED_BRIGHTNESS_H
#define LED_BRIGHTNESS_H

#ifdef __cplusplus
extern "C" {
#endif

void display_brightness_init(void);

void display_brightness_set(int brightness_percent);

void display_backlight_off(void);

void display_backlight_on(void);

#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif