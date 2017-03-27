#include "as3910_com.h"

static Crypto1_TypeDef Crypto1_State;
static volatile u8 CipherOld;

s8 CMD_ReadFIFOAc(u8 *pBuffer,u8 *iLenght)
{
    u8 Temp;
    as3910ReadRegister(AS3910_FIFO_STATUS,&Temp);
    Temp >>= 2;
    *iLenght = Temp;
    if(Temp)
    {
        as3910ReadFIFO(pBuffer,Temp);
#ifdef DEBUG
        printf("Read %d Byte from FIFO:\r\n",Temp);
        for(u16 i = 0;i < Temp;i++)
            printf("0x%X\t",*(pBuffer + i));
        printf("\r\n");
#endif
        return ERR_NONE;
    }else
    {
#ifdef DEBUG
        printf("Recv FIFO Empty\r\n");
#endif
        return ERR_FIFOEMPTY;
    }
}

s8 CMD_GetUID(u8 *uid)
{
    u8 Temp[10],iLenght;
    
    //-------------------------Send REQA Command------------------------
    as3910ModifyRegister(AS3910_CONFIGURATION3,0x80,0x80);  //crc_rx->1
    as3910ClearInterrupts(0xFF);
    as3910ExecuteCommand(AS3910_DC_TX_REQA);
    if(as3910WaitForInterruptTimed(AS3910_IRQ_MASK_RXE, 100, NULL) == ERR_TIMEOUT)		// wait for interrupt
        return ERR_TIMEOUT;
    //-------------------------Read------------------------
    CMD_ReadFIFOAc(Temp,&iLenght);
    if((Temp[0] != 0x04) | (Temp[1] != 0x00))
        return ERR_IO;
    //-------------------------Send ANTI Collision Command------------------------
    as3910ExecuteCommand(AS3910_DC_CLEAR_FIFO);
    Temp[0] = (unsigned char)((2 << 6)&0x00C0) | 0x01; // <--- antcl bit
    Temp[1] = (unsigned char)((2 >> 2)&0x00FF);
    as3910ContinuousWrite(AS3910_NUM_OF_TRANS0,Temp,2);
    
    as3910ModifyRegister(AS3910_CONFIGURATION3,0x80,0x80);  // crc_rx->1
    
    Temp[0] = 0x93;
    Temp[1] = 0x20;
    as3910WriteFIFO(Temp,2);
    
    as3910ClearInterrupts(0xFF);
    as3910ExecuteCommand(AS3910_DC_TX_WO_CRC);
    if(as3910WaitForInterruptTimed(AS3910_IRQ_MASK_RXE, 100, NULL) == ERR_TIMEOUT)		// wait for interrupt
        return ERR_TIMEOUT;
    
    //-------------------------Read------------------------
    CMD_ReadFIFOAc(uid,&iLenght);
    if(iLenght == 5)
        return ERR_NONE;
    else
        return ERR_IO;
}

s8 CMD_SelectCard(u8 *uid)
{
    u8 Temp[10],iLenght;
    as3910ExecuteCommand(AS3910_DC_CLEAR_FIFO);
    Temp[0] = (unsigned char)((7 << 6)&0x00C0);
    Temp[1] = (unsigned char)((7 >> 2)&0x00FF);
    as3910ContinuousWrite(AS3910_NUM_OF_TRANS0,Temp,2);
    
    as3910ModifyRegister(AS3910_CONFIGURATION3,0x80,0x00);  //crc_rx->0
    
    Temp[0] = 0x93;
    Temp[1] = 0x70;
    for(u8 i = 0;i < 5;i++)
        Temp[i+2] = *(uid + i);
    as3910WriteFIFO(Temp,7);
    
    as3910ClearInterrupts(0xFF);
    as3910ExecuteCommand(AS3910_DC_TX_W_CRC);
    if(as3910WaitForInterruptTimed(AS3910_IRQ_MASK_RXE, 100, NULL) == ERR_TIMEOUT)		// wait for interrupt
        return ERR_TIMEOUT;
    //-------------------------Read SAK------------------------
    CMD_ReadFIFOAc(Temp,&iLenght);
    if((Temp[0] != 0x08))
        return ERR_IO;
    else
        return ERR_NONE;
}

