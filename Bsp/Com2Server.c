/**
  ******************************************************************************
  * @file    Com2Server.c
  * @author  
  * @version V1.0
  * @date    7-May-2015
  * @brief   发送底盘位置至电脑&接收电脑指令.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "Com2Server.h"
#include "FreeRTOS.h"
#include "task.h"
#include "app.h"
#include "finger.h"
#define  GetRealTimeTick()      (TIM2->CNT)
#include <string.h>

/* Private variables ---------------------------------------------------------*/
uint8_t DmaUsartTxBuf[24], DmaUsartTimBuf[14];
UsartRxMsg UsartRx1,UsartRx3;

uint32_t SyncFlag = 0;
uint32_t SyncTimOut = 0;


QueueHandle_t xCharsForTx3;
/* Extern variable prototypes-------------------------------------------------*/
extern uint32_t SendCorrectFlag, AngCorrectFlag;
extern uint32_t tFirst;

extern uint32_t CarReadyFlag;
extern uint32_t SelfTestFlag;
/* Private define ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void DmaSendPos(void);



extern uint32_t test_state;
extern uint32_t test_value;
/* USART1 RX -----------------------------------------------------------------*/
extern int16_t correctDis;
Receive_Typedef RecTemp;


/**
  * @brief  USART1 接收中断，接收羽毛球落点
  * @param  None					
  * @retval None
  */
u8 serial_data_step = 0;
u8 serial_receive_over = 0;
void USART1_IRQHandler(void)
{
    if((USART1->SR & USART_FLAG_RXNE) != (uint16_t)RESET)
	{
        static unsigned char come_data;
        
		come_data = (uint16_t)(USART1->DR & (uint16_t)0x01FF);
		switch(serial_data_step)
		{
			case 0:
				if(come_data == 0x01)
				{
					serial_data_step = 1;
				}
				else
				{
					serial_data_step = 0;
					UsartRx1.DLC = 0;
				}
				break;
			case 1:
				if(come_data == 0xef)
				{
					UsartRx1.DLC = 0;
					UsartRx1.Data[UsartRx1.DLC++] = 0x01;
					UsartRx1.Data[UsartRx1.DLC++] = 0xef;
					serial_data_step = 2;
				}
				else
				{
					serial_data_step = 0;
					UsartRx1.DLC = 0;
				}break;
			
			case 2: 
				if(UsartRx1.DLC<24)
				{
					UsartRx1.Data[UsartRx1.DLC++] = come_data;
					if(UsartRx1.DLC == 24)
					{
						serial_receive_over = 1;
						serial_data_step = 0;
						UsartRx1.DLC = 0;
					}
				}
				
				break;
			}
		
    }
}
#if COM3_DBG == COM_RELEASE
void USART3_IRQHandler(void)
{
    if((USART3->SR & USART_FLAG_RXNE) != (uint16_t)RESET)
	{
        static unsigned char come_data;
        
		come_data = (uint16_t)(USART3->DR & (uint16_t)0x01FF);
		switch(serial_data_step)
		{
			case 0:
				if(come_data == 0x01)
				{
					serial_data_step = 1;
				}
				else
				{
					serial_data_step = 0;
					UsartRx3.DLC = 0;
				}
				break;
			case 1:
				if(come_data == 0xef)
				{
					UsartRx3.DLC = 0;
					UsartRx3.Data[UsartRx3.DLC++] = 0x01;
					UsartRx3.Data[UsartRx3.DLC++] = 0xef;
					serial_data_step = 2;
				}
				else
				{
					serial_data_step = 0;
					UsartRx3.DLC = 0;
				}break;
			
			case 2: 
				if(UsartRx3.DLC<24)
				{
					UsartRx3.Data[UsartRx3.DLC++] = come_data;
					if(UsartRx3.DLC == 24)
					{
						serial_receive_over = 1;
						serial_data_step = 0;
						UsartRx3.DLC = 0;
					}
				}
				
				break;
			}
    }

 
}
#endif

/* USART1 TX 使用TIM7 -----------------------------------------------------------------*/
/**
  * @brief  采用定时器调用串口DMA向电脑发送当前位置
  * @param  uint8_t PPr, uint8_t SPr, uint16_t period_ms
  * @retval None
  * @note   period_ms 应小于6553ms 且不宜太快
  */
void TIM7_Configuration(uint8_t PPr, uint8_t SPr, uint16_t period_ms)
{
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
	NVIC_InitTypeDef          NVIC_InitStructure;

    assert_param(period_ms <= 6553);
    
	/* TIM7 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7 , ENABLE);

	/* Time base configuration (TIM7 clocked at 84 MHz)*/
	TIM_TimeBaseStructure.TIM_Period = period_ms*10;//
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;// 100us
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure); 

	/* TIM7 IT enable */
	TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);

    /* Enable the TIM7 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PPr;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = SPr;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
    
	/* TIM7 enable counter */
	TIM_Cmd(TIM7, DISABLE);
    TIM7->CNT = 0;
}


