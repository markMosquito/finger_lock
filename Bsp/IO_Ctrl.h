/**
  ******************************************************************************
  * @file    BSP.h
  * @author  
  * @version V1.0
  * @date    7-May-2015
  * @brief   Header file for BSP.c module. 
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __IO_CTRL_H
#define __IO_CTRL_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"
#include "Redef_GPIO.h"


#define IO_FINGER_DEV_POWER    PB(1)
#define IO_LOCK_MOTOR_1       PB(8)
#define IO_LOCK_MOTOR_2        PB(9)
#define IO_DEFAULT_KEY       PC(1)

#define FINGER_DEV_POWER_ON   GPIO_SetBits(IO_GPIO_PORT(IO_FINGER_DEV_POWER), IO_GPIO_PIN(IO_FINGER_DEV_POWER))
#define FINGER_DEV_POWER_OFF   GPIO_ResetBits(IO_GPIO_PORT(IO_FINGER_DEV_POWER), IO_GPIO_PIN(IO_FINGER_DEV_POWER))

#define LOCK_MOTOR_OPEN   GPIO_SetBits(IO_GPIO_PORT(IO_LOCK_MOTOR_1), IO_GPIO_PIN(IO_LOCK_MOTOR_1));\
							GPIO_ResetBits(IO_GPIO_PORT(IO_LOCK_MOTOR_2), IO_GPIO_PIN(IO_LOCK_MOTOR_2));\
							vTaskDelay( 200 * portTICK_MS);\
							GPIO_ResetBits(IO_GPIO_PORT(IO_LOCK_MOTOR_1), IO_GPIO_PIN(IO_LOCK_MOTOR_1));\
							GPIO_ResetBits(IO_GPIO_PORT(IO_LOCK_MOTOR_2), IO_GPIO_PIN(IO_LOCK_MOTOR_2))
							
							
#define LOCK_MOTOR_CLOSE   GPIO_ResetBits(IO_GPIO_PORT(IO_LOCK_MOTOR_1), IO_GPIO_PIN(IO_LOCK_MOTOR_1));\
							GPIO_SetBits(IO_GPIO_PORT(IO_LOCK_MOTOR_2), IO_GPIO_PIN(IO_LOCK_MOTOR_2));\
							vTaskDelay( 200 * portTICK_MS);\
							GPIO_ResetBits(IO_GPIO_PORT(IO_LOCK_MOTOR_1), IO_GPIO_PIN(IO_LOCK_MOTOR_1));\
							GPIO_ResetBits(IO_GPIO_PORT(IO_LOCK_MOTOR_2), IO_GPIO_PIN(IO_LOCK_MOTOR_2))
		
#define LOCK_MOTOR_STOP   GPIO_ResetBits(IO_GPIO_PORT(IO_LOCK_MOTOR_1), IO_GPIO_PIN(IO_LOCK_MOTOR_1));\
							GPIO_ResetBits(IO_GPIO_PORT(IO_LOCK_MOTOR_2), IO_GPIO_PIN(IO_LOCK_MOTOR_2))		
							
#define READ_DEFAULT_KEY   	IO_GPIO_ReadInputDataBit(IO_DEFAULT_KEY)	
/* Exported macro ------------------------------------------------------------*/
/* Exported Function----------------------------------------------------------*/

#endif //BSP.h
