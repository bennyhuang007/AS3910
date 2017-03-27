/*
*****************************************************************************
* Copyright @ 2009 by austriamicrosystems AG                                *
* All rights are reserved.                                                  *
*                                                                           *
* Reproduction in whole or in part is prohibited without the written consent*
* of the copyright owner. Austriamicrosystems reserves the right to make    *
* changes without notice at any time. The software is provided as is and    *
* Austriamicrosystems makes no warranty, expressed, implied or statutory,   *
* including but not limited to any implied warranty of merchantability or   *
* fitness for any particular purpose, or that the use will not infringe any *
* third party patent, copyright or trademark. Austriamicrosystems should    *
* not be liable for any loss or damage arising from its use.                *
*****************************************************************************
*/

/*
* PROJECT: AS3910 firmware
* $Revision: $
* LANGUAGE: ANSI C
*/

/*! \file as3910_interrupt.c
*
* \author Oliver Regenfelder
*
* \brief AS3910 interrupt handling and ISR
*/

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

#include "as3910_interrupt.h"
#include "timer_driver.h"
#include "errno.h"
#include "stdio.h"
#include "as3910_def.h"
#include "as3910_io.h"
/*
******************************************************************************
* DEFINES
******************************************************************************
*/

/*
******************************************************************************
* MACROS
******************************************************************************
*/

/*!
*****************************************************************************
* \brief Clear the interrupt flag associated with the as3910 interrupt.
*****************************************************************************
*/
#define AS3910_IRQ_CLR() { NVIC_ClearPendingIRQ(IROn_PIOA);}

/*!
*****************************************************************************
* \brief Evaluates to true if there is a pending interrupt request from the
* AS3910.
*****************************************************************************
*/
#define AS3910_IRQ_IS_SET() (NVIC_GetPendingIRQ(IROn_PIOA)!= 0)

/*
******************************************************************************
* LOCAL DATA TYPES
******************************************************************************
*/

/*
******************************************************************************
* LOCAL VARIABLES
******************************************************************************
*/

/*! AS3910 interrutp mask. */
static volatile u8 as3910InterruptMask = 0;
/*! Accumulated AS3910 interrupt status. */
static volatile u8 as3910InterruptStatus = 0;

/*
******************************************************************************
* LOCAL TABLES
******************************************************************************
*/

/*
******************************************************************************
* LOCAL FUNCTION PROTOTYPES
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL VARIABLE DEFINITIONS
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL FUNCTIONS
******************************************************************************
*/
s8 as3910EnableInterrupts(u8 mask)
{
    s8 error = ERR_NONE;
	u8 irqMask = 0;
	
    AS3910_IRQ_OFF();
	
    error |= as3910ReadRegister(AS3910_MASK_INTERRUPT,&irqMask);
    irqMask &= ~mask;
	as3910InterruptMask = ~irqMask;
    as3910InterruptMask |= mask;
    error |= as3910WriteRegister(AS3910_MASK_INTERRUPT,irqMask);
	
    AS3910_IRQ_ON();
	
    if (ERR_NONE == error)
        return ERR_NONE;
    else
        return ERR_IO;
}

s8 as3910DisableInterrupts(u8 mask)
{
    s8 error = ERR_NONE;
    u8 irqMask = 0;
	
    AS3910_IRQ_OFF();
	
    error |= as3910ReadRegister(AS3910_MASK_INTERRUPT,&irqMask);
    irqMask |= mask;
	as3910InterruptMask = ~irqMask;
    as3910InterruptMask &=  ~mask;
    error |= as3910WriteRegister(AS3910_MASK_INTERRUPT,irqMask);
	
    AS3910_IRQ_ON();
	
    if (ERR_NONE == error)
        return ERR_NONE;
    else
        return ERR_IO;
}

s8 as3910ClearInterrupts(u8 mask)
{
    s8 error = ERR_NONE;
    u8 irqStatus = 0;
	
    AS3910_IRQ_OFF();
	
    error |= as3910ReadRegister(AS3910_INTERRUPT_STATUS,&irqStatus);
    as3910InterruptStatus |= irqStatus & as3910InterruptMask;
    as3910InterruptStatus &= ~mask;
	
    AS3910_IRQ_ON();
	
    if (ERR_NONE == error)
        return ERR_NONE;
    else
        return ERR_IO;
}

s8 as3910WaitForInterruptTimed(u8 mask, u16 timeout, u8 *irqs)
{
    bool_t timerExpired = FALSE;
    u8 irqStatus = 0;
	
    if (timeout > 0)
        timerStart(timeout);
	
    do
    {
        irqStatus = as3910InterruptStatus & mask;
		
        if (timeout > 0)
        {
            if (!timerIsRunning())
                timerExpired = TRUE;
        }
    } while (!irqStatus && !timerExpired);
	
    AS3910_IRQ_OFF();
    as3910InterruptStatus &= ~irqStatus;
    AS3910_IRQ_ON();
	
    *irqs = irqStatus;
	if(timerExpired)
		return ERR_TIMEOUT;
	else
		return ERR_NONE;
}

s8 as3910GetInterrupts(u8 mask, u8 *irqs)
{
    AS3910_IRQ_OFF();
	
    *irqs = as3910InterruptStatus & mask;
    as3910InterruptStatus &= ~mask;
	
    AS3910_IRQ_ON();
	
    return ERR_NONE;
}

// AS3910 ÖÐ¶ÏÏìÓ¦º¯Êý
void PIOA31_IRQHandler(const Pin *IrqPin)
{
    u8 irqStatus = 0;
    do
    {
        AS3910_IRQ_CLR();
        
        as3910ReadRegister(AS3910_INTERRUPT_STATUS,&irqStatus);
        as3910InterruptStatus |= irqStatus & as3910InterruptMask;
    }
    while (AS3910_IRQ_IS_SET());
#ifdef DEBUG
    printf("IRQ Status: %X,REG:%X\r\n",as3910InterruptStatus,irqStatus);
#endif
}

/*
******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************
*/
