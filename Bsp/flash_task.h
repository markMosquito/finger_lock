/**
  ******************************************************************************
  * @file    HitTask.h
  * @author  
  * @version V1.0
  * @date    7-May-2015
  * @brief   Header file for HitTask.c module. 
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLASH_TASK_H
#define __FLASH_TASK_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"
#include "app.h"

int main1(void);
int writeRegInfoToFlash(void);
int readRegInfoFromFlash(void);


#endif //HitTask.h
