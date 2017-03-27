#ifndef _OTHER_H_
#define _OTHER_H_

#include "dbgu/dbgu.h"
#include "ams_types.h"
#include "as3910_io.h"
#include "stdio.h"

struct{
	uint32_t odd, even;
}typedef Crypto1_TypeDef;

void CRC_CheckRev(u8 *pBuffer,u8 cLenght,u8 *pResultMSB,u8 *pResultLSB);
unsigned char CheckFrame(unsigned char *pchar,unsigned char lenght);

#endif