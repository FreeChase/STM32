#ifndef __FLASH_INNER_H__
#define __FLASH_INNER_H__

#include "stdint.h"


#ifdef __cplusplus
extern "C" {
#endif

#define FLASH_PARAMS_START      (0x080e0000)
#define FLASH_PARAMS_END        (0x08100000)


uint32_t InnerFlashWrite(uint32_t Address,char * pdata, uint32_t byteNum);
uint32_t InnerFlashErase(uint32_t startAddress,uint32_t endAddress);
uint32_t InnerFlashSelfIncWrite(char * pdata, uint32_t byteNum);

#ifdef __cplusplus
}
#endif

#endif 

