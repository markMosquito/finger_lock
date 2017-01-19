/**
  ******************************************************************************
  * @file    Base_Fun.c
  * @author
  * @version V1.0
  * @date    6-May-2015
  * @brief   基本初始化函数
             包含：
                MyGPIOInit
                MyAFGPIOInit
                MyNVICInit
                MyUSARTInit
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "Base_Fun.h"

/* Functions -----------------------------------------------------------------*/
void MyGPIOInit(GPIO_TypeDef    * GPIOx,
    uint32_t          GPIO_Pin,
    GPIOMode_TypeDef  GPIO_Mode,
    GPIOSpeed_TypeDef GPIO_Speed,
    GPIOOType_TypeDef GPIO_OType,
    GPIOPuPd_TypeDef  GPIO_PuPd)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    switch ((uint32_t)GPIOx)
    {
        case (uint32_t)GPIOA: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); break;
        case (uint32_t)GPIOB: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); break;
        case (uint32_t)GPIOC: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); break;
        case (uint32_t)GPIOD: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); break;
        case (uint32_t)GPIOE: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); break;
        case (uint32_t)GPIOF: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); break;
        case (uint32_t)GPIOG: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE); break;
        case (uint32_t)GPIOH: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE); break;
        case (uint32_t)GPIOI: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE); break;
        default: break;
    }

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed;
    GPIO_InitStructure.GPIO_OType = GPIO_OType;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd;
    GPIO_Init(GPIOx, &GPIO_InitStructure);
}

void MyAFGPIOInit(GPIO_TypeDef    * GPIOx,
    uint32_t          GPIO_Pin,
    GPIOSpeed_TypeDef GPIO_Speed,
    GPIOOType_TypeDef GPIO_OType,
    GPIOPuPd_TypeDef  GPIO_PuPd,
    uint8_t           GPIO_AF)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    switch ((uint32_t)GPIOx)
    {
        case (uint32_t)GPIOA: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); break;
        case (uint32_t)GPIOB: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); break;
        case (uint32_t)GPIOC: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); break;
        case (uint32_t)GPIOD: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); break;
        case (uint32_t)GPIOE: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); break;
        case (uint32_t)GPIOF: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); break;
        case (uint32_t)GPIOG: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE); break;
        case (uint32_t)GPIOH: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE); break;
        case (uint32_t)GPIOI: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE); break;
        default: break;
    }
    if (GPIO_Pin & GPIO_Pin_0)
        GPIO_PinAFConfig(GPIOx, GPIO_PinSource0, GPIO_AF);
    if (GPIO_Pin & GPIO_Pin_1)
        GPIO_PinAFConfig(GPIOx, GPIO_PinSource1, GPIO_AF);
    if (GPIO_Pin & GPIO_Pin_2)
        GPIO_PinAFConfig(GPIOx, GPIO_PinSource2, GPIO_AF);
    if (GPIO_Pin & GPIO_Pin_3)
        GPIO_PinAFConfig(GPIOx, GPIO_PinSource3, GPIO_AF);
    if (GPIO_Pin & GPIO_Pin_4)
        GPIO_PinAFConfig(GPIOx, GPIO_PinSource4, GPIO_AF);
    if (GPIO_Pin & GPIO_Pin_5)
        GPIO_PinAFConfig(GPIOx, GPIO_PinSource5, GPIO_AF);
    if (GPIO_Pin & GPIO_Pin_6)
        GPIO_PinAFConfig(GPIOx, GPIO_PinSource6, GPIO_AF);
    if (GPIO_Pin & GPIO_Pin_7)
        GPIO_PinAFConfig(GPIOx, GPIO_PinSource7, GPIO_AF);
    if (GPIO_Pin & GPIO_Pin_8)
        GPIO_PinAFConfig(GPIOx, GPIO_PinSource8, GPIO_AF);
    if (GPIO_Pin & GPIO_Pin_9)
        GPIO_PinAFConfig(GPIOx, GPIO_PinSource9, GPIO_AF);
    if (GPIO_Pin & GPIO_Pin_10)
        GPIO_PinAFConfig(GPIOx, GPIO_PinSource10, GPIO_AF);
    if (GPIO_Pin & GPIO_Pin_11)
        GPIO_PinAFConfig(GPIOx, GPIO_PinSource11, GPIO_AF);
    if (GPIO_Pin & GPIO_Pin_12)
        GPIO_PinAFConfig(GPIOx, GPIO_PinSource12, GPIO_AF);
    if (GPIO_Pin & GPIO_Pin_13)
        GPIO_PinAFConfig(GPIOx, GPIO_PinSource13, GPIO_AF);
    if (GPIO_Pin & GPIO_Pin_14)
        GPIO_PinAFConfig(GPIOx, GPIO_PinSource14, GPIO_AF);
    if (GPIO_Pin & GPIO_Pin_15)
        GPIO_PinAFConfig(GPIOx, GPIO_PinSource15, GPIO_AF);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed;
    GPIO_InitStructure.GPIO_OType = GPIO_OType;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd;
    GPIO_Init(GPIOx, &GPIO_InitStructure);
}


void MyNVICInit(uint8_t NVIC_IRQChannel,
    uint8_t NVIC_IRQChannelPreemptionPriority,
    uint8_t NVIC_IRQChannelSubPriority,
    FunctionalState NVIC_IRQChannelCmd)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = NVIC_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_IRQChannelPreemptionPriority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_IRQChannelSubPriority;
    NVIC_InitStructure.NVIC_IRQChannelCmd = NVIC_IRQChannelCmd;
    NVIC_Init(&NVIC_InitStructure);
}

void MyUSARTInit(USART_TypeDef* USARTx, uint32_t USART_BaudRate)
{
    USART_InitTypeDef USART_InitStructure;
    switch ((uint32_t)USARTx)
    {
        case (uint32_t)USART1: RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); break;
        case (uint32_t)USART2: RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); break;
        case (uint32_t)USART3: RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); break;
        case (uint32_t)UART4:  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE); break;
        case (uint32_t)UART5:  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE); break;
        case (uint32_t)USART6: RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE); break;
        
        default: break;
    }
    
    USART_InitStructure.USART_BaudRate = USART_BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USARTx, &USART_InitStructure);
    USART_Cmd(USARTx, ENABLE);
}

/**
 * @brief: 两个粗略延时函数,168Mhz主频下
 */
void delay_ms(unsigned int t)
{
    int i;
    for (i = 0; i < t; i++)
    {
        int a = 42000;
        while (a--);
    }
}

void delay_us(unsigned int t)
{
    int i;
    for (i = 0; i < t; i++)
    {
        int a = 40;
        while (a--);
    }
}


/********************************* END OF FILE ********************************/
