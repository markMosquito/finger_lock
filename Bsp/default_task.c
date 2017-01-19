#include "flash_task.h"
#include "bsp.h"
#include "oled.h"
#include "display_task.h"
u16 default_count = 0;
u8 default_flag = 0;
void vDefaultTask( void *p_arg )
{
	
	
	(void)p_arg;

	
	while(1)
	{
		if(READ_DEFAULT_KEY == 0)
		{
			default_count++;
		}
		else
		{
			default_count = 0;
		}
		if(default_count >= 60)
		{
			default_flag = 1;
			returnTodefault();
			OLED_Clear();			
			OLED_ShowString(0,16,"Return to default.",1);
			vTaskDelay( 2000 * portTICK_MS);
		}
		vTaskDelay( 50 * portTICK_MS);
	}
}
