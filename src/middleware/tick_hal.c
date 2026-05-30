#include "tick_hal.h"
/* "" is used because we are importing our own library and not the standard ones*/
#include <time.h>

uint32_t tick_hal_get_ms(void)
{
 struct timespec ts;
 /* tv_sec for seconds tv_nsec for nanoseconds so we define our function to store time in high resolution that's it. */
 clock_gettime(CLOCK_MONOTONIC,&ts);
 return (uint32_t)(ts.tv_sec*1000 + ts.tv_nsec / 1000000); 
}
