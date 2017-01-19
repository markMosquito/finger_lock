/**
  ******************************************************************************
  * @file    Rsing_Flash.c
  * @author  Rsing
  * @version V1.0
  * @date    8-August-2014
  * @brief   Initializate the Flash.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "Rsing_Flash.h"
/* Private variables ---------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define true 1
#define false 0
  
/* Private function prototypes -----------------------------------------------*/
static uint32_t GetSector(uint32_t Address);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Erase the user Flash area.
  * @param  None
  * @retval None
  */
void Flash_EraseAll(void)
{
  uint32_t uwStartSector = 0;
  uint32_t uwEndSector = 0;
//  uint32_t uwAddress = 0;
  uint32_t uwSectorCounter = 0;
  
  /* Unlock the Flash *********************************************************/
  /* Enable the flash control register access */
  FLASH_Unlock();
    
  /* Erase the user Flash area ************************************************/
  /* area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR */

  /* Clear pending flags (if any) */  
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 

  /* Get the number of the start and end sectors */
  uwStartSector = GetSector(FLASH_USER_START_ADDR);
  uwEndSector = GetSector(FLASH_USER_END_ADDR);

  /* Strat the erase operation */
  uwSectorCounter = uwStartSector;
  while (uwSectorCounter <= uwEndSector) 
  {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
       be done by word(32-bit) */ 
    if (FLASH_EraseSector(uwSectorCounter, VoltageRange_3) != FLASH_COMPLETE)
    { 
      /* Error occurred while sector erase. 
         User can add here some code to deal with this error  */
//      while (1)
//      {
//        printf("Error occurred while sector erase.  \r\n");
//      }
    }
    /* jump to the next sector */
      uwSectorCounter += 8;
  }

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) */
  FLASH_Lock(); 
}


u8 flash_erase_sector(uint32_t uwAddress)
{
  uint32_t uwSectorCounter = 0;
  
  /* Unlock the Flash *********************************************************/
  /* Enable the flash control register access */
  FLASH_Unlock();
    
  /* Erase the user Flash area ************************************************/
  /* area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR */

  /* Clear pending flags (if any) */  
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 

  /* Get the number of the start and end sectors */
  uwSectorCounter = GetSector(uwAddress);
  
  if (FLASH_EraseSector(uwSectorCounter, VoltageRange_3) != FLASH_COMPLETE)
  { 
    /* Error occurred while sector erase. 
       User can add here some code to deal with this error  */
//    while (1)
//    {
//    }
    return false;
  }
  
  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) */
  FLASH_Lock(); 
  return true;
}
/*!
 *  @brief      写入32 bit数据到 flash指定地址
 *  @param      uwAddress      扇区号（0 ~ FLASH_SECTOR_NUM）
 *  @param      DATA_32            需要写入的数据
 *  @return     执行结果(1成功，0失败)
 */
u8 flash_write_u32(uint32_t uwAddress, u32 DATA_32)
{
  /* Unlock the Flash *********************************************************/
  /* Enable the flash control register access */
  FLASH_Unlock();
  
  /* Clear pending flags (if any) */  
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 
  
  /* Program the user Flash area word by word ********************************/
  if (FLASH_ProgramWord(uwAddress, DATA_32) == FLASH_COMPLETE)
  {
    ;
  }
  else
  { 
    /* Error occurred while writing data in Flash memory. 
       User can add here some code to deal with this error */
//    while (1)
//    {
      return false;
//    }
  }
  
  /* Lock the Flash to disable the flash control register access (recommended
   to protect the FLASH memory against possible unwanted operation) */
  FLASH_Lock(); 
  return true;
}

/*!
 *  @brief      写入16 bit数据到 flash指定地址
 *  @param      uwAddress      
 *  @param      DATA_16            需要写入的数据
 *  @return     执行结果(1成功，0失败)
 */
u8 flash_write_u16(uint32_t uwAddress, u16 DATA_16)
{
  /* Unlock the Flash *********************************************************/
  /* Enable the flash control register access */
  FLASH_Unlock();
  
  /* Clear pending flags (if any) */  
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 
  
  /* Program the user Flash area word by word ********************************/
  if (FLASH_ProgramHalfWord(uwAddress, DATA_16) == FLASH_COMPLETE)
  {
    ;
  }
  else
  { 
    /* Error occurred while writing data in Flash memory. 
       User can add here some code to deal with this error */
//    while (1)
//    {
      return false;
//    }
  }
  
  /* Lock the Flash to disable the flash control register access (recommended
   to protect the FLASH memory against possible unwanted operation) */
  FLASH_Lock(); 
  return true;
}

/*!
 *  @brief      写入8 bit数据到 flash指定地址
 *  @param      uwAddress      
 *  @param      DATA_8            需要写入的数据
 *  @return     执行结果(1成功，0失败)
 */
u8 flash_write_u8(uint32_t uwAddress, u8 DATA_8)
{
  /* Unlock the Flash *********************************************************/
  /* Enable the flash control register access */
  FLASH_Unlock();
  
  /* Clear pending flags (if any) */  
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 
  
  /* Program the user Flash area word by word ********************************/
  if (FLASH_ProgramByte(uwAddress, DATA_8) == FLASH_COMPLETE)
  {
    ;
  }
  else
  { 
    /* Error occurred while writing data in Flash memory. 
       User can add here some code to deal with this error */
//    while (1)
//    {
      return false;
//    }
  }
  
  /* Lock the Flash to disable the flash control register access (recommended
   to protect the FLASH memory against possible unwanted operation) */
  FLASH_Lock(); 
  return true;
}

/**
  * @brief  Gets the sector of a given address
  * @param  None
  * @retval The sector of a given address
  */
static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_Sector_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_Sector_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_Sector_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_Sector_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_Sector_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_Sector_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_Sector_6;  
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_Sector_7;  
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_Sector_8;  
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_Sector_9;  
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_Sector_10;  
  }
  else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
  {
    sector = FLASH_Sector_11;  
  }

  return sector;
}

/*********************************END OF FILE**********************************/
