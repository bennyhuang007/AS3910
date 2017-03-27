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
 
/*! \file as3910_interrupt.h
 *
 * \author Oliver Regenfelder
 *
 * \brief AS3910 interrupt handling.
 */

/*! \defgroup as3910IrqHandling AS3910 Interrupt Handling
 * \ingroup as3910
 *
 * \brief This part of the AS3910 module abstracts AS3910 interrupt handling.
 *
 * The AS3910 interrupt logic consists of three byte registers to mask or
 * unmask interupt
 * sources and three additional registers to indicate which interrupts are
 * pending. The content of this pending interrupt registers is automatically
 * cleared on a register read. Additionally a single interrupt line is used
 * to signal any interrupt pending interrupt condition to the microcontroller.
 *
 * This module abstracts this so the user no longer needs to know to which
 * interrupt register an interrupt source belongs to. To achive this a flat
 * hirarchy of interrupt masks is provided. This module also serves any
 * interrupt requests from the AS3910 and accumulates the pending interrupt
 * requests until the user reads out the interrupt status (as3910GetInterrupts(),
 * as3910WaitForInterruptTimed()).
 *
 * \section sec_1 Enabling and disabling processing of interrupt request from the AS3910
 *
 * The macros AS3910_IRQ_ON() and AS3910_IRQ_OFF() enable or disable processing
 * of AS3910 interrupt requests by the PIC controller (and thus by this module).
 * If interrupt processing is disabled via AS3910_IRQ_OFF, then no interrupt
 * request will reach the software even if the interrupt source is enabled via
 * as3910EnableInterrupts().
 *
 * Add startup processing of interrupts by the microcontroller is enabled and
 * therefore AS3910_IRQ_ON() needs to called at least once to enable AS3910
 * interrupt processing.
 *
 * \section sec_2 Enabling and disabling interrupts.
 *
 * The functions as3910EnableInterrupts() and as3910DisableInterrupts()
 * can be used to enable or disable specific AS3910 interrupt sources.
 * 
 * \section sec_3 Retreiving and reseting the interrupt status.
 *
 * The function as3910GetInterrupts() can be used to retreive the interrupt
 * status of any combination of AS3910 interrupt source(s). If an interrupt
 * from a source is pending \em and this interrupt is read out via
 * as3910GetInterrupts() then the penging interrupt is automatically
 * cleared. So a subsequent call to
 * as3910GetInterrupt() will mark that source as not pending (if no additional
 * interrupt happened in between).
 *
 * The function as3910ClearInterrupts() can be used to clear the interrupt
 * status of an interrupt source.
 *
 * The function as3910WaitForInterruptTimed can be used to wait for any
 * interrupt out of a set of interrupts to occure. Additionally a timeout can
 * be specified. This function blocks until at least one of the selected
 * interrupts occure or the timeout expires. This function does not enable
 * or disable interrupts. So any interrupt source to wait for needs to be
 * enabled prior to the call via as3910EnableInterrupts.
 */

#ifndef _AS3910_INTERRUPT_H_
#define _AS3910_INTERRUPT_H_

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

// #include <p24Fxxxx.h>
#include "AT91SAM3U4.h"
#include "board.h"
#include "pio/pio.h"
#include "irq/irq.h"
#include "ams_types.h"

/*
******************************************************************************
* DEFINES
******************************************************************************
*/

/*! \name Interrupt masks */
/**@{*/
/*! \ingroup as3910IrqHandling */
/* Main interrupt register. */
#define AS3910_IRQ_MASK_ALL             0xFF     /*!< All AS3910 interrupt sources. */
#define AS3910_IRQ_MASK_NONE            0        /*!< No AS3910 interrupt source. */
#define AS3910_IRQ_MASK_OSC             0x80     /*!< AS3910 oscillator stable interrupt. */
#define AS3910_IRQ_MASK_NFC             0x40     /*!< AS3910 FIFO NFC event inerrupt. */
#define AS3910_IRQ_MASK_WL              0x20     /*!< AS3910 FIFO water level inerrupt. */
#define AS3910_IRQ_MASK_RXE             0x10     /*!< AS3910 end of receive interrupt. */
#define AS3910_IRQ_MASK_TXE             0x08     /*!< AS3910 end of transmission interrupt. */
#define AS3910_IRQ_MASK_ERR             0x04     /*!< AS3910 parity error and/or framing error interrupt. */
#define AS3910_IRQ_MASK_CRC             0x02     /*!< AS3910 CRC error interrupt. */
#define AS3910_IRQ_MASK_COL             0x01     /*!< AS3910 bit collision interrupt. */

