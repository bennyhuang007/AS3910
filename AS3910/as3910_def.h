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
 
/*! \file as3910_def.h
 *
 * \author Oliver Regenfelder
 *
 * \brief AS3910 register and direct command definitions.
 */

#ifndef _AS3910_DEF_H_
#define _AS3910_DEF_H_

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

#include "ams_types.h"

/*
******************************************************************************
* DEFINES
******************************************************************************
*/

/*! \name AS3910 register addresses */
/**@{*/
/*! \ingroup as3910 */

/* AS3910 registers. */
#define AS3910_ISO_MODE_DEF                 0x00
#define AS3910_OPERATION_CONTROL            0x01		// RW Operation control register
#define AS3910_CONFIGURATION2               0x02		// RW Configuration register 2
#define AS3910_CONFIGURATION3               0x03		// RW Configuration register 3 (ISO 14443A and NFC)
#define AS3910_CONFIGURATION4               0x04		// RW Configuration register 4 (ISO 14443B)
#define AS3910_CONFIGURATION5               0x05		// RW Configuration register 5
#define AS3910_RECEIVER_CONF                0x06		// RW Receiver configuration register 6
#define AS3910_MASK_INTERRUPT               0x07		// RW Mask interrupt register
#define AS3910_INTERRUPT_STATUS             0x08		// R  Interrupt register
#define AS3910_FIFO_STATUS                  0x09		// R  FIFO status register
#define AS3910_COLLISION                    0x0A		// R  Collision register (for 14443A only)
#define AS3910_NUM_OF_TRANS0                0x0B		// RW Number of transmitted bytes register 0
#define AS3910_NUM_OF_TRANS1                0x0C		// RW Number of transmitted bytes register 1
#define AS3910_AD_OUTPUT                    0x0D		// R  A/D output register
#define AS3910_ANTENNA_CALIBRATION          0x0E		// R  Antenna calibration register
#define AS3910_EXTERNAL_TRIM                0x0F		// RW External trim register
#define AS3910_MODULATION_DEPTH_DEF         0x10		// RW NFCIP field detection threshold register
#define AS3910_REFULATORS_DISPLAY           0x15		// R Regulators Display Register

/**@}*/

/*! \name AS3910 direct commands */
/**@{*/
/*! \ingroup as3910 */
/* AS3910 direct commands. */
#define AS3910_DC_SET_DEFAULT               0x01		// Puts the chip in default state (same as after power-up)
#define AS3910_DC_CLEAR_FIFO                0x02		// Stops all activities and clears FIFO
#define AS3910_DC_TX_W_CRC                  0x04		// Transmit with CRC
#define AS3910_DC_TX_WO_CRC                 0x05		// Transmit without CRC
#define AS3910_DC_TX_REQA                   0x06		// Transmit REQA
#define AS3910_DC_TX_WUPA                   0x07		// Transmit WUPA
#define AS3910_DC_INITIAL_RF_COLLISION      0x08		// NFC transmit with Initial RF Collision Avoidance
#define AS3910_DC_RESPONSE_RF_COLLISION_N   0x09		// NFC transmit with Response RF Collision Avoidance
#define AS3910_DC_RESPONSE_RF_COLLISION_0   0x0A		// NFC transmit with Response RF Collision Avoidance with n=0
#define AS3910_DC_MASK_RECEIVE_DATA         0x10		// Mask recive data
#define AS3910_DC_UNMASK_RECEIVE_DATA       0x11		// Unmask recive data
#define AS3910_DC_RF_MEASUREMENT            0x12		// Start-up with RF measurment
#define AS3910_DC_MEASURE_RF                0x13		// Measure Rf
#define AS3910_DC_SQUELCH                   0x14		// Squelch
#define AS3910_DC_CLEAR_SQUELCH             0x15		// Clear Squelch
#define AS3910_DC_ADJUST_REGULATORS         0x16		// Adjust regulators
#define AS3910_DC_CALIBRATE_MODULATION      0x17		// Calibrate modulation depth
#define AS3910_DC_CALIBRATE_ANTENNA         0x18		// Calibrate antenna
#define AS3910_DC_CHECK_ANTENNA_RESONANCE   0x19		// check antenna resonsnce
#define AS3910_DC_MEASURE_RSSI              0x1A		// clear RSSI bits and restart the measurement
#define AS3910_DC_TRANSPARENT_MODE          0x1C		// Transparent mode

/**@}*/

/*! \name AS3910 mifare commands */
/**@{*/
/*! \ingroup as3910 */
/* AS3910 mifare commands. */
#define AS3910_FC_AUTHEN_KEYA               0x60
#define AS3910_FC_AUTHEN_KEYB               0x61

/**@}*/

#endif /* AS3910_DEF_H */
