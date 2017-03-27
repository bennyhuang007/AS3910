#ifndef _SPI_DRIVER_H_
#define _SPI_DRIVER_H_

#include "ams_types.h"
#include "pio/pio.h"
#include "spi/spi.h"

#define AS3910_SENHigh() {PIO_Set(&pin_as3910_sen);}
#define AS3910_SENLow() {PIO_Clear(&pin_as3910_sen);}

typedef struct
{
    u32 frequency;      /*!< desired spi frequency in Hz */
    AT91PS_SPI instance;        /*!< desired spi instance */
    u8 deviceId;        /*!< desired spi device id (selected with cs) */
    u8 clockPhase;      /*!< 0 or 1 - data centered on first or second edge of SCK period */
    u8 clockPolarity;   /*!< 0 or 1 - SCK low or high when idle */
}spiConfig_t;

/* Variable declaration */
static const Pin pin_as3910_miso = PIN_SPI0_MISO;
static const Pin pin_as3910_mosi = PIN_SPI0_MOSI;
static const Pin pin_as3910_spck = PIN_SPI0_SPCK;
static const Pin pin_as3910_sen = PIN_AS3910_SEN;
static const Pin pin_as3910_en = PIN_AS3910_EN;
static const Pin pin_as3910_intr = PIN_AS3910_INTR;

/* Function declaration*/
s8 spiInitialize(void);
s8 spiDeinitialize();
s8 spiTransferNBytes(const u8 *outbuf, u8 *inbuf, u8 length, bool_t sync);
s8 spiSync(void);
s8 spiTxRxNSEN(u8 TxData);

#endif
