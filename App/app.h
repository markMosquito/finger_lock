/**
  ******************************************************************************
  * @file    APP.h
  * @author  
  * @version V1.0
  * @date    8-May-2015
  * @brief   Header file for APP.c module. 
  * @Note    优先级数字越大优先级越高
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
//堆栈大小定义
#define AppTaskStartStk                 (configMINIMAL_STACK_SIZE)

//任务优先级定义
#define APP_CFG_TASK_START_PRIO         ( tskIDLE_PRIORITY + 1UL )
#define APP_CFG_TASK_TX_PRIO            ( tskIDLE_PRIORITY + 1UL )
#define APP_CFG_TASK_RX_PRIO            ( tskIDLE_PRIORITY + 1UL )

/*蜂鸣器*/
#define AppTaskBeepStk                  (configMINIMAL_STACK_SIZE)
#define APP_CFG_TASK_BEEP_PRIO          ( tskIDLE_PRIORITY + 2UL )
void vBeepTask( void *p_arg );
extern TaskHandle_t BeepTaskHandle;


/*位置计算任务*/
#define AppTaskMainStk                  (512)
#define APP_CFG_TASK_MAIN_PRIO          ( tskIDLE_PRIORITY + 4UL )
void vMainTask( void *p_arg );
extern TaskHandle_t MainTaskHandle;

/*指静脉任务*/
#define FingerTaskStk                  (512)
#define FINGER_TASK_PRIO          ( tskIDLE_PRIORITY + 4UL )
void vFingerTask( void *p_arg );
extern TaskHandle_t FingerTaskHandle;

/*显示任务*/
#define DisplayTaskStk                  (512)
#define DISPLAY_TASK_PRIO          ( tskIDLE_PRIORITY + 4UL )
void vDisplayTask( void *p_arg );
extern TaskHandle_t DisplayTaskHandle;

/*显示任务*/
#define UiTaskStk                  (512)
#define UI_TASK_PRIO          ( tskIDLE_PRIORITY + 4UL )
void vUiTask( void *p_arg );
extern TaskHandle_t UiTaskHandle;

/*flash*/
#define FlashTaskStk                  (512)
#define FLASH_TASK_PRIO          ( tskIDLE_PRIORITY + 4UL )
void vFlashTask( void *p_arg );
extern TaskHandle_t FlashTaskHandle;

/*按键任务*/
#define KeyTaskStk                  (512)
#define KEY_TASK_PRIO          ( tskIDLE_PRIORITY + 4UL )
void vKeyTask( void *p_arg );
extern TaskHandle_t KeyTaskHandle;

/*恢复出厂任务*/
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
