/*******************************************************************************
 * 文件名  ：rtc.c
 * 描述    ：利用STM32的RTC实现一个简易的电子时钟。
 *           显示格式为 Time: XX : XX : XX(时：分：秒)。         
 * 实验平台：野火STM32开发板
 * 硬件连接：-------------------
 *          |                   |
 *          | VBAT引脚需外接电池|
 *          |                   |
 *           -----------------
 * 库版本  ：ST3.5.0
 * 作者    ：保留 
 * 论坛    ：http://www.amobbs.com/forum-1008-1.html
 * 淘宝    ：http://firestm32.taobao.com
**********************************************************************************/
#include "rtc.h"
#include "stdio.h"

/* 秒中断标志，进入秒中断时置1，当时间被刷新之后清0 */
__IO uint32_t TimeDisplay;	

extern u8 time_show_refresh;
/*
 * 函数名：NVIC_Configuration
 * 描述  ：配置RTC秒中断的主中断优先级为1，次优先级为0
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
//void NVIC_Configuration(void)
//{
//	NVIC_InitTypeDef NVIC_InitStructure;
//	
//	/* Configure one bit for preemption priority */
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
//	
//	/* Enable the RTC Interrupt */
//	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//}

/*
 * 函数名：RTC_Configuration
 * 描述  ：配置RTC
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
RTC_InitTypeDef   RTC_InitStructure;
RTC_TimeTypeDef   RTC_TimeStructure;
RTC_DateTypeDef   RTC_DateStructure;
__IO uint32_t AsynchPrediv = 0, SynchPrediv = 0;
__IO uint32_t TimeDisplay = 0;
uint32_t errorindex = 0, i = 0;


#define RTC_BKP_DR_NUMBER   0x14
#define FIRST_DATA          0x32F2



void Calendar_Show(void)
{
  while(1)
  {
    /* If 1s has elapsed */
    if (TimeDisplay == 1)
    {
      /* Display current time */
     // Time_Display();

      /* Display current date */
     // Date_Display();

      TimeDisplay = 0;
    }
  }
}

void RTC_main(void)
{
	  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f2xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f2xx.c file
     */     
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef  EXTI_InitStructure;


  /* Enable the PWR APB1 Clock Interface */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Configure one bit for preemption priority */
  //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

/* EXTI configuration *******************************************************/
  EXTI_ClearITPendingBit(EXTI_Line22);
  EXTI_InitStructure.EXTI_Line = EXTI_Line22;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
//RTC_Configuration();

  if(RTC_ReadBackupRegister(RTC_BKP_DR0) != FIRST_DATA)
  //if(1)
  {
   

    /* RTC Configuration */
    RTC_Configuration();
	 // while(1);

    /* Adjust Current Time */
   // Time_Adjust();

    /* Adjust Current Date */
    //Date_Adjust();
  }
  else
  {
    /* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro();
    RTC_ClearITPendingBit(RTC_IT_WUT);
    EXTI_ClearITPendingBit(EXTI_Line22);

/*  Backup SRAM ***************************************************************/
    /* Enable BKPSRAM Clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);

  }
}
void RTC_Configuration(void)
{
	
	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);
	
	/* Enable LSE */
	RCC_LSEConfig(RCC_LSE_ON);
	/* Wait till LSE is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{}
	
	/* Select LSE as RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	
	SynchPrediv = 0xFF;
    AsynchPrediv = 0x7F;
	/* Enable RTC Clock */
	RCC_RTCCLKCmd(ENABLE);
	
	/* Wait for RTC registers synchronization */
	RTC_WaitForSynchro();
	
	RTC_WriteBackupRegister(RTC_BKP_DR0, FIRST_DATA);
		
	RTC_TimeStructure.RTC_Hours   = 0x23;
	RTC_TimeStructure.RTC_Minutes = 0x51;
	RTC_TimeStructure.RTC_Seconds = 0x30;

	/* Set the Date */
	RTC_DateStructure.RTC_Month = RTC_Month_March;
	RTC_DateStructure.RTC_Date = 0x18;  
	RTC_DateStructure.RTC_Year = 0x11; 
	RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Friday; 

	/* Calendar Configuration */
	RTC_InitStructure.RTC_AsynchPrediv = AsynchPrediv;
	RTC_InitStructure.RTC_SynchPrediv =  SynchPrediv;
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
	RTC_Init(&RTC_InitStructure);
	
	/* Set Current Time and Date */
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);  
	RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure); 
	
	RTC_WakeUpCmd(DISABLE);
	RTC_ClearITPendingBit(RTC_IT_WUT);
    EXTI_ClearITPendingBit(EXTI_Line22);
	
	 /* Configure the RTC Wakeup Clock source and Counter (Wakeup event each 1 second) */
	RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);
	RTC_SetWakeUpCounter(0x7FF);

	/* Enable the Wakeup Interrupt */
	RTC_ITConfig(RTC_IT_WUT, ENABLE);

	/* Enable Wakeup Counter */
	RTC_WakeUpCmd(ENABLE); 
	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);

	/* Write to Backup SRAM with 32-Bit Data */
	for (i = 0x0; i < 0x1000; i += 4)
	{
	*(__IO uint32_t *) (BKPSRAM_BASE + i) = i;
	}
	/* Check the written Data */
	for (i = 0x0; i < 0x1000; i += 4)
	{
		if ((*(__IO uint32_t *) (BKPSRAM_BASE + i)) != i)
		{
		  errorindex++;
		}
	}
	
	/* Enable the Backup SRAM low power Regulator to retain it's content in VBAT mode */
	PWR_BackupRegulatorCmd(ENABLE);

	/* Wait until the Backup SRAM low power Regulator is ready */
	while(PWR_GetFlagStatus(PWR_FLAG_BRR) == RESET)
	{
	}

	/* RTC Backup Data Registers **************************************************/
	/* Write to RTC Backup Data Registers */
	RTC_WriteBackupRegister(RTC_BKP_DR0, FIRST_DATA);
}

/**
  * @brief  This function handles RTC Wakeup Timer Handler.
  * @param  None
  * @retval None
  */
extern u8 forbid_time_refresh;
void RTC_WKUP_IRQHandler(void)
{
  if(RTC_GetITStatus(RTC_IT_WUT) != RESET)
  {
	  if(forbid_time_refresh == 0)
	  {
		  time_show_refresh = 1;
	  }
			
	  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
      RTC_ClearITPendingBit(RTC_IT_WUT);
	  EXTI_ClearITPendingBit(EXTI_Line22);
  }
}
