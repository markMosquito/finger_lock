/*
    FreeRTOS V8.0.1 - Copyright (C) 2014 Real Time Engineers Ltd. 
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that has become a de facto standard.             *
     *                                                                       *
     *    Help yourself get started quickly and support the FreeRTOS         *
     *    project by purchasing a FreeRTOS tutorial book, reference          *
     *    manual, or both from: http://www.FreeRTOS.org/Documentation        *
     *                                                                       *
     *    Thank you!                                                         *
     *                                                                       *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available from the following
    link: http://www.freertos.org/a00114.html

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org - Documentation, books, training, latest versions,
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
    Integrity Systems to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
	BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER FOR UART0.
*/
#include <stdio.h>
/* Scheduler includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

/* Library includes. */
#include "stm32f4xx.h"

/* Demo application includes. */
#include "serial.h"
#include "Redef_GPIO.h"
/*-----------------------------------------------------------*/

/* Misc defines. */
#define serINVALID_QUEUE				( ( QueueHandle_t ) 0 )
#define serNO_BLOCK						( ( TickType_t ) 0 )
#define serTX_BLOCK_TIME				( 40 / portTICK_PERIOD_MS )

/*-----------------------------------------------------------*/

/* The queue used to hold received characters. */
static QueueHandle_t xRxedChars;
static QueueHandle_t xCharsForTx;

/*-----------------------------------------------------------*/
#ifndef vUARTInterruptHandler
#error "vUARTInterruptHandler is not define, please define it in 'FreeRTOSConfig'"
#endif

/* UART interrupt handler. */
void vUARTInterruptHandler( void );

/*-----------------------------------------------------------*/
static uint8_t g_ui8SerialPort = 0;

USART_TypeDef* USART_Num[4]=
{
  USART1, USART2, USART3, USART6
};


#define USART1_TX   PA(9)   //PA(9), PB(6)
#define USART1_RX   PA(10)  //PA(10),PB(7)

#define USART2_TX   PA(2)   //PA(2), PD(5)
#define USART2_RX   PA(3)   //PA(3), PD(6)

#define USART3_TX   PB(10)  //PB(10),PC(10),PD(8)
#define USART3_RX   PB(11)  //PB(11),PC(11),PD(9)

#define USART6_TX   PC(6)   //PC(6),PG(10)
#define USART6_RX   PC(7)   //PC(7),PG(9)

uint16_t USART_TX[4]=
{
  USART1_TX,
  USART2_TX,
  USART3_TX,
  USART6_TX
};

uint16_t USART_RX[4]=
{
  USART1_RX,
  USART2_RX,
  USART3_RX,
  USART6_RX
};

//RCC_APB1/2_Periph_USART
uint32_t RCC_APBX_PERIPH_USART[4]=
{
  RCC_APB2Periph_USART1,
  RCC_APB1Periph_USART2,
  RCC_APB1Periph_USART3,
  RCC_APB2Periph_USART6
};

//GPIO_AF_USART
uint8_t GPIO_AF_USART[4]=
{
  GPIO_AF_USART1,
  GPIO_AF_USART2,
  GPIO_AF_USART3,
  GPIO_AF_USART6
};

//USART_IRQn
uint8_t USART_IRQN[4]=
{
  USART1_IRQn,
  USART2_IRQn,
  USART3_IRQn,
  USART6_IRQn
};

