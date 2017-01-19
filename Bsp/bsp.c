/**
  ******************************************************************************
  * @file    BSP.c
  * @author  
  * @version V1.0
  * @date    22-July-2014
  * @brief   
  ******************************************************************************
  * @note
  *  FreeRTOS中调用操作系统函数的中断优先级不得超过 configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY = 5
  ******************************************************************************
  */
#include "bsp.h"
#include "app.h"
#include "rtc.h"

#include "Base_Fun.h"
#define Delay_ms(tims_ms)       vTaskDelay( ( TickType_t )tims_ms * portTICK_MS)

static void IO_Configuration(void);
void TIM2_init(void);

/*
 * 函数名：BSP_Init
 * 描述  ：时钟初始化、硬件初始化
 * 输入  ：无
 * 输出  ：无
 */
  extern RTC_TimeTypeDef   RTC_TimeStructure;
void BSP_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    
    IO_Configuration();
	
	USART3_Configuration();
	
	RTC_main();
	
	
//	while(1)
//	{
//		RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
//		vTaskDelay(5*portTICK_MS);
//	}
	FINGER_DEV_POWER_OFF;
	LOCK_MOTOR_STOP;
}


void IO_Configuration(void)
{
    // 蜂鸣器
    MyGPIOInit(
        IO_GPIO_PORT(IO_BEEP),
        IO_GPIO_PIN(IO_BEEP),
        GPIO_Mode_OUT,
        GPIO_Speed_100MHz,
        GPIO_OType_PP,
        GPIO_PuPd_NOPULL);
	
	 MyGPIOInit(
        IO_GPIO_PORT(IO_FINGER_DEV_POWER),
        IO_GPIO_PIN(IO_FINGER_DEV_POWER),
        GPIO_Mode_OUT,
        GPIO_Speed_100MHz,
        GPIO_OType_PP,
        GPIO_PuPd_NOPULL);
	
	MyGPIOInit(
        IO_GPIO_PORT(IO_LOCK_MOTOR_1),
        IO_GPIO_PIN(IO_LOCK_MOTOR_1),
        GPIO_Mode_OUT,
        GPIO_Speed_100MHz,
        GPIO_OType_PP,
        GPIO_PuPd_NOPULL);
		
	MyGPIOInit(
        IO_GPIO_PORT(IO_LOCK_MOTOR_2),
        IO_GPIO_PIN(IO_LOCK_MOTOR_2),
        GPIO_Mode_OUT,
        GPIO_Speed_100MHz,
        GPIO_OType_PP,
        GPIO_PuPd_NOPULL);
		
	MyGPIOInit(
        IO_GPIO_PORT(IO_DEFAULT_KEY),
        IO_GPIO_PIN(IO_DEFAULT_KEY),
        GPIO_Mode_IN,
        GPIO_Speed_100MHz,
        GPIO_OType_PP,
        GPIO_PuPd_NOPULL);
}

void TIM2_init(void)            //Debug Time
{
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;

	/* TIM2 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);

	/* Time base configuration (TIM2 clocked at 84 MHz)*/
	TIM_TimeBaseStructure.TIM_Period = 0xffffffff;
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;           

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 

	/* TIM2 enable counter */
	TIM_Cmd(TIM2, ENABLE);
}


/********************************* END OF FILE ********************************/
