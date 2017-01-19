#ifndef __OLED_H
#define __OLED_H			  	 
#include "sys.h"
#include "stdlib.h"	
#include "stm32f2xx.h"
#include "Redef_GPIO.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//SSD1306 OLED 驱动IC驱动代码
//驱动方式:8080并口/4线串口
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/6/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  

#define IO_OLED_DC           PB(12)
#define IO_OLED_CS           PA(1)
#define IO_OLED_SCLK         PB(13)
#define IO_OLED_SDIN         PB(15)
#define IO_OLED_RST          PB(14)

#define OLED_DC_H  GPIO_SetBits(IO_GPIO_PORT(IO_OLED_DC), IO_GPIO_PIN(IO_OLED_DC))
#define OLED_DC_L  GPIO_ResetBits(IO_GPIO_PORT(IO_OLED_DC), IO_GPIO_PIN(IO_OLED_DC))
#define OLED_CS_H  GPIO_SetBits(IO_GPIO_PORT(IO_OLED_CS), IO_GPIO_PIN(IO_OLED_CS))
#define OLED_CS_L  GPIO_ResetBits(IO_GPIO_PORT(IO_OLED_CS), IO_GPIO_PIN(IO_OLED_CS))
#define OLED_SCLK_H  GPIO_SetBits(IO_GPIO_PORT(IO_OLED_SCLK), IO_GPIO_PIN(IO_OLED_SCLK))
#define OLED_SCLK_L  GPIO_ResetBits(IO_GPIO_PORT(IO_OLED_SCLK), IO_GPIO_PIN(IO_OLED_SCLK))
#define OLED_SDIN_H  GPIO_SetBits(IO_GPIO_PORT(IO_OLED_SDIN), IO_GPIO_PIN(IO_OLED_SDIN))
#define OLED_SDIN_L  GPIO_ResetBits(IO_GPIO_PORT(IO_OLED_SDIN), IO_GPIO_PIN(IO_OLED_SDIN))
#define OLED_RST_H  GPIO_SetBits(IO_GPIO_PORT(IO_OLED_RST), IO_GPIO_PIN(IO_OLED_RST))
#define OLED_RST_L  GPIO_ResetBits(IO_GPIO_PORT(IO_OLED_RST), IO_GPIO_PIN(IO_OLED_RST))

//OLED模式设置
//0:4线串行模式
//1:并行8080模式
#define OLED_MODE 0
		    						  
//-----------------OLED端口定义----------------  					   
#define OLED_CS PCout(9)
#define OLED_RST  PBout(14)//在MINISTM32上直接接到了STM32的复位脚！	
#define OLED_RS PCout(8)
#define OLED_WR PCout(7)		  
#define OLED_RD PCout(6)

//PB0~7,作为数据线
#define DATAOUT(x) GPIOB->ODR=(GPIOB->ODR&0xff00)|(x&0x00FF); //输出
  
//使用4线串行接口时使用 
#define OLED_SCLK PBout(0)
#define OLED_SDIN PBout(1)
		     
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据
//OLED控制用函数
void OLED_WR_Byte(u8 dat,u8 cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);		   
							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size,u8 mode);
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 mode);	
void OLED_ShowString_Scroll(u8 x,u8 y,const u8 *p, u8 mode);
void OLED_ShowAst(u8 x, u8 y, int num);
void OLED_ShowString_CH(u16 x,u16 y,const u8 *p,u8 mode);
void OLED_ShowChineseChar(u16 x,u16 y,const u8 *c,u8 mode);
#endif  
	 



