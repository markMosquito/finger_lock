/**
  ******************************************************************************
  * @file    Rsing_Can.c
  * @author  Rsing
  * @version V1.0
  * @date    14-November-2014
  * @brief   Initializate the Can.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "Rsing_Can.h"
#include <string.h>
#include "Redef_GPIO.h"

/* Private variables ---------------------------------------------------------*/
/* Define --------------------------------------------------------------------*/
#define IO_CAN1_TX     PA(12)
#define IO_CAN1_RX     PA(11)
#define IO_CAN2_TX     PB(13)
#define IO_CAN2_RX     PB(12)

//CAN1 ---------------------------------------------------------
#define GPIO_CAN1_TX            IO_GPIO_PORT(IO_CAN1_TX)
#define GPIO_CAN1_TX_PIN        IO_GPIO_PIN(IO_CAN1_TX)
#define RCC_GPIO_CAN1_TX        IO_RCC_AHB1_PERIPH(IO_CAN1_TX)
#define GPIO_PINSOURCE_CAN1_TX  IO_GPIO_PINSOURCE(IO_CAN1_TX)

#define GPIO_CAN1_RX            IO_GPIO_PORT(IO_CAN1_RX)
#define GPIO_CAN1_RX_PIN        IO_GPIO_PIN(IO_CAN1_RX)
#define RCC_GPIO_CAN1_RX        IO_RCC_AHB1_PERIPH(IO_CAN1_RX)
#define GPIO_PINSOURCE_CAN1_RX  IO_GPIO_PINSOURCE(IO_CAN1_RX)
//CAN2 ---------------------------------------------------------
#define GPIO_CAN2_TX            IO_GPIO_PORT(IO_CAN2_TX)
#define GPIO_CAN2_TX_PIN        IO_GPIO_PIN(IO_CAN2_TX)
#define RCC_GPIO_CAN2_TX        IO_RCC_AHB1_PERIPH(IO_CAN2_TX)
#define GPIO_PINSOURCE_CAN2_TX  IO_GPIO_PINSOURCE(IO_CAN2_TX)

#define GPIO_CAN2_RX            IO_GPIO_PORT(IO_CAN2_RX)
#define GPIO_CAN2_RX_PIN        IO_GPIO_PIN(IO_CAN2_RX)
#define RCC_GPIO_CAN2_RX        IO_RCC_AHB1_PERIPH(IO_CAN2_RX)
#define GPIO_PINSOURCE_CAN2_RX  IO_GPIO_PINSOURCE(IO_CAN2_RX)


T_SysErrors g_SysErrors = {0,};

/* Private variables ---------------------------------------------------------*/
// 常数定义
const uint8_t TxMailBox_IdleNO_Tbl[] = {
				0xFF/*0x0:Busy*/,	0/*0x1:0*/, 	1/*010b:1*/, 
				0/*011b:0,1*/,		2/*100b:2*/,	0/*101b:0,2*/, 
				1/*110b:1,2*/,		0/*111b:0,1,2*/};

// 队列参数
/* 发送中断，应用层发送函数直接将数据设置为欲发送的CAN发送邮箱格式
 * T_CanFrame/CAN_TypeDef打包
 * 发送时如果邮箱有空则直接将数据压入邮箱，否则将数据压入队列
 */
T_CanQueue CanTxQueue;
// CAN发送缓冲区
static T_CanFrame CanTxBuf[CAN_QUE_TX_Nframe] = {0,};


/*********************************************************************************************************
** 函数名称: CAN_QueueCreate
** 功能描述: 初始化数据队列
** 输　入: Buf     ： 为队列分配的存储空间地址
**         pDataBuf： 数据存放缓冲区
**		   nData:	  数据元素个数,以*pDataBuf为单元的个数
**         ReadEmpty：为队列读空时处理程序
**         WriteFull：为队列写满时处理程序
**         WriteFullParam: 队列写满函数调用时的参数
** 输　出: NOT_OK:参数错误
**         QUEUE_OK:成功
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
********************************************************************************************************/
static t_err CAN_QueueCreate(register T_CanQueue *Queue, T_CanFrame *pDataBuf,
                          	t_size_opt nData    /*, t_size_opt nSizeOfData*/
							#if (CAN_QUEUE_READ_EMPTY_EN > 0)
                          	,t_err (* ReadEmpty)()
							#endif
							#if (CAN_QUEUE_WRITE_FULL_EN > 0)
                          	,t_err (* WriteFull)(uint32_t)
							,uint32_t   WriteFullParam
							#endif
                          	)
{    	
	if (Queue != NULL && pDataBuf != NULL)                  /* 判断参数是否有效 */
	{
        CAN_QUE_OS_CPU_SR_ALLOC();
		CAN_QUE_OS_ENTER_CRITICAL();
		                                                	/* 初始化结构体数据 */
		Queue->Buf = pDataBuf;
		Queue->MaxData = nData;				               	/* 计算队列可以存储的数据数目 */
		Queue->End = &Queue->Buf[Queue->MaxData];           /* 计算数据缓冲的结束地址,已超出缓冲区 */
		Queue->Out = Queue->Buf;
		Queue->In = Queue->Buf;
		Queue->NData = 0;
		
		#if (CAN_QUEUE_READ_EMPTY_EN > 0)
		Queue->ReadEmpty = ReadEmpty;
		#endif
        
		#if (CAN_QUEUE_WRITE_FULL_EN > 0)
		Queue->WriteFull = WriteFull;
		Queue->WriteFullParam = WriteFullParam;
		#endif
        
		CAN_QUE_OS_EXIT_CRITICAL();

		return QUEUE_OK;
	}
 	else
	{
		return NOT_OK;
	}
}

