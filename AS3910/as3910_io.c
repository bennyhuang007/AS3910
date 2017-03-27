#include "as3910_io.h"
#include "main.h"
#define AS3910_SPI_ADDRESS_MASK         (0x3F)
#define AS3910_SPI_CMD_READ_REGISTER    (0x40)
#define AS3910_SPI_CMD_WRITE_REGISTER   (0x00)
#define AS3910_SPI_CMD_READ_FIFO        (0xBF)
#define AS3910_SPI_CMD_WRITE_FIFO       (0x80)
#define AS3910_SPI_CMD_DIREC_CMD        (0xC0)

//-------------------------------------------------
// as3910WriteRegister
// 写寄存器
// SPI命令0b00AAAAAA
//-------------------------------------------------
s8 as3910WriteRegister(u8 address, u8 data)
{
    __disable_irq();
    
    AS3910_SENHigh();
    
    spiTxRxNSEN(address & AS3910_SPI_ADDRESS_MASK);
    
    spiTxRxNSEN(data);
    
    spiSync();
    
    AS3910_SENLow();
    
    __enable_irq();
    
    return ERR_NONE;
}

//-------------------------------------------------
// as3910ReadRegister
// 读寄存器
// SPI命令0b01AAAAAA
//-------------------------------------------------
s8 as3910ReadRegister(u8 address, u8 *data)
{	
    __disable_irq();
    
    AS3910_SENHigh();
    
    spiTxRxNSEN(AS3910_SPI_CMD_READ_REGISTER | (address & AS3910_SPI_ADDRESS_MASK));
    
    *data = spiTxRxNSEN(0x00);
    
    spiSync();
    
    AS3910_SENLow();
    
    __enable_irq();
    
    return ERR_NONE;
}

//-------------------------------------------------
// as3910ModifyRegister
// 修改寄存器的值，mask位为1的值修改
//-------------------------------------------------
s8 as3910ModifyRegister(u8 address, u8 mask, u8 data)
{
    s8 error = ERR_NONE;
    u8 registerValue = 0;
    
    error |= as3910ReadRegister(address, &registerValue);
    
    registerValue = (registerValue & ~mask) | (mask & data);
    
    error |= as3910WriteRegister(address, registerValue);
    
    if (ERR_NONE == error)
        return ERR_NONE;
    else
        return ERR_IO;
}

//-------------------------------------------------
// as3910ContinueRead
// 寄存器连续写
//-------------------------------------------------
s8 as3910ContinuousWrite(u8 address,const u8 *data, u8 length)
{
    __disable_irq();
    
    AS3910_SENHigh();
    
    spiTxRxNSEN(AS3910_SPI_CMD_WRITE_REGISTER | (address & AS3910_SPI_ADDRESS_MASK));
    
    spiTransferNBytes(data,NULL,length,TRUE);
    
    spiSync();
    
    AS3910_SENLow();
	
    __enable_irq();
    
    return ERR_NONE;
}

//-------------------------------------------------
// as3910ContinueRead
// 寄存器连续读
//-------------------------------------------------
s8 as3910ContinuousRead(u8 address, u8 *data, u8 length)
{
    __disable_irq();
    
    AS3910_SENHigh();
    
    spiTxRxNSEN(AS3910_SPI_CMD_READ_REGISTER | (address & AS3910_SPI_ADDRESS_MASK));
    
    spiTransferNBytes(NULL,data,length,TRUE);
    
    spiSync();
    
    AS3910_SENLow();

    __enable_irq();
    
    return ERR_NONE;
}

//-------------------------------------------------
// as3910WriteFIFO
// 写FIFO
// SPI命令 0b10111111
//-------------------------------------------------
s8 as3910WriteFIFO(const u8 *data, u8 length)
{
    //关闭中断
    __disable_irq();
    
    AS3910_SENHigh();		// SEN High
    
    spiTxRxNSEN(AS3910_SPI_CMD_WRITE_FIFO);
    
    spiTransferNBytes(data,NULL,length,TRUE);
    
    spiSync();
    
    AS3910_SENLow();		// SEN Low
    //开启总中断
    __enable_irq();
    
    return ERR_NONE;
}

//-------------------------------------------------
// as3910ReadFIFO
// 读FIFO
// SPI命令 0b10111111
//-------------------------------------------------
s8 as3910ReadFIFO(u8 *data, u8 length)
{
    __disable_irq();
    
    AS3910_SENHigh();
    
    spiTxRxNSEN(AS3910_SPI_CMD_READ_FIFO);
    
    spiTransferNBytes(NULL,data,length,TRUE);
    
    spiSync();
    
    AS3910_SENLow();

    __enable_irq();
    
    return ERR_NONE;
}

//-------------------------------------------------
// as3910ExecuteCommand
// 发送Direct Command
// SPI命令 0b11XXXXXX
//-------------------------------------------------
s8 as3910ExecuteCommand(u8 directCommand)
{
    //关闭中断
    __disable_irq();
    
    AS3910_SENHigh();		// SEN High
    
    spiTxRxNSEN(AS3910_SPI_CMD_DIREC_CMD | (directCommand & AS3910_SPI_ADDRESS_MASK));
    
    spiSync();
    
    AS3910_SENLow();		// SEN Low
    //开启总中断
    __enable_irq();
    
    return ERR_NONE;
}

// Trans Mode CMD
s8 as3910TransModeCommand(void)
{
    //关闭中断
    __disable_irq();
    
    AS3910_SENHigh();		// SEN High
    
    spiTxRxNSEN(0xDC);
    
    spiSync();
    //开启总中断
    __enable_irq();
    
    return ERR_NONE;
}
