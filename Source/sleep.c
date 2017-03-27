#include "sleep.h"
#include "timer_driver.h"

void sleepMilliseconds(u16 milliseconds)
{
    timerStart(milliseconds);
    while(timerIsRunning());
}

