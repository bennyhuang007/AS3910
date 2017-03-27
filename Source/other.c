#include "other.h"

void CRC_CheckRev(u8 *pBuffer,u8 cLenght,u8 *pResultMSB,u8 *pResultLSB)
{
    u16 CRC_Reg = 0x6363;
    u8 Temp,i;
    
    for(i = 0;i < cLenght;i++)
    {
        Temp = *(pBuffer + i)^(u8)(CRC_Reg & 0x00FF);
        Temp ^= Temp << 4;
        
        CRC_Reg = (CRC_Reg >> 8)^((u16)Temp << 8)^\
            ((u16)Temp << 3)^((u16)Temp >> 4);
    }
    
    *pResultMSB = (u8)(CRC_Reg >> 8);
    *pResultLSB = (u8)CRC_Reg;
}

unsigned char CheckFrame(unsigned char *pchar,unsigned char lenght)
{
    unsigned char i;
    unsigned char xor_result = 0;
    
    if(*pchar != 0x5a)
        return FALSE;
    if(*(pchar+1) != lenght)
        return FALSE;
    
    for(i = 0;i < lenght - 2;i++)
        xor_result ^= *(pchar + i);
    
    if(*(pchar + lenght - 2) == xor_result)
        return TRUE;
    else
        return FALSE;
}