void TIM7_IRQHandler()
{
    if(TIM_GetITStatus(TIM7, TIM_IT_Update))
    {
        TIM7->SR = (uint16_t)~TIM_IT_Update;
        if(SyncTimOut > 2)
        {
            SyncTimOut = 0;
            SyncFlag = 0;
            //LEDOff(LED1B);
        }
        if(!SyncFlag)
            DmaSendPos();
        else
            SyncTimOut++;
    }
}

/* USART1&DMA 初始化----------------------------------------------------------*/
/** @note DMA中断优先级应高于USART，否则会出错 */
void USART1_Configuration(void)
{
	//初始化串口
    {
        GPIO_InitTypeDef GPIO_InitStructure;
        USART_InitTypeDef USART_InitStructure;
        RCC_AHB1PeriphClockCmd(IO_RCC_AHB1_PERIPH(IO_USART1_TX), ENABLE);

        /* Configure USART Tx () */
        GPIO_InitStructure.GPIO_Pin = IO_GPIO_PIN(IO_USART1_TX);
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

        GPIO_Init( IO_GPIO_PORT(IO_USART1_TX), &GPIO_InitStructure );

        /* Configure USART Rx () */
        GPIO_InitStructure.GPIO_Pin = IO_GPIO_PIN(IO_USART1_RX);
        GPIO_Init( IO_GPIO_PORT(IO_USART1_RX), &GPIO_InitStructure );
        
        GPIO_PinAFConfig(IO_GPIO_PORT(IO_USART1_TX), IO_GPIO_PINSOURCE(IO_USART1_TX), GPIO_AF_USART1);
        GPIO_PinAFConfig(IO_GPIO_PORT(IO_USART1_RX), IO_GPIO_PINSOURCE(IO_USART1_RX), GPIO_AF_USART1);
        
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
        USART_DeInit(USART1);
        USART_InitStructure.USART_BaudRate = 115200;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        USART_Init(USART1, &USART_InitStructure);
        
        USART_Cmd(USART1, ENABLE);
    }

    //配置NVIC
    {
        NVIC_InitTypeDef  NVIC_InitStructure;
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
        USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);        //开串口中断
        
        NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;          
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 9; 
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           
        NVIC_Init(&NVIC_InitStructure);
    }
    
    //DMA发送
    {
		NVIC_InitTypeDef NVIC_InitStructure;
        DMA_InitTypeDef DMA_InitStructure;
        
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
        USART_DMACmd(USART1,USART_DMAReq_Tx ,ENABLE);
        
		NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);	
        
        DMA_DeInit(DMA2_Stream7);
        DMA_InitStructure.DMA_Channel= DMA_Channel_4;
        DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(USART1->DR));
        DMA_InitStructure.DMA_Memory0BaseAddr = (u32)(&(DmaUsartTxBuf));
        DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
        DMA_InitStructure.DMA_BufferSize = 0;
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
        DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
        DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
        DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
        DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
        DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
        DMA_Init(DMA2_Stream7, &DMA_InitStructure);
        DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TC);
        DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE);
    }
}

void USART3_Configuration(void)
{
	//初始化串口
    {
        GPIO_InitTypeDef GPIO_InitStructure;
        USART_InitTypeDef USART_InitStructure;
        RCC_AHB1PeriphClockCmd(IO_RCC_AHB1_PERIPH(IO_USART3_TX), ENABLE);

        /* Configure USART Tx () */
        GPIO_InitStructure.GPIO_Pin = IO_GPIO_PIN(IO_USART3_TX);
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

        GPIO_Init( IO_GPIO_PORT(IO_USART3_TX), &GPIO_InitStructure );

        /* Configure USART Rx () */
        GPIO_InitStructure.GPIO_Pin = IO_GPIO_PIN(IO_USART3_RX);
        GPIO_Init( IO_GPIO_PORT(IO_USART3_RX), &GPIO_InitStructure );
        
        GPIO_PinAFConfig(IO_GPIO_PORT(IO_USART3_TX), IO_GPIO_PINSOURCE(IO_USART3_TX), GPIO_AF_USART3);
        GPIO_PinAFConfig(IO_GPIO_PORT(IO_USART3_RX), IO_GPIO_PINSOURCE(IO_USART3_RX), GPIO_AF_USART3);
        
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
        USART_DeInit(USART1);
        USART_InitStructure.USART_BaudRate = 115200;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        USART_Init(USART3, &USART_InitStructure);
        
        USART_Cmd(USART3, ENABLE);
    }

    //配置NVIC
    {
        NVIC_InitTypeDef  NVIC_InitStructure;
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
        USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);        //开串口中断
        
        NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;          
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 9; 
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           
        NVIC_Init(&NVIC_InitStructure);
    }
    
    //DMA发送
    {
		NVIC_InitTypeDef NVIC_InitStructure;
        DMA_InitTypeDef DMA_InitStructure;
        
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
        USART_DMACmd(USART3,USART_DMAReq_Tx ,ENABLE);
        
		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);	
        
        DMA_DeInit(DMA1_Stream3);
        DMA_InitStructure.DMA_Channel= DMA_Channel_4;
        DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(USART3->DR));
        DMA_InitStructure.DMA_Memory0BaseAddr = (u32)(&(DmaUsartTxBuf));
        DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
        DMA_InitStructure.DMA_BufferSize = 0;
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
        DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
        DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
        DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
        DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
        DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
        DMA_Init(DMA1_Stream3, &DMA_InitStructure);
        DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_TC);
        DMA_ITConfig(DMA1_Stream3, DMA_IT_TC, ENABLE);
    }
}
/**
  * @brief  SumCheck 校验和
  * @param  *pData：数据起始地址指针
            length：数据长度
  * @retval (uint8_t)校验和
  */
