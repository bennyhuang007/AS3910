#include "spi_driver.h"
#include "errno.h"
#include "board.h"
#include "spi/spi.h"
#include "pio/pio.h"

//------------------------------------------------------------------------------
//! \brief spiInitialize.
//! \param *config
//			  frequency      spi frequency in Hz
//			  instance       spi instance
//            deviceId       spi device id(reserve)
//            clockPhase     0 or 1 - data centered on first or second edge of SCK period
//            clockPolarity  0 or 1 - SCK low or high when idle
//! \return ERR_NONE
//------------------------------------------------------------------------------
s8 spiInitialize(void)
{
    // IO Config
    PIO_Configure(&pin_as3910_miso,1);
    PIO_Configure(&pin_as3910_mosi,1);
    PIO_Configure(&pin_as3910_spck,1);
    PIO_Configure(&pin_as3910_sen,1);   // SEN
    PIO_Configure(&pin_as3910_en,1);    // EN
    
    // SPI Module Config
    SPI_Configure(AT91C_BASE_SPI0,AT91C_ID_SPI0,
                  AT91C_SPI_MSTR | AT91C_SPI_MODFDIS);
    // 外设信号选择 & 时钟电平控制
    SPI_ConfigureNPCS(AT91C_BASE_SPI0, 0, 
                      SPI_SCBR(100000,BOARD_MCK));
    SPI_Enable(AT91C_BASE_SPI0);
    
    return ERR_NONE;
}

//------------------------------------------------------------------------------
//! \brief spiDeinitialize.
//! \param none
//! \return ERR_NONE
//------------------------------------------------------------------------------
s8 spiDeinitialize()
{
    return ERR_NONE;
}

//------------------------------------------------------------------------------
//! \brief spiTransferNBytes.(without SEN signal control)
//! \param outbuf   Buffer to be transmitted
//         outlen   length of the data to be transmitted 
//         inbuf    Buffer where received data is written to 
//                  OR NULL in order to perform a write-only operation
//         inlen    length of the data to be received
//         sync     TRUE if function should not return before all bytes
//                  have been sent. FALSE if function can return sooner.
//! \return ERR_IO      Error during SPI communication, returned data may be unvalid.
//          ERR_NONE    No error, all length bytes transmitted/received.
//------------------------------------------------------------------------------
s8 spiTransferNBytes(const u8 *outbuf,u8 *inbuf, u8 length, bool_t sync)
{
    u8 counter;
    for(counter = 0;counter < length;counter++)
    {
        if(outbuf != NULL)
            SPI_Write(AT91C_BASE_SPI0,1,*(outbuf+counter));
        else
            SPI_Write(AT91C_BASE_SPI0,1,0x00);
        if(inbuf != NULL)	// 不需要接收数据
            *(inbuf+counter) = SPI_Read(AT91C_BASE_SPI0);
        else
            SPI_Read(AT91C_BASE_SPI0);	// Dummy Read
    }
    if(sync)
        spiSync();
    return ERR_NONE;
}

//------------------------------------------------------------------------------
//! \brief spiDeinitialize.
//! \param none
//! \return ERR_NONE
//------------------------------------------------------------------------------
s8 spiSync(void)
{
    // Dummy Read
    //SPI_Read(AT91C_BASE_SPI0);
    // Reset Error Flag
    // ......
    return ERR_NONE;
}

//------------------------------------------------------------------------------
//! \brief spiTxRxNSEN.(without SEN signal control)
//         Transfer 1 byte without SEN signal control
//! \param TxData   Data to transmi
//! \return Recv data
//------------------------------------------------------------------------------
s8 spiTxRxNSEN(u8 TxData)
{
    s8 Temp;
    
    SPI_Write(AT91C_BASE_SPI0,1,TxData);
    Temp = SPI_Read(AT91C_BASE_SPI0);
    
    return Temp;
}