s8 CMD_Authentication(u8 *uid,u8 Key_Type,u8 Addr,u8 *key)
{
    u8 Temp[10],iLenght;
    u32 UID,nT,nR = 0x12345678,nR_suc;          // nR 可以是任意的32位随机数
    u32 aR,aR_suc,aT,aT_suc,ks1,ks2,ks3;
    //-------------------------Send Authentication Command------------------------
    as3910ExecuteCommand(AS3910_DC_CLEAR_FIFO);
    Temp[0] = (unsigned char)((4 << 6)&0x00C0);
    Temp[1] = (unsigned char)((4 >> 2)&0x00FF);
    as3910ContinuousWrite(AS3910_NUM_OF_TRANS0,Temp,2);
    
    as3910ModifyRegister(AS3910_CONFIGURATION3,0x80,0x80);  //crc_rx->0
    
    Temp[0] = Key_Type;			// Key
    Temp[1] = Addr;			// Block
    CRC_CheckRev(Temp,2,&Temp[3],&Temp[2]);
    
    as3910WriteFIFO(Temp,4);
    
    as3910ClearInterrupts(0xFF);
    
    as3910ExecuteCommand(AS3910_DC_TX_WO_CRC);
    
    if(as3910WaitForInterruptTimed(AS3910_IRQ_MASK_RXE, 100, NULL) == ERR_TIMEOUT)		// wait for interrupt
        return ERR_TIMEOUT;
    //-------------------------Read nT------------------------
    CMD_ReadFIFOAc(Temp,&iLenght);
    if(iLenght != 4)
        return ERR_IO;
    
    nT = ((u32)Temp[0] << 24)|((u32)Temp[1] << 16)|((u32)Temp[2] << 8)|((u32)Temp[3]);
#ifdef DEBUG
    printf("Get nT: %X\r\nSet nR: %X\r\n",nT,nR);
#endif
    //---------------------Translate UID-----------------------
    UID = ((u32)*uid << 24)|((u32)*(uid+1) << 16)|((u32)*(uid+2) << 8)|((u32)*(uid+3));
    
    //------------------Caculate {nR},{aR}---------------------
    // 这部分忽略了奇偶校验位的加密
    crypto1_init(&Crypto1_State, key);
    crypto1_word(&Crypto1_State, UID ^ nT, 0);
    ks1 = crypto1_word(&Crypto1_State, nR, 0);
    nR_suc = nR ^ ks1;
    prng_successor(nT, &aR, &aT);
    ks2 = crypto1_word (&Crypto1_State, 0, 0);
    ks3 = crypto1_word (&Crypto1_State, 0, 0);
    aR_suc = aR ^ ks2;
    aT_suc = aT ^ ks3;
#ifdef DEBUG
    printf("{nR},{aR},{aT} = %X %X %X\r\n",nR_suc,aR_suc,aT_suc);
#endif   
    //-------------------------Send {nR},{aR} Command------------------------
    as3910ExecuteCommand(AS3910_DC_CLEAR_FIFO);
    Temp[0] = (unsigned char)((8 << 6)&0x00C0);
    Temp[1] = (unsigned char)((8 >> 2)&0x00FF);
    as3910ContinuousWrite(AS3910_NUM_OF_TRANS0,Temp,2);
    
    as3910ModifyRegister(AS3910_CONFIGURATION3,0x80,0x80);  //crc_rx->0
    
    Temp[0] = (u8)(nR_suc >> 24);Temp[1] = (u8)(nR_suc >> 16);
    Temp[2] = (u8)(nR_suc >> 8);Temp[3] = (u8)(nR_suc);
    
    Temp[4] = (u8)(aR_suc >> 24);Temp[5] = (u8)(aR_suc >> 16);
    Temp[6] = (u8)(aR_suc >> 8);Temp[7] = (u8)(aR_suc);
    
    //-------------------------------------
    as3910WriteFIFO(Temp,8);
    
    as3910ClearInterrupts(0xFF);
    
    as3910ExecuteCommand(AS3910_DC_TX_WO_CRC);
    //-------------------------------------
    if(as3910WaitForInterruptTimed(AS3910_IRQ_MASK_RXE, 100, NULL) == ERR_TIMEOUT)	// wait for interrupt
        return ERR_TIMEOUT;
    //-------------------------Read nT------------------------
    CMD_ReadFIFOAc(Temp,&iLenght);
    if(iLenght != 4)
        return ERR_IO;
#ifdef DEBUG
    if(iLenght == 4)
        printf("Recv %d data: %2X %2X %2X %2X\r\n",iLenght,Temp[0],Temp[1],Temp[2],Temp[3]);
#endif
    
    // 这部分没有加密奇偶校验位，读取一次，修正密钥偏移
    CipherOld = crypto1_bit(&Crypto1_State, 0, 0);
    
    return ERR_NONE;
}

