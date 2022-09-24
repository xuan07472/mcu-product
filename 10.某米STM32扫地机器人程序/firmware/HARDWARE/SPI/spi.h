#ifndef __SPI_H
#define __SPI_H

#include "sys.h"
#include "delay.h"
/***************************BMI160-SPI引脚定义**********************************/
#define      BMI160_SPIx                        SPI2
#define      BMI160_SPI_APBxClock_FUN          RCC_APB1PeriphClockCmd
#define      BMI160_SPI_CLK                     RCC_APB1Periph_SPI2

//CS(NSS)引脚 片选选普通GPIO即可
#define      BMI160_SPI_CS_APBxClock_FUN       RCC_APB2PeriphClockCmd
#define      BMI160_SPI_CS_CLK                  RCC_APB2Periph_GPIOB    
#define      BMI160_SPI_CS_PORT                 GPIOB
#define      BMI160_SPI_CS_PIN                  GPIO_Pin_12

//SCK引脚
#define      BMI160_SPI_SCK_APBxClock_FUN      RCC_APB2PeriphClockCmd
#define      BMI160_SPI_SCK_CLK                 RCC_APB2Periph_GPIOB   
#define      BMI160_SPI_SCK_PORT                GPIOB   
#define      BMI160_SPI_SCK_PIN                 GPIO_Pin_13
//MISO引脚
#define      BMI160_SPI_MISO_APBxClock_FUN     RCC_APB2PeriphClockCmd
#define      BMI160_SPI_MISO_CLK                RCC_APB2Periph_GPIOB    
#define      BMI160_SPI_MISO_PORT               GPIOB 																																							
#define      BMI160_SPI_MISO_PIN                GPIO_Pin_14
//MOSI引脚
#define      BMI160_SPI_MOSI_APBxClock_FUN     RCC_APB2PeriphClockCmd
#define      BMI160_SPI_MOSI_CLK                RCC_APB2Periph_GPIOB    
#define      BMI160_SPI_MOSI_PORT               GPIOB 
#define      BMI160_SPI_MOSI_PIN                GPIO_Pin_15

#define  		SPI_BMI160_CS_LOW()     						GPIO_ResetBits( BMI160_SPI_CS_PORT, BMI160_SPI_CS_PIN )
#define  		SPI_BMI160_CS_HIGH()    						GPIO_SetBits( BMI160_SPI_CS_PORT, BMI160_SPI_CS_PIN )

u8 SPI2_SendByte(u8 byte);
void vBMI160_SPI2_Init(void);
void BMI160_WriteOneByteToSlave(u8 RegisterAddress,u8 Data);
u8 BMI160_ReadOneByteFromSlave(u8 RegisterAddress);
#endif
