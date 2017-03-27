#include "main.h"

#define FIFO_LENGHT 32

void AS3910_IntrConfig(void);

static u8 UID[5];
//------------------------------------------------------------------------------
/// main function
//------------------------------------------------------------------------------
u32 main(void)
{
    u8 counter;
    u8 Temp[20];
    u8 buf_input[30],pointer_buf = 0;
    // Debug Interface
    TRACE_CONFIGURE(DBGU_STANDARD, 115200, BOARD_MCK);
    printf("-- Compiled: %s %s --\r\n", __DATE__, __TIME__);
    printf("Application Start...\r\n");
    
    // LED IO Config
    PIO_Configure(&pin_led,1);
    // SPI Config
    spiInitialize();
    // AS3910_IRQ EXT Interrupt Config
    AS3910_IntrConfig();
    // Timer Config
    timerInitialize();
    // Enable Global Interrupt
    __enable_irq();
    
    //-------------------------------------------------------
    as3910ExecuteCommand(AS3910_DC_SET_DEFAULT);        // 复位AS3910
    sleepMilliseconds(10);
    
    as3910EnableInterrupts(0xFF);
    
    as3910WriteRegister(AS3910_CONFIGURATION2, 0x0C);           // 27.12MHz晶振，引脚输出13.56MHz信号
    as3910WriteRegister(AS3910_OPERATION_CONTROL, 0x80);        // EN
    sleepMilliseconds(10);
    
    as3910WriteRegister(AS3910_OPERATION_CONTROL, 0xD0);        // enable rx/tx
    
    as3910ModifyRegister(AS3910_CONFIGURATION5,0x40,0x00);      // OOK
    
    // 调整工作电压
    as3910ModifyRegister(AS3910_RECEIVER_CONF,0x80,0x00);       // AGC disable
    as3910ExecuteCommand(AS3910_DC_ADJUST_REGULATORS);
    sleepMilliseconds(10);
#ifdef DEBUG
    as3910ReadRegister(AS3910_REFULATORS_DISPLAY,Temp);
    printf("Regulators adjust result:%X\r\n",*Temp);
#endif
    // 天线自动调谐
    as3910ModifyRegister(AS3910_EXTERNAL_TRIM,0x80,0x00);
    as3910ExecuteCommand(AS3910_DC_CALIBRATE_ANTENNA);
    sleepMilliseconds(5);
#ifdef DEBUG
    as3910ReadRegister(AS3910_ANTENNA_CALIBRATION,Temp);
    printf("Antenna Calibration result:%X\r\n",*Temp);
#endif
    
    // enable AGC, 14443A 106kb/s
    as3910WriteRegister(AS3910_RECEIVER_CONF, 0x80);
    // 配置透明传输模式(还没进入该模式)
    as3910TransModeConfig();
    
    // Enable EXTI
    AT91C_BASE_PIOA->PIO_ISR;           // DummyRead,Clear Interrupt Flag
    NVIC_ClearPendingIRQ(IROn_PIOA);
    AS3910_IRQ_ON();
    
    printf("PC Interface Start...\r\n");
    
    pointer_buf = 0;
    
    while(1)
    {
        Temp[0] = DBGU_GetChar();
        buf_input[pointer_buf++] = Temp[0];
        if((Temp[0] == '\n') && CheckFrame(buf_input,pointer_buf))
        {
            AS3910_IRQ_OFF();
            switch(buf_input[2])
            {
                //------------------------基础命令------------------------
            case 0x00:  // 读寄存器命令
                as3910ReadRegister(buf_input[3],&Temp[0]);
                printf("Read 0x%hhx : 0x%hhx\r\n",buf_input[3],Temp[0]);
                break;
            case 0x01:  // 写寄存器命令
                as3910WriteRegister(buf_input[3],buf_input[4]);
                printf("Write 0x%hhx : 0x%hhx\r\n",buf_input[3],buf_input[4]);
                break;
            case 0x02:  // 发送直接命令
                as3910ExecuteCommand(buf_input[3]);
                printf("Command 0x%hhx was Send!\r\n",buf_input[3]);
                break;
            case 0x03:  // 写FIFO
                as3910WriteFIFO(buf_input+4,buf_input[3]);
                printf("Write %d Byte to FIFO\r\n",buf_input[3]);
                break;
            case 0x04:  // 读FIFO
                as3910ReadFIFO(Temp,buf_input[3]);
                printf("Read %d Byte from FIFO：\r\n",buf_input[3]);
                for(counter = 0;counter < buf_input[3];counter++)
                    printf("0x%hhx\t",Temp[counter]);
                printf("\r\n");
                break;
            case 0x05:  // 控制载波,降低功耗
                if(buf_input[3] == 0x00)
                    as3910ModifyRegister(AS3910_OPERATION_CONTROL,0x80,0x00);
                else
                    as3910ModifyRegister(AS3910_OPERATION_CONTROL,0x80,0x80);
                break;
                //----------------------TAG 操作命令----------------------
            case 0x10:  // 读卡号
                if(CMD_GetUID(UID) == ERR_NONE)
                    printf("Get UID %2X %2X %2X %2X %2X\r\n",UID[0],UID[1],UID[2],UID[3],UID[4]);
                else
                    printf("Unable to get UID\r\n");
                break;
            case 0x11:  // 选择TAG
                if(CMD_SelectCard(UID) == ERR_NONE)
                    printf("Select a TAG!\r\n");
                else
                    printf("Unable to select a TAG\r\n");
                break;
            case 0x12:  // 验证buf_input[3]:0x60:密码A,61H:密码B
                if(CMD_Authentication(UID,buf_input[3],buf_input[4],buf_input + 5) == ERR_NONE)
                    printf("Authentication Success\r\n");
                else
                    printf("Authentication Fail\r\n");
                break;
            case 0x13:  // 块读
                if(CMD_ReadBlock(buf_input[3],Temp) == ERR_NONE)
                    printf("Read Block Success\r\n");
                else
                    printf("Read Block Fail\r\n");
                break;
            case 0x14:  // 块写
                if(CMD_WriteBlock(buf_input[3],buf_input + 4) == ERR_NONE)
                    printf("Write Block Success\r\n");
                else
                    printf("Write Block Fail\r\n");
                break;
            case 0x15:  // 停止(卡片休眠)
                if(CMD_Halt() == ERR_NONE)
                    printf("TAG Halt\r\n");
                else
                    printf("Half cmd fail\r\n");
                break;
            default:
                printf("Command Error\r\n");
                break;
            }
            AS3910_IRQ_ON();
            pointer_buf = 0;
        }
    }
}

void AS3910_IntrConfig(void)
{
    PIO_Configure(&pin_as3910_intr,1);
    PIO_ConfigureIt(&pin_as3910_intr,PIOA31_IRQHandler);
    PIO_EnableIt(&pin_as3910_intr);
    // High Level Interrupt
    pin_as3910_intr.pio->PIO_REHLSR |= pin_as3910_intr.mask;
    pin_as3910_intr.pio->PIO_LSR |= pin_as3910_intr.mask;
    pin_as3910_intr.pio->PIO_AIMER |= pin_as3910_intr.mask;
    //IRQ_EnableIT(IROn_PIOA);
}
//------------------------------------------------------------------------------