s8 CMD_ReadBlock(u8 Addr,u8 *pData)
{
    u8 Temp[20],Temp1[20],Temp2[20];
    u16 iLenght;
    u8 cLenght8;
    
    as3910ExecuteCommand(AS3910_DC_CLEAR_FIFO);
    //as3910ModifyRegister(AS3910_NUM_OF_TRANS0,0x02,0x02);       // 可接收4ByteNACK数据
    //as3910ModifyRegister(AS3910_CONFIGURATION3,0xC0,0xC0);      // 接收不进行CRC、奇偶校验
    
    Temp[0] = 0x30;		// Command
    Temp[1] = Addr;		// Block
    CRC_CheckRev(Temp,2,&Temp[3],&Temp[2]);
    
    Encrypt_prarity(Temp,4,Temp1,&iLenght);
    MillerEncoder(Temp1,iLenght,Temp2,&iLenght);
    
    as3910ClearInterrupts(0xFF);
    
    as3910TransModeEnter();
    for(u8 i = 0;i < 10;i++);
    as3910TransModeSendNBit(Temp2,iLenght);
    for(u8 i = 0;i < 10;i++);
    as3910TransModeLeave();
    
    if(as3910WaitForInterruptTimed(AS3910_IRQ_MASK_RXE, 100, NULL) == ERR_TIMEOUT)		// wait for interrupt
        return ERR_TIMEOUT;
    //-------------------------------------------------
    CMD_ReadFIFOAc(Temp,&cLenght8);
    Decrypt(Temp,pData,cLenght8);
    
    if(cLenght8 != 18)
        return ERR_IO;
    
    printf("Decrypt Result:\r\nLenght:%d\r\nData:\r\n",cLenght8);
    for(Temp[0] = 0;Temp[0] < cLenght8;Temp[0]++)
        printf("%X\t",pData[Temp[0]]);
    
    CRC_CheckRev(pData,16,&Temp[1],&Temp[0]);
    if((Temp[0] != pData[16])|(Temp[1] != pData[17]))
        return ERR_CRC;
#ifdef DEBUG
    else
        printf("CRC Check:%X %X,Correct\r\n",Temp[0],Temp[1]);
#endif
    return ERR_NONE;
}

