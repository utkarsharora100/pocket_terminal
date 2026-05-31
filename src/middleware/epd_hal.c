#include "epd_hal.h"
#include "EPD_2in13_V4.h"
#include "DEV_Config.h"
#include <string.h>
#include <pthread.h>
#include "GT1151.h"
#include "touch_hal.h"
/* string.h allows us to use memset for clearing the framebuffer */
static uint8_t epd_buf[16*250];
static int partial_refresh_counter = 5;
extern int IIC_Address;
void epd_hal_init(){
 IIC_Address = 0x14;
 DEV_ModuleInit();
 memset(epd_buf, 0xFF, sizeof(epd_buf));
 pthread_create(&touch_thread, NULL, touch_irq_thread, NULL);
 EPD_2in13_V4_Init(EPD_2IN13_V4_FULL);
 EPD_2in13_V4_Clear();
 GT_Init();
 DEV_Delay_ms(100);
 EPD_2in13_V4_Sleep();

}

/* LVGL renders its own internal buffer ("draw_buff"). When its done rendering an area it calls this function and send the
pixels to the screen and we will need a conversion because LVGL gives 1 byte per pixel and   
waveshare needs 1 bit per pixel , 8 pixels are packed into 1 bytes */

/* lv_display_t *disp is the display object*/
/* const lv_area_t *area is a rectangle describing whch part of the screen LVGL rendered*/

/* 250 pixels / 8 pixels per byte = 31.25 bytes that is 32 */
/* Eink uses MSB therefore we start from 7th index of the byte sequence */
void epd_hal_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{  
   printf("flush called: x1=%d y1=%d x2=%d y2=%d\n", area->x1, area->y1, area->x2, area->y2);
   int stride = (16);  // LVGL actual stride = 32 bytes
   int w = area->x2 - area->x1 + 1;

    for (int y = area->y1; y <= area->y2; y++) {
        for (int x = area->x1; x <= area->x2; x++) {
            if (x >= 250) continue;
        
        int local_x = x - area->x1;
        int local_y = y - area->y1;

        int src_byte = local_y * stride + local_x / 8;
        int src_bit  = 7 - (local_x % 8);
        int pixel    = (px_map[src_byte] >> src_bit) & 1;

        int dst_byte = y * 16 + x / 8;
        int dst_bit  = 7 - (x % 8);

        if (pixel)
            epd_buf[dst_byte] &=  ~(1 << dst_bit);
        else
            epd_buf[dst_byte] |= (1 << dst_bit);
    }
}
       // count dark pixels
        int dark = 0;
        for (int i = 0; i < 32 * 122; i++) {
           if (epd_buf[i] != 0xFF) dark++;
        }
        printf("dark bytes in buf: %d\n", dark);
	if (lv_display_flush_is_last(disp))
	{
		partial_refresh_counter++;
                printf("pushing to display, counter=%d\n", partial_refresh_counter);

		if (partial_refresh_counter >=5)
			{
				EPD_2in13_V4_Init(EPD_2IN13_V4_FULL);
				EPD_2in13_V4_Display(epd_buf);
				EPD_2in13_V4_Sleep();
				partial_refresh_counter=0;
			}
		else
		{
			EPD_2in13_V4_Init(EPD_2IN13_V4_PART);
			EPD_2in13_V4_Display_Partial_Wait(epd_buf);
			EPD_2in13_V4_Sleep();
		}
	}
	lv_display_flush_ready(disp);
}
lv_display_t* epd_hal_create_display(){
	static lv_display_t *disp;
	static uint8_t draw_buf[250*122];
	disp = lv_display_create(122,250);
	lv_display_set_buffers(disp, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
	lv_display_set_flush_cb(disp, epd_hal_flush);
	lv_display_set_color_format(disp, LV_COLOR_FORMAT_I1);
	return disp;
}