int calcSum(uint8_t *pData)
{
	int sum = 0, i;
	int len;
	sum = pData[6] + pData[7] + pData[8];
	len=pData[7]+(((int)pData[8])<<8);
	for (i = 0; i < len - 2; i++)
	{
		sum += pData[i+9];
	}
	sum = sum&0xffff;
	return sum;
}
int checkSum(uint8_t *pData)
{
	int sum = 0, i;
	int len;
	sum = pData[6] + pData[7] + pData[8];
	len=pData[7]+(((int)pData[8])<<8);
	for (i = 0; i < len - 2; i++)
	{
		sum += pData[i+9];
	}
	sum = sum&0xffff;
	if(sum == (pData[len+7] + (pData[len+8] <<8)))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


/**
  * @brief  DMA串口中断
  * @param  int32_t x, int32_t y
  * @retval None
  */
void DMA2_Stream7_IRQHandler(void)
{
//    static UsartTxMsg txMsg;
	DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TCIF7);
}
void DMA1_Stream3_IRQHandler(void)
{
//    static UsartTxMsg txMsg;
	DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_TCIF3);
}
#define CHANGE_SIGN(x)  ((x)<0?(-(x) + 0x2000):(x))
/**
  * @brief  DMA发送位置
  * @param  int32_t x, int32_t y, int32_t vx, int32_t vy
  * @retval None
  * @note   发送间隔
  */
static void DmaSendPos(void)
{
    
    if(DMA2_Stream7->NDTR == 0)
    {
        DMA2_Stream7->NDTR = 14;
        DMA2_Stream7->M0AR = (uint32_t)(DmaUsartTxBuf);
        DMA2_Stream7->CR |= (uint32_t)DMA_SxCR_EN;//DMA_Cmd(DMA2_Stream7, ENABLE);
    }
}

