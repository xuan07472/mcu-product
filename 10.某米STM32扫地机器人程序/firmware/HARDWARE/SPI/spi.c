/**
 * @Filename   ：spi.c
 * @Author     : HuangWentao       
 * @Version    : 1.0
 * @Date       : 2019/11/14
 * @Discription : SPI2用于BMI160通信
 * @hardware connect:
 *       -------------CS   PB12------------------------
 *       -------------SCK  PB13------------------------
 *       -------------MISO PB14------------------------
 *       -------------MOSI PB15------------------------

 */
#include "spi.h"

/**
  * @Description  往从设备地址写入数据
  * @param  RegisterAddress：从设备寄存器地址（8位）  Data 写入的数据（8位）
  * @retval 无
  */
void BMI160_WriteOneByteToSlave(u8 RegisterAddress,u8 Data)
{
	
	SPI_BMI160_CS_LOW();//片选BMI160
	SPI2_SendByte(RegisterAddress&0x7f);//发送地址，第一位为0表示接下来进行写操作
	SPI2_SendByte(Data);//发送数据
	SPI_BMI160_CS_HIGH();//取消片选
}



/**
  * @Description  读从设备地址数据
  * @param  RegisterAddress：从设备寄存器地址
  * @retval ret 返回该地址的一字节数据
  */
u8 BMI160_ReadOneByteFromSlave(u8 RegisterAddress)
{

	u8 ret;
	SPI_BMI160_CS_LOW();//片选BMI160
	SPI2_SendByte(RegisterAddress|0x80);//第一位为1表示接下来进行读操作
	ret = SPI2_SendByte(0xff);//读数据,发送0xff只是为了提供时钟
	SPI_BMI160_CS_HIGH();//取消片选
	return ret;
}






/**
  * @Description  通过SPI2发送一个字节数据、同时返回一个字节数据（全双工）
  * @param   byte 将要发送的数据
  * @retval  返回一字节数据
  */
u8 SPI2_SendByte(u8 byte)
{

	
	u8 retry=0;	
	 while (SPI_I2S_GetFlagStatus(BMI160_SPIx , SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI_I2S_SendData(BMI160_SPIx , byte);
	retry=0;
	while (SPI_I2S_GetFlagStatus(BMI160_SPIx , SPI_I2S_FLAG_RXNE) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位
		{
			retry++;
			if(retry>200)return 0;
		}

	return SPI_I2S_ReceiveData(BMI160_SPIx ); //返回通过SPIx最近接收的数据				
	
}


/**
  * @Description  SPI2初始化函数
  * @param   无
  * @retval  无
  */
void vBMI160_SPI2_Init(void)
{
	
	

	
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
	
	/* 使能SPI时钟 */
	BMI160_SPI_APBxClock_FUN ( BMI160_SPI_CLK, ENABLE );
	
	/* 使能SPI引脚相关的时钟 */
 	BMI160_SPI_CS_APBxClock_FUN ( BMI160_SPI_CS_CLK|BMI160_SPI_SCK_CLK|
																	BMI160_SPI_MISO_CLK|BMI160_SPI_MOSI_CLK, ENABLE );
	
  /* 配置SPI的 CS引脚，普通IO即可 */
  GPIO_InitStructure.GPIO_Pin = BMI160_SPI_CS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(BMI160_SPI_CS_PORT, &GPIO_InitStructure);
	
  /* 配置SPI的 SCK引脚*/
  GPIO_InitStructure.GPIO_Pin = BMI160_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(BMI160_SPI_SCK_PORT, &GPIO_InitStructure);

  /* 配置SPI的 MISO引脚*/
  GPIO_InitStructure.GPIO_Pin = BMI160_SPI_MISO_PIN;
  GPIO_Init(BMI160_SPI_MISO_PORT, &GPIO_InitStructure);

  /* 配置SPI的 MOSI引脚*/
  GPIO_InitStructure.GPIO_Pin = BMI160_SPI_MOSI_PIN;
  GPIO_Init(BMI160_SPI_MOSI_PORT, &GPIO_InitStructure);

  /* 停止信号 BMI160: CS引脚高电平*/
  SPI_BMI160_CS_HIGH();

  /* SPI 模式配置 */
  // BMI160芯片 支持SPI模式0及模式3，据此设置CPOL CPHA = 1 1
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(BMI160_SPIx , &SPI_InitStructure);

  /* 使能 SPI  */
  SPI_Cmd(BMI160_SPIx , ENABLE);
	GPIO_SetBits( GPIOB, GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15 );
	
	
}
