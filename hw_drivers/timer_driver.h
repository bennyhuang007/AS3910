#ifndef _TIMER_DRIVER_H_
#define _TIMER_DRIVER_H_

#include "ams_types.h"
#include "AT91SAM3U4.h"
#include "irq/irq.h"
#include "tc/tc.h"
#include "pio/pio.h"

static const Pin pin_extclk = PIN_EXTCLOCK;

extern void SysTick_Handler(void);
void timerInitialize(void);
void timerStart( u16 mSec );
void timerStop(void);
BOOL timerIsRunning( void);
//------------------------------------
void TC0_Config(void);

#endif
