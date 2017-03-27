/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include <board.h>
#include <board_memories.h>
#include <utility/trace.h>
#include <utility/assert.h>
#include <fatfs/src/diskio.h>
#include <memories/Media.h>
#include <Medias.h>
#include <string.h>
#include <stdio.h>

#include "diskio.h"
#include <includes.h>

//------------------------------------------------------------------------------
//         Internal variables
//------------------------------------------------------------------------------

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/


DSTATUS disk_initialize (
    BYTE drv                /* Physical drive number (0..) */
)
{
    DSTATUS stat = STA_NOINIT;
    if(drv==DRV_SDMMC)
    {
        stat = 0;
    }
    else if(drv==DRV_NAND)
    {
        stat = 0;
    } 

    return stat;
}

/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE drv        /* Physical drive number (0..) */
)
{
    DSTATUS stat=STA_NOINIT;

    if(drv==DRV_SDMMC)
    {
        stat = 0;
    }
    else if(drv==DRV_NAND)
    {
        stat = 0;
    } 

    return stat;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE drv,        /* Physical drive number (0..) */
    BYTE *buff,        /* Data buffer to store read data */
    DWORD sector,    /* Sector number (LBA) */
    BYTE count        /* Sector count (1..255) */
)
{
    unsigned char result;
    DRESULT res = RES_ERROR;

    unsigned int addr, len;
    if (medias[drv].blockSize < SECTOR_SIZE_DEFAULT) {
        addr = sector * (SECTOR_SIZE_DEFAULT / medias[drv].blockSize);
        len  = count * (SECTOR_SIZE_DEFAULT / medias[drv].blockSize);
    }
    else {
        addr = sector;
        len  = count;
    }
    
    result = MED_Read(&medias[drv],
                      addr,               // address
                      (void*)buff,          // data                                            
                      len,                // data size
                      NULL,
                      NULL);

    if( result == MED_STATUS_SUCCESS ) {
        res = RES_OK;
    }
    else {
        TRACE_ERROR("MED_Read pb: 0x%X\n\r", result);
        res = RES_ERROR;
    }

    return res;
}
//获取到的扇区数减半，导致格式化之后容量减半 1931264 SD_CSD_BLOCKNR(pSd);

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _READONLY == 0

DRESULT disk_write (
    BYTE drv,            /* Physical drive number (0..) */
    const BYTE *buff,    /* Data to be written */
    DWORD sector,        /* Sector number (LBA) */
    BYTE count            /* Sector count (1..255) */
)
{
    DRESULT res=RES_PARERR;
    unsigned int result;
    void * tmp;
    tmp = (void *) buff;

    unsigned int addr, len;
    if (medias[drv].blockSize < SECTOR_SIZE_DEFAULT) {
        addr = sector * (SECTOR_SIZE_DEFAULT / medias[drv].blockSize);
        len  = count * (SECTOR_SIZE_DEFAULT / medias[drv].blockSize);
    }
    else {
        addr = sector;
        len  = count;
    }
    
    //printf("disk_write(%d,%d)\r\n", addr, len);
    result = MED_Write(&medias[drv],
                       addr,              // address
                       (void*)tmp,         // data
                       len,               // data size
                       NULL,
                       NULL);

    if( result == MED_STATUS_SUCCESS ) {
      
        res = RES_OK;
    }
    else {
      
        TRACE_ERROR("MED_Write pb: 0x%X\n\r", result);
        res = RES_ERROR;
    }

    return res;
}
#endif /* _READONLY */

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
// Command    Description 
//
//CTRL_SYNC    Make sure that the disk drive has finished pending write process.
// When the disk I/O module has a write back cache, flush the dirty sector immediately.
// In read-only configuration, this command is not needed.
//
//GET_SECTOR_COUNT    Returns total sectors on the drive into the DWORD variable pointed by Buffer.
// This command is used in only f_mkfs function.
//
//GET_BLOCK_SIZE    Returns erase block size of the memory array in unit
// of sector into the DWORD variable pointed by Buffer.
// When the erase block size is unknown or magnetic disk device, return 1.
// This command is used in only f_mkfs function.
/*-----------------------------------------------------------------------*/
extern void Flash_FlushAll(void);
DRESULT disk_ioctl (
    BYTE drv,        /* Physical drive number (0..) */
    BYTE ctrl,        /* Control code */
    void *buff        /* Buffer to send/receive control data */
)
{ 
    DRESULT res=RES_PARERR;
   
    if (drv==DRV_SDMMC) {

        switch (ctrl) { 

            case GET_BLOCK_SIZE:
                *(unsigned int *)buff = 1; 
                res = RES_OK; 
                break; 
            
            case GET_SECTOR_COUNT :   /* Get number of sectors on the disk (DWORD) */ 
                //*(DWORD*)buff = (DWORD)(medias[DRV_MMC].size);
                if (medias[DRV_SDMMC].blockSize < SECTOR_SIZE_DEFAULT)
                {
                        *(DWORD*)buff = (DWORD)(medias[DRV_SDMMC].size /
                                                (SECTOR_SIZE_DEFAULT /
                                                medias[DRV_SDMMC].blockSize));
                }
                else
                {
                    *(DWORD*)buff = (DWORD)(medias[DRV_SDMMC].size);
                    //*(DWORD*)buff = (DWORD)(medias[DRV_SDMMC].size)*2;
                }
                res = RES_OK; 
                break; 

            case GET_SECTOR_SIZE :   /* Get sectors on the disk (WORD) */ 
                //*(WORD*)buff = medias[DRV_MMC].blockSize;
                if (medias[DRV_SDMMC].blockSize < SECTOR_SIZE_DEFAULT)
                    *(WORD*)buff = SECTOR_SIZE_DEFAULT;
                else
                    *(WORD*)buff =medias[DRV_SDMMC].blockSize;
                res = RES_OK;
                break;

            case CTRL_SYNC :   /* Make sure that data has been written */ 
                res = RES_OK; 
                break; 

            default: 
                res = RES_PARERR; 
        }
    }

    else if (drv==DRV_NAND) {
            switch (ctrl) { 

                case GET_BLOCK_SIZE:
                    *(WORD*)buff = 1; 
                    res = RES_OK; 
                    break; 

                case GET_SECTOR_COUNT :   /* Get number of sectors on the disk (DWORD) */ 
                    if (medias[DRV_NAND].blockSize < SECTOR_SIZE_DEFAULT)
                        *(DWORD*)buff = (DWORD)(medias[DRV_NAND].size /
                                                (SECTOR_SIZE_DEFAULT /
                                                medias[DRV_NAND].blockSize));
                    else
                        *(DWORD*)buff = (DWORD)(medias[DRV_NAND].size);
                    res = RES_OK;
                    break; 

                case GET_SECTOR_SIZE :	 /* Get sectors on the disk (WORD) */ 
                    //*(WORD*)buff = medias[DRV_MMC].blockSize;
                    if (medias[DRV_NAND].blockSize < SECTOR_SIZE_DEFAULT)
                        *(WORD*)buff = SECTOR_SIZE_DEFAULT;
                    else
                        *(WORD*)buff = medias[DRV_NAND].blockSize;
                    res = RES_OK;
                    break;

                case CTRL_SYNC :   /* Make sure that data has been written */ 
                    MED_Flush(&medias[DRV_NAND]);
                    res = RES_OK; 
                    break; 

                default: 
                    res = RES_PARERR; 
        }

    }

   return res; 
}


