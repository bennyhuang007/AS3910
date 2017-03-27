#ifndef _AS3910_COM_H_
#define _AS3910_COM_H_

#include "main.h"
#include "ams_types.h"
#include "as3910_io.h"
#include "as3910_def.h"
#include "as3910_interrupt.h"
#include "stdio.h"
#include "errno.h"
#include "other.h"
#include "crapto1.h"

s8 CMD_ReadFIFOAc(u8 *pBuffer,u8 *cLenght);
s8 CMD_GetUID(u8 *uid);
s8 CMD_SelectCard(u8 *uid);
s8 CMD_Authentication(u8 *uid,u8 Key_Type,u8 Addr,u8 *key);
s8 CMD_ReadBlock(u8 Addr,u8 *pData);
s8 CMD_AckForRead(u8 Addr);
void MillerEncoder(u8* pSour,u16 iSourLen_Bit,u8 *pDest,u16 *iDestLen_Bit);
void Encrypt_prarity(u8 *pSour,u16 iSourLen_Byte,u8 *pDest,u16 *iDestLen_Bit);
void Decrypt(u8 *pSour,u8 *pDest,u16 iSourLen_Byte);
s8 CMD_WriteBlock(u8 Addr,u8 *pData);
s8 CMD_Halt(void);

#endif