/**
  ******************************************************************************
  * @file    Rsing_Can.h
  * @author  Rsing
  * @version V1.0
  * @date    14-November-2014
  * @brief   Header file for Rsing_Can.c module. 
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RSING_CAN_H
#define __RSING_CAN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"


#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"


/****************************************************************************/
// 编译开关
#define RELEASE		1		// 1: 发布, 0:非发布
#define IS_RELEASE()		(RELEASE == 1)
#define IS_N_RELEASE()		(RELEASE != 1)

#if IS_N_RELEASE()
    #define  DBG_CAN				// 调试CAN
    #define	 DBG_CAN_QUE_MAX		// CAN调用记录队列最大值
#endif



/* -------------------------------------------------------------------------- */
typedef uint32_t 			t_ureg;				/* 无符号寄存器类型 */
typedef int32_t				t_sreg;				/* 有符号寄存器类型 */
typedef uint16_t			t_ureg_opt;			/* 无符号优化寄存器类型 */
typedef int16_t				t_sreg_opt;			/* 有符号优化寄存器类型 */

typedef t_ureg				t_bool;				/* boolean类型 */
typedef t_ureg				t_err;				/* 错误码类型 */
typedef uint32_t			t_size;				/* 数据长度类型 */
typedef uint16_t			t_size_opt;			/* 优化数据长度 */

#ifndef BIT
    #define BIT(n)			(1<<(n))
#endif
#define LSHFT(x,n)		((x) << (n))
#define RSHFT(x,n)		((x) >> (n))
#define SETBIT(x,n) 	x |= (1<<(n))
#define CLRBIT(x,n) 	x &= (~(1<<(n)))
#define SETBITs(x,msk)	x|=(msk)
#define CLRBITs(x,msk)	x&=(~(msk))
#define lenof(x) 	( sizeof((x)) / sizeof(*(x)) )	// Add By Xsky 2008-9-03 15:56
    
#ifndef NOT_OK
    #define NOT_OK              0xff                        /* 参数错误                                     */
#endif
#define  ERR_NONE					0x0000
#define QUEUE_FULL          8                           /* 队列满                                       */
#define QUEUE_EMPTY         4                           /* 无数据                                       */
#define QUEUE_OK            0                           /* 操作成功                                     */


typedef struct T_SysErrors_{
	// CAN_错误
	uint32_t	CAN_LEC[6];					// CAN各种错误状态次数:位填充,格式,确认,隐性,显性,CRC
	uint32_t	CAN_BOF;					// CAN离线错误次数
	uint32_t	CAN_EPV;					// CAN错误被动次数
	uint32_t	CAN_EWG;					// CAN错误警告标志次数
	//uint32_t	CAN_TX;					// CAN发送错误
	//uint32_t	CAN_ALST;				// CAN由于仲裁引起的错误
	//uint32_t	CAN_FOVR;				// CAN溢出错误
	uint32_t	CAN_SendQueOVF;				// CAN发送队列溢出
	uint32_t	CAN_RcvQueOVF;				// CAN接收队列溢出
	#if IS_N_RELEASE()
	uint8_t	CAN_SendQueMaxN;			// CAN发送队列最大值
	uint8_t	CAN_RcvQueMaxN;				// CAN接收队列最大元素个数
	#endif
}T_SysErrors;


/* Defines -------------------------------------------------------------------*/
#define  CAN_QUE_RX_Nframe	128		/* CAN接收队列帧数目,以结构体T_CanFrame为单元来计算 */
#define  CAN_QUE_TX_Nframe	256		/* CAN发送队列帧数目,以结构体T_CanFrame为单元来计算 */


// STM32 CAN寄存器相关定义
#define CAN_TSR_TME_BITnL			26						/* CAN发送 */
#define CAN_TIxR_IDE				((uint32_t)BIT(2))		/* 0:标准标识符, 1:扩展标识符 */
#define CAN_TIxR_RTR				((uint32_t)BIT(1))		/* 0:数据帧, 1:远程帧 */
#define CAN_TIxR_TXRQ         		((uint32_t)0x00000001)	/* register CAN_TIxR */
#define CAN_TDTxR_DLC         		((uint32_t)0x0000000F)	/* register CAN_TDTxR  */
#define CAN_ESR_LEC_BITnL			4						/* CAN_ESC_LEC最低位号 */
#define CAN_ESR_LEC_USER			CAN_ESR_LEC				/* CAN_ESC_LEC位掩码 */
#define CAN_ESR_TEC_BITnL			16						/* CAN_ESR_TEC最低位号 */

