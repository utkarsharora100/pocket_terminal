#include "epd_hal.h"
#include "EPD_2in13_V4.h"
#include "DEV_Config.h"
#include <string.h>
#include <pthread.h>
#include "GT1151.h"
#include "touch_hal.h"
/* string.h allows us to use memset for clearing the framebuffer */
static uint8_t epd_buf[250*122/8];
static int partial_refresh_counter = 0;
extern int IIC_Address;
void epd_hal_init(){
 IIC_Address = 0x14;
 DEV_ModuleInit();
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
  for (int y=area->y1;y<=area->y2;y++){
		for(int x = area->x1;x<=area->x2;x++){
			int byte_index = y *32 + x/8;
			int bit_pos = 7 - (x%8);

			if (*px_map!=0x00){
					epd_buf[byte_index] |= (1 << bit_pos);
				}
			else{
				epd_buf[byte_index] &= ~( 1<< bit_pos);
				}
			px_map++;
		}
	}

	if (lv_display_flush_is_last(disp))
	{
		partial_refresh_counter++;

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
			EPD_2in13_V4_Display_Partial(epd_buf);
			EPD_2in13_V4_Sleep();
		}
	}
	lv_display_flush_ready(disp);
}
lv_display_t* epd_hal_create_display(){
	static lv_display_t *disp;
	static uint8_t draw_buf[250*122];
	disp = lv_display_create(250,122);
	lv_display_set_buffers(disp, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
	lv_display_set_flush_cb(disp, epd_hal_flush);
	lv_display_set_color_format(disp, LV_COLOR_FORMAT_I1);
	return disp;
}

