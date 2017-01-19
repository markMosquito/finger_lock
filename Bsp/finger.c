#include "finger.h"
#include "Com2Server.h"
#include "oled.h"
#include "IO_Ctrl.h"
int registered_flag = 0;
int member_number = 0;
int member_id[100];

union Password password;

extern u8 identify_run_flag;
extern u8 menu_select;

u8 modeling_step = 0;
u8 finger_dev_opened = 0;

int cur_id_detected = -1;
u8 manager_id_true = 0;
u8 member_deteted_flag = 0;
u8 forbid_time_refresh = 0;
extern u8 time_show_refresh;

u8 touch_status = 0;
void vFingerTask( void *p_arg )
{
	int i = 0;
	 (void)p_arg;
	
	vTaskDelay( 10 * portTICK_MS);
	FINGER_DEV_POWER_ON;
	vTaskDelay( 200 * portTICK_MS);
	
	while(finger_dev_opened != 1)
	{
		finger_dev_opened = open_dev();
		vTaskDelay( 30 * portTICK_MS);
	}
	
	finger_dev_opened = 1;
	
	xTaskCreate(    vDisplayTask,               /* Pointer to the function that implements the task.              */
					"Display",                  /* Text name for the task.  This is to facilitate debugging only. */
					DisplayTaskStk,          /* Stack depth in words.                                          */
					NULL,                    /* We are not using the task parameter.                           */
					DISPLAY_TASK_PRIO,  /* This task will run at priority x.                              */
					&DisplayTaskHandle );  /* We are not using the task handle.*/
					
	 xTaskCreate(    vUiTask,               /* Pointer to the function that implements the task.              */
					"Display",                  /* Text name for the task.  This is to facilitate debugging only. */
					UiTaskStk,          /* Stack depth in words.                                          */
					NULL,                    /* We are not using the task parameter.                           */
					UI_TASK_PRIO,  /* This task will run at priority x.                              */
					&UiTaskHandle );  /* We are not using the task handle.*/
	xTaskCreate(    vKeyTask,               /* Pointer to the function that implements the task.              */
					"Display",                  /* Text name for the task.  This is to facilitate debugging only. */
					KeyTaskStk,          /* Stack depth in words.                                          */
					NULL,                    /* We are not using the task parameter.                           */
					KEY_TASK_PRIO,  /* This task will run at priority x.                              */
					&KeyTaskHandle );  /* We are not using the task handle.*/
	//vTaskDelay( 500 * portTICK_MS);
	
	
	
	while(1)
	{
		//open_dev();
		if(identify_run_flag)
		{
			//
			//touch_status = get_touch_status();
			//if(touch_status == 1)
			if(1)
			{
				touch_status = 0;
				cur_id_detected = identification();
				if(cur_id_detected == member_id[0])
				{
					manager_id_true = 1;
				}
				
				for(i = 0; i< member_number;i++)
				{
					if(cur_id_detected == member_id[i] && menu_select == 0)
					{
						member_deteted_flag = 1;
					}
				}
				
				if(member_deteted_flag == 1)
				{
					member_deteted_flag = 0;
					forbid_time_refresh = 1;
					Buzzer(1, 250,250);
					LOCK_MOTOR_OPEN;
					
					OLED_Clear();			
					OLED_ShowString_CH(0,0,"Çë½ø",1);
					
					
					vTaskDelay( 5000 * portTICK_MS);
					
					LOCK_MOTOR_CLOSE;
					
					//vTaskDelay( 2000 * portTICK_MS);
					time_show_refresh = 1;
					member_deteted_flag = 0;
					forbid_time_refresh = 0;
				}
			}
			
		}
			
		vTaskDelay( 5 * portTICK_MS);
	}
}