/**@}*/

/*
******************************************************************************
* GLOBAL MACROS
******************************************************************************
*/

/*! \ingroup as3910IrqHandling
 *****************************************************************************
 * \brief Enable processing of AS3910 interrupts by the PIC.
 *****************************************************************************
 */
#define AS3910_IRQ_ON()  { IRQ_EnableIT(AT91C_ID_PIOA);}
/*! \ingroup as3910IrqHandling
 *****************************************************************************
 *  \brief Disable processing of AS3910 interrupts by the PIC.
 *****************************************************************************
 */
#define AS3910_IRQ_OFF() { IRQ_DisableIT(AT91C_ID_PIOA);}


/*
******************************************************************************
* GLOBAL DATA TYPES
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL VARIABLE DECLARATIONS
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL FUNCTION PROTOTYPES
******************************************************************************
*/

/*! \ingroup as3910IrqHandling
 *****************************************************************************
 * \brief Enable certain AS3910 interrupts.
 *
 * \param[in] mask The interrupts to enable.
 *
 * \return ERR_IO: Error during communication.
 * \return ERR_NONE: No error, the given interrupts are enabled.
 *****************************************************************************
 */
s8 as3910EnableInterrupts(u8 mask);

/*! \ingroup as3910IrqHandling
 *****************************************************************************
 * \brief Disable certain AS3910 interrupts.
 *
 * \param[in] mask The interrupts to enable.
 *
 * \return ERR_IO: Error during communication.
 * \return ERR_NONE: No error, the given interrupts are disabled.
 *****************************************************************************
 */
s8 as3910DisableInterrupts(u8 mask);

/*! \ingroup as3910IrqHandling
 *****************************************************************************
 * \brief Clear pending AS3910 interrupts.
 *
 * \param[in] mask The interrupts to clear.
 *
 * \return ERR_IO: Error during communication.
 * \return ERR_NONE: No error, the given interrupts are cleard.
 *****************************************************************************
 */
s8 as3910ClearInterrupts(u8 mask);

/*! \ingroup as3910IrqHandling
 *****************************************************************************
 * \brief Wait for certain AS3910 interrupts.
 *
 * \param[in] mask The interrupts to wait for.
 * \param[in] timeout
 * \li timeout > 0: Wait operation timeout (milliseconds).
 * \li timeout == 0: No timeout, the function call blocks until one
 * of the interrupts defined by \a mask occur.
 * \param[out] irqs Set to the interrupt(s) wich occured.
 *
 * \note Any interrupt that occurs which is not selected by mask will not
 * cause the wait operation to terminate and will also not be reported in
 * \a irqs after the function call returns.
 * \note All reported interrupts are automatically cleared.
 *
 * \return ERR_NONE: No error, one of the defined interrupts occured or
 * the timeout expired.
 *****************************************************************************
 */
s8 as3910WaitForInterruptTimed(u8 mask, u16 timeout, u8 *irqs);

/*! \ingroup as3910IrqHandling
 *****************************************************************************
 * \brief Get interrupt status of certain interrupts of the AS3910.
 *
 * \param[in] mask The interrupts which status has to be checked.
 * \param[out] irqs Set to the interrupts which are currently pending.
 *
 * \note A pending interrupt that is not selected for readout by \a mask
 * will not be reported.
 * \note All reported interrupts are automatically cleared.
 *
 * \return ERR_NONE: No error, interrupt status read out successful.
 *****************************************************************************
 */
s8 as3910GetInterrupts(u8 mask, u8 *irqs);

void PIOA31_IRQHandler(const Pin *IrqPin);

#endif /* AS3910_INTERRUPT_H */
