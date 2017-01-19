/**
  ******************************************************************************
  * @file    Rsing_Led.h
  * @author  Rsing
  * @version V1.0
  * @date    29-November-2014
  * @brief   Header file for Rsing_Led.c module. 
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RSING_LED_H
#define __RSING_LED_H

/* Includes ------------------------------------------------------------------*/
 #include "stm32f2xx.h"
/* Private functions ---------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef enum 
{
  LED1B = 0,//B1    //靠近车中心
  LED1R = 1,//R1
  LED1G = 2,//G1
  LED2B = 3,//B2
  LED2R = 4,//R2
  LED2G = 5,//R2
} Led_TypeDef;
/* Exported constants --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
void LEDInit(Led_TypeDef Led);
void LEDOn(Led_TypeDef Led);
void LEDOff(Led_TypeDef Led);
void LEDToggle(Led_TypeDef Led);
#endif //Rsing_Led.h
