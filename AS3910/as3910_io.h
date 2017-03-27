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
 
/*! \file as3910_io.h
 *
 * \author Oliver Regenfelder
 *
 * \brief AS3910 SPI communication.
 */

/*! \defgroup as3910SpiIo AS3910 SPI IO
 * \ingroup as3910
 *
 * \brief This part of the AS3910 module provides access to the AS3910 via the
 * SPI.
 *
 * This module abstracts the SPI interface of the AS3910. It contains functions
 * to read or write single registers:
 * - as3910ReadRegister()
 * - as3910WriteRegister()
 *
 * to read or write a continuous block of registers in a single SPI operation
 * (using AS3910 auto increment mode):
 * - as3910ContinuousRead()
 * - as3910ContinuousWrite()
 * 
 * to modify a register (performs a read, modify, write operation):
 * - as3910ModifyRegister()
 *
 * to access the FIFO of the AS3910:
 * - as3910ReadFIFO()
 * - as3910WriteFIFO()
 *
 * to send direct commands to the AS3910:
 * - as3910ExecuteCommand()
 *
 * and to access the test mode registers:
 * - as3910ReadTestRegister()
 * - as3910WriteTestRegister()
 *
 * All SPI data transfers are guaranteed to be completed after any of the
 * function returns. But do note that the execution of a direct command might
 * take some time to complete even after the transmission of that command is
 * completed.
 */

#ifndef _AS3910_IO_H_
#define _AS3910_IO_H_

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

#include "ams_types.h"
#include "board.h"
#include "errno.h"
#include "spi_driver.h"
#include "core_cm3.h"

/*
******************************************************************************
* DEFINES
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL MACROS
******************************************************************************
*/

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

/*! \ingroup as3910SpiIo
 *****************************************************************************
 * \brief Write to a register of the AS3910.
 *
 * \param[in] address Address of the register.
 * \param[in] data The data to write to regsiter \a address.
 *
 * \return ERR_IO: Error during communication.
 * \return ERR_NONE: No error, register write operaiton successful.
 *****************************************************************************
 */
s8 as3910WriteRegister(u8 address, u8 data);

/*! \ingroup as3910SpiIo
 *****************************************************************************
 * \brief Read a register of the AS3910.
 *
 * \param[in] address Address of the register to read out.
 * \param[out] data Set to the content of register \a address.
 * 
 * \return ERR_IO: Error during communication.
 * \return ERR_NONE: No error, register read operation successful.
 *****************************************************************************
 */
s8 as3910ReadRegister(u8 address, u8 *data);

/*! \ingroup as3910SpiIo
 *****************************************************************************
 * \brief Modify certain bits of a register while not touching other bits of
 * the same register.
 *
 * All register bits where the respective \a mask bits are one are set to
 * the value defined by \a value. In boolean notation this is equivalent to:
 * register = (register & ~mask) | value;
 *
 * \note This is implemented via a read modify write operation.
 *
 * \param[in] address Address of the register modify.
 * \param[in] mask Defines the bit which are modified.
 * \param[in] value New value of the bits selected for modification by \a mask.
 * 
 * \return ERR_IO: Error during communication.
 * \return ERR_NONE: No error, register modify operation successful.
 *****************************************************************************
 */
s8 as3910ModifyRegister(u8 address, u8 mask, u8 value);

/*! \ingroup as3910SpiIo
 *****************************************************************************
 * \brief Write to a continuous set of registers of the AS3910 using auto
 * increment mode.
 *
 * \param[in] address Start address of the continuous write operation.
 * \param[in] data The data to write to the registers \a address to \a address
 * + \a length - 1
 * \param[in] length Number of bytes to write to the AS3910.
 *
 * \return ERR_IO: Error during communication.
 * \return ERR_NONE: No error, continuous write operation successful.
 *****************************************************************************
 */
s8 as3910ContinuousWrite(u8 address, const u8 *data, u8 length);

/*! \ingroup as3910SpiIo
 *****************************************************************************
 * \brief Read a continuous set of registers of the AS3910 using auto
 * increment mode.
 *
 * \param[in] address Start address of the continuous read operation.
 * \param[out] data Set to the content of the registers \a addresss to \a address
 * + \a length - 1.
 * \param[in] length Number of bytes to read from the AS3910.
 *
 * \note The array pointed to by \a data must be large enough to hold at least
 * \a length bytes.
 *
 * \return ERR_IO: Error during communication.
 * \return ERR_NONE: No error, continuous read operation successful.
 *****************************************************************************
 */
s8 as3910ContinuousRead(u8 address, u8 *data, u8 length);

/*! \ingroup as3910SpiIo
 *****************************************************************************
 * \brief Write to the FIFO of the AS3910.
 *
 * \param[in] data The data to write into the FIFO.
 * \param[in] length Number of bytes to write into the FIFO.
 *
 * \return ERR_IO: Error during communication.
 * \return ERR_NONE: No error, FIFO write operation successful.
 *****************************************************************************
 */
s8 as3910WriteFIFO(const u8 *data, u8 length);

/*! \ingroup as3910SpiIo
 *****************************************************************************
 * \brief Read from the FIFO of the AS3910.
 *
 * \param[out] data Stores the data read from the FIFO.
 * \param[in] length Number of bytes to read from the FIFO.
 * 
 * \note The array pointed to by \a data must be large enought to
 * hold at least \a length bytes.
 *
 * \return ERR_IO: Error during communication.
 * \return ERR_NONE: No error, FIFO read operation successful.
 *****************************************************************************
 */
s8 as3910ReadFIFO(u8 *data, u8 length);

/*! \ingroup as3910SpiIo
 *****************************************************************************
 * \brief Send a direct command to the AS3910.
 *
 * \param[in] directCommand Direct command to send to the AS3910.
 *
 * \return ERR_IO: Error during communication.
 * \return ERR_NONE: No error, Direct command send operation successful.
 *****************************************************************************
 */
s8 as3910ExecuteCommand(u8 directCommand);


s8 as3910TransModeCommand(void);

#endif /* AS3910_IO_H */
