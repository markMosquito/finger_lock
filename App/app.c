/**
  ******************************************************************************
  * @file    APP.c
  * @author  
  * @version V1.0
  * @date    8-May-2015
  * @brief   Initializate the APP
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "app.h"
#include "bsp.h"


/* Private variables ---------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/

/* TaskHandles ---------------------------------------------------------------*/
TaskHandle_t MainTaskHandle;
TaskHandle_t FingerTaskHandle;
TaskHandle_t DisplayTaskHandle;
TaskHandle_t FlashTaskHandle;
TaskHandle_t UiTaskHandle;
TaskHandle_t KeyTaskHandle;
TaskHandle_t DefaultTaskHandle;
/* Semaphores ----------------------------------------------------------------*/
//xSemaphoreHandle ElmoSem = 0;

/* QueueHandles --------------------------------------------------------------*/
//QueueHandle_t HitInQueue = 0;

/* Private function ----------------------------------------------------------*/

/**
  * @brief  vStartTask
  * @param  None
  * @retval None
  */
 
static void vStartTask( void *p_arg )
{
    (void)p_arg;
    
    //等待上电完全
    vTaskDelay( 300 * portTICK_MS );
    
    BSP_Init();
    
    //AppTaskCreate();----------------------------------------------------------
    vTaskSuspendAll();
    
    /* Initialise the com port then spawn the Rx and Tx tasks. */
	//vAltStartComTestTasks(APP_CFG_TASK_TX_PRIO, APP_CFG_TASK_RX_PRIO, 115200);
  
    /* Create one task. */
    xTaskCreate(    vMainTask,               /* Pointer to the function that implements the task.              */
                    "Main",                  /* Text name for the task.  This is to facilitate debugging only. */
                    AppTaskMainStk,          /* Stack depth in words.                                          */
                    NULL,                    /* We are not using the task parameter.                           */
                    APP_CFG_TASK_MAIN_PRIO,  /* This task will run at priority x.                              */
                    &MainTaskHandle );  /* We are not using the task handle.                              */
  
     xTaskCreate(    vFingerTask,               /* Pointer to the function that implements the task.              */
                    "Finger",                  /* Text name for the task.  This is to facilitate debugging only. */
                    FingerTaskStk,          /* Stack depth in words.                                          */
                    NULL,                    /* We are not using the task parameter.                           */
                    FINGER_TASK_PRIO,  /* This task will run at priority x.                              */
                    &FingerTaskHandle );  /* We are not using the task handle.*/
					

	xTaskCreate(    vDefaultTask,               /* Pointer to the function that implements the task.              */
                    "Finger",                  /* Text name for the task.  This is to facilitate debugging only. */
                    DefaultTaskStk,          /* Stack depth in words.                                          */
                    NULL,                    /* We are not using the task parameter.                           */
                    DEFAULT_TASK_PRIO,  /* This task will run at priority x.                              */
                    &DefaultTaskHandle );  /* We are not using the task handle.*/
//	 xTaskCreate(    vDisplayTask,               /* Pointer to the function that implements the task.              */
//					"Display",                  /* Text name for the task.  This is to facilitate debugging only. */
//					DisplayTaskStk,          /* Stack depth in words.                                          */
//					NULL,                    /* We are not using the task parameter.                           */
//					DISPLAY_TASK_PRIO,  /* This task will run at priority x.                              */
//					&DisplayTaskHandle );  /* We are not using the task handle.*/
//					
//	 xTaskCreate(    vUiTask,               /* Pointer to the function that implements the task.              */
//					"Display",                  /* Text name for the task.  This is to facilitate debugging only. */
//					UiTaskStk,          /* Stack depth in words.                                          */
//					NULL,                    /* We are not using the task parameter.                           */
//					UI_TASK_PRIO,  /* This task will run at priority x.                              */
//					&UiTaskHandle );  /* We are not using the task handle.*/
//	xTaskCreate(    vKeyTask,               /* Pointer to the function that implements the task.              */
//					"Display",                  /* Text name for the task.  This is to facilitate debugging only. */
//					KeyTaskStk,          /* Stack depth in words.                                          */
//					NULL,                    /* We are not using the task parameter.                           */
//					KEY_TASK_PRIO,  /* This task will run at priority x.                              */
//					&KeyTaskHandle );  /* We are not using the task handle.*/
    xTaskResumeAll();
    //End-AppTaskCreate();------------------------------------------------------
    
    vTaskDelete(NULL);
}

//--------------------------------------------------------------
/**
  * @brief  vMainTask
  * @param  None					
  * @retval None
  */
portTASK_FUNCTION( vMainTask, pvParameters )
{
    TickType_t xLastFlashTime;
    ( void ) pvParameters;
    
    xLastFlashTime = xTaskGetTickCount();
    for(;;)
    {

        vTaskDelayUntil( &xLastFlashTime, 500 * portTICK_MS );
    }
}
//--------------------------------------------------------------


//--------------------------------------------------------------
void StartTaskCreate(void)
{
    /* Create one task. */
    xTaskCreate(vStartTask,         /* Pointer to the function that implements the task.              */
        "Start",                    /* Text name for the task.  This is to facilitate debugging only. */
        AppTaskStartStk,            /* Stack depth in words.                                          */
        NULL,                       /* We are not using the task parameter.                           */
        APP_CFG_TASK_START_PRIO,    /* This task will run at priority x.                              */
        NULL);                      /* We are not using the task handle.                              */

    /* Create one task. */
    xTaskCreate(vBeepTask,          /* Pointer to the function that implements the task.              */
        "Beep",                     /* Text name for the task.  This is to facilitate debugging only. */
        AppTaskBeepStk,             /* Stack depth in words.                                          */
        NULL,                       /* We are not using the task parameter.                           */
        APP_CFG_TASK_BEEP_PRIO,     /* This task will run at priority x.                              */
        &BeepTaskHandle);
}

/******************************** END OF FILE *********************************/
