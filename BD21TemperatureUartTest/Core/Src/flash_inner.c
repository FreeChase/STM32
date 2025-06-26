#include "main.h"
#include "../component/xprintf/xprintf.h"
#include "../../Middlewares/Third_Party/letter-shell/src/shell.h"
#include "flash_inner.h"
#include "usart.h"
//* stm32 inner flash interface

//? 此值考虑存储置flash
uint32_t gRecordBaseAddr = ADDR_FLASH_SECTOR_11;

/**
  * @brief  Gets the sector of a given address
  * @param  None
  * @retval The sector of a given address
  */
static uint32_t GetInnerFlashSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_SECTOR_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_SECTOR_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_SECTOR_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_SECTOR_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_SECTOR_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_SECTOR_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_SECTOR_6;  
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_SECTOR_7;  
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_SECTOR_8;  
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_SECTOR_9;  
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_SECTOR_10;  
  }
  else /* (Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11) */
  {
    sector = FLASH_SECTOR_11;
  }

  return sector;
}


/**
  * @brief  Gets sector Size
  * @param  None
  * @retval The size of a given sector
  */
static uint32_t  GetInnerFlashSectorSize(uint32_t Sector)
{
  uint32_t sectorsize = 0x00;

  if((Sector == FLASH_SECTOR_0) || (Sector == FLASH_SECTOR_1) || (Sector == FLASH_SECTOR_2) || (Sector == FLASH_SECTOR_3))
  {
    sectorsize = 16 * 1024;
  }
  else if(Sector == FLASH_SECTOR_4)
  {
    sectorsize = 64 * 1024;
  }
  else
  {
    sectorsize = 128 * 1024;
  }  
  return sectorsize;
}


/**
  * @brief  Write some data to inner flash
  * @param  None
  * @retval The sector of a given address
  */
uint32_t InnerFlashWrite(uint32_t Address,char * pdata, uint32_t byteNum)
{

    //* 0.flash unlock
    HAL_FLASH_Unlock();

    //* 1.读取数据,与新数据组成 新sector

    //* 2.擦除扇区

    //* 3.写动作

    //* 4.flash lock
    HAL_FLASH_Lock(); 

		return 0;
}
/**
  * @brief  Write some data to inner flash
  * @param  None
  * @retval 0 success other fail
  */
uint32_t InnerFlashErase(uint32_t startAddress,uint32_t endAddress)
{
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError = 0;
    uint32_t FirstSector = 0, NbOfSectors = 0;
    //* 擦除保护
    if(startAddress < FLASH_PARAMS_START || endAddress > FLASH_PARAMS_END)
    {

        xprintf("param error !(%s)\r\n",__FUNCTION__);
        return 1;
    }
    /* Get the 1st sector to erase */
    FirstSector = GetInnerFlashSector(startAddress);
    /* Get the number of sector to erase from 1st sector*/
    NbOfSectors = GetInnerFlashSector(endAddress) - FirstSector + 1;

    //* 0.flash unlock
    HAL_FLASH_Unlock();

    __HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
    __HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
    __HAL_UART_DISABLE_IT(&huart3, UART_IT_RXNE);
    __HAL_UART_DISABLE_IT(&huart4, UART_IT_RXNE);
    __HAL_UART_DISABLE_IT(&huart5, UART_IT_RXNE);
    /* Fill EraseInit structure*/
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    EraseInitStruct.Sector = FirstSector;
    EraseInitStruct.NbSectors = NbOfSectors;
    if(HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
    { 
        /* 
        Error occurred while sector erase. 
        User can add here some code to deal with this error. 
        SectorError will contain the faulty sector and then to know the code error on this sector,
        user can call function 'HAL_FLASH_GetError()'
        */
        /*
        FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError();
        */
       xprintf("flash erase fail(%s)!\r\n",__FUNCTION__);
        // Error_Handler();
    }

    //* 4.flash lock
    HAL_FLASH_Lock(); 
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
    __HAL_UART_ENABLE_IT(&huart4, UART_IT_RXNE);
    __HAL_UART_ENABLE_IT(&huart5, UART_IT_RXNE);
	return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), InnerFlashErase, InnerFlashErase, startaddr endaddr);


/**
  * @brief  Self increment write
  * @param  None
  * @retval The sector of a given address
  */
uint32_t InnerFlashSelfIncWrite(char * pdata, uint32_t byteNum)
{
    HAL_StatusTypeDef ret = HAL_OK;
    int  i = 0;
    uint32_t writeAddr = gRecordBaseAddr;
    //* 总共控制20个LED， 单次数据写(20byte)， 按32取整往后写
    //* 使用最后一个sector存储
    
    if(byteNum>32)
    {
        xprintf("param error !(%s)\r\n",__FUNCTION__);
        return 1;
    }
    __HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
    __HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
    __HAL_UART_DISABLE_IT(&huart3, UART_IT_RXNE);
    __HAL_UART_DISABLE_IT(&huart4, UART_IT_RXNE);
    __HAL_UART_DISABLE_IT(&huart5, UART_IT_RXNE);

    HAL_FLASH_Unlock();

    //* 按32byte为一个写入基本单元， 0x10000 / 32 = 2048
    while (1)
    {
        //*找到合适的写入位置
        if ((*(uint8_t *)writeAddr == 0xff) && (writeAddr < (gRecordBaseAddr + 0x10000)))
        {
            for (i = 0; i < byteNum; i++)
            {
                ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, writeAddr + i, pdata[i]);
                if( ret != HAL_OK)
                {
                  xprintf("Flash Program Error(%d)\r\n",ret);
                }
            }
            // 写完20字节，跳出
            break;
        }
        //* 32byte为基本单元
        writeAddr += 32;
    }

    HAL_FLASH_Lock();
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
    __HAL_UART_ENABLE_IT(&huart4, UART_IT_RXNE);
    __HAL_UART_ENABLE_IT(&huart5, UART_IT_RXNE); 
    
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), InnerFlashSelfIncWrite, InnerFlashSelfIncWrite, selfIncreWrite pdata num);