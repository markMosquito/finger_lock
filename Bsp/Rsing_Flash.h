/**
  ******************************************************************************
  * @file    Rsing_Flash.h
  * @author  Rsing
  * @version V1.0
  * @date    8-August-2014
  * @brief   Header file for Rsing_Flash.c module. 
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RSING_FLASH_H
#define __RSING_FLASH_H

/* Includes ------------------------------------------------------------------*/
 #include "stm32f4xx.h"

/* Private functions ---------------------------------------------------------*/
#define flash_read(uwAddress,type)         (*(__IO type*)(uwAddress))
/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
#define FLASH_USER_START_ADDR   ADDR_FLASH_SECTOR_0   /* Start address of user Flash area */
#define FLASH_USER_END_ADDR     ADDR_FLASH_SECTOR_11  /* End address of user Flash area */

/* Base address of the Flash sectors */ 
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base address of Sector 0, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base address of Sector 1, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base address of Sector 2, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base address of Sector 3, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base address of Sector 4, 64 Kbytes   */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base address of Sector 5, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base address of Sector 6, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base address of Sector 7, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base address of Sector 8, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base address of Sector 9, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base address of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base address of Sector 11, 128 Kbytes */

/* Exported functions --------------------------------------------------------*/
u8 flash_erase_sector(uint32_t uwAddress);
u8 flash_write_u32(uint32_t uwAddress, u32 DATA_32);
u8 flash_write_u16(uint32_t uwAddress, u16 DATA_16);
u8 flash_write_u8(uint32_t uwAddress, u8 DATA_8);
//u8 flash_read(uint32_t uwAddress, u32 *DATA_32);
#endif //Rsing_Flash.h
