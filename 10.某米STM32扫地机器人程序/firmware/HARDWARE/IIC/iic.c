/**
 * @Filename   ：iic.c
 * @Author     : HuangWentao       
 * @Version    : 1.0
 * @Date       : 2019/11/11
 * @Discription : IIC用于BMI160通信
 * @hardware connect:
 *       -------------I2C_SCL  PB13------------------------
 *       -------------I2C_SDA  PB15------------------------
 */
#include "iic.h"


/**
  * @Description  IIC初始化
  * @param  无
  * @retval 无
  */
void I2C_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
    GPIO_InitStructure.GPIO_Pin= I2C_SCL|I2C_SDA;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
 
    I2C_SCL_H;
    I2C_SDA_H;
}


/**
  * @Description  BMI160通过IIC读取addr地址的数据
  * @param  addr：8位目的地址
  * @retval temp：读取到的数据
  */
u8 BMI160_ReadOneByte(u8 addr)
{
    u8 temp=0;
    I2C_Start(); 
    I2C_Send_Byte(0xd0);//0xd2 为0x69（BMI160IIC地址）左移1位 0110 1001  1101 0010 
    I2C_Wait_Ack();
    I2C_Send_Byte(addr);
    I2C_Wait_Ack();
    I2C_Start();
    I2C_Send_Byte(0xd1);//10100011
    I2C_Wait_Ack();
    temp=I2C_Read_Byte(0); 
    I2C_NAck();
    I2C_Stop();
		delay_ms(10);
    return temp; 


}


/**
  * @Description  BMI160通过IIC往addr地址写入数据dt
  * @param  addr：8位目标地址，dt：8位数据
  * @retval 无
  */
void BMI160_WriteOneByte(u8 addr,u8 dt)
{
    I2C_Start();
    I2C_Send_Byte(0xd2);
    I2C_Wait_Ack();
    I2C_Send_Byte(addr); 
    I2C_Wait_Ack();
    I2C_Send_Byte(dt);
    I2C_Wait_Ack();
    I2C_Stop();

}



void I2C_SDA_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 
		GPIO_InitStructure.GPIO_Pin= I2C_SDA;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;

		GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
}


void I2C_SDA_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 

		GPIO_InitStructure.GPIO_Pin= I2C_SDA;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
		GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
}


void I2C_Start(void)
{
  I2C_SDA_OUT();
 
    I2C_SDA_H;
    I2C_SCL_H;
    delay_us(5);
    I2C_SDA_L;
    delay_us(6);
    I2C_SCL_L;
}



void I2C_Stop(void)
{
   I2C_SDA_OUT();
 
 
   I2C_SCL_L;
   I2C_SDA_L;
   I2C_SCL_H;
   delay_us(6);
   I2C_SDA_H;
   delay_us(6);
}


void I2C_Ack(void)
{
   I2C_SCL_L;
   I2C_SDA_OUT();
   I2C_SDA_L;
   delay_us(2);
   I2C_SCL_H;
   delay_us(5);
   I2C_SCL_L;
}


void I2C_NAck(void)
{
   I2C_SCL_L;
   I2C_SDA_OUT();
   I2C_SDA_H;
   delay_us(2);
   I2C_SCL_H;
   delay_us(5);
   I2C_SCL_L;
}




u8 I2C_Wait_Ack(void)
{
    u8 tempTime=0;
 
    I2C_SDA_IN();
    I2C_SDA_H;
    delay_us(1);
    I2C_SCL_H;
    delay_us(1);
 
 
    while(Read_SDA_BIT)
    {
        tempTime++;
        if(tempTime>250)
        {
            I2C_Stop();
            return 1;
        } 
    }
    I2C_SCL_L;
    return 0;

	}

	
	
void I2C_Send_Byte(u8 txd)
{
    u8 i=0;
 
    I2C_SDA_OUT();
    I2C_SCL_L;
 
    for(i=0;i<8;i++)
    {
        if((txd&0x80)>0) //0x80 ?1000 0000
            I2C_SDA_H;
        else
            I2C_SDA_L;
 
        txd<<=1;
        I2C_SCL_H;
        delay_us(2); 
        I2C_SCL_L;
        delay_us(2);
    }
}


u8 I2C_Read_Byte(u8 ack)
{
u8 i=0,receive=0;
 
I2C_SDA_IN();
for(i=0;i<8;i++)
{
     I2C_SCL_L;
        delay_us(2);
        I2C_SCL_H;
        receive<<=1;
        if(Read_SDA_BIT)
         receive++;
        delay_us(1); 
}
 if(ack==0)
     I2C_NAck();
    else
      I2C_Ack();
    return receive;
}


