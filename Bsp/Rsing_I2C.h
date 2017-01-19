/**
  ******************************************************************************
  * @file    Rsing_I2C.h
  * @author  Rsing
  * @version V1.0
  * @date    31-July-2014
  * @brief   Header file for Rsing_I2C.c module. 
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RSING_I2C_H
#define __RSING_I2C_H

/* Includes ------------------------------------------------------------------*/
 #include "stm32f2xx.h"
#include "Redef_GPIO.h"
/* Defines -------------------------------------------------------------------*/


#define IO_I2C_SCL  PB(6)
#define IO_I2C_SDA  PB(7)
#define IO_I2C_IRQ   PB(5)
#define IO_I2C_EN   PC(11)
#define IO_I2C_RST  PC(12)


#define I2C_ENABLE   GPIO_ResetBits(IO_GPIO_PORT(IO_I2C_EN), IO_GPIO_PIN(IO_I2C_EN))
#define I2C_DISABLE   GPIO_SetBits(IO_GPIO_PORT(IO_I2C_EN), IO_GPIO_PIN(IO_I2C_EN))

#define I2C_RST_H   GPIO_SetBits(IO_GPIO_PORT(IO_I2C_RST), IO_GPIO_PIN(IO_I2C_RST))
#define I2C_RST_L   GPIO_ResetBits(IO_GPIO_PORT(IO_I2C_RST), IO_GPIO_PIN(IO_I2C_RST))


#define I2C_SCL_H  GPIO_SetBits(IO_GPIO_PORT(IO_I2C_SCL), IO_GPIO_PIN(IO_I2C_SCL))
#define I2C_SCL_L  GPIO_ResetBits(IO_GPIO_PORT(IO_I2C_SCL), IO_GPIO_PIN(IO_I2C_SCL))
#define I2C_SDA_H  GPIO_SetBits(IO_GPIO_PORT(IO_I2C_SDA), IO_GPIO_PIN(IO_I2C_SDA))
#define I2C_SDA_L  GPIO_ResetBits(IO_GPIO_PORT(IO_I2C_SDA), IO_GPIO_PIN(IO_I2C_SDA))

#define READ_I2C_SCL   IO_GPIO_ReadInputDataBit(IO_I2C_SCL) 
#define READ_I2C_SDA   IO_GPIO_ReadInputDataBit(IO_I2C_SDA)  

#define READ_I2C_IRQ   IO_GPIO_ReadInputDataBit(IO_I2C_IRQ)

#define TSM_ADDR (0x68)
/*模拟I2C端口输出输入定义*/
#define SCL_H         I2C_SCL_H
#define SCL_L         I2C_SCL_L
   
#define SDA_H         I2C_SDA_H
#define SDA_L         I2C_SDA_L

#define SCL_read      READ_I2C_SCL
#define SDA_read      READ_I2C_SDA

/* Private functions ---------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void I2C_Configuration(void);
uint8_t I2C_WriteByte(uint8_t slave_addr, uint8_t reg_addr, uint8_t reg_data);
uint8_t I2C_ReadByte(uint8_t slave_addr, uint8_t reg_addr);
uint8_t I2C_WriteBuf(uint8_t slave_addr,
                        uint8_t reg_addr,
                        uint16_t len,   
                        uint8_t *data_ptr);
uint8_t I2C_ReadBuf(uint8_t slave_addr,
                    uint8_t reg_addr,
                    uint16_t len, 
                    uint8_t *data_ptr);

int get_key(void);
int IIC_Write(uint8_t addr, uint8_t reg, uint16_t len, uint8_t* data);
int IIC_Read(uint8_t addr, uint8_t reg, uint16_t len, uint8_t *buf);

u8 TSM_Write_2Byte(u8 WriteAddr, u8 WriteData1, u8 WriteData2);
u8 TSM_ReadOneByte(u8 writeAddr);
#endif //Rsing_I2C.h
