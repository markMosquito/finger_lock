/**
  ******************************************************************************
  * @file    Rsing_I2C.c
  * @author  Rsing
  * @version V1.0
  * @date    10-August-2015
  * @brief   Initializate the I2C.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "Rsing_I2C.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Base_Fun.h"
#include "math.h"
#include "mpr121.h"
#include "TSM12.h"
#define TRUE    1
#define FALSE   0


#define MPR_ADDR 0x5A


#define Delay_ms(tims_ms)       vTaskDelay( ( TickType_t )tims_ms * portTICK_MS)
/* Private variables ---------------------------------------------------------*/


#define  I2C_Direction_Transmitter      ((uint8_t)0x00)
#define  I2C_Direction_Receiver         ((uint8_t)0x01)
/* Private functions ---------------------------------------------------------*/

void mpr121Write(uint8_t reg_addr, uint8_t reg_data)
{
	I2C_WriteByte(MPR_ADDR,reg_addr,reg_data);
}
u8 mpr121Read(uint8_t reg_addr)
{
	return I2C_ReadByte(MPR_ADDR,reg_addr);
}

void TSM_Write_Byte(uint8_t reg_addr, uint8_t reg_data)
{
	I2C_WriteByte(TSM_ADDR,reg_addr,reg_data);
}

/**
  * @brief  I2C_Config
  * @param  None
  * @retval None
  */
void I2C_Configuration(void)
{
  
	MyGPIOInit(IO_GPIO_PORT(IO_I2C_SCL), IO_GPIO_PIN(IO_I2C_SCL), GPIO_Mode_OUT, 
							GPIO_Speed_100MHz, GPIO_OType_OD, GPIO_PuPd_NOPULL);	
	MyGPIOInit(IO_GPIO_PORT(IO_I2C_SDA), IO_GPIO_PIN(IO_I2C_SDA), GPIO_Mode_OUT, 
							GPIO_Speed_100MHz, GPIO_OType_OD, GPIO_PuPd_NOPULL);

	MyGPIOInit(IO_GPIO_PORT(IO_I2C_IRQ), IO_GPIO_PIN(IO_I2C_IRQ), GPIO_Mode_IN, 
							GPIO_Speed_100MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL);
	
//	MyGPIOInit(IO_GPIO_PORT(IO_I2C_RST), IO_GPIO_PIN(IO_I2C_RST), GPIO_Mode_OUT, 
//							GPIO_Speed_100MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL);
//	
//	MyGPIOInit(IO_GPIO_PORT(IO_I2C_EN), IO_GPIO_PIN(IO_I2C_EN), GPIO_Mode_OUT, 
//							GPIO_Speed_100MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL);
	
	I2C_DISABLE;
	Delay_ms(200);
	SCL_H;
	SDA_H;
	
	I2C_RST_H;
	Delay_ms(50);
	I2C_RST_L;
	Delay_ms(100);
	
	I2C_ENABLE;
	Delay_ms(5);
	TSM_Write_Byte(CTRL2, 0x0f);
	TSM_Write_Byte(CTRL2, 0x03);//enable sleep mode
	TSM_Write_Byte(SENS1, CH_SENS(3,3));// 9, 6
	TSM_Write_Byte(SENS2, CH_SENS(3,3));// 3, #
	TSM_Write_Byte(SENS3, CH_SENS(3,3));// 8, 5
	TSM_Write_Byte(SENS4, CH_SENS(3,3));// 2, 0
	TSM_Write_Byte(SENS5, CH_SENS(3,3));// *, 1
	TSM_Write_Byte(SENS6, CH_SENS(3,3));// 4, 7
	
	TSM_Write_Byte(CTRL1, 0x1b);
	
	TSM_Write_2Byte(REF_RST1, 0x00, 0x00);//renew value of everyone channel
	TSM_Write_2Byte(CH_HOLD1, 0x00, 0x00);//enable all channels
	
	
}
int get_key(void)
{
	//return I2C_ReadByte(MPR_ADDR,0x5D);
	I2C_ReadByte(MPR_ADDR,0x01) ;
	return I2C_ReadByte(MPR_ADDR,0x00) ;
}
/**
  * @brief  Simulation IIC Timing series delay
  * @param  None
  * @retval None
  */
static void I2C_delay(void)
{
   u8 i=30;//��������Ż��ٶȣ���������͵�5����д��
   while(i)
   {
     i--;
   }
}


/**
  * @brief  Master Start Simulation IIC Communication
  * @param  None
  * @retval Wheather Start
  */
