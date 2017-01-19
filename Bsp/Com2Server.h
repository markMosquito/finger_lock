/**
  ******************************************************************************
  * @file    Com2Server.h
  * @author  
  * @version V1.0
  * @date    7-May-2015
  * @brief   Header file for Com2Server.c module. 
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COM2SERVER_H
#define __COM2SERVER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"
#include "Redef_GPIO.h"
#include "Rsing_led.h"


/* Private functions ---------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef struct
{
    unsigned char DLC;
    unsigned char Data[16];
}UsartTxMsg;

typedef struct
{
    unsigned char DLC;
    unsigned char Data[32];
} UsartRxMsg;

typedef struct
{
	int32_t x;
	int32_t y;
	int32_t z;
	uint32_t t;
	uint32_t spd;
    float spd_real;
	uint32_t ang;
	int32_t instruct_mode;
}Receive_Typedef;

/* Exported variables --------------------------------------------------------*/

extern uint32_t DbgChoose;
extern int16_t hitting;
extern uint32_t CarReadyFlag;

/* Exported functions --------------------------------------------------------*/
int checkSum(uint8_t *pData);
int calcSum(uint8_t *pData);
void DmaSendWord(uint32_t ComX, uint8_t command, uint32_t data);
void USART1_Configuration(void);
void USART3_Configuration(void);
void TIM7_Configuration(uint8_t PPr, uint8_t SPr, uint16_t period_ms);

int open_dev(void);
int close_dev(void);
int modeling(u8 step);
int modeling_complete(void);
int identification(void);
int read_id(void);
int concel_waiting(void);
int delete_all_model(void);
int delete_model(int id);
int get_touch_status(void);
#endif //Com2Server.h
