#ifndef _AS3910_TANSPARENTMODE_H_
#define _AS3910_TANSPARENTMODE_H_

#include "tc/tc.h"
#include "ams_types.h"
#include "errno.h"
#include "as3910_com.h"

struct{
    u8 *Data;
    u16 Lenght_Bit;
}typedef structSendData;

extern structSendData gSendData;
extern u16 iSendCount;
extern volatile u8 Flag_SendOver;

void as3910TransModeConfig(void);
s8 as3910TransModeEnter(void);
s8 as3910TransModeLeave(void);
s8 as3910TransModeSendNBit(u8 *pData,u16 cLenght_Bit);

#endif