u8 I2C_Start(void)//SCLΪ��ʱ��SDA�Ӹ߱�ͼ�Ϊstart
{
	SDA_H;
    I2C_delay();
	SCL_H;
	I2C_delay();
	if(!SDA_read)
		return FALSE;	//SDA��Ϊ�͵�ƽ������æ,�˳�
	SDA_L;
	I2C_delay();
	if(SDA_read)
		return FALSE;	//SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
	SCL_L;
	I2C_delay();
	return TRUE;
}

/**
  * @brief  Master Stop Simulation IIC Communication
  * @param  None
  * @retval None
  */
void I2C_Stop(void)
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;
	I2C_delay();
}


/**
  * @brief  Master Send Acknowledge Single
  * @param  None
  * @retval None
  */
void I2C_Ack(void)
{	
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

/**
  * @brief  Master Send No Acknowledge Single
  * @param  None
  * @retval None
  */
void I2C_NoAck(void)
{	
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

/**
  * @brief  Master Reserive Slave Acknowledge Single
  * @param  None
  * @retval Wheather	 Reserive Slave Acknowledge Single
  */
u8 I2C_WaitAck(void) 	 //����Ϊ:=1��ACK,=0��ACK
{
	SCL_L;//Ӧ�����������SDA���ߣ�Ȼ������Ӧ�ڼ䣬��������SDA���͡�
	I2C_delay();
	SDA_H;			
	I2C_delay();
	SCL_H;
	I2C_delay();

	if(SDA_read)
	{
        SCL_L;
        I2C_delay();
        return FALSE;
	}

	SCL_L;
	return TRUE;
}


/**
  * @brief  Master Send a Byte to Slave
  * @param  Will Send Date
  * @retval None
  */
void I2C_SendByte(u8 SendByte) //���ݴӸ�λ����λ//
{
    u8 i=8;
    while(i--)
    {
        SCL_L;
        I2C_delay();
        if(SendByte&0x80)
            SDA_H;  
        else 
            SDA_L;
        SendByte <<= 1;
        I2C_delay();
        SCL_H;
        I2C_delay();
    }
    SCL_L;
}

/**
  * @brief  Master Reserive a Byte From Slave
  * @param  None
  * @retval Date From Slave 
  */
u8 I2C_ReceiveByte(void)  //���ݴӸ�λ����λ//
{ 
    u8 i=8;
    u8 ReceiveByte=0;

    SDA_H;
    while(i--)
    {
      ReceiveByte<<=1;      
      SCL_L;
      I2C_delay();
      SCL_H;
      I2C_delay();	
      if(SDA_read)
      {
        ReceiveByte|=0x01;
      }
    }
    SCL_L;
    return ReceiveByte;
}

/**
  * @brief  Single Byte Write
  * @param  None
  * @retval Wheather Write
  */
uint32_t i2c_error_count = 0;
uint8_t I2C_WriteByte(uint8_t slave_addr, uint8_t reg_addr, uint8_t reg_data)
{
	
  	if(!I2C_Start())
        return FALSE;
    I2C_SendByte(slave_addr << 1 | I2C_Direction_Transmitter);   //�����豸��ַ+д�ź�
    if(!I2C_WaitAck())
    {
      i2c_error_count++;
      I2C_Stop();
      return FALSE;
    }
    I2C_SendByte(reg_addr);   //���õ���ʼ��ַ
    I2C_WaitAck();	
    I2C_SendByte(reg_data);
    I2C_WaitAck();   
    I2C_Stop(); 
	
    return TRUE;
}

/**
  * @brief  ��I2C�豸��ȡһ���ֽ�����
  * @param  None
  * @retval ��ȡ��������
  */
uint8_t I2C_ReadByte(uint8_t slave_addr, uint8_t reg_addr)
{
    u8 REG_data;   
	
    if(!I2C_Start())
        return FALSE;
    I2C_SendByte(slave_addr << 1 | I2C_Direction_Transmitter);
    if(!I2C_WaitAck())
    {
        I2C_Stop();
        return FALSE;
    }
    I2C_SendByte(reg_addr);   //���õ���ʼ��ַ      
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(slave_addr << 1 | I2C_Direction_Receiver);
    I2C_WaitAck();

    REG_data= I2C_ReceiveByte();
    I2C_NoAck();
    I2C_Stop();
    //return TRUE;
    return REG_data;
}

/**
  * @brief  ��I2C�豸д�����ֽ�����
  * @param  None
  * @retval Wheather Write
  */
uint8_t I2C_WriteBuf(uint8_t slave_addr,
                    uint8_t reg_addr,
                    uint16_t len,
                    uint8_t *data_ptr)
{
    unsigned char i=0;
    I2C_Start();                  //��ʼ�ź�
    I2C_SendByte(slave_addr << 1 | I2C_Direction_Transmitter);//7λ�����ӵ�ַ+��дλ
    if (!I2C_WaitAck()) 
    {
        I2C_Stop();
        return FALSE;
    }
    I2C_SendByte(reg_addr);    //�ڲ��Ĵ�����ַ
    I2C_WaitAck();
    for(i = 0; i < len; i++)
    {
        I2C_SendByte(*data_ptr);
        if (!I2C_WaitAck()) 
        {
          I2C_Stop();
          return FALSE;
        }
        data_ptr++;
    }
    I2C_Stop();                   //����ֹͣ�ź�
    return TRUE;
}

/**
  * @brief  ��I2C�豸��ȡ����ֽ�����
  * @param  None
  * @retval �Ƿ��ȡ������
  */
uint8_t I2C_ReadBuf(uint8_t slave_addr,
                    uint8_t reg_addr,
                    uint16_t len, 
                    uint8_t *data_ptr)
{
    if (!I2C_Start())
        return FALSE;
    I2C_SendByte(slave_addr << 1 | I2C_Direction_Transmitter);
    if (!I2C_WaitAck())
    {
        I2C_Stop();
        return FALSE;
    }
    I2C_SendByte(reg_addr);
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(slave_addr << 1 | I2C_Direction_Receiver);
    I2C_WaitAck();
    while (len)
    {
        *data_ptr = I2C_ReceiveByte();
        if (len == 1)
            I2C_NoAck();
        else
            I2C_Ack();
        data_ptr++;
        len--;
    }
    I2C_Stop();
    return TRUE;
}



uint8_t I2C_ReadByte2Point(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data)
{
    u8 REG_data;     	
    if(!I2C_Start())
        return FALSE;
    I2C_SendByte(slave_addr << 1 | I2C_Direction_Transmitter); //I2C_SendByte(((REG_Address & 0x0700) >>7) | REG_Address & 0xFFFE);//���ø���ʼ��ַ+������ַ 
    if(!I2C_WaitAck())
    {
        I2C_Stop();
        return FALSE;
    }
    I2C_SendByte(reg_addr);   //���õ���ʼ��ַ      
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(slave_addr << 1 | I2C_Direction_Receiver);
    I2C_WaitAck();

    REG_data= I2C_ReceiveByte();
    I2C_NoAck();
    I2C_Stop();
    *data = REG_data;
    return TRUE;
    //return REG_data;
}

/*----------------------------------------------------------------------*/
//����ֵ 0��д�ɹ�
//		-1��дʧ��
int IIC_Write(uint8_t addr, uint8_t reg, uint16_t len, uint8_t* data)
{
	if(I2C_WriteBuf(addr, reg, len, data) != FALSE)
		return 0;
	else
		return -1;
}

//����ֵ 0�����ɹ�
//		-1����ʧ��
int IIC_Read(uint8_t addr, uint8_t reg, uint16_t len, uint8_t *buf)
{
	if(I2C_ReadBuf(addr, reg, len, buf) != FALSE)
		return 0;
	else
		return -1;
}

u8 TSM_Write_2Byte(u8 WriteAddr, u8 WriteData1, u8 WriteData2)
{
	/* ????? */
	I2C_Start();
	/* ?????? */
	I2C_SendByte(TSM_ADDR << 1 | I2C_Direction_Transmitter);
	if(!I2C_WaitAck()) 
		return ERROR;
	/* ??????????????? */
	I2C_SendByte(WriteAddr);
	if(!I2C_WaitAck()) 
		return ERROR;
	/* ????1?????,?LSB */
	I2C_SendByte(WriteData1);
	if(!I2C_WaitAck()) 
		return ERROR;
	/* ????2?????,?MSB */
	I2C_SendByte(WriteData2);
	if(!I2C_WaitAck()) 
		return ERROR;
	/* ????? */
	I2C_Stop();
	return SUCCESS;
}
u8 TSM_ReadOneByte(u8 writeAddr)
{
	u8 buffer = 0;
	//????
	/* ????? */
	if (!I2C_Start())
		return FALSE;
	/* ?????? */
	I2C_SendByte(TSM_ADDR << 1 | I2C_Direction_Transmitter);
	if(!I2C_WaitAck())
		return ERROR;
	/* ??????????? */
	I2C_SendByte(writeAddr);
	if(!I2C_WaitAck()) 
		return ERROR;
	/* ????? */
	I2C_Stop();

	//�ڶ��׶�
	if (!I2C_Start())
		return FALSE;
	/* ??GT216L???? */
	I2C_SendByte(TSM_ADDR << 1 | I2C_Direction_Receiver);
	if(!I2C_WaitAck()) 
		return ERROR;
	/* ?????????? */
	buffer = I2C_ReceiveByte();
	I2C_NoAck();
	/* ????? */
	I2C_Stop();

	
	return buffer;
}
/*********************************END OF FILE**********************************/
