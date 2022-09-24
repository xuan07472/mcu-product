#include "sys.h"
#include "delay.h"
/***************IICÇý¶¯**************************/
#define I2C_SCL  GPIO_Pin_13
#define I2C_SDA  GPIO_Pin_15
#define I2C_SCL_H  GPIO_SetBits(GPIOB,I2C_SCL)
#define I2C_SDA_H  GPIO_SetBits(GPIOB,I2C_SDA)
#define I2C_SCL_L  GPIO_ResetBits(GPIOB,I2C_SCL)
#define I2C_SDA_L  GPIO_ResetBits(GPIOB,I2C_SDA)

#define Read_SDA_BIT GPIO_ReadInputDataBit(GPIOB, I2C_SDA)
void I2C_INIT(void);
void I2C_SDA_OUT(void);
void I2C_SDA_IN(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Ack(void);
void I2C_NAck(void);
u8 I2C_Wait_Ack(void);
void I2C_Send_Byte(u8 txd);
u8 I2C_Read_Byte(u8 ack);
u8 BMI160_ReadOneByte(u8 addr);
void BMI160_WriteOneByte(u8 addr,u8 dt);

