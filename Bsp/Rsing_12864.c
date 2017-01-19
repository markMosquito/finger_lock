/**
  ******************************************************************************
  * @file    Rsing_12864.c
  * @author  Rsing
  * @version V1.0
  * @date    17-July-2014
  * @brief   Initializate the 12864.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "Rsing_12864.h"
#include <stdio.h>
#include <stdarg.h>
/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
static void HJ12864_GPIO_init(void);
static void LCD_WriteData(u8 Data);
static void LCD_delay(void);
static void LCD_delay_72us(void);
static void SendByteLCD(u8 WLCDData);
static void LCD_WriteCmd(u8 CMD);
static void LCD_WriteData(u8 Data);

/**
  * @brief  HJ12864ZW GPIO Configuration
  * @param  None					
  * @retval None
  */
static void HJ12864_GPIO_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
#if defined (STM32F10X_HD) || defined (STM32F10X_MD)
	//---------------指令/数据端口设置
	RCC_APB2PeriphClockCmd(RCC_GPIO_RS, ENABLE);      //打开端口时钟 
	GPIO_InitStructure.GPIO_Pin =  GPIO_RS_PIN;       // 使能端口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽复用输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //50M时钟速度
	GPIO_Init(GPIO_RS, &GPIO_InitStructure);
	
	//---------------读/写端口设置
	RCC_APB2PeriphClockCmd(RCC_GPIO_RW, ENABLE);      //打开端口时钟 
	GPIO_InitStructure.GPIO_Pin =  GPIO_RW_PIN;       // 使能端口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //标准输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //50M时钟速度
	GPIO_Init(GPIO_RW, &GPIO_InitStructure);
	
	//---------------使能端口设置
	RCC_APB2PeriphClockCmd(RCC_GPIO_EN, ENABLE);      //打开端口时钟 
	GPIO_InitStructure.GPIO_Pin =  GPIO_EN_PIN;       // 使能端口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //50M时钟速度
	GPIO_Init(GPIO_EN, &GPIO_InitStructure);
	
	//---------------并口/串口 PSB
    #if USE_PSB == 1
	RCC_APB2PeriphClockCmd(RCC_GPIO_PSB, ENABLE);      //打开端口时钟 
	GPIO_InitStructure.GPIO_Pin =  GPIO_PSB_PIN;       // 使能端口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //50M时钟速度
	GPIO_Init(GPIO_PSB, &GPIO_InitStructure);
    #endif
	
	//---------------RST
	RCC_APB2PeriphClockCmd(RCC_GPIO_RST, ENABLE);      //打开端口时钟 
	GPIO_InitStructure.GPIO_Pin =  GPIO_RST_PIN;       // 使能端口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //50M时钟速度
	GPIO_Init(GPIO_RST, &GPIO_InitStructure);
	
	#if PSB_SELECT == 1
	//---------------数据端口设置
	RCC_APB2PeriphClockCmd(RCC_GPIO_DATA, ENABLE);    //打开数据端口时钟 
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_DATA_0_PIN;   //DATA_0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //50M时钟速度
	GPIO_Init(GPIO_DATA_0, &GPIO_InitStructure);      //初始化端口
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_DATA_1_PIN;   //DATA_1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //50M时钟速度
	GPIO_Init(GPIO_DATA_1, &GPIO_InitStructure);      //初始化端口
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_DATA_2_PIN;   //DATA_2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //50M时钟速度
	GPIO_Init(GPIO_DATA_2, &GPIO_InitStructure);      //初始化端口
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_DATA_3_PIN;   //DATA_3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //50M时钟速度
	GPIO_Init(GPIO_DATA_3, &GPIO_InitStructure);      //初始化端口
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_DATA_4_PIN;   //DATA_4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //50M时钟速度
	GPIO_Init(GPIO_DATA_4, &GPIO_InitStructure);      //初始化端口
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_DATA_5_PIN;   //DATA_5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //50M时钟速度
	GPIO_Init(GPIO_DATA_5, &GPIO_InitStructure);      //初始化端口
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_DATA_6_PIN;   //DATA_6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //50M时钟速度
	GPIO_Init(GPIO_DATA_6, &GPIO_InitStructure);      //初始化端口
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_DATA_7_PIN;   //DATA_7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //50M时钟速度
	GPIO_Init(GPIO_DATA_7, &GPIO_InitStructure);      //初始化端口
	#endif
    