#define CAN_INT_TX_DISABLE()		CLRBITs(CAN1->IER, CAN_IT_TME)	/* 关闭CAN发送中断 */
#define CAN_INT_TX_ENABLE()			SETBITs(CAN1->IER, CAN_IT_TME)	/* 开启CAN发送中断 */
#define CAN_INT_RX0_DISABLE()		CLRBITs(CAN1->IER, CAN_IT_FMP0) /* 关闭CAN接中断 */
#define CAN_INT_RX0_ENABLE()		SETBITs(CAN1->IER, CAN_IT_FMP0) /* 开启CAN接中断 */
#define CAN_INT_RX1_DISABLE()		CLRBITs(CAN1->IER, CAN_IT_FMP1) /* 关闭CAN接中断 */
#define CAN_INT_RX1_ENABLE()		SETBITs(CAN1->IER, CAN_IT_FMP1) /* 开启CAN接中断 */
#define CAN_INT_RX_DISABLE()		CLRBITs(CAN1->IER, CAN_IT_FMP0 | CAN_IT_FMP1) /* 关闭CAN接中断 */
#define CAN_INT_RX_ENABLE()			SETBITs(CAN1->IER, CAN_IT_FMP0 | CAN_IT_FMP1) /* 开启CAN接中断 */


#ifndef CAN1_SCE_IRQ_Handler_EXT_EN
    #define CAN1_SCE_IRQ_Handler_EXT_EN	0
#endif

#ifndef CAN_QUEUE_CRITICAL_EN
    #define CAN_QUEUE_CRITICAL_EN		1
#endif


#if (CAN_QUEUE_CRITICAL_EN > 0)
    #ifdef INC_FREERTOS_H
        #define OS_CPU_SR_ALLOC()           UBaseType_t uxSavedInterruptStatus
        #define OS_ENTER_CRITICAL()		    uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR()
        #define OS_EXIT_CRITICAL()		    portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus )
    #else
        #define OS_CPU_SR_ALLOC()           OS_CPU_SR  cpu_sr = (OS_CPU_SR)0
    #endif
    #define CAN_QUE_OS_CPU_SR_ALLOC()       OS_CPU_SR_ALLOC()
    #define CAN_QUE_OS_ENTER_CRITICAL()		OS_ENTER_CRITICAL()
    #define CAN_QUE_OS_EXIT_CRITICAL()		OS_EXIT_CRITICAL()
#else
    #define CAN_QUE_OS_ENTER_CRITICAL()		
    #define CAN_QUE_OS_EXIT_CRITICAL()		
#endif

#ifndef CAN_QUEUE_WRITE_FULL_EN
    #define CAN_QUEUE_WRITE_FULL_EN		0
#endif

#ifndef CAN_QUEUE_READ_EMPTY_EN
    #define CAN_QUEUE_READ_EMPTY_EN		0
#endif

#ifndef CAN_QUEUE_READ_EN
    #define  CAN_QUEUE_READ_EN          0
#endif

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

typedef union
{
    uint8_t 	u08[8];		/* 定义字节型以满足需求 */
    uint16_t	u16[4];		
    uint32_t	u32[2];		/* 定义寄存器类型以提高读取效率 */
}T_CanData;

// 因为发送邮箱与接收邮箱数据结构基本相同，所以队列数据可直接使用
typedef struct
{
    uint32_t	IxR;	/* 与TIxR/RIxR对应 */                      //TIR
    uint32_t 	DTxR;	/* 与TDTxR/RDTxR对应 */                    //TDTR
	T_CanData 	Data; 	/* TLxR:THxR/RLxR:RHxR, 八字节数据 */      //TDLR,TDTR
}T_CanFrame;

typedef struct
{
    // 经测试将In, Out, 定义为索引方式比定义为指针执行效率低
    T_CanFrame		*Out;					/* 指向数据输出位置 		*/
    T_CanFrame 		*In;					/* 指向数据输入位置 		*/
    T_CanFrame 		*End;					/* 指向Buf的结束位置		*/
    uint16_t			NData;					/* 队列中数据个数			*/
    uint16_t			MaxData;				/* 队列中允许存储的数据个数 */
    #if (CAN_QUEUE_READ_EMPTY_EN > 0)
    t_err			(* ReadEmpty)(void);	/* 读空处理函数 			*/
    #endif
    #if (CAN_QUEUE_WRITE_FULL_EN > 0)
    t_err			(* WriteFull)(uint32);	/* 写满处理函数 			*/
    uint32			WriteFullParam;
    #endif
    T_CanFrame 		*Buf; 					/* 存储数据的空间			*/
}T_CanQueue;

/* Private functions ---------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
extern T_CanQueue CanTxQueue;
extern const uint8_t TxMailBox_IdleNO_Tbl[];
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
#if defined ( __CC_ARM )
// 不关中断
__inline t_err CAN_QueueWriteQuick(register T_CanQueue *Queue, T_CanFrame *pData)
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

#if defined ( __ICCARM__ )
#define __inline
t_err CAN_QueueWriteQuick(register T_CanQueue *Queue, T_CanFrame *pData);
#endif

t_err CAN_SendFrame(T_CanFrame *pCanFrame);
void CAN_Config(CAN_TypeDef* CANx);
uint8_t Can_Send_Msg(CAN_TypeDef* CAN_Num, uint8_t* msg, uint8_t len, uint8_t id);
#endif //Rsing_Can.h