/* Write ---------------------------------------------------------------------*/
/******************************************************************************
** 函数名称: CAN_QueueWrite
** 功能描述: 写指定的数据量至队列
** 输　入: Queue:指向队列的指针
**         pData:消息数据
** 返　回: NOT_OK     ：参数错误
**         QUEUE_OK   ：收到消息
**         QUEUE_EMPTY：无消息
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
*******************************************************************************/
#if defined ( __ICCARM__ )
t_err CAN_QueueWriteQuick(register T_CanQueue *Queue, T_CanFrame *pData)
{
	register t_err err;
	register T_CanFrame *pIn;
	
    err = NOT_OK;
    if (Queue != NULL)                             /* 队列是否有效 */
    {
        if (Queue->NData < Queue->MaxData)         	/* 队列是否满  */
        {                                  			/* 不满        */
        	pIn = Queue->In;
            *pIn = *pData;                 			/* 数据入队    */
            ++pIn;
            if (pIn >= Queue->End)
            {
                pIn = Queue->Buf;
            }
            Queue->In = pIn;                    	/* 调整入队指针*/
            Queue->NData++;                         /* 数据增加    */
            err = QUEUE_OK;
        }
        else
        {                                           /* 满           */
            err = QUEUE_FULL;
			#if (CAN_QUEUE_WRITE_FULL_EN > 0)
            if (Queue->WriteFull != NULL)        	/* 调用用户处理函数 */
            {
                err = Queue->WriteFull(Queue->WriteFullParam);
            }
			#endif
		}
    }
    return err;
}
#endif

/* Read ----------------------------------------------------------------------*/
#if CAN_QUEUE_READ_EN > 0
/******************************************************************************
** 函数名称: CAN_QueueRead
** 功能描述: 获取队列中的一条数据单元
** 输　入: Queue:指向队列的指针
**         pData:数据存放地
** 返　回: NOT_OK     ：参数错误
**         QUEUE_OK   ：收到消息
**         QUEUE_EMPTY：无消息
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
*******************************************************************************/
__inline t_err CAN_QueueRead(register T_CanQueue *Queue, T_CanFrame *pData)
{
	register t_err err;
	register uint16 uData;
	register T_CanFrame *pOut, *pOutOld;
    
    CAN_QUE_OS_CPU_SR_ALLOC();
    
    CAN_QUE_OS_ENTER_CRITICAL();
	if (Queue != NULL)                              /* 队列是否有效 */
	{                                               /* 有效 */
		if (Queue->NData > 0)                       /* 队列是否为空 */
		{                                    		/* 不空      */
			pOut = Queue->Out;                      /* 数据出队  */
			pOutOld = pOut;
			++pOut;
			if (pOut >= Queue->End)
			{
				pOut = Queue->Buf;
			}
			Queue->Out = pOut;                  	/* 调整出队指针 */
			Queue->NData--;                         /* 数据减少      */
			
			CAN_INT_RX_ENABLE();					/* 开启所有接收中断 */
			*pData = *pOutOld;						/* 开启中断再复制数据以减少中断时间 */
			err = QUEUE_OK;
		}
		else
		{                                           /* 空              */
			err = QUEUE_EMPTY;
			#if (CAN_QUEUE_READ_EMPTY_EN > 0)
			if (Queue->ReadEmpty != NULL)           /* 调用用户处理函数 */
			{
				err = Queue->ReadEmpty(Queue);
			}
			#endif
		}

	}
    else
    {
        err = NOT_OK;
    }
    CAN_QUE_OS_EXIT_CRITICAL();
	return err;
}
#endif	// end: #if CAN_QUEUE_READ_EN > 0