#elif defined (STM32F40_41xxx) || defined (STM32F40XX)
	//---------------指令/数据端口设置
	RCC_AHB1PeriphClockCmd(RCC_GPIO_RS, ENABLE);      //打开端口时钟 
	GPIO_InitStructure.GPIO_Pin =  GPIO_RS_PIN;       // 使能端口
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_Init(GPIO_RS, &GPIO_InitStructure);
	
	//---------------读/写端口设置
	RCC_AHB1PeriphClockCmd(RCC_GPIO_RW, ENABLE);      //打开端口时钟 
	GPIO_InitStructure.GPIO_Pin =  GPIO_RW_PIN;       // 使能端口
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_Init(GPIO_RW, &GPIO_InitStructure);
	
	//---------------使能端口设置
	RCC_AHB1PeriphClockCmd(RCC_GPIO_EN, ENABLE);      //打开端口时钟 
	GPIO_InitStructure.GPIO_Pin =  GPIO_EN_PIN;       // 使能端口
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_Init(GPIO_EN, &GPIO_InitStructure);
	
	//---------------并口/串口 PSB
    #if USE_PSB == 1
	RCC_AHB1PeriphClockCmd(RCC_GPIO_PSB, ENABLE);      //打开端口时钟 
	GPIO_InitStructure.GPIO_Pin =  GPIO_PSB_PIN;       // 使能端口
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_Init(GPIO_PSB, &GPIO_InitStructure);
	#endif
    
	//---------------RST
	RCC_AHB1PeriphClockCmd(RCC_GPIO_RST, ENABLE);      //打开端口时钟 
	GPIO_InitStructure.GPIO_Pin =  GPIO_RST_PIN;       // 使能端口
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_Init(GPIO_RST, &GPIO_InitStructure);
#endif
}

#if PSB_SELECT == 1
/**
  * @brief  输出8位数据到GPIO
  * @param  要输出的8位数据
  * @retval None
  */
static void Data_out(unsigned char x)                     //DATA输出
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_DATA_7_PIN;   //DATA7
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIO_DATA_7, &GPIO_InitStructure);      //打开忙检测端口
	
	GPIO_ResetBits(GPIO_DATA_0, GPIO_DATA_0_PIN);  //DATA0
	GPIO_ResetBits(GPIO_DATA_0, GPIO_DATA_1_PIN);  //DATA1
	GPIO_ResetBits(GPIO_DATA_0, GPIO_DATA_2_PIN);  //DATA2
	GPIO_ResetBits(GPIO_DATA_0, GPIO_DATA_3_PIN);  //DATA3
	GPIO_ResetBits(GPIO_DATA_0, GPIO_DATA_4_PIN);  //DATA4
	GPIO_ResetBits(GPIO_DATA_0, GPIO_DATA_5_PIN);  //DATA5
	GPIO_ResetBits(GPIO_DATA_0, GPIO_DATA_6_PIN);  //DATA6
	GPIO_ResetBits(GPIO_DATA_0, GPIO_DATA_7_PIN);  //DATA7
	if(x&0X01)GPIO_SetBits(GPIO_DATA_0, GPIO_DATA_0_PIN);//DATA0
	if(x&0X02)GPIO_SetBits(GPIO_DATA_0, GPIO_DATA_1_PIN);//DATA1
	if(x&0X04)GPIO_SetBits(GPIO_DATA_0, GPIO_DATA_2_PIN);//DATA2
	if(x&0X08)GPIO_SetBits(GPIO_DATA_0, GPIO_DATA_3_PIN);//DATA3
	if(x&0X10)GPIO_SetBits(GPIO_DATA_0, GPIO_DATA_4_PIN);//DATA4
	if(x&0X20)GPIO_SetBits(GPIO_DATA_0, GPIO_DATA_5_PIN);//DATA5
	if(x&0X40)GPIO_SetBits(GPIO_DATA_0, GPIO_DATA_6_PIN);//DATA6
	if(x&0X80)GPIO_SetBits(GPIO_DATA_0, GPIO_DATA_7_PIN);//DATA7
}

