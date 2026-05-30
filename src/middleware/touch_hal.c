#include "touch_hal.h"
#include "GT1151.h"
#include "DEV_Config.h"
#include <pthread.h>
extern GT1151_Dev Dev_Now;
extern int IIC_Address;
pthread_t touch_thread;
extern UBYTE thread_flag = 1;

void *touch_irq_thread(void *arg)
{
    while(thread_flag) {
        if(DEV_Digital_Read(INT) == 0)
            Dev_Now.Touch = 1;
        else
            Dev_Now.Touch = 0;
        DEV_Delay_ms(0.01);
    }
    pthread_exit(NULL);
}
void touch_hal_init()
{
	thread_flag = 1;
}
void touch_hal_read(lv_indev_t* indev, lv_indev_data_t* data){
	GT_Scan();
	if (Dev_Now.TouchpointFlag && Dev_Now.TouchCount>0) {
		data->state = LV_INDEV_STATE_PRESSED;
		data->point.x = Dev_Now.X[0];
		data->point.y = Dev_Now.Y[0];
		Dev_Now.TouchpointFlag = 0;
	}
	else{
		data->state = LV_INDEV_STATE_RELEASED;
	}
}

lv_indev_t * touch_hal_create_indev(){
	static lv_indev_t* indev;
	indev = lv_indev_create();
	lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
        lv_indev_set_read_cb(indev, touch_hal_read);
	return indev;
}