// 不中断
__inline static t_err CAN_QueueReadQuick(T_CanFrame *pData, register T_CanQueue *Queue)
{
	register t_err err;
	register T_CanFrame *pOut;
	
	err = NOT_OK;
	
	if (Queue != NULL)                           	/* 队列是否有效 */
	{                                               /* 有效 */
		//CAN_QUE_OS_ENTER_CRITICAL();
		if (Queue->NData > 0)                       /* 队列是否为空 */
		{                                           /* 不空         */
			pOut = Queue->Out;                      /* 数据出队     */
			*pData = *pOut;
			++pOut;
			if (pOut >= Queue->End)
			{
				pOut = Queue->Buf;
			}
			Queue->Out = pOut;                  	/* 调整出队指针 */
			Queue->NData--;                         /* 数据减少      */
			err = QUEUE_OK;
		}
		else
		{                                           /* 空              */
			err = QUEUE_EMPTY;
			#if (CAN_QUEUE_READ_EMPTY_EN > 0)
			if (Queue->ReadEmpty != NULL)           /* 调用用户处理函数 */
			{
				err = Queue->ReadEmpty(Queue);
			}
			#endif
		}
		//CAN_QUE_OS_EXIT_CRITICAL();
	}
	return err;
}

#if CAN1_SCE_IRQ_Handler_EXT_EN == 0
/* CAN1控制及状态变化中断 */
void CAN1_SCE_IRQHandler (void)
{
    if(CAN_GetITStatus(CAN1, CAN_IT_LEC))
    {
        g_SysErrors.CAN_LEC[(CAN1->ESR & CAN_ESR_LEC) >> 4]++;// CAN各种错误状态次数:位填充,格式,确认,隐性,显性,CRC
        CAN_ClearITPendingBit(CAN1,CAN_IT_LEC);
    }
    if(CAN_GetITStatus(CAN1, CAN_IT_BOF))       // CAN离线错误次数
    {
        g_SysErrors.CAN_BOF++;
        CAN_ClearITPendingBit(CAN1,CAN_IT_BOF);
    }
    if(CAN_GetITStatus(CAN1, CAN_IT_EPV))       // CAN错误被动次数
    {
        g_SysErrors.CAN_EPV++;
        CAN_ClearITPendingBit(CAN1,CAN_IT_EPV);
    }
    if(CAN_GetITStatus(CAN1, CAN_IT_EWG))       // CAN错误警告标志次数
    {
        g_SysErrors.CAN_EWG++;
        CAN_ClearITPendingBit(CAN1,CAN_IT_EWG);
    }
}
#endif		// end: #if CAN1_SCE_IRQ_Handler_EXT_EN == 0


/*----------------------------------------------------------------------------
  CAN transmit interrupt handler
 *----------------------------------------------------------------------------*/
void CAN1_TX_IRQHandler(void)	// CAN1_TX_IRQHandler_Name
{
	register t_ureg dw_r;
	register CAN_TxMailBox_TypeDef *pTxMailBox;

    /* 发送FIFO空中断 */
    if(CanTxQueue.NData > 0)
    {
    	// 如果发送队列不为空
		dw_r = (CAN1->TSR >> CAN_TSR_TME_BITnL) & 0x07;	// 查找空的发送邮箱
        dw_r = TxMailBox_IdleNO_Tbl[dw_r];			// 获取一个空邮箱号
        pTxMailBox = &CAN1->sTxMailBox[dw_r];
        CAN_QueueReadQuick((T_CanFrame *)pTxMailBox, &CanTxQueue);
        SETBITs(pTxMailBox->TIR, CAN_TIxR_TXRQ);    // 请求发送
    }
    else
    {
		/* Clear CAN_TSR_RQCPx (rc_w1) */
		CAN1->TSR = CAN_TSR_RQCP0|CAN_TSR_RQCP1|CAN_TSR_RQCP2;	// 清除发送请求,以防止再次进入中断, Add by Xsky 2011-07-30 17:51
        CAN_INT_TX_DISABLE();
	}
}

