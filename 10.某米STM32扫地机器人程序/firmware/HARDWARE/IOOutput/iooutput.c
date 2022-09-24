#include "iooutput.h"
#
/******************************************************************************
* �ļ���  ��iooutput.c
* �ļ�������xiaomi������ͨ���IO����
* ������  ��Pu Sheng
* ��    �ڣ�2019��11��8��
* 
* Ӳ�����ӣ�PE6  - ��Դ����IO       �ߵ�ƽ�ػ�
* Ӳ�����ӣ�PE2  - �����Դ����IO   �ߵ�ƽʹ��
* Ӳ�����ӣ�PE8  - VA+��Դ����IO    �ߵ�ƽʹ��
* Ӳ�����ӣ�PC8  - VCC+��Դ����IO   �ߵ�ƽʹ��
* Ӳ�����ӣ�PA11 - V+��Դ����IO     �ߵ�ƽʹ��
            PC6  - ����������IO     �ߵ�ƽʹ��
            PC12 - ��ˢ��������     1 ��ת
            PA8  - ��ˢ��������     0 ��ת
            PE4  - ���ֿ�������     0 ��ת
            PE7  - ���ֿ�������     0 ��ת
            
            4ѡ1ģ��ͨ����������
            PD9
            PD11

******************************************************************************/
	    

/*******************************************************************************
* Function Name  : vAllOutputPinInit
* Description    : �����������IO  ������ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vAllOutputPinInit(void)
{
    vBoradPowerCtlPinInit();
    vFanPowerCtlPinInit();
    vVccPowerCtlPinInit();
    vVAPowerCtlPinInit();
    vVPowerCtlPinInit();
    vSGM4878CtlPinInit();
    vWheelCtlPinInit();
    vSideBrushCtlPinInit();
    vRollBrushCtlPinInit();
    vUltrasonic_Init();
}

/*******************************************************************************
* Function Name  : vRollBrushCtlPinInit
* Description    : ��ˢ�������ų�ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vRollBrushCtlPinInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��PA�˿�ʱ��
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
/*******************************************************************************
* Function Name  : vSideBrushCtlPinInit
* Description    : ��ˢ�������ų�ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vSideBrushCtlPinInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//ʹ��PC�˿�ʱ��
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}
/*******************************************************************************
* Function Name  : vWheelCtlPinInit
* Description    : �����ֿ������ų�ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vWheelCtlPinInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	//ʹ��PE�˿�ʱ��
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : vBoradPowerCtlPinInit
* Description    : �����Դ��������  ������ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vBoradPowerCtlPinInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	//ʹ��PE�˿�ʱ��
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOE, GPIO_Pin_6);   //���� ά��ϵͳ����
}
/*******************************************************************************
* Function Name  : vFanPowerCtlPinInit
* Description    : �����Դ��������  ������ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vFanPowerCtlPinInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	//ʹ��PE�˿�ʱ��
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOE, GPIO_Pin_2);  

}

/*******************************************************************************
* Function Name  : vVccPowerCtlPinInit
* Description    : VA+ ��5v���������� �ߵ�ƽʹ�� 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vVAPowerCtlPinInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	//ʹ��PE�˿�ʱ��
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    GPIO_SetBits(GPIOE, GPIO_Pin_8);  

}

/*******************************************************************************
* Function Name  : vVccPowerCtlPinInit
* Description    : VCC+ ��3.3v���������� �ߵ�ƽʹ�� 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vVccPowerCtlPinInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//ʹ��PC�˿�ʱ��
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC, GPIO_Pin_8);  

}

/*******************************************************************************
* Function Name  : vVccPowerCtlPinInit
* Description    : V+ ��5v���������� �ߵ�ƽʹ�� 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vVPowerCtlPinInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��PA�˿�ʱ��
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_11);  

}
/*******************************************************************************
* Function Name  : vSGM4878CtlPinInit
* Description    : 4ѡ1ģ��ͨ��оƬSGM4878�Ŀ������� 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vSGM4878CtlPinInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	//ʹ��PD�˿�ʱ��
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOD, &GPIO_InitStructure);  
}


/*******************************************************************************
* Function Name  : Ultransonic_Init
* Description    :  ��������ʼ��  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vUltrasonic_Init(void)
{	
	vUltrasonic_GPIO_config();

}
/*******************************************************************************
* Function Name  : vvUltrasonic_GPIO_config
* Description    :  �������������ų�ʼ��  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vUltrasonic_GPIO_config(void)
{
	GPIO_InitTypeDef    GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(TRIG_CLOCK, ENABLE);	 //ʹ��PC�˿�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	 //ʹ�ܸ���ʱ�Ӷ˿�ʱ��	
	//���ͳ�ʼ��

	GPIO_InitStructure.GPIO_Pin = TRIG_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(TRIG_PORT, &GPIO_InitStructure);					 //�����趨������ʼ��GPIO
	GPIO_ResetBits(TRIG_PORT,TRIG_PIN);
 

	
}
/*******************************************************************************
* Function Name  :vUltrasonic_Emit
* Description    : ����������
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void vUltrasonic_Emit(void)
{
	//���ͳ�����
	vTaskSuspendAll();  //������������� 
    TIM7_CNT_SET( 0 );  //��ʱ��7��������
	GPIO_SetBits( TRIG_PORT, TRIG_PIN );   
	delay_us(30);
	GPIO_ResetBits( TRIG_PORT ,TRIG_PIN );    
	xTaskResumeAll();  //�ָ���������� lhc
    

	
}

