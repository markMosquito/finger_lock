/**
  ******************************************************************************
  * @file    Rsing_12864.h
  * @author  Rsing
  * @version V1.0
  * @date    17-July-2014
  * @brief   Header file for Rsing_12864.c module. 
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RSING_12864_H
#define __RSING_12864_H

/* Includes ------------------------------------------------------------------*/
#if defined (STM32F10X_HD) || defined (STM32F10X_MD)
 #include "stm32f10x.h"
#elif defined (STM32F40_41xxx) || defined (STM32F40XX)
 #include "stm32f4xx.h"
#endif

#include "Redef_GPIO.h"

/* Defines -------------------------------------------------------------------*/
#define PSB_SELECT    0                     //1£º²¢¿Ú£¬0£º´®¿Ú
#define USE_PSB       0

#define IO_RST          PC(4)
#define IO_RS           PB(1)
#define IO_RW           PB(0)
#define IO_EN           PC(5)

/********************************* ¸÷¶Ë¿Ú¶¨Òå *********************************/
#if defined (STM32F10X_HD) || defined (STM32F10X_MD)

#if USE_PSB == 1
    #define GPIO_PSB          GPIOA                 //1£º²¢¿Ú¿ØÖÆ£¬0£º´®¿Ú¿ØÖÆ
    #define GPIO_PSB_PIN      GPIO_Pin_5
    #define RCC_GPIO_PSB      RCC_APB2Periph_GPIOA 
#endif

    #define GPIO_RST          GPIOC                 //0£º¸´Î»
    #define GPIO_RST_PIN      GPIO_Pin_4
    #define RCC_GPIO_RST      RCC_APB2Periph_GPIOC

    #define GPIO_RS       GPIOB                 //1£ºÊý¾Ý£¬0£ºÃüÁî  ;CS£¬´®ÐÐÆ¬Ñ¡
    #define GPIO_RS_PIN   GPIO_Pin_1
    #define RCC_GPIO_RS   RCC_APB2Periph_GPIOB

    #define GPIO_RW       GPIOB                 //0£ºÐ´  £¬1£º¶Á    ;SID£¬´®ÐÐÊý¾ÝÊäÈë
    #define GPIO_RW_PIN   GPIO_Pin_0
    #define RCC_GPIO_RW   RCC_APB2Periph_GPIOB

    #define GPIO_EN       GPIOC                 //Ê¹ÄÜ              ;SCLK£¬´®ÐÐÊ±ÖÓ
    #define GPIO_EN_PIN   GPIO_Pin_5
    #define RCC_GPIO_EN   RCC_APB2Periph_GPIOC

    #define PSB_H  GPIO_PSB->BSRR = GPIO_PSB_PIN  //PSB Ê¹ÄÜ  Êä³ö1
    #define PSB_L  GPIO_PSB->BRR  = GPIO_PSB_PIN  //         Êä³ö0
    #define RST_H  GPIO_RST->BSRR = GPIO_RST_PIN  //RST Ê¹ÄÜ  Êä³ö1
    #define RST_L  GPIO_RST->BRR  = GPIO_RST_PIN  //         Êä³ö0
    #define RS_H   GPIO_RS->BSRR = GPIO_RS_PIN    //RS Ö¸Áî  Êä³ö1
    #define RS_L   GPIO_RS->BRR  = GPIO_RS_PIN    //         Êä³ö0 
    #define RW_H   GPIO_RW->BSRR = GPIO_RW_PIN    //RW ¶ÁÐ´  Êä³ö1
    #define RW_L   GPIO_RW->BRR  = GPIO_RW_PIN    //         Êä³ö0
    #define EN_H   GPIO_EN->BSRR = GPIO_EN_PIN    //EN Ê¹ÄÜ  Êä³ö1
    #define EN_L   GPIO_EN->BRR  = GPIO_EN_PIN    //         Êä³ö0

#elif defined (STM32F40_41xxx) || defined (STM32F40XX)

#if USE_PSB == 1
    #define GPIO_PSB        GPIOA                 //1£º²¢¿Ú¿ØÖÆ£¬0£º´®¿Ú¿ØÖÆ
    #define GPIO_PSB_PIN    GPIO_Pin_5
    #define RCC_GPIO_PSB    RCC_AHB1Periph_GPIOA 
