/**
  ******************************************************************************
  * @file    Base_Fun.h
  * @author  
  * @version V1.0
  * @date    6-May-2015
  * @brief   Header file for Base_Fun.c module. 
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BASE_FUN_H
#define __BASE_FUN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported Function----------------------------------------------------------*/
void MyGPIOInit(GPIO_TypeDef    * GPIOx,
                uint32_t          GPIO_Pin,
                GPIOMode_TypeDef  GPIO_Mode,
                GPIOSpeed_TypeDef GPIO_Speed,
                GPIOOType_TypeDef GPIO_OType,
                GPIOPuPd_TypeDef  GPIO_PuPd);

void MyAFGPIOInit(GPIO_TypeDef    * GPIOx,
                  uint32_t          GPIO_Pin,
                  GPIOSpeed_TypeDef GPIO_Speed,
                  GPIOOType_TypeDef GPIO_OType,
                  GPIOPuPd_TypeDef  GPIO_PuPd,
                  uint8_t           GPIO_AF);

void MyUSARTInit(USART_TypeDef* USARTx,uint32_t USART_BaudRate);
                    
void MyNVICInit(uint8_t NVIC_IRQChannel,
                uint8_t NVIC_IRQChannelPreemptionPriority,
                uint8_t NVIC_IRQChannelSubPriority,
                FunctionalState NVIC_IRQChannelCmd);

void delay_ms(unsigned int t);
void delay_us(unsigned int t);
#endif //Base_Fun.h
