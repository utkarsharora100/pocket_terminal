#ifndef TICK_HAL_H
#define TICK_HAL_H
#include <stdint.h>
/* stdint is used to let us define exact width integer types */
uint32_t tick_hal_get_ms(void);
/* We have defined a function tick_hal_get_ms to get the time ellapsed for lvgl animations, touch responses etc. */
#endif
/* The ifndef and end if contains the include gaurd */
/*The actual function logic is defined in the .c file*/
