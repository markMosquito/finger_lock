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
#ifndef __FINGERTASK_H
#define __FINGERTASK_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"
#include "app.h"

union Password
{
	unsigned char ch[8];
	int pass[2];
};

extern int registered_flag;
extern int member_number;
extern int member_id[100];

extern u8 finger_dev_opened;

#endif //HitTask.h
