#ifndef _MAIN_H_
#define _MAIN_H_

#include "board.h"

#include "pio/pio.h"
#include "pio/pio_it.h"
#include "irq/irq.h"
#include "tc/tc.h"

#include "timer_driver.h"
#include "sleep.h"

#include "as3910_io.h"
#include "as3910_com.h"

#include "other.h"

#include "as3910_transparentmode.h"

#include <utility/trace.h>
#include <stdio.h>

#if defined(cortexm3)
#include <systick/systick.h>
#endif

#define APPL_COM_EMV_TOGGLE_CARRIER 0xE0 /*!< EMV TTA L1 carrier button pressed. */
#define APPL_COM_EMV_POLL           0xE1 /*!< EMV TTA L1 poll button prossed. */
#define APPL_COM_EMV_RESET          0xE2 /*!< EMV TTA L1 reset button presed. */
#define APPL_COM_EMV_WUPA           0xE3 /*!< EMV TTA L1 WUPA button pressed. */
#define APPL_COM_EMV_WUPB           0xE4 /*!< EMV TTA L1 WUPB button pressed. */
#define APPL_COM_EMV_RATS           0xE5 /*!< EMV TTA L1 RATS button pressed. */
#define APPL_COM_EMV_ATTRIB         0xE6 /*!< EMV TTA L1 ATTRIB button pressed. */
#define APPL_COM_EMV_PREVALIDATION  0xE7 /*!< EMV TTA L1 prevalidation test start button pressed. */
#define APPL_COM_EMV_DIGITAL        0xE8 /*!< EMV TTA L1 digital test start button pressed. */
#define APPL_COM_EMV_STOP           0xEA /*!< EMV TTA L1 prevalidation or digital test application stop button pressed. */
#define APPL_COM_EMV_INIT           0xEF /*!< EMV mode initialization command. */
#define APPL_COM_REG_DUMP           0xF0 /*!< Dump register content. */
#define APPL_COM_REG_WRITE          0xF1 /*!< Write register. */
#define APPL_COM_REG_READ           0xF2 /*!< Read register. */
#define APPL_COM_DIRECT_COMMAND     0xF3 /*!< Execute an AS3911 direct command. */

static const Pin pin_led = PIN_LED;

void AS3910_IntrConfig(void);
void displayRegisterValue(u8 address);
void displayTestRegisterValue(u8 address);
void EMV_Init(void);

#endif