/**
  * @brief  检测LCD是否处于忙状态
  * @param  None					
  * @retval 忙返回1, 空闲返回0
  */
static u8 LCD_CheckBusy()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	u8 busy;
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_DATA_7_PIN;   //DATA7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;     //输入模式 = 上拉输入
	GPIO_Init(GPIO_DATA_7, &GPIO_InitStructure);      //打开忙检测端口

    RS_L;
    RW_H;
    EN_H;
    busy = GPIO_ReadInputDataBit(GPIO_DATA_7, GPIO_DATA_7_PIN);
    EN_L;

    return busy;
}


/**
  * @brief  写命令到12864
  * @param  8bit命令					
  * @retval None
  */
static void LCD_WriteCmd(u8 cmd)
{
    while( LCD_CheckBusy() );
    RS_L;
    RW_L;
    EN_L;
    Data_out(cmd);
    EN_H;
    EN_L;
}

/**
  * @brief  写数据到12864
  * @param  8bit数据					
  * @retval None
  */
static void LCD_WriteData(u8 data)
{
    while( LCD_CheckBusy() );
    RS_H;
    LCD_delay();
    RW_L;
    LCD_delay();
    EN_L;
    Data_out(data);
    EN_H;
    LCD_delay();
    EN_L;
    LCD_delay();
}
#else
static void LCD_delay(void)
{	
   u16 i = 0; 
   while(i) 
   { 
     i--; 
   } 
}
static void LCD_delay_72us(void)
{	
   u16 i = 5000; 
   while(i) 
   { 
     i--; 
   } 
}

static void SendByteLCD(u8 WLCDData)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		EN_L;
        LCD_delay();
		if((WLCDData<<i)&0x80) RW_H;
		else RW_L;
        LCD_delay();
		EN_H;
        LCD_delay();
	}
}

static void SPIWR(u8 Wdata,u8 WRS)
{
	SendByteLCD(0xf8+(WRS<<1));
	SendByteLCD(Wdata&0xf0);
	SendByteLCD((Wdata<<4)&0xf0);
    EN_L;
    LCD_delay();
}

static void LCD_WriteCmd(u8 CMD)
{
	RS_L;
    LCD_delay();
	RS_H;
	SPIWR(CMD,0);
    RS_L;
	LCD_delay_72us();
}
static void LCD_WriteData(u8 Data)
{
	RS_L;
    LCD_delay();
	RS_H;
	SPIWR(Data,1);
    RS_L;
	LCD_delay_72us();
}
#endif
/* Exported functios --------------------------------------------------------*/
/**
  * @brief  初始化LCD-8位接口
  * @param  None					
  * @retval None
  */
void LCD_Configuration(void)
{
	HJ12864_GPIO_init();
    #if USE_PSB == 1
        #if PSB_SELECT == 1
        PSB_H;//并口
        #else
        PSB_L;//串口
        #endif
    #endif
	RST_H;
	EN_L;
	RST_L;
    LCD_delay_72us();
	RST_H;
    LCD_delay_72us();
	LCD_WriteCmd(0x30); //基本指令集,8位并行
    LCD_delay_72us();
	LCD_WriteCmd(0x06); //启始点设定：光标右移
    LCD_delay_72us();
	LCD_WriteCmd(0x01); //清除显示DDRAM
    LCD_delay_72us();
	LCD_WriteCmd(0x0C); //显示状态开关：整体显示开，光标显示关，光标显示反白关
    LCD_delay_72us();
	LCD_WriteCmd(0x02); //地址归零
    LCD_delay_72us();
}

