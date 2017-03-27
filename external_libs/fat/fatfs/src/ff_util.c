/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support 
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include "ff_util.h"


#if _USE_LFN
#include "option/ccsbcs.c"
char Lfname[_MAX_LFN+1];
#endif

#define FRESULT_ENTRY(f)        { (f), (#f) } 
//------------------------------------------------------------------------------
//         Global functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Scan the directory passed in argument and display all the files and 
/// directories contained in it and in its subdirectories
/// (recursive function)
/// \param path  directory path to scan
//------------------------------------------------------------------------------

void FF_ScanDir(char* path)
{
    FILINFO finfo;
    DIR dirs;
    int i;
    char *fn;
#if _USE_LFN
    finfo.lfname = Lfname;
    finfo.lfsize = sizeof(Lfname);
    memset(finfo.lfname,0,sizeof(Lfname));
#endif
    if (f_opendir(&dirs, path) == FR_OK) {
      
        i = strlen(path);
        while ((f_readdir(&dirs, &finfo) == FR_OK) && finfo.fname[0]) {
          
            if (finfo.fattrib & AM_DIR) {
#if _USE_LFN
		fn = *finfo.lfname ? finfo.lfname : finfo.fname;
#else
		fn = finfo.fname;
#endif              
                sprintf(&path[i], "/%s", fn);
                printf("%s\n\r", path);
                FF_ScanDir(path);
                path[i] = 0;
            } 
            else {
#if _USE_LFN
		fn = *finfo.lfname ? finfo.lfname : finfo.fname;
#else
		fn = finfo.fname;
#endif
                printf("%s/%s\n\r", path, fn);
            }
        }
    }
}

//------------------------------------------------------------------------------
/// Get Time for FatFs
//------------------------------------------------------------------------------
DWORD get_fattime( void )
{
    unsigned int date;
    if(OemTime.year<2011)
    {
    date = ((2011 - 1980) << 25)
        | (9 << 21)
        | (1 << 16)
        | (WORD) (12 << 11)
        | (WORD) (0 << 5)
        | (WORD) (0 >> 1);
    }
    else
    {
    date = ((OemTime.UTCyear - 1980) << 25)
        | (OemTime.UTCmonth << 21)
        | (OemTime.UTCday << 16)
        | (WORD) (OemTime.UTChour << 11)
        | (WORD) (OemTime.min << 5)
        | (WORD) (OemTime.sec >> 1);
    }
    return date;
}

//------------------------------------------------------------------------------
// Convert file access error number in string
//------------------------------------------------------------------------------
const char* FF_GetStrResult(FRESULT res)
{
    switch(res) {
        case FR_OK :              return "FR_OK";
        case FR_DISK_ERR :        return "FR_DISK_ERR";
        case FR_INT_ERR :         return "FR_INT_ERR";
        
        case FR_NOT_READY :       return "FR_NOT_READY";
        case FR_NO_FILE :         return "FR_NO_FILE";
        case FR_NO_PATH :         return "FR_NO_PATH";
        case FR_INVALID_NAME :    return "FR_INVALID_NAME";
        case FR_DENIED :          return "FR_DENIED";   
        case FR_EXIST :           return "FR_EXIST"; 
        
        case FR_INVALID_OBJECT :  return "FR_INVALID_OBJECT";
        case FR_WRITE_PROTECTED : return "FR_WRITE_PROTECTED";
        case FR_INVALID_DRIVE :   return "FR_INVALID_DRIVE";
        case FR_NOT_ENABLED :     return "FR_NOT_ENABLED";
        case FR_NO_FILESYSTEM :   return "FR_NO_FILESYSTEM";
        case FR_MKFS_ABORTED :    return "FR_MKFS_ABORTED";  
        case FR_TIMEOUT :         return "FR_TIMEOUT";  
        default : return "?";
    }  
}
