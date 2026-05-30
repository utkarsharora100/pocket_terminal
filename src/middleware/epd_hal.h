#ifndef EPD_HAL_H
#define EPD_HAL_H
#include <lvgl/lvgl.h>
#include "GT1151.h"
#include <pthread.h>
/* We need this because we need to use LVGL types lv_display_t, lv_area_t, unit8_t */
void epd_hal_init(void);
lv_display_t* epd_hal_create_display(void);
void epd_hal_flush(lv_display_t *disp,const lv_area_t *area, uint8_t *px_map);
#endif