#endif
    
    #define GPIO_RST        IO_GPIO_PORT(IO_RST)                 //0£º¸´Î»
    #define GPIO_RST_PIN    IO_GPIO_PIN(IO_RST)
    #define RCC_GPIO_RST    IO_RCC_AHB1_PERIPH(IO_RST)

    #define GPIO_RS         IO_GPIO_PORT(IO_RS)                 //1£ºÊý¾Ý£¬0£ºÃüÁî  ;CS£¬´®ÐÐÆ¬Ñ¡
    #define GPIO_RS_PIN     IO_GPIO_PIN(IO_RS)
    #define RCC_GPIO_RS     IO_RCC_AHB1_PERIPH(IO_RS)

    #define GPIO_RW         IO_GPIO_PORT(IO_RW)                 //0£ºÐ´  £¬1£º¶Á    ;SID£¬´®ÐÐÊý¾ÝÊäÈë
    #define GPIO_RW_PIN     IO_GPIO_PIN(IO_RW)
    #define RCC_GPIO_RW     IO_RCC_AHB1_PERIPH(IO_RW)

    #define GPIO_EN         IO_GPIO_PORT(IO_EN)                 //Ê¹ÄÜ              ;SCLK£¬´®ÐÐÊ±ÖÓ
    #define GPIO_EN_PIN     IO_GPIO_PIN(IO_EN)
    #define RCC_GPIO_EN     IO_RCC_AHB1_PERIPH(IO_EN)

#if USE_PSB == 1
    #define PSB_H  GPIO_PSB->BSRRL = GPIO_PSB_PIN  //PSB Ê¹ÄÜ  Êä³ö1
    #define PSB_L  GPIO_PSB->BSRRH = GPIO_PSB_PIN  //          Êä³ö0
#endif 
    
    #define RST_H  GPIO_RST->BSRRL = GPIO_RST_PIN  //RST Ê¹ÄÜ  Êä³ö1
    #define RST_L  GPIO_RST->BSRRH = GPIO_RST_PIN  //          Êä³ö0
    #define RS_H   GPIO_RS->BSRRL = GPIO_RS_PIN    //RS Ö¸Áî   Êä³ö1
    #define RS_L   GPIO_RS->BSRRH = GPIO_RS_PIN    //          Êä³ö0 
    #define RW_H   GPIO_RW->BSRRL = GPIO_RW_PIN    //RW ¶ÁÐ´   Êä³ö1
    #define RW_L   GPIO_RW->BSRRH = GPIO_RW_PIN    //          Êä³ö0
    #define EN_H   GPIO_EN->BSRRL = GPIO_EN_PIN    //EN Ê¹ÄÜ   Êä³ö1
    #define EN_L   GPIO_EN->BSRRH = GPIO_EN_PIN    //          Êä³ö0
#endif

#if PSB_SELECT == 1
    // ²¢¿Ú ------------------------------------------------------------------
    #define GPIO_DATA_0       GPIOB
    #define GPIO_DATA_0_PIN   GPIO_Pin_8
    #define GPIO_DATA_1       GPIOB
    #define GPIO_DATA_1_PIN   GPIO_Pin_9
    #define GPIO_DATA_2       GPIOB
    #define GPIO_DATA_2_PIN   GPIO_Pin_10
    #define GPIO_DATA_3       GPIOB
    #define GPIO_DATA_3_PIN   GPIO_Pin_11
    #define GPIO_DATA_4       GPIOB
    #define GPIO_DATA_4_PIN   GPIO_Pin_12
    #define GPIO_DATA_5       GPIOB
    #define GPIO_DATA_5_PIN   GPIO_Pin_13
    #define GPIO_DATA_6       GPIOB
    #define GPIO_DATA_6_PIN   GPIO_Pin_14
    #define GPIO_DATA_7       GPIOB
    #define GPIO_DATA_7_PIN   GPIO_Pin_15
    #define RCC_GPIO_DATA     RCC_APB2Periph_GPIOB  //Êý¾ÝÏßÊ±ÖÓ×é

    // ´®¿Ú ------------------------------------------------------------------
#endif


/* Private Functions ---------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void LCD_Configuration(void);                           //12864³õÊ¼»¯
void LCD_SetPosition(uint8_t x, uint8_t y);             //ÉèÖÃ×ø±ê
void LCD_WriteString(char * str);                       //ÏÔÊ¾×Ö·û£¨¿ÉÖÐÎÄ£©
void LCD_Show(uint8_t x, uint8_t y,char * str);         //ÔÚÖ¸¶¨Î»ÖÃºóÏÔÊ¾×Ö·
void LCD_ShowWidth(u8 x, u8 y, u8 width, char * str);   //ÔÚÖ¸¶¨Î»ÖÃÏÔÊ¾Ö¸¶¨¿í¶È×Ö·û
void LCD_Clear(void);                                   //ÇåÆÁ
//void LCD_WRGDRAM(uint8_t x, uint8_t clong, uint8_t hight, char *TAB);//ÏÔÊ¾Í¼Æ¬
void LCD_Printf(uint16_t Xpos, uint16_t Ypos,const char *fmt,...);

#endif //Rsing_12864.h
