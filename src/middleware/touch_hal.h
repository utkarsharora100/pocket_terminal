#ifndef TOUCH_HAL_H
#define TOUCH_HAL_H
#include <lvgl/lvgl.h>
#include <pthread.h>
#include <DEV_Config.h>
void touch_hal_init(void);
lv_indev_t* touch_hal_create_indev(void);
void touch_hal_read(lv_indev_t* indev, lv_indev_data_t* data);
void *touch_irq_thread(void *arg);
extern pthread_t touch_thread;
extern UBYTE thread_flag;
#endif

