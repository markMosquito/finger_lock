/**
  ******************************************************************************
  * @file    Rsing_Led.c
  * @author  Rsing
  * @version V1.0
  * @date    29-November-2014
  * @brief   Initializate the Led.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "Rsing_Led.h"
#include "Redef_GPIO.h"


/* Private variables ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define LEDn                             3
#define IO_LED0     PA(0)   //B1
#define IO_LED1     PA(1)   //R1
#define IO_LED2     PA(2)  //G1
//#define IO_LED3     PA(0)
//#define IO_LED4     PA(1)
//#define IO_LED5     PA(2)

#define LED0_PIN                         IO_GPIO_PIN(IO_LED0)
#define LED0_GPIO_PORT                   IO_GPIO_PORT(IO_LED0)
#define LED0_GPIO_CLK                    IO_RCC_AHB1_PERIPH(IO_LED0)  
  
#define LED1_PIN                         IO_GPIO_PIN(IO_LED1)
#define LED1_GPIO_PORT                   IO_GPIO_PORT(IO_LED1)
#define LED1_GPIO_CLK                    IO_RCC_AHB1_PERIPH(IO_LED1)  
  
#define LED2_PIN                         IO_GPIO_PIN(IO_LED2)
#define LED2_GPIO_PORT                   IO_GPIO_PORT(IO_LED2)
#define LED2_GPIO_CLK                    IO_RCC_AHB1_PERIPH(IO_LED2)  
  
#define LED3_PIN                         IO_GPIO_PIN(IO_LED3)
#define LED3_GPIO_PORT                   IO_GPIO_PORT(IO_LED3)
#define LED3_GPIO_CLK                    IO_RCC_AHB1_PERIPH(IO_LED3)

//#define LED4_PIN                         IO_GPIO_PIN(IO_LED4)
//#define LED4_GPIO_PORT                   IO_GPIO_PORT(IO_LED4)
//#define LED4_GPIO_CLK                    IO_RCC_AHB1_PERIPH(IO_LED4)

//#define LED5_PIN                         IO_GPIO_PIN(IO_LED5)
//#define LED5_GPIO_PORT                   IO_GPIO_PORT(IO_LED5)
//#define LED5_GPIO_CLK                    IO_RCC_AHB1_PERIPH(IO_LED5)

//#define LED6_PIN                         IO_GPIO_PIN(IO_LED6)
//#define LED6_GPIO_PORT                   IO_GPIO_PORT(IO_LED6)
//#define LED6_GPIO_CLK                    IO_RCC_AHB1_PERIPH(IO_LED6)

GPIO_TypeDef* GPIO_PORT[LEDn] = {LED0_GPIO_PORT, LED1_GPIO_PORT, LED2_GPIO_PORT};
const uint16_t GPIO_PIN[LEDn] = {LED0_PIN, LED1_PIN, LED2_PIN};
const uint32_t GPIO_CLK[LEDn] = {LED0_GPIO_CLK, LED1_GPIO_CLK, LED2_GPIO_CLK};
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Configures LED GPIO.
  * @param  Led: Specifies the Led to be configured. 
  *   This parameter can be one of following parameters:
  *     @arg LED0
  *     @arg LED1
  *     @arg LED2
  *     @arg LED3
  * @retval None
  */
void LEDInit(Led_TypeDef Led)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* GPIO clock enable */
  RCC_AHB1PeriphClockCmd(GPIO_CLK[Led], ENABLE);
  
  GPIO_InitStructure.GPIO_Pin =  GPIO_PIN[Led];
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIO_PORT[Led], &GPIO_InitStructure);
  LEDOff(Led);
}

/**
  * @brief  Turns selected LED On.
  * @param  Led: Specifies the Led to be set on. 
  *   This parameter can be one of following parameters:
  *     @arg LED0
  *     @arg LED1
  *     @arg LED2
  *     @arg LED3  
  * @retval None
  */
void LEDOn(Led_TypeDef Led)
{
  GPIO_PORT[Led]->BSRRL = GPIO_PIN[Led]; 
}

/**
  * @brief  Turns selected LED Off.
  * @param  Led: Specifies the Led to be set off. 
  *   This parameter can be one of following parameters:
  *     @arg LED0
  *     @arg LED1
  *     @arg LED2
  *     @arg LED3 
  * @retval None
  */
void LEDOff(Led_TypeDef Led)
{
  GPIO_PORT[Led]->BSRRH = GPIO_PIN[Led]; 
}

/**
  * @brief  Toggles the selected LED.
  * @param  Led: Specifies the Led to be toggled. 
  *   This parameter can be one of following parameters:
  *     @arg LED0
  *     @arg LED1
  *     @arg LED2
  *     @arg LED3  
  * @retval None
  */
void LEDToggle(Led_TypeDef Led)
{
  GPIO_PORT[Led]->ODR ^= GPIO_PIN[Led];
}
/*********************************END OF FILE**********************************/