s8 CMD_WriteBlock(u8 Addr,u8 *pData)
{
    u8 Temp[50],Temp1[50],Temp2[50];
    u16 iLenght;
    u8 cLenght8;
    
    as3910ExecuteCommand(AS3910_DC_CLEAR_FIFO);
    //as3910ModifyRegister(AS3910_NUM_OF_TRANS0,0x02,0x02);       // 可接收4Bit数据(不使用Transmit命令#4 & #5不能接收4bit数据)
    //as3910ModifyRegister(AS3910_CONFIGURATION3,0xC0,0xC0);      // 接收不进行CRC、奇偶校验
    
    Temp[0] = 0xA0;		// Command
    Temp[1] = Addr;		// Block
    CRC_CheckRev(Temp,2,&Temp[3],&Temp[2]);
    
    Encrypt_prarity(Temp,4,Temp1,&iLenght);
    MillerEncoder(Temp1,iLenght,Temp2,&iLenght);
    
    as3910ClearInterrupts(0xFF);
    
    as3910TransModeEnter();
    for(u8 i = 0;i < 10;i++);
    as3910TransModeSendNBit(Temp2,iLenght);
    for(u8 i = 0;i < 10;i++);
    as3910TransModeLeave();
    
    if(as3910WaitForInterruptTimed(AS3910_IRQ_MASK_RXE, 100, NULL) == ERR_TIMEOUT)		// wait for interrupt
        return ERR_TIMEOUT;
    CMD_ReadFIFOAc(Temp,&cLenght8);
#ifdef DEBUG
    if(cLenght8 == 0)
    {
        as3910ReadFIFO(Temp,1);
        printf("#9 Reg = 0,but get %X from FIFO\r\n",*Temp);
    }
#endif
    // ACK/NACK 加密，偏移4bits
    crypto1_bit(&Crypto1_State, 0, 0);
    crypto1_bit(&Crypto1_State, 0, 0);
    crypto1_bit(&Crypto1_State, 0, 0);
    CipherOld = crypto1_bit(&Crypto1_State, 0, 0);
    //-------------------------------------------------
    as3910ExecuteCommand(AS3910_DC_CLEAR_FIFO);
    //as3910ModifyRegister(AS3910_NUM_OF_TRANS0,0x02,0x02);       // 可接收4Bit数据(不使用Transmit命令#4 & #5不能接收4bit数据)
    //as3910ModifyRegister(AS3910_CONFIGURATION3,0xC0,0xC0);      // 接收不进行CRC、奇偶校验
    
    for(u8 i = 0;i < 16;i++)
        Temp[i] = *(pData + i);
    
    CRC_CheckRev(Temp,16,&Temp[17],&Temp[16]);
    
    Encrypt_prarity(Temp,18,Temp1,&iLenght);
    MillerEncoder(Temp1,iLenght,Temp2,&iLenght);
    
    as3910ClearInterrupts(0xFF);
    
    as3910TransModeEnter();
    for(u8 i = 0;i < 10;i++);
    as3910TransModeSendNBit(Temp2,iLenght);
    for(u8 i = 0;i < 10;i++);
    as3910TransModeLeave();
    
    if(as3910WaitForInterruptTimed(AS3910_IRQ_MASK_RXE, 100, NULL) == ERR_TIMEOUT)		// wait for interrupt
        return ERR_TIMEOUT;
    CMD_ReadFIFOAc(Temp,&cLenght8);
#ifdef DEBUG
    if(cLenght8 == 0)
    {
        as3910ReadFIFO(Temp,1);
        printf("#9 Reg = 0,but get %X from FIFO\r\n",*Temp);
    }
#endif
    
    // ACK/NACK 加密，偏移4bits
    crypto1_bit(&Crypto1_State, 0, 0);
    crypto1_bit(&Crypto1_State, 0, 0);
    crypto1_bit(&Crypto1_State, 0, 0);
    CipherOld = crypto1_bit(&Crypto1_State, 0, 0);
    
    return ERR_NONE;
}

s8 CMD_Halt(void)
{
    u8 Temp[20],Temp1[20],Temp2[20];
    u16 iLenght;
    
    as3910ExecuteCommand(AS3910_DC_CLEAR_FIFO);
    //as3910ModifyRegister(AS3910_NUM_OF_TRANS0,0x02,0x02);       // 可接收4ByteNACK数据
    //as3910ModifyRegister(AS3910_CONFIGURATION3,0xC0,0xC0);      // 接收不进行CRC、奇偶校验
    
    Temp[0] = 0x50;		// Command
    Temp[1] = 0x00;		// Block
    CRC_CheckRev(Temp,2,&Temp[3],&Temp[2]);
    
    Encrypt_prarity(Temp,4,Temp1,&iLenght);
    MillerEncoder(Temp1,iLenght,Temp2,&iLenght);
    
    as3910ClearInterrupts(0xFF);
    
    as3910TransModeEnter();
    for(u8 i = 0;i < 10;i++);
    as3910TransModeSendNBit(Temp2,iLenght);
    for(u8 i = 0;i < 10;i++);
    as3910TransModeLeave();
    
    if(as3910WaitForInterruptTimed(AS3910_IRQ_MASK_RXE, 100, NULL) == ERR_TIMEOUT) // wait for interrupt
        return ERR_NONE;        // 正常情况下HALT命令传送后不会有任何数据接收
    else
        return ERR_IO;
}

