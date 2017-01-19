/**
  ******************************************************************************
  * @file    Redef_GPIO.h
  * @author  
  * @version V1.0
  * @date    6-May-2015
  * @brief   GPIO调用重定义
             
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __REDEF_GPIO_H
#define __REDEF_GPIO_H

/* Includes ------------------------------------------------------------------*/
#if defined (STM32F10X_HD)
 #include "stm32f10x.h"
#elif defined (STM32F2XX)
 #include "stm32f2xx.h"
#elif defined (STM32F40_41xxx)
 #include "stm32f4xx.h"
#endif

/* Define --------------------------------------------------------------------*/
#define PA(x)       (16*0 + (x))
#define PB(x)       (16*1 + (x))
#define PC(x)       (16*2 + (x))
#define PD(x)       (16*3 + (x))
#define PE(x)       (16*4 + (x))
#define PF(x)       (16*5 + (x))
#define PG(x)       (16*6 + (x))
#define PH(x)       (16*7 + (x))

//GPIO_Port
#if defined (__STM32F10x_H)
    #define IO_GPIO_PORT(n) \
            (\
                ((uint16_t)n /16 == 0)?GPIOA:\
              ( ((uint16_t)n /16 == 1)?GPIOB:\
              ( ((uint16_t)n /16 == 2)?GPIOC:\
              ( ((uint16_t)n /16 == 3)?GPIOD:\
              ( ((uint16_t)n /16 == 4)?GPIOE:\
              ( ((uint16_t)n /16 == 5)?GPIOF:GPIOG\
              )))))\
            )
#elif defined (__STM32F2xx_H)
    #define IO_GPIO_PORT(n) \
            (\
                ((n)/16 == 0)?GPIOA:\
              ( ((n)/16 == 1)?GPIOB:\
              ( ((n)/16 == 2)?GPIOC:\
              ( ((n)/16 == 3)?GPIOD:\
              ( ((n)/16 == 4)?GPIOE:\
              ( ((n)/16 == 5)?GPIOF:\
              ( ((n)/16 == 6)?GPIOG:GPIOH\
              ))))))\
            )
#elif defined (__STM32F4xx_H)
    #define IO_GPIO_PORT(n) \
            (\
                ((n)/16 == 0)?GPIOA:\
              ( ((n)/16 == 1)?GPIOB:\
              ( ((n)/16 == 2)?GPIOC:\
              ( ((n)/16 == 3)?GPIOD:\
              ( ((n)/16 == 4)?GPIOE:\
              ( ((n)/16 == 5)?GPIOF:\
              ( ((n)/16 == 6)?GPIOG:GPIOH\
              ))))))\
            )
#endif

        
//GPIO_Pin
#define IO_GPIO_PIN(n)              ((uint16_t)(1 << ((n)%16)) )

//GPIO_PinSource
#define IO_GPIO_PINSOURCE(n)        ((uint8_t) ( (n)%16 ) )

//RCC_AHB1_Periph_GPIO
#define IO_RCC_AHB1_PERIPH(n)     ((uint32_t)(1 << ((n)/16)) )

//RCC_APB2_PERIPH_GPIO
#define IO_RCC_APB2_PERIPH_GPIO(n)     ((uint32_t)(1 << (2 + (n /16)) ))

//EXTI_PORTSOURCE
#define IO_EXTI_PORTSOURCE(n)       ((uint8_t) ( (n)/16 ) )

//EXTI_LINE
#define IO_EXTI_LINE(n)             ((uint32_t)(1 << ((n)%16)) )

//EXTI_IRQn
#define IO_EXTI_IRQN(n)             \
        (\
            ((n)%16 < 5)?((n)%16 + EXTI0_IRQn):\
          ( ((n)%16 < 10)?EXTI9_5_IRQn:EXTI15_10_IRQn\
          )\
        )
        
//GPIO_ReadInputDataBit
#define IO_GPIO_ReadInputDataBit(n)    GPIO_ReadInputDataBit(IO_GPIO_PORT(n), IO_GPIO_PIN(n))

#define IO_GPIO_SetBits(n)             GPIO_SetBits(IO_GPIO_PORT(n), IO_GPIO_PIN(n))
#define IO_GPIO_ResetBits(n)           GPIO_ResetBits(IO_GPIO_PORT(n), IO_GPIO_PIN(n))
#define IO_GPIO_ToggleBits(n)          GPIO_ToggleBits(IO_GPIO_PORT(n), IO_GPIO_PIN(n))


#define IO_USART1_TX   PA(9)   //PA(9), PB(6)
#define IO_USART1_RX   PA(10)  //PA(10),PB(7)

#define IO_USART2_TX   PA(2)   //PA(2), PD(5)
#define IO_USART2_RX   PA(3)   //PA(3), PD(6)

#define IO_USART3_TX   PB(10)  //PB(10),PC(10),PD(8)
#define IO_USART3_RX   PB(11)  //PB(11),PC(11),PD(9)

#define IO_USART6_TX   PC(6)   //PC(6),PG(10)
#define IO_USART6_RX   PC(7)   //PC(7),PG(9)
/* Private functions ---------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

#endif //Redef_GPIO.h