xComPortHandle xSerialPortInit( eCOMPort Port, uint32_t Baud, unsigned portBASE_TYPE uxBufferLength )
{
  xComPortHandle xReturn;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  #if (\
      (USART1_TX == PA(9) && USART1_RX == PA(10)) ||\
      (USART1_TX == PB(6) && USART1_RX == PB(7))  ||\
      \
      (USART2_TX == PA(2) && USART2_RX == PA(3))  ||\
      (USART2_TX == PD(5) && USART2_RX == PD(6))  ||\
      \
      (USART3_TX == PB(10) && USART3_RX == PB(11))||\
      (USART3_TX == PC(10) && USART3_RX == PC(11))||\
      (USART3_TX == PD(8)  && USART3_RX == PD(9)) ||\
      \
      (USART6_TX == PC(6)  && USART6_RX == PC(7)) ||\
      (USART6_TX == PG(10) && USART6_RX == PG(9))   \
      )
  #else
    #error "USART TX/RX GPIO Config Error!"
  #endif
  
  /* Create the queues used to hold Rx/Tx characters. */
	xRxedChars = xQueueCreate( uxBufferLength, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
	xCharsForTx = xQueueCreate( uxBufferLength + 1, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
  
  /* If the queue/semaphore was created correctly then setup the serial port
	hardware. */
	if( ( xRxedChars != serINVALID_QUEUE ) && ( xCharsForTx != serINVALID_QUEUE ) )
	{
    g_ui8SerialPort = Port;
    
    /* USART clock enable */
    if(Port == COM1 || Port == COM6)
      RCC_APB2PeriphClockCmd(RCC_APBX_PERIPH_USART[Port] , ENABLE);
    else
      RCC_APB1PeriphClockCmd(RCC_APBX_PERIPH_USART[Port] , ENABLE);

    //--------------------------------------------------------------
    /* GPIO clock enable */
    RCC_AHB1PeriphClockCmd(IO_RCC_AHB1_PERIPH(USART_TX[Port]), ENABLE);
    
    /* Configure USART Tx () */
    GPIO_InitStructure.GPIO_Pin = IO_GPIO_PIN(USART_TX[Port]);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

    GPIO_Init( IO_GPIO_PORT(USART_TX[Port]), &GPIO_InitStructure );

    /* Configure USART Rx () */
    GPIO_InitStructure.GPIO_Pin = IO_GPIO_PIN(USART_RX[Port]);
    GPIO_Init( IO_GPIO_PORT(USART_RX[Port]), &GPIO_InitStructure );
    
    GPIO_PinAFConfig(IO_GPIO_PORT(USART_TX[Port]), IO_GPIO_PINSOURCE(USART_TX[Port]), GPIO_AF_USART[Port]);
    GPIO_PinAFConfig(IO_GPIO_PORT(USART_RX[Port]), IO_GPIO_PINSOURCE(USART_RX[Port]), GPIO_AF_USART[Port]);
      
    USART_InitStructure.USART_BaudRate = Baud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
    USART_Init( USART_Num[Port], &USART_InitStructure );
    
    USART_ITConfig( USART_Num[Port], USART_IT_RXNE, ENABLE );
    
    NVIC_InitStructure.NVIC_IRQChannel = USART_IRQN[Port];
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &NVIC_InitStructure );
    
    USART_Cmd( USART_Num[Port], ENABLE );
  }
  else
  {
    xReturn = ( xComPortHandle ) 0;
  }
  return xReturn;
}

/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialGetChar( xComPortHandle pxPort, signed char *pcRxedChar, TickType_t xBlockTime )
{
	/* The port handle is not required as this driver only supports one port. */
	( void ) pxPort;

	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	if( xQueueReceive( xRxedChars, pcRxedChar, xBlockTime ) )
	{
		return pdTRUE;
	}
	else
	{
		return pdFALSE;
	}
}
/*-----------------------------------------------------------*/

void vSerialPutString( xComPortHandle pxPort, const signed char * const pcString, unsigned short usStringLength )
{
signed char *pxNext;

	/* A couple of parameters that this port does not use. */
	( void ) usStringLength;
	( void ) pxPort;

	/* NOTE: This implementation does not handle the queue being full as no
	block time is used! */

	/* The port handle is not required as this driver only supports UART1. */
	( void ) pxPort;

	/* Send each character in the string, one at a time. */
	pxNext = ( signed char * ) pcString;
	while( *pxNext )
	{
		xSerialPutChar( pxPort, *pxNext, serNO_BLOCK );
		pxNext++;
	}
}
/*-----------------------------------------------------------*/

