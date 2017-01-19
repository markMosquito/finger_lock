/**
  ******************************************************************************
  * @file    APP.h
  * @author  
  * @version V1.0
  * @date    8-May-2015
  * @brief   Header file for APP.c module. 
  * @Note    ���ȼ�����Խ�����ȼ�Խ��
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_H
#define __APP_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#include "Rsing_Beep.h"

/* Defines -------------------------------------------------------------------*/
//��ջ��С����
#define AppTaskStartStk                 (configMINIMAL_STACK_SIZE)

//�������ȼ�����
#define APP_CFG_TASK_START_PRIO         ( tskIDLE_PRIORITY + 1UL )
#define APP_CFG_TASK_TX_PRIO            ( tskIDLE_PRIORITY + 1UL )
#define APP_CFG_TASK_RX_PRIO            ( tskIDLE_PRIORITY + 1UL )

/*������*/
#define AppTaskBeepStk                  (configMINIMAL_STACK_SIZE)
#define APP_CFG_TASK_BEEP_PRIO          ( tskIDLE_PRIORITY + 2UL )
void vBeepTask( void *p_arg );
extern TaskHandle_t BeepTaskHandle;


/*λ�ü�������*/
#define AppTaskMainStk                  (512)
#define APP_CFG_TASK_MAIN_PRIO          ( tskIDLE_PRIORITY + 4UL )
void vMainTask( void *p_arg );
extern TaskHandle_t MainTaskHandle;

/*ָ��������*/
#define FingerTaskStk                  (512)
#define FINGER_TASK_PRIO          ( tskIDLE_PRIORITY + 4UL )
void vFingerTask( void *p_arg );
extern TaskHandle_t FingerTaskHandle;

/*��ʾ����*/
#define DisplayTaskStk                  (512)
#define DISPLAY_TASK_PRIO          ( tskIDLE_PRIORITY + 4UL )
void vDisplayTask( void *p_arg );
extern TaskHandle_t DisplayTaskHandle;

/*��ʾ����*/
#define UiTaskStk                  (512)
#define UI_TASK_PRIO          ( tskIDLE_PRIORITY + 4UL )
void vUiTask( void *p_arg );
extern TaskHandle_t UiTaskHandle;

/*flash*/
#define FlashTaskStk                  (512)
#define FLASH_TASK_PRIO          ( tskIDLE_PRIORITY + 4UL )
void vFlashTask( void *p_arg );
extern TaskHandle_t FlashTaskHandle;

/*��������*/
#define KeyTaskStk                  (512)
#define KEY_TASK_PRIO          ( tskIDLE_PRIORITY + 4UL )
void vKeyTask( void *p_arg );
extern TaskHandle_t KeyTaskHandle;

/*�ָ���������*/
#define DefaultTaskStk                  (512)
#define DEFAULT_TASK_PRIO          ( tskIDLE_PRIORITY + 4UL )
void vDefaultTask( void *p_arg );
extern TaskHandle_t DefaultTaskHandle;
/* Exported xSemaphoreHandle ----------------------------------------------------------*/
//extern xSemaphoreHandle PosSem;

/* Exported Function----------------------------------------------------------*/
void StartTaskCreate(void);
void AppTaskCreate (void);
#endif //APP.h
