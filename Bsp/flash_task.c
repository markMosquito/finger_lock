#include "flash_task.h"
#include "Com2Server.h"
#include "oled.h"
#include "finger.h"


extern union Password password;

#define ADDR_FLASH_START     ((uint32_t)0x080E0000)  /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR     FLASH_Sector_11

uint32_t addr = 0;
int data = 0;
int writeRegInfoToFlash(void)
{
	int i = 0;
	/* Unlock the Flash to enable the flash control register access *************/ 
    FLASH_Unlock();
	
	/* Clear pending flags (if any) */  
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 
	
	FLASH_EraseSector(ADDR_FLASH_SECTOR, VoltageRange_3);//先擦除才能写
	//  注册否|用户密码前四字节|用户密码后四字节|成员数目|各成员ID|家长第一
	addr = ADDR_FLASH_START;
	if(FLASH_ProgramWord(addr, registered_flag) == FLASH_COMPLETE)
	{
		addr += 4;
	}
	else
	{
		FLASH_Lock();
		return 0;
	}
	
	
	if(FLASH_ProgramWord(addr, password.pass[0]) == FLASH_COMPLETE)
	{
		addr += 4;
	}
	else
	{
		FLASH_Lock();
		return 0;
	}
	
	if(FLASH_ProgramWord(addr, password.pass[1]) == FLASH_COMPLETE)
	{
		addr += 4;
	}
	else
	{
		FLASH_Lock();
		return 0;
	}
	
	if(FLASH_ProgramWord(addr, member_number) == FLASH_COMPLETE)
	{
		addr += 4;
	}
	else
	{
		FLASH_Lock();
		return 0;
	}
	
	for(i = 0; i< member_number;i++)
	{
		if(FLASH_ProgramWord(addr, member_id[i])== FLASH_COMPLETE )
		{
			addr += 4;
		}
		else
		{
			FLASH_Lock();
			return 0;
		}
		
	}
	/* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
	FLASH_Lock();
	return 1;
}
int readRegInfoFromFlash(void)
{
	int i = 0;
	
	addr = ADDR_FLASH_START;
	registered_flag = *(__IO uint32_t*)addr;
	addr += 4;
	
	password.pass[0] = *(__IO uint32_t*)addr;
	addr += 4;

	password.pass[1] = *(__IO uint32_t*)addr;
	addr += 4;
	
	member_number = *(__IO uint32_t*)addr;
	addr += 4;
	
	for(i = 0; i< member_number;i++)
	{
		member_id[i] = *(__IO uint32_t*)addr;
		addr += 4;
	}
	return 1;
}
void vFlashTask( void *p_arg )
{
	
	
	(void)p_arg;
	
	//main1();
	
	
	
	readRegInfoFromFlash();
	while(1)
	{
		
		vTaskDelay( 5 * portTICK_MS);
	}
}
