#include <unistd.h> /*for usleep() (loop delay) */
#include "lvgl/lvgl.h"
#include "src/ui/ui.h"
#include "src/middleware/epd_hal.h"
#include "src/middleware/touch_hal.h"
#include "src/middleware/tick_hal.h"

int main(){

epd_hal_init();
touch_hal_init();

lv_init();

lv_tick_set_cb(tick_hal_get_ms);

epd_hal_create_display();
touch_hal_create_indev();
ui_init();
while(1) /* I have been waiting for sooooooo long to write this line. */
{
	lv_timer_handler();
	ui_tick();
	usleep(5000);
}
return 0;
}


