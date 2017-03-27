#include "timer_driver.h"

#define TIMER_MAX_VALUE (SYSTICK_MAXCOUNT / 6000)   /* real max is 1048 */

static volatile s8 timer1Running;

/* timer value - useful for timer values > 1000 */
static volatile u16 timerValue;

void SysTick_Handler(void)
{
    if ( timerValue > 0 )
    {
        /* reload */
        if ( timerValue < TIMER_MAX_VALUE )
        {
            SysTick->LOAD = ( uint32_t )timerValue*6000;   // Reload
            timerValue = 0; /* end of outer loop */
        }
        else /* not the last time we let timer overflow */
        {
            timerValue -= TIMER_MAX_VALUE;
        }
    }
    else /* timer has expired */
    {
        timerStop();
    }
}

// Config SysTick Clock Source
void timerInitialize(void)
{
    // Clock Source = MCLK / 8
    SysTick->CTRL &= (1 << SYSTICK_CLKSOURCE);
}

// Stop Systick
void timerStop(void)
{
    SysTick->CTRL &= (1 << SYSTICK_ENABLE);    // Counter Stop
    SysTick->VAL = 0x00;                // A write of any value clears the field
    timer1Running = 0;                  // Flag
}

// Start Systick
void timerStart( u16 mSec )
{
    timerValue = mSec;
    if ( mSec > TIMER_MAX_VALUE ) /* cannot handle values bigger than 1048 */
        mSec = TIMER_MAX_VALUE;

    timerValue -= mSec;
    timer1Running = 1;    /* set running flag */

    NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);  /* set Priority for Systick Interrupt */
    /* Load the SysTick Counter Value */
    SysTick->CTRL |= (1 << SYSTICK_TICKINT);  /* Enable SysTick IRQ */
    
    SysTick->LOAD = (uint32_t)mSec*6000;
    SysTick->VAL &= 0x00;
    SysTick->CTRL |= (1 << SYSTICK_ENABLE);
}

BOOL timerIsRunning(void)
{
    return (BOOL)( timer1Running == 1 );
}

//------------------------------------------------------------------------------
// 配置TC0，中断频率 = 13.56MHz / 64
void TC0_Config(void)
{
    // 使能外设时钟
    AT91C_BASE_PMC->PMC_PCER = (1 << AT91C_ID_TC0) | (1 << AT91C_ID_PIOA);
    // 外部输入时钟引脚配置
    PIO_Configure(&pin_extclk,1);
    // TC Config
    AT91C_BASE_TCB0->TCB_TC0.TC_CCR = AT91C_TC_CLKDIS;      // Counter Clk Disable
    AT91C_BASE_TCB0->TCB_TC0.TC_CMR = AT91C_TC_CLKS_XC2 | AT91C_TC_WAVESEL_UP_AUTO | AT91C_TC_WAVE;
    AT91C_BASE_TCB0->TCB_TC0.TC_RC = 64;
    // Interrupt Config
    AT91C_BASE_TCB0->TCB_TC0.TC_SR;        // Clear Status
    AT91C_BASE_TC0->TC_IER = 0x10;
    IRQ_EnableIT(AT91C_ID_TC0);
}
