#ifndef __RTC_H
#define	__RTC_H


#include "stm32f2xx.h"

/* ������������������Ļ�,PC13�ͻ����Ƶ��ΪRTC Clock/64��ʱ�� */   
//#define RTCClockOutput_Enable  /* RTC Clock/64 is output on tamper pin(PC.13) */

void RTC_main(void);

void NVIC_Configuration(void);
void RTC_Configuration(void);
uint32_t Time_Regulate(void);
void Time_Adjust(void);
void Time_Display(uint32_t TimeVar);
void Time_Show(void);
uint8_t USART_Scanf(uint32_t value);

#endif /* __XXX_H */