#include "hardwareManage.h"



void vSystemHardwareDriverInit(void)
{
	vIIC_Init();            //IIC PB6 7 bq24773��Դ����оƬ
    vKey_Init();            //������ʼ�� 
    delay_init();	        //��ʱ������ʼ��
    vUart1_Init();          //����1��ʼ��
    vUart2_Init();          //����2��ʼ��    
	vLEDPWM_Init();         //��ʼ��LED
    vAll_PWM_Init();        //����PWM��ʼ��
	vTIM6_Int_Init();       //�����Ƕ�ʱ��
    vAllAdcPinInit();       //adc��ʼ��	
	vBMI160_SPI2_Init();    //������SPI��ʼ��
	vAllOutputPinInit();    //������ͨ���IO��ʼ��	
	vAll_EXTI_PinInit();    //�ⲿ�ж����ų�ʼ��
	vTIM7_Configuration();  //������
    vAll_TIM_CaptureInit(); //���ж�ʱ�������ʼ�� 

}