u8 delay_300ms_count = 0;
int open_dev(void)
{
	short sum = 0;
	DmaUsartTxBuf[0] = 0x01;
	DmaUsartTxBuf[1] = 0xef;//sig  0xef01
	DmaUsartTxBuf[2] = 0x00;
	DmaUsartTxBuf[3] = 0x00;//seq  0x0000
	DmaUsartTxBuf[4] = 0xff;
	DmaUsartTxBuf[5] = 0xff;//addr
	DmaUsartTxBuf[6] = 0x01;//type
	DmaUsartTxBuf[7] = 0x0f;
	DmaUsartTxBuf[8] = 0x00;//length  0x000f
	DmaUsartTxBuf[9] = 0x00;//cmd 0x00打开设备
	DmaUsartTxBuf[10] = 0x01;
	DmaUsartTxBuf[11] = 0x00;//离线模式 0x0001
	DmaUsartTxBuf[12] = 0x00;
	DmaUsartTxBuf[13] = 0x00;
	DmaUsartTxBuf[14] = 0x00;
	DmaUsartTxBuf[15] = 0x00;
	DmaUsartTxBuf[16] = 0x00;
	DmaUsartTxBuf[17] = 0x00;
	DmaUsartTxBuf[18] = 0x00;
	DmaUsartTxBuf[19] = 0x00;
	DmaUsartTxBuf[20] = 0x00;
	DmaUsartTxBuf[21] = 0x00;//data  十个字节
	sum = calcSum(DmaUsartTxBuf);
	DmaUsartTxBuf[22] = sum & 0xff;
	DmaUsartTxBuf[23] = sum >>8;
	
	if(DMA1_Stream3->NDTR == 0)
    {
		DMA1_Stream3->CR &= ~(uint32_t)DMA_SxCR_EN;
        DMA1_Stream3->NDTR = 24;
        DMA1_Stream3->M0AR = (uint32_t)(DmaUsartTxBuf);
        DMA1_Stream3->CR |= (uint32_t)DMA_SxCR_EN;//DMA_Cmd(DMA2_Stream7, ENABLE);
    }
	serial_receive_over = 0;
	delay_300ms_count = 0;
	while(serial_receive_over == 0)//等待接收
	{
		
		vTaskDelay(300);
		delay_300ms_count++;
		if(delay_300ms_count>= 20)
			return -1;//超时
	}
	serial_receive_over = 0;
	if(checkSum(UsartRx3.Data) == 1)
	{
		if(UsartRx3.Data[9] == 0)
		{
			return 1;//打开成功
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
		//数据不对
	}
}

int close_dev(void)
{
	short sum = 0;
	DmaUsartTxBuf[0] = 0x01;
	DmaUsartTxBuf[1] = 0xef;//sig  0xef01
	DmaUsartTxBuf[2] = 0x00;
	DmaUsartTxBuf[3] = 0x00;//seq  0x0000
	DmaUsartTxBuf[4] = 0xff;
	DmaUsartTxBuf[5] = 0xff;//addr
	DmaUsartTxBuf[6] = 0x01;//type
	DmaUsartTxBuf[7] = 0x0f;
	DmaUsartTxBuf[8] = 0x00;//length  0x000f
	DmaUsartTxBuf[9] = 0x01;//cmd 0x01关闭设备
	DmaUsartTxBuf[10] = 0x01;
	DmaUsartTxBuf[11] = 0x00;//离线模式 0x0001
	DmaUsartTxBuf[12] = 0x00;
	DmaUsartTxBuf[13] = 0x00;
	DmaUsartTxBuf[14] = 0x00;
	DmaUsartTxBuf[15] = 0x00;
	DmaUsartTxBuf[16] = 0x00;
	DmaUsartTxBuf[17] = 0x00;
	DmaUsartTxBuf[18] = 0x00;
	DmaUsartTxBuf[19] = 0x00;
	DmaUsartTxBuf[20] = 0x00;
	DmaUsartTxBuf[21] = 0x00;//data  十个字节
	sum = calcSum(DmaUsartTxBuf);
	DmaUsartTxBuf[22] = sum & 0xff;
	DmaUsartTxBuf[23] = sum >>8;
	
	if(DMA1_Stream3->NDTR == 0)
    {
		DMA1_Stream3->CR &= ~(uint32_t)DMA_SxCR_EN;
        DMA1_Stream3->NDTR = 24;
        DMA1_Stream3->M0AR = (uint32_t)(DmaUsartTxBuf);
        DMA1_Stream3->CR |= (uint32_t)DMA_SxCR_EN;//DMA_Cmd(DMA2_Stream7, ENABLE);
    }
	serial_receive_over = 0;
	delay_300ms_count = 0;
	while(serial_receive_over == 0)//等待接收
	{
		
		vTaskDelay(300);
		delay_300ms_count++;
		if(delay_300ms_count>= 20)
			return -1;//超时
	}
	serial_receive_over = 0;
	if(checkSum(UsartRx3.Data) == 1)
	{
		if(UsartRx3.Data[9] == 0)
		{
			return 1;//打开成功
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
		//数据不对
	}
}

int get_touch_status(void)
{
	short sum = 0;
	DmaUsartTxBuf[0] = 0x01;
	DmaUsartTxBuf[1] = 0xef;//sig  0xef01
	DmaUsartTxBuf[2] = 0x00;
	DmaUsartTxBuf[3] = 0x00;//seq  0x0000
	DmaUsartTxBuf[4] = 0xff;
	DmaUsartTxBuf[5] = 0xff;//addr
	DmaUsartTxBuf[6] = 0x01;//type
	DmaUsartTxBuf[7] = 0x0f;
	DmaUsartTxBuf[8] = 0x00;//length  0x000f
	DmaUsartTxBuf[9] = 0x03;//cmd 0x03//获取触摸状态
	DmaUsartTxBuf[10] = 0x01;
	DmaUsartTxBuf[11] = 0x00;//离线模式 0x0001
	DmaUsartTxBuf[12] = 0x00;
	DmaUsartTxBuf[13] = 0x00;
	DmaUsartTxBuf[14] = 0x00;
	DmaUsartTxBuf[15] = 0x00;
	DmaUsartTxBuf[16] = 0x00;
	DmaUsartTxBuf[17] = 0x00;
	DmaUsartTxBuf[18] = 0x00;
	DmaUsartTxBuf[19] = 0x00;
	DmaUsartTxBuf[20] = 0x00;
	DmaUsartTxBuf[21] = 0x00;//data  十个字节
	sum = calcSum(DmaUsartTxBuf);
	DmaUsartTxBuf[22] = sum & 0xff;
	DmaUsartTxBuf[23] = sum >>8;
	
	if(DMA1_Stream3->NDTR == 0)
    {
		DMA1_Stream3->CR &= ~(uint32_t)DMA_SxCR_EN;
        DMA1_Stream3->NDTR = 24;
        DMA1_Stream3->M0AR = (uint32_t)(DmaUsartTxBuf);
        DMA1_Stream3->CR |= (uint32_t)DMA_SxCR_EN;//DMA_Cmd(DMA2_Stream7, ENABLE);
    }
	serial_receive_over = 0;
	delay_300ms_count = 0;
	while(serial_receive_over == 0)//等待接收
	{
		
		vTaskDelay(300);
		delay_300ms_count++;
		if(delay_300ms_count>= 20)
			return -1;//超时
	}
	serial_receive_over = 0;
	if(checkSum(UsartRx3.Data) == 1)
	{
		if(UsartRx3.Data[9] == 0)
		{
			
			return UsartRx3.Data[10]+(UsartRx3.Data[11]<<8);//打开成功
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
		//数据不对
	}
}

int modeling(u8 step)
{
	short sum = 0;
	DmaUsartTxBuf[0] = 0x01;
	DmaUsartTxBuf[1] = 0xef;//sig  0xef01
	DmaUsartTxBuf[2] = 0x00;
	DmaUsartTxBuf[3] = 0x00;//seq  0x0000
	DmaUsartTxBuf[4] = 0xff;
	DmaUsartTxBuf[5] = 0xff;//addr
	DmaUsartTxBuf[6] = 0x01;//type
	DmaUsartTxBuf[7] = 0x0f;
	DmaUsartTxBuf[8] = 0x00;//length  0x000f
	DmaUsartTxBuf[9] = 0x04;//cmd 0x04建模
	DmaUsartTxBuf[10] = step;
	DmaUsartTxBuf[11] = 0x00;//建模步骤
	DmaUsartTxBuf[12] = 0x00;
	DmaUsartTxBuf[13] = 0x00;
	DmaUsartTxBuf[14] = 0x00;
	DmaUsartTxBuf[15] = 0x00;
	DmaUsartTxBuf[16] = 0x00;
	DmaUsartTxBuf[17] = 0x00;
	DmaUsartTxBuf[18] = 0x00;
	DmaUsartTxBuf[19] = 0x00;
	DmaUsartTxBuf[20] = 0x00;
	DmaUsartTxBuf[21] = 0x00;//data  十个字节
	sum = calcSum(DmaUsartTxBuf);
	DmaUsartTxBuf[22] = sum & 0xff;
	DmaUsartTxBuf[23] = sum >>8;
	
	if(DMA1_Stream3->NDTR == 0)
    {
		DMA1_Stream3->CR &= ~(uint32_t)DMA_SxCR_EN;
        DMA1_Stream3->NDTR = 24;
        DMA1_Stream3->M0AR = (uint32_t)(DmaUsartTxBuf);
        DMA1_Stream3->CR |= (uint32_t)DMA_SxCR_EN;//DMA_Cmd(DMA2_Stream7, ENABLE);
    }
	serial_receive_over = 0;
	delay_300ms_count = 0;
	while(serial_receive_over == 0)//等待接收
	{
		
		vTaskDelay(100);
		delay_300ms_count++;
		if(delay_300ms_count>= 50)
			return -1;//超时
	}
	serial_receive_over = 0;
	if(checkSum(UsartRx3.Data) == 1)
	{
		switch(UsartRx3.Data[9])
		{
			case 0:return 1;//ok
			case 4:return -1;//需要调整手指
			case 5:return -1;//手指不同
			case 7:return -1;//参数不对
			case 8:return -1;//超时
			default:return -1;
		}
	}
	else
	{
		return -1;
		//数据不对
	}
}
int modeling_complete(void)
{
	short sum = 0;
	int ID = 0;
	DmaUsartTxBuf[0] = 0x01;
	DmaUsartTxBuf[1] = 0xef;//sig  0xef01
	DmaUsartTxBuf[2] = 0x00;
	DmaUsartTxBuf[3] = 0x00;//seq  0x0000
	DmaUsartTxBuf[4] = 0xff;
	DmaUsartTxBuf[5] = 0xff;//addr
	DmaUsartTxBuf[6] = 0x01;//type
	DmaUsartTxBuf[7] = 0x0f;
	DmaUsartTxBuf[8] = 0x00;//length  0x000f
	DmaUsartTxBuf[9] = 0x05;//cmd 0x05建模完成
	DmaUsartTxBuf[10] = 0x00;
	DmaUsartTxBuf[11] = 0x00;//设备自定
	DmaUsartTxBuf[12] = 0x00;
	DmaUsartTxBuf[13] = 0x00;
	DmaUsartTxBuf[14] = 0x00;
	DmaUsartTxBuf[15] = 0x00;
	DmaUsartTxBuf[16] = 0x00;
	DmaUsartTxBuf[17] = 0x00;
	DmaUsartTxBuf[18] = 0x00;
	DmaUsartTxBuf[19] = 0x00;
	DmaUsartTxBuf[20] = 0x00;
	DmaUsartTxBuf[21] = 0x00;//data  十个字节
	sum = calcSum(DmaUsartTxBuf);
	DmaUsartTxBuf[22] = sum & 0xff;
	DmaUsartTxBuf[23] = sum >>8;
	
	if(DMA1_Stream3->NDTR == 0)
    {
		DMA1_Stream3->CR &= ~(uint32_t)DMA_SxCR_EN;
        DMA1_Stream3->NDTR = 24;
        DMA1_Stream3->M0AR = (uint32_t)(DmaUsartTxBuf);
        DMA1_Stream3->CR |= (uint32_t)DMA_SxCR_EN;//DMA_Cmd(DMA2_Stream7, ENABLE);
    }
	serial_receive_over = 0;
	delay_300ms_count = 0;
	while(serial_receive_over == 0)//等待接收
	{
		
		vTaskDelay(300);
		delay_300ms_count++;
		if(delay_300ms_count>= 20)
			return -1;//超时
	}
	serial_receive_over = 0;
	if(checkSum(UsartRx3.Data) == 1)
	{
		switch(UsartRx3.Data[9])
		{
			case 0:ID = UsartRx3.Data[10] + (UsartRx3.Data[11]<<8) + (UsartRx3.Data[12]<< 16)+ (UsartRx3.Data[13]<<24);
				return ID; //ok，ID为之后四个字节
			case 7:return -1; //建模失败
			case 0x0a:return -1; //ID已存在
			default:return -1; 
		}
	}
	else
	{
		return -1;
		//数据不对
	}
}
int identification(void)
{
	short sum = 0;
	int ID = 0;
	DmaUsartTxBuf[0] = 0x01;
	DmaUsartTxBuf[1] = 0xef;//sig  0xef01
	DmaUsartTxBuf[2] = 0x00;
	DmaUsartTxBuf[3] = 0x00;//seq  0x0000
	DmaUsartTxBuf[4] = 0xff;
	DmaUsartTxBuf[5] = 0xff;//addr
	DmaUsartTxBuf[6] = 0x01;//type
	DmaUsartTxBuf[7] = 0x0f;
	DmaUsartTxBuf[8] = 0x00;//length  0x000f
	DmaUsartTxBuf[9] = 0x07;//cmd 0x07认证
	DmaUsartTxBuf[10] = 0x00;
	DmaUsartTxBuf[11] = 0x00;//设备自定
	DmaUsartTxBuf[12] = 0x00;
	DmaUsartTxBuf[13] = 0x00;
	DmaUsartTxBuf[14] = 0x00;
	DmaUsartTxBuf[15] = 0x00;
	DmaUsartTxBuf[16] = 0x00;
	DmaUsartTxBuf[17] = 0x00;
	DmaUsartTxBuf[18] = 0x00;
	DmaUsartTxBuf[19] = 0x00;
	DmaUsartTxBuf[20] = 0x00;
	DmaUsartTxBuf[21] = 0x00;//data  十个字节
	sum = calcSum(DmaUsartTxBuf);
	DmaUsartTxBuf[22] = sum & 0xff;
	DmaUsartTxBuf[23] = sum >>8;
	
	if(DMA1_Stream3->NDTR == 0)
    {
		DMA1_Stream3->CR &= ~(uint32_t)DMA_SxCR_EN;
        DMA1_Stream3->NDTR = 24;
        DMA1_Stream3->M0AR = (uint32_t)(DmaUsartTxBuf);
        DMA1_Stream3->CR |= (uint32_t)DMA_SxCR_EN;//DMA_Cmd(DMA2_Stream7, ENABLE);
    }
	serial_receive_over = 0;
	delay_300ms_count = 0;
	while(serial_receive_over == 0)//等待接收
	{
		
		vTaskDelay(300);
		delay_300ms_count++;
		if(delay_300ms_count>= 20)
			return -1;//超时
	}
	serial_receive_over = 0;
	if(checkSum(UsartRx3.Data) == 1)
	{
		switch(UsartRx3.Data[9])
		{
			case 0:ID = UsartRx3.Data[10] + (UsartRx3.Data[11]<<8) + (UsartRx3.Data[12]<< 16)+ (UsartRx3.Data[13]<<24);
				return ID; //ok，ID为之后四个字节
			case 4:return -1; //调整手指
			case 8:return -1; //超时
			default:return -1; 
		}
	}
	else
	{
		return -1;
		//数据不对
	}
}

int read_id(void)
{
	short sum = 0;
	
	DmaUsartTxBuf[0] = 0x01;
	DmaUsartTxBuf[1] = 0xef;//sig  0xef01
	DmaUsartTxBuf[2] = 0x00;
	DmaUsartTxBuf[3] = 0x00;//seq  0x0000
	DmaUsartTxBuf[4] = 0xff;
	DmaUsartTxBuf[5] = 0xff;//addr
	DmaUsartTxBuf[6] = 0x01;//type
	DmaUsartTxBuf[7] = 0x0f;
	DmaUsartTxBuf[8] = 0x00;//length  0x000f
	DmaUsartTxBuf[9] = 18;//cmd 18 读取id
	DmaUsartTxBuf[10] = 0x00;
	DmaUsartTxBuf[11] = 0x00;//设备自定
	DmaUsartTxBuf[12] = 0x00;
	DmaUsartTxBuf[13] = 0x00;
	DmaUsartTxBuf[14] = 0x00;
	DmaUsartTxBuf[15] = 0x00;
	DmaUsartTxBuf[16] = 0x00;
	DmaUsartTxBuf[17] = 0x00;
	DmaUsartTxBuf[18] = 0x00;
	DmaUsartTxBuf[19] = 0x00;
	DmaUsartTxBuf[20] = 0x00;
	DmaUsartTxBuf[21] = 0x00;//data  十个字节
	sum = calcSum(DmaUsartTxBuf);
	DmaUsartTxBuf[22] = sum & 0xff;
	DmaUsartTxBuf[23] = sum >>8;
	
	if(DMA1_Stream3->NDTR == 0)
    {
		DMA1_Stream3->CR &= ~(uint32_t)DMA_SxCR_EN;
        DMA1_Stream3->NDTR = 24;
        DMA1_Stream3->M0AR = (uint32_t)(DmaUsartTxBuf);
        DMA1_Stream3->CR |= (uint32_t)DMA_SxCR_EN;//DMA_Cmd(DMA2_Stream7, ENABLE);
    }
	serial_receive_over = 0;
	delay_300ms_count = 0;
	while(serial_receive_over == 0)//等待接收
	{
		
		vTaskDelay(300);
		delay_300ms_count++;
		if(delay_300ms_count>= 20)
			return 0;//超时
	}
	serial_receive_over = 0;
	if(checkSum(UsartRx3.Data) == 1)
	{
		switch(UsartRx3.Data[9])
		{
			case 0:
				return 1; //ok，ID为之后四个字节
			case 0xff:return -1;//调整手指
			default:return -1;
		}
	}
	else
	{
		return -1;
		//数据不对
	}
}

int concel_waiting(void)
{
	short sum = 0;
	
	DmaUsartTxBuf[0] = 0x01;
	DmaUsartTxBuf[1] = 0xef;//sig  0xef01
	DmaUsartTxBuf[2] = 0x00;
	DmaUsartTxBuf[3] = 0x00;//seq  0x0000
	DmaUsartTxBuf[4] = 0xff;
	DmaUsartTxBuf[5] = 0xff;//addr
	DmaUsartTxBuf[6] = 0x01;//type
	DmaUsartTxBuf[7] = 0x0f;
	DmaUsartTxBuf[8] = 0x00;//length  0x000f
	DmaUsartTxBuf[9] = 19;//cmd 19取消等待
	DmaUsartTxBuf[10] = 0x00;
	DmaUsartTxBuf[11] = 0x00;//设备自定
	DmaUsartTxBuf[12] = 0x00;
	DmaUsartTxBuf[13] = 0x00;
	DmaUsartTxBuf[14] = 0x00;
	DmaUsartTxBuf[15] = 0x00;
	DmaUsartTxBuf[16] = 0x00;
	DmaUsartTxBuf[17] = 0x00;
	DmaUsartTxBuf[18] = 0x00;
	DmaUsartTxBuf[19] = 0x00;
	DmaUsartTxBuf[20] = 0x00;
	DmaUsartTxBuf[21] = 0x00;//data  十个字节
	sum = calcSum(DmaUsartTxBuf);
	DmaUsartTxBuf[22] = sum & 0xff;
	DmaUsartTxBuf[23] = sum >>8;
	
	if(DMA1_Stream3->NDTR == 0)
    {
		DMA1_Stream3->CR &= ~(uint32_t)DMA_SxCR_EN;
        DMA1_Stream3->NDTR = 24;
        DMA1_Stream3->M0AR = (uint32_t)(DmaUsartTxBuf);
        DMA1_Stream3->CR |= (uint32_t)DMA_SxCR_EN;//DMA_Cmd(DMA2_Stream7, ENABLE);
    }
	serial_receive_over = 0;
	delay_300ms_count = 0;
	while(serial_receive_over == 0)//等待接收
	{
		
		vTaskDelay(300);
		delay_300ms_count++;
		if(delay_300ms_count>= 20)
			return 0;//超时
	}
	serial_receive_over = 0;
	if(checkSum(UsartRx3.Data) == 1)
	{
		switch(UsartRx3.Data[9])
		{
			case 0:
				return 1; //ok，ID为之后四个字节
			
			default:return -1; 
		}
	}
	else
	{
		return -1;
		//数据不对
	}
}

int delete_all_model(void)
{
	short sum = 0;
	
	DmaUsartTxBuf[0] = 0x01;
	DmaUsartTxBuf[1] = 0xef;//sig  0xef01
	DmaUsartTxBuf[2] = 0x00;
	DmaUsartTxBuf[3] = 0x00;//seq  0x0000
	DmaUsartTxBuf[4] = 0xff;
	DmaUsartTxBuf[5] = 0xff;//addr
	DmaUsartTxBuf[6] = 0x01;//type
	DmaUsartTxBuf[7] = 0x0f;
	DmaUsartTxBuf[8] = 0x00;//length  0x000f
	DmaUsartTxBuf[9] = 0x0a;//cmd 0x0a删除所有模板
	DmaUsartTxBuf[10] = 0x00;
	DmaUsartTxBuf[11] = 0x00;//设备自定
	DmaUsartTxBuf[12] = 0x00;
	DmaUsartTxBuf[13] = 0x00;
	DmaUsartTxBuf[14] = 0x00;
	DmaUsartTxBuf[15] = 0x00;
	DmaUsartTxBuf[16] = 0x00;
	DmaUsartTxBuf[17] = 0x00;
	DmaUsartTxBuf[18] = 0x00;
	DmaUsartTxBuf[19] = 0x00;
	DmaUsartTxBuf[20] = 0x00;
	DmaUsartTxBuf[21] = 0x00;//data  十个字节
	sum = calcSum(DmaUsartTxBuf);
	DmaUsartTxBuf[22] = sum & 0xff;
	DmaUsartTxBuf[23] = sum >>8;
	
	if(DMA1_Stream3->NDTR == 0)
    {
		DMA1_Stream3->CR &= ~(uint32_t)DMA_SxCR_EN;
        DMA1_Stream3->NDTR = 24;
        DMA1_Stream3->M0AR = (uint32_t)(DmaUsartTxBuf);
        DMA1_Stream3->CR |= (uint32_t)DMA_SxCR_EN;//DMA_Cmd(DMA2_Stream7, ENABLE);
    }
	serial_receive_over = 0;
	delay_300ms_count = 0;
	while(serial_receive_over == 0)//等待接收
	{
		
		vTaskDelay(300);
		delay_300ms_count++;
		if(delay_300ms_count>= 20)
			return 0;//超时
	}
	serial_receive_over = 0;
	if(checkSum(UsartRx3.Data) == 1)
	{
		switch(UsartRx3.Data[9])
		{
			case 0:
				return 1; //ok，ID为之后四个字节
			default:return -1;
		}
	}
	else
	{
		return -1;
		//数据不对
	}
}
int delete_model(int id)
{
	short sum = 0;
	DmaUsartTxBuf[0] = 0x01;
	DmaUsartTxBuf[1] = 0xef;//sig  0xef01
	DmaUsartTxBuf[2] = 0x00;
	DmaUsartTxBuf[3] = 0x00;//seq  0x0000
	DmaUsartTxBuf[4] = 0xff;
	DmaUsartTxBuf[5] = 0xff;//addr
	DmaUsartTxBuf[6] = 0x01;//type
	DmaUsartTxBuf[7] = 0x0f;
	DmaUsartTxBuf[8] = 0x00;//length  0x000f
	DmaUsartTxBuf[9] = 0x09;//cmd 0x09删除指定模板
	DmaUsartTxBuf[10] = id&0xff;
	DmaUsartTxBuf[11] = (id>>8) & 0xff;//设备自定
	DmaUsartTxBuf[12] = (id>>16) & 0xff;
	DmaUsartTxBuf[13] = (id>>24) & 0xff;
	DmaUsartTxBuf[14] = 0x00;
	DmaUsartTxBuf[15] = 0x00;
	DmaUsartTxBuf[16] = 0x00;
	DmaUsartTxBuf[17] = 0x00;
	DmaUsartTxBuf[18] = 0x00;
	DmaUsartTxBuf[19] = 0x00;
	DmaUsartTxBuf[20] = 0x00;
	DmaUsartTxBuf[21] = 0x00;//data  十个字节
	sum = calcSum(DmaUsartTxBuf);
	DmaUsartTxBuf[22] = sum & 0xff;
	DmaUsartTxBuf[23] = sum >>8;
	
//	if(id == member_id[0])
//	{
//		return 0;
//	}
	if(DMA1_Stream3->NDTR == 0)
    {
		DMA1_Stream3->CR &= ~(uint32_t)DMA_SxCR_EN;
        DMA1_Stream3->NDTR = 24;
        DMA1_Stream3->M0AR = (uint32_t)(DmaUsartTxBuf);
        DMA1_Stream3->CR |= (uint32_t)DMA_SxCR_EN;//DMA_Cmd(DMA2_Stream7, ENABLE);
    }
	serial_receive_over = 0;
	delay_300ms_count = 0;
	while(serial_receive_over == 0)//等待接收
	{
		
		vTaskDelay(300);
		delay_300ms_count++;
		if(delay_300ms_count>= 20)
			return -1;//超时
	}
	serial_receive_over = 0;
	if(checkSum(UsartRx3.Data) == 1)
	{
		switch(UsartRx3.Data[9])
		{
			case 0:
				return 1; //ok，ID为之后四个字节
			default:return -1;
		}
	}
	else
	{
		return -1;
		//数据不对
	}
}
/*********************************END OF FILE**********************************/