//signed portBASE_TYPE xSerialPutChar( xComPortHandle pxPort, signed char cOutChar, TickType_t xBlockTime )
//{
//signed portBASE_TYPE xReturn;

//	if( xQueueSend( xCharsForTx, &cOutChar, xBlockTime ) == pdPASS )
//	{
//		xReturn = pdPASS;
//		USART_ITConfig( USART_Num[g_ui8SerialPort], USART_IT_TXE, ENABLE );
//	}
//	else
//	{
//		xReturn = pdFAIL;
//	}

//	return xReturn;
//}
extern xQueueHandle xCharsForTx3;
signed portBASE_TYPE xSerialPutChar( xComPortHandle pxPort, signed char cOutChar, TickType_t xBlockTime )
{
signed portBASE_TYPE xReturn;

	if( xQueueSend( xCharsForTx, &cOutChar, xBlockTime ) == pdPASS )
	{
		xReturn = pdPASS;
		USART_ITConfig( USART3, USART_IT_TXE, ENABLE );
	}
	else
	{
		xReturn = pdFAIL;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
void uart_putbuff(unsigned char *buff, unsigned int len)
{
  while(len--)
  {
    xSerialPutChar( 0, *buff, serNO_BLOCK );
    buff++;
  }
}
/*-----------------------------------------------------------*/

void vSerialClose( xComPortHandle xPort )
{
	/* Not supported as not required by the demo application. */
}
/*-----------------------------------------------------------*/
#if COM3_DBG != COM_RELEASE
void vUARTInterruptHandler( void )
{
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
char cChar;

	if( USART_GetITStatus( USART_Num[g_ui8SerialPort], USART_IT_TXE ) == SET )
	{
		/* The interrupt was caused by the THR becoming empty.  Are there any
		more characters to transmit? */
		if( xQueueReceiveFromISR( xCharsForTx, &cChar, &xHigherPriorityTaskWoken ) == pdTRUE )
		{
			/* A character was retrieved from the queue so can be sent to the
			THR now. */
			USART_SendData( USART_Num[g_ui8SerialPort], cChar );
		}
		else
		{
			USART_ITConfig( USART_Num[g_ui8SerialPort], USART_IT_TXE, DISABLE );		
		}		
	}
	
	if( USART_GetITStatus( USART_Num[g_ui8SerialPort], USART_IT_RXNE ) == SET )
	{
		cChar = USART_ReceiveData( USART_Num[g_ui8SerialPort] );
		xQueueSendFromISR( xRxedChars, &cChar, &xHigherPriorityTaskWoken );
	}
	
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}
#endif

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
//int fputc(int ch, FILE *f)
//{
//  /* Place your implementation of fputc here */
////  xSerialPutChar( 0, (uint16_t) ch, serNO_BLOCK );
//    while((USART3->SR&0X40)==0);//循环发送,直到发送完毕   
//    USART3->DR = (u8) ch; 
//    return ch;
//}


//#include <stdarg.h>
//void RsPrintf(const char *fmt,...)
//{
//    signed char *pxNext;
//    char LCD_BUF[128]; 

//    va_list ap;
//    va_start(ap,fmt);
//    vsprintf((char *)LCD_BUF,fmt,ap);
//    va_end(ap);
//    
//	/* Send each character in the string, one at a time. */
//	pxNext = ( signed char * ) LCD_BUF;
//	while( *pxNext )
//	{
//        xQueueSend( xCharsForTx, pxNext, serNO_BLOCK );
//		pxNext++;
//	}
//    USART_ITConfig( USART_Num[g_ui8SerialPort], USART_IT_TXE, ENABLE );
//}

//void RsPrintfFromISR(const char *fmt,...)
//{
//    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
//    signed char *pxNext;
//    char LCD_BUF[128] = {0}; 
//    
//    va_list ap;
//    va_start(ap,fmt);
//    vsprintf((char *)LCD_BUF,fmt,ap);
//    va_end(ap);
//    
//	/* Send each character in the string, one at a time. */
//	pxNext = ( signed char * ) LCD_BUF;
//	while( *pxNext )
//	{
//        xQueueSendFromISR( xCharsForTx, pxNext, &xHigherPriorityTaskWoken );
//		pxNext++;
//	}
//    USART_ITConfig( USART_Num[g_ui8SerialPort], USART_IT_TXE, ENABLE );
//}


void simpleSerialPortInit( eCOMPort Port, uint32_t Baud)
{
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  #if (\
      (USART1_TX == PA(9) && USART1_RX == PA(10)) ||\
      (USART1_TX == PB(6) && USART1_RX == PB(7))  ||\
      \
      (USART2_TX == PA(2) && USART2_RX == PA(3))  ||\
      (USART2_TX == PD(5) && USART2_RX == PD(6))  ||\
      \
      (USART3_TX == PB(10) && USART3_RX == PB(11))||\
      (USART3_TX == PC(10) && USART3_RX == PC(11))||\
      (USART3_TX == PD(8)  && USART3_RX == PD(9)) ||\
      \
      (USART6_TX == PC(6)  && USART6_RX == PC(7)) ||\
      (USART6_TX == PG(10) && USART6_RX == PG(9))   \
      )
  #else
    #error "USART TX/RX GPIO Config Error!"
  #endif

    /* USART clock enable */
    if(Port == COM1 || Port == COM6)
      RCC_APB2PeriphClockCmd(RCC_APBX_PERIPH_USART[Port] , ENABLE);
    else
      RCC_APB1PeriphClockCmd(RCC_APBX_PERIPH_USART[Port] , ENABLE);

    //--------------------------------------------------------------
    /* GPIO clock enable */
    RCC_AHB1PeriphClockCmd(IO_RCC_AHB1_PERIPH(USART_TX[Port]), ENABLE);
    
    /* Configure USART Tx () */
    GPIO_InitStructure.GPIO_Pin = IO_GPIO_PIN(USART_TX[Port]);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

    GPIO_Init( IO_GPIO_PORT(USART_TX[Port]), &GPIO_InitStructure );

    /* Configure USART Rx () */
    GPIO_InitStructure.GPIO_Pin = IO_GPIO_PIN(USART_RX[Port]);
    GPIO_Init( IO_GPIO_PORT(USART_RX[Port]), &GPIO_InitStructure );
    
    GPIO_PinAFConfig(IO_GPIO_PORT(USART_TX[Port]), IO_GPIO_PINSOURCE(USART_TX[Port]), GPIO_AF_USART[Port]);
    GPIO_PinAFConfig(IO_GPIO_PORT(USART_RX[Port]), IO_GPIO_PINSOURCE(USART_RX[Port]), GPIO_AF_USART[Port]);
      
    USART_InitStructure.USART_BaudRate = Baud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
    USART_Init( USART_Num[Port], &USART_InitStructure );
    
    USART_Cmd( USART_Num[Port], ENABLE );
}

void simpleSerialPortClose(eCOMPort Port)
{
    USART_Cmd( USART_Num[Port], DISABLE );
}


#include <stdarg.h>
void simpleSerialPortPrintf(eCOMPort Port, const char *fmt,...)
{
    uint32_t index = 0;
    char PRINT_BUF[128];
    char *pxNext;

    
    va_list ap;
    va_start(ap,fmt);
	vsprintf((char *)PRINT_BUF,fmt,ap);
	va_end(ap);
	
    
	pxNext = ( char * ) PRINT_BUF;
    while(*pxNext != '\0' && index++ < 128)
    {
        while(USART_GetFlagStatus(USART_Num[Port], USART_FLAG_TXE) == RESET);
        USART_Num[Port]->DR = *pxNext;
        pxNext++;
    }
}