/**@note 仅作中断保护 未做任务保护*/
t_err CAN_SendFrame(T_CanFrame *pCanFrame)
{
	register CAN_TxMailBox_TypeDef *pTxMailBox;
	register t_ureg dw_r;
	t_err err = ERR_NONE;
	/* 发送报文的流程为：应用程序选择1个空置的发送邮箱；设置标识符，
	   数据长度和待发送数据；然后对CAN_TIxR寄存器的TXRQ位置’1’，来请求发送。 
	   发送状态CAN_TSR,Datasheet.CN:p441 */
	taskENTER_CRITICAL();
    CAN_INT_TX_DISABLE();           //关闭发送中断
    if(CanTxQueue.NData > 0)        // 队列不为空则直接写入发送队列
    {
        #ifdef DBG_CAN_QUE_MAX      // 调试,记录发送队列最大存储数量            
        if(CanTxQueue.NData < g_History.SysErrors.CAN_SendQueMaxN)
        {
            g_History.SysErrors.CAN_SendQueMaxN = CanTxQueue.NData;
        }
        #endif
        err = CAN_QueueWriteQuick(&CanTxQueue, pCanFrame);
        CAN_INT_TX_ENABLE();        // 开启发送中断
    }
    else    // 如果队列为空
    {
        dw_r = (CAN1->TSR >> CAN_TSR_TME_BITnL) & 0x07;
        if(dw_r)	// 有空闲的发送邮箱
        {
            pTxMailBox = &CAN1->sTxMailBox[TxMailBox_IdleNO_Tbl[dw_r]];
			pTxMailBox->TIR = pCanFrame->IxR;
            pTxMailBox->TDTR = pCanFrame->DTxR;
            pTxMailBox->TDLR = pCanFrame->Data.u32[0];
            pTxMailBox->TDHR = pCanFrame->Data.u32[1];
            pTxMailBox->TIR |= CAN_TIxR_TXRQ;
            CAN_INT_TX_ENABLE();	// 开启发送中断
        }
        else		// 无空闲邮箱
        {
            err = CAN_QueueWriteQuick(&CanTxQueue, pCanFrame);
            CAN_INT_TX_ENABLE();    // 开启发送中断
        }
    }
	taskEXIT_CRITICAL();
	return err;
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initialization for CAN
  * @param  None
  * @retval None
  */
void CAN_Config(CAN_TypeDef* CANx)
{
    GPIO_InitTypeDef         GPIO_InitStructure;
    CAN_InitTypeDef          CAN_InitStructure;
    CAN_FilterInitTypeDef    CAN_FilterInitStructure;      
    NVIC_InitTypeDef         NVIC_InitStructure;

    if( CANx == CAN1 )
    {
        /* Enable GPIO clock */
        RCC_AHB1PeriphClockCmd(RCC_GPIO_CAN1_TX|RCC_GPIO_CAN1_RX, ENABLE);

        /* Enable CAN clock */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

        /* Configure CAN RX and TX pins */
        GPIO_InitStructure.GPIO_Pin = GPIO_CAN1_TX_PIN;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
        GPIO_Init(GPIO_CAN1_TX, &GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_CAN1_RX_PIN;
        GPIO_Init(GPIO_CAN1_RX, &GPIO_InitStructure);

        /* Connect CAN pins to AF9 */
        GPIO_PinAFConfig(GPIO_CAN1_TX, GPIO_PINSOURCE_CAN1_TX, GPIO_AF_CAN1);
        GPIO_PinAFConfig(GPIO_CAN1_RX, GPIO_PINSOURCE_CAN1_RX, GPIO_AF_CAN1);		
    }
    else if( CANx == CAN2 )
    {  
        /* Enable GPIO clock */
        RCC_AHB1PeriphClockCmd(RCC_GPIO_CAN2_TX|RCC_GPIO_CAN2_RX, ENABLE);

        /* Enable CAN clock */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1 | RCC_APB1Periph_CAN2, ENABLE);

        /* Configure CAN RX and TX pins */
        GPIO_InitStructure.GPIO_Pin = GPIO_CAN2_TX_PIN;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
        GPIO_Init(GPIO_CAN2_TX, &GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_CAN2_RX_PIN;
        GPIO_Init(GPIO_CAN2_RX, &GPIO_InitStructure);

        /* Connect CAN pins to AF9 */
        GPIO_PinAFConfig(GPIO_CAN2_TX, GPIO_PINSOURCE_CAN2_TX, GPIO_AF_CAN2);
        GPIO_PinAFConfig(GPIO_CAN2_RX, GPIO_PINSOURCE_CAN2_RX, GPIO_AF_CAN2);
    }

    /* CAN register init */
    CAN_DeInit(CANx);
    CAN_StructInit(&CAN_InitStructure);	 

    /* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;   // Enable or disable the time triggered communication mode.   
	CAN_InitStructure.CAN_ABOM = DISABLE;   // Enable or disable the automatic bus-off management.
	CAN_InitStructure.CAN_AWUM = DISABLE;   // Enable or disable the automatic wake-up mode.
	CAN_InitStructure.CAN_NART = DISABLE;   // Enable or disable the non-automatic retransmission mode.
	CAN_InitStructure.CAN_RFLM = DISABLE;   // Enable or disable the Receive FIFO Locked mode.
	CAN_InitStructure.CAN_TXFP = ENABLE;    // Enable or disable the transmit FIFO priority. 
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
    CAN_InitStructure.CAN_SJW  = CAN_SJW_1tq;

    /* Baudrate = 1Mbps (CAN clocked at 42 MHz) */
    CAN_InitStructure.CAN_BS1 = CAN_BS1_9tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_4tq;
    CAN_InitStructure.CAN_Prescaler = 3;     //CAN波特率42/(1+9+4)/3=1Mbps
    CAN_Init(CANx, &CAN_InitStructure);

    if( CANx == CAN1 )
    {
        /* CAN filter init */
        CAN_FilterInitStructure.CAN_FilterNumber = 0;                        
        CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;      
        CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;     
        CAN_FilterInitStructure.CAN_FilterIdHigh = (((u32)0x0012<<21)&0xFFFF0000)>>16;               
        CAN_FilterInitStructure.CAN_FilterIdLow = (((u32)0x0012<<21)|CAN_ID_STD|CAN_RTR_DATA)&0xFFFF;                     
        CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;           
        CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;               
        CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;               
        CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
        CAN_FilterInit(&CAN_FilterInitStructure);

        /* CAN FIFO0 message pending interrupt enable */ 
        CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE);

        /* Enable the CAN1 global Interrupt */
        NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
        
		CAN_ITConfig(CAN1,CAN_IT_TME, ENABLE);

		/* Enable the CAN1 global Interrupt */
		NVIC_InitStructure.NVIC_IRQChannel = CAN1_TX_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
        
        /* Enable the CAN1 global Interrupt */
        #ifdef DBG_CAN
        CAN1->ESR |= CAN_ESR_LEC_USER;				// 将上次错误号设置为用户状态(程序不会设置该状态)以区分新的状态
        CAN_ITConfig(CAN1, CAN_IT_ERR|CAN_IT_LEC|CAN_IT_BOF|CAN_IT_EPV|CAN_IT_EWG, ENABLE);
		NVIC_InitStructure.NVIC_IRQChannel = CAN1_SCE_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
        #endif
        /* 应用程序队列初始化 */
        // TX, 发送队列
        CAN_QueueCreate(&CanTxQueue, CanTxBuf, lenof(CanTxBuf));			// 发送队列
    }
    else if( CANx == CAN2 )
    {
        /* CAN filter init */
        CAN_FilterInitStructure.CAN_FilterNumber = 14;                        
        CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;      
        CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;     
        CAN_FilterInitStructure.CAN_FilterIdHigh = (((u32)0x0012<<21)&0xFFFF0000)>>16;               
        CAN_FilterInitStructure.CAN_FilterIdLow = (((u32)0x0012<<21)|CAN_ID_STD|CAN_RTR_DATA)&0xFFFF;                     
        CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;           
        CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;               
        CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;               
        CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
        CAN_FilterInit(&CAN_FilterInitStructure);

        /* CAN FIFO0 message pending interrupt enable */ 
        CAN_ITConfig(CAN2,CAN_IT_FMP0, ENABLE);

        /* Enable the CAN2 global Interrupt */
        NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);	
    }
}

/**
  * @brief  can发送一组数据
  * @param  CAN_Num:CAN端口
            len:数据长度,最大为8	
            msg:数据指针,最大为8个字节.
            id: 发送标识符
  * @retval 0,成功;
            1,失败
  */
uint8_t Can_Send_Msg(CAN_TypeDef* CAN_Num, uint8_t* msg, uint8_t len, uint8_t id)
{	 
    u8 mbox;
    u16 i=0;
    CanTxMsg TxMessage;
    
    TxMessage.StdId=id;					 	//标准标识符，11位
    TxMessage.IDE=CAN_Id_Standard;	        //使用标准标识符
    TxMessage.RTR=CAN_RTR_Data;			    //发送数据帧
    TxMessage.DLC=len;						//发送len字节
    
    for(i=0;i<len;i++)
        TxMessage.Data[i]=msg[i];
    
    mbox = CAN_Transmit(CAN_Num, &TxMessage);     //获取邮箱号

    i = 0;
    while((CAN_TransmitStatus(CAN_Num, mbox)!=CAN_TxStatus_Ok)&&(i<0xfff))i++;	//等待发送结束  
    if(i>=0xfff)return 1;
    
    return 0;		
}


/*********************************END OF FILE**********************************/
