#include "as3910_transparentmode.h"
#include "spi_driver.h"
#include "pio/pio.h"

structSendData gSendData;
u16 iSendCount;
volatile u8 Flag_SendOver;

s8 as3910TransModeEnter(void)
{
    // Disable CRC & Parity Checking
    as3910ModifyRegister(AS3910_CONFIGURATION3,0xC0,0xC0);
    // Clear FIFO
    as3910ExecuteCommand(AS3910_DC_CLEAR_FIFO);
    // Mask Recv Data
    as3910ExecuteCommand(AS3910_DC_MASK_RECEIVE_DATA);
    // 发送Transparent模式切换命令(MCU不能控制MOSI引脚空闲状态，所以只能这么搞了)
    __disable_irq();
    AS3910_SENHigh();
    spiTxRxNSEN(0xDC);
    __enable_irq();
    // SPI I/O Mode Change
    PIO_SetInput(pin_as3910_miso.pio,pin_as3910_miso.mask,1,0);     // MISO
    PIO_SetOutput(pin_as3910_mosi.pio,pin_as3910_mosi.mask,0,0,0);  // MOSI
    PIO_SetOutput(pin_as3910_spck.pio,pin_as3910_spck.mask,0,0,0);  // SEN
    AS3910_SENLow(); //<---注意位置要在IO模式切换后
    return ERR_NONE;
}

s8 as3910TransModeLeave(void)
{
    AS3910_SENHigh();
    // SPI I/O Mode Change
    PIO_SetPeripheralA(pin_as3910_miso.pio,pin_as3910_miso.mask,0);  // MISO
    PIO_SetPeripheralA(pin_as3910_mosi.pio,pin_as3910_mosi.mask,0);  // MOSI
    PIO_SetPeripheralA(pin_as3910_spck.pio,pin_as3910_spck.mask,0);  // SEN
    // Unmask Recv Data
    as3910ExecuteCommand(AS3910_DC_UNMASK_RECEIVE_DATA);
    
    return ERR_NONE;
}

void as3910TransModeConfig(void)
{
    // TC config
    TC0_Config();
}

// LSB->MSB
s8 as3910TransModeSendNBit(u8 *pData,u16 cLenght_Bit)
{
    gSendData.Data = pData;
    gSendData.Lenght_Bit = cLenght_Bit;
    iSendCount = 0;
    Flag_SendOver = 0;
    pin_as3910_mosi.pio->PIO_CODR = pin_as3910_mosi.mask;
    TC_Start(AT91C_BASE_TC0);
    
    while(!Flag_SendOver);
    return ERR_NONE;
}

void TC0_IrqHandler(void)
{
    u16 countByte;
    u8 countBit,BitValue;
    countByte = iSendCount >> 3;
    countBit = iSendCount & 0x07;
    BitValue = *(gSendData.Data + countByte) & (1 << countBit);
    if(BitValue)
        pin_as3910_mosi.pio->PIO_SODR = pin_as3910_mosi.mask;
    
    AT91C_BASE_TC0->TC_SR;      // Dumy Read,Clear IT Status
    NVIC_ClearPendingIRQ(IROn_TC0);
    
    iSendCount ++;
    if(iSendCount == gSendData.Lenght_Bit)
    {
        AT91C_BASE_TC0->TC_CCR = AT91C_TC_CLKDIS;       // Stop TC0
        Flag_SendOver = 1;
    }
    pin_as3910_mosi.pio->PIO_CODR = pin_as3910_mosi.mask;
}