/**
  * @brief  设置坐标
  * @param  x, y					
  * @retval None
  */
void LCD_SetPosition(u8 x, u8 y)
{
	u8 p;
    switch(x%4)
    {
        case 0: p = 0x80; break; //第一行开始地址
        case 1: p = 0x90; break; //第二行
        case 2: p = 0x88; break; //第三行
        case 3: p = 0x98; break; //第四行
    }
    p += y;
    LCD_WriteCmd(p);
}

/**
  * @brief  显示字符
  * @param  str
  * @retval None
  */
void LCD_WriteString(char * str)
{
	u8 i = 0;
    while(str[i] != '\0')
    {
        LCD_WriteData(str[i++]);
    }
}

/**
  * @brief  在指定位置显示字符
  * @param  x, y, str
  * @retval None
  */
void LCD_Show(u8 x, u8 y, char * str)
{
	u8 p,i =0;
    switch(x%4)
    {
        case 0: p = 0x80; break; //第一行开始地址
        case 1: p = 0x90; break; //第二行
        case 2: p = 0x88; break; //第三行
        case 3: p = 0x98; break; //第四行
    }
    p += y;
    LCD_WriteCmd(p);
    while(str[i] != '\0')
    {
        LCD_WriteData(str[i++]);
    }
}

/**
  * @brief  在指定位置显示指定宽度字符
  * @param  x, y, str
  * @retval None
  */
void LCD_ShowWidth(u8 x, u8 y, u8 width, char * str)
{
	u8 p,i =0;
    switch(x%4)
    {
        case 0: p = 0x80; break; //第一行开始地址
        case 1: p = 0x90; break; //第二行
        case 2: p = 0x88; break; //第三行
        case 3: p = 0x98; break; //第四行
    }
    p += y;
    LCD_WriteCmd(p);
    while(str[i] != '\0' && i < width)
    {
        LCD_WriteData(str[i++]);
    }
    while(i < width)
    {
        LCD_WriteData(' ');
        i++;
    }
}

/**
  * @brief  清屏
  * @param  None
  * @retval None
  */
void LCD_Clear(void) 
{ 
	LCD_WriteCmd(0x01); 
	LCD_WriteCmd(0x34); 
	LCD_WriteCmd(0x30); 
}

/**
  * @brief  写入GDRAM 绘图
  * @param  2个字节一行,CLONG是图形长度,以字节为单位;HIGHT是图形高度,TAB是图形数据表.
  * @retval None
  */
//void LCD_WRGDRAM(u8 x, u8 clong, u8 hight, char *TAB)
//{
//	u16 k;
//	u8 i,j;
//	LCD_WriteCmd(0x34);
//	LCD_WriteCmd(0x36);
//	for(j = 0; j < hight; j++)//32
//	{
//		//先上半屏
//		LCD_WriteCmd(0x80 + j);   //Y总坐标,即第几行
//		LCD_WriteCmd(0x80 + x);   //X坐标，即横数第几个字节开始写起
//		for(i = 0; i < clong; i++)
//		{
//			LCD_WriteData(TAB[clong*j+i]);
//		}
//		//后下半屏
//		for(k = 0; k < clong; k++)
//		{
//			LCD_WriteData(TAB[clong*(j+hight)+k]);
//		}
//	}
//}

//x:行数 y:列数
void LCD_Printf(uint16_t Xpos, uint16_t Ypos, const char *fmt,...)
{
	static char LCD_BUF[128]; 
    uint8_t p =0;
    
    va_list ap;
    va_start(ap,fmt);
	vsprintf((char *)LCD_BUF,fmt,ap);
	va_end(ap);
	
    switch(Xpos%4)
    {
        case 0: p = 0x80; break; //第一行开始地址
        case 1: p = 0x90; break; //第二行
        case 2: p = 0x88; break; //第三行
        case 3: p = 0x98; break; //第四行
    }
    p += Ypos;
    LCD_WriteCmd(p);
    LCD_WriteString((char*)LCD_BUF);
}

/*********************************END OF FILE**********************************/
