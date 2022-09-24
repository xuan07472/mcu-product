/**
 * @Filename   ��spi.c
 * @Author     : HuangWentao       
 * @Version    : 1.0
 * @Date       : 2019/11/14
 * @Discription : SPI2����BMI160ͨ��
 * @hardware connect:
 *       -------------CS   PB12------------------------
 *       -------------SCK  PB13------------------------
 *       -------------MISO PB14------------------------
 *       -------------MOSI PB15------------------------

 */
#include "spi.h"

/**
  * @Description  �����豸��ַд������
  * @param  RegisterAddress�����豸�Ĵ�����ַ��8λ��  Data д������ݣ�8λ��
  * @retval ��
  */
void BMI160_WriteOneByteToSlave(u8 RegisterAddress,u8 Data)
{
	
	SPI_BMI160_CS_LOW();//ƬѡBMI160
	SPI2_SendByte(RegisterAddress&0x7f);//���͵�ַ����һλΪ0��ʾ����������д����
	SPI2_SendByte(Data);//��������
	SPI_BMI160_CS_HIGH();//ȡ��Ƭѡ
}



/**
  * @Description  �����豸��ַ����
  * @param  RegisterAddress�����豸�Ĵ�����ַ
  * @retval ret ���ظõ�ַ��һ�ֽ�����
  */
u8 BMI160_ReadOneByteFromSlave(u8 RegisterAddress)
{

	u8 ret;
	SPI_BMI160_CS_LOW();//ƬѡBMI160
	SPI2_SendByte(RegisterAddress|0x80);//��һλΪ1��ʾ���������ж�����
	ret = SPI2_SendByte(0xff);//������,����0xffֻ��Ϊ���ṩʱ��
	SPI_BMI160_CS_HIGH();//ȡ��Ƭѡ
	return ret;
}






/**
  * @Description  ͨ��SPI2����һ���ֽ����ݡ�ͬʱ����һ���ֽ����ݣ�ȫ˫����
  * @param   byte ��Ҫ���͵�����
  * @retval  ����һ�ֽ�����
  */
u8 SPI2_SendByte(u8 byte)
{

	
	u8 retry=0;	
	 while (SPI_I2S_GetFlagStatus(BMI160_SPIx , SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI_I2S_SendData(BMI160_SPIx , byte);
	retry=0;
	while (SPI_I2S_GetFlagStatus(BMI160_SPIx , SPI_I2S_FLAG_RXNE) == RESET) //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
		{
			retry++;
			if(retry>200)return 0;
		}

	return SPI_I2S_ReceiveData(BMI160_SPIx ); //����ͨ��SPIx������յ�����				
	
}


/**
  * @Description  SPI2��ʼ������
  * @param   ��
  * @retval  ��
  */
void vBMI160_SPI2_Init(void)
{
	
	

	
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
	
	/* ʹ��SPIʱ�� */
	BMI160_SPI_APBxClock_FUN ( BMI160_SPI_CLK, ENABLE );
	
	/* ʹ��SPI������ص�ʱ�� */
 	BMI160_SPI_CS_APBxClock_FUN ( BMI160_SPI_CS_CLK|BMI160_SPI_SCK_CLK|
																	BMI160_SPI_MISO_CLK|BMI160_SPI_MOSI_CLK, ENABLE );
	
  /* ����SPI�� CS���ţ���ͨIO���� */
  GPIO_InitStructure.GPIO_Pin = BMI160_SPI_CS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(BMI160_SPI_CS_PORT, &GPIO_InitStructure);
	
  /* ����SPI�� SCK����*/
  GPIO_InitStructure.GPIO_Pin = BMI160_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(BMI160_SPI_SCK_PORT, &GPIO_InitStructure);

  /* ����SPI�� MISO����*/
  GPIO_InitStructure.GPIO_Pin = BMI160_SPI_MISO_PIN;
  GPIO_Init(BMI160_SPI_MISO_PORT, &GPIO_InitStructure);

  /* ����SPI�� MOSI����*/
  GPIO_InitStructure.GPIO_Pin = BMI160_SPI_MOSI_PIN;
  GPIO_Init(BMI160_SPI_MOSI_PORT, &GPIO_InitStructure);

  /* ֹͣ�ź� BMI160: CS���Ÿߵ�ƽ*/
  SPI_BMI160_CS_HIGH();

  /* SPI ģʽ���� */
  // BMI160оƬ ֧��SPIģʽ0��ģʽ3���ݴ�����CPOL CPHA = 1 1
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

  /* ʹ�� SPI  */
  SPI_Cmd(BMI160_SPIx , ENABLE);
	GPIO_SetBits( GPIOB, GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15 );
	
	
}
