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
#ifndef __DISPLAY_TASK_H
#define __DISPLAY_TASK_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"
#include "app.h"


u8 RegisterShow(void);
u8 manager_info_identify(void);
int menu_modify_display(void);
int modify_passport(void);
int modify_finger_info(void);
u8 manager_info_identify(void);
int member_finger_info_add(void);
int returnTodefault(void);
#endif //HitTask.h