//-------------------------------------------------------
// 米勒编码
//-------------------------------------------------------
#define SEQ_X   0x2     // AS3910透明传输模式时,高电平调制，需要反码
#define SEQ_Y   0x0
#define SEQ_Z   0x1
void MillerEncoder(u8* pSour,u16 SourLen_Bit,u8 *pDest,u16 *iDestLen_Bit)
{
    u16 CountSour = 0,CountDest = 0;
    u16 CountByte,CountBit;
    u8 BitValue,BitValueOld;
    
    *pDest = SEQ_Z << CountDest;       // 通讯开始，序列Z
    BitValueOld = 0;    // 起始位是序列Z，当成0作处理
    for(CountSour = 0,CountDest = 2;CountSour < SourLen_Bit;CountSour ++,CountDest += 2)
    {
        CountByte = CountSour >> 3;
        CountBit = CountSour & 0x07;
        BitValue = *(pSour + CountByte) & (0x01 << CountBit); // 遍历顺序LSB->MSB
        
        CountByte = CountDest >> 3;
        CountBit = CountDest & 0x07;
        
        if(CountBit == 0)
            *(pDest + CountByte) = 0;
        
        if(BitValue)                    // 当前位为1
            *(pDest + CountByte) |= SEQ_X << CountBit;
        else if(BitValueOld == 0)       // 连续两个位为0
            *(pDest + CountByte) |= SEQ_Z << CountBit;
        else                            // 仅当前位为0
            *(pDest + CountByte) |= SEQ_Y << CountBit;
        
        // 更新旧值
        BitValueOld = BitValue;
    }
    //---------------------------------通讯结束---------------------------------
    // 0 + 序列Y
    CountByte = CountDest >> 3;
    CountBit = CountDest & 0x07;
    
    if(CountBit == 0)
        *(pDest + CountByte) = 0;
    
    if(BitValueOld == 0)
        *(pDest + CountByte) |= SEQ_Z << CountBit;
    else
        *(pDest + CountByte) |= SEQ_Y << CountBit;
    CountDest += 2;
    
    CountByte = CountDest >> 3;
    CountBit = CountDest & 0x07;
    
    if(CountBit == 0)
        *(pDest + CountByte) = 0;
    
    *(pDest + CountByte) |= SEQ_Y << CountBit;
    //--------------------------------------------------------------------------
    *iDestLen_Bit = CountDest + 2;
}

//------------------------------------------------------------------------------
// 流加密
// 遍历顺序LSB->MSB,奇偶校验位和下一字节的第一位公用同一个密钥
//------------------------------------------------------------------------------
void Encrypt_prarity(u8 *pSour,u16 iSourLen_Byte,u8 *pDest,u16 *ciDestLen_Bit)
{
    u16 CountSour,CountDest;
    u16 CountByte,CountBit;
    u8 BitValue,Cipher;
    
    for(CountSour = 0,CountDest = 0;CountSour < 8 * iSourLen_Byte;CountSour++)
    {
        CountByte = CountSour >> 3;
        CountBit = CountSour & 0x07;
        
        BitValue = (*(pSour + CountByte) >> CountBit) & 0x01;
        
        if(CountBit == 0)
            Cipher = CipherOld;
        else
            Cipher = crypto1_bit(&Crypto1_State, 0, 0);
        
        BitValue ^= Cipher;
        
        // 写入加密值
        CountByte = CountDest >> 3;
        CountBit = CountDest & 0x07;
        if(CountBit == 0)
            *(pDest + CountByte) = 0x0;
        *(pDest + CountByte) |= BitValue << CountBit;
        CountDest ++;
        
        // 插入奇校验位
        CountByte = CountSour >> 3;
        CountBit = CountSour & 0x07;
        if(CountBit == 0x07)
        {
            BitValue = *(pSour + CountByte);
            BitValue ^= BitValue >> 4;
            BitValue ^= BitValue >> 2;
            BitValue ^= BitValue >> 1;
            BitValue &= 0x01;
            BitValue ^= 0x01;
            
            CipherOld = crypto1_bit(&Crypto1_State, 0, 0);
            BitValue ^= CipherOld;
            
            // 写入加密值
            CountByte = CountDest >> 3;
            CountBit = CountDest & 0x07;
            if(CountBit == 0)
                *(pDest + CountByte) = 0x0;
            *(pDest + CountByte) |= BitValue << CountBit;
            CountDest ++;
        }
    }
    *ciDestLen_Bit = CountDest;
}

//------------------------------------------------------------------------------
// 流解密，不用考虑奇偶校验位
void Decrypt(u8 *pSour,u8 *pDest,u16 iSourLen_Byte)
{
    u8 Count;
    u8 CountByte,CountBit;
    u8 BitValue;
    for(Count = 0;Count < iSourLen_Byte * 8;Count++)
    {
        CountByte = Count >> 3;
        CountBit = Count & 0x07;
        if(CountBit == 0)
            *(pDest + CountByte) = 0;
        
        BitValue = *(pSour + CountByte) & (0x01 << CountBit);
        BitValue ^= CipherOld << CountBit;
        *(pDest + CountByte) |= BitValue;
        
        CipherOld = crypto1_bit(&Crypto1_State, 0, 0);
    }
}
