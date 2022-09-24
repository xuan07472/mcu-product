/**
 * @Filename   ��Garbage_boxes_check.c
 * @Author     : HuangWentao       
 * @Version    : 1.0
 * @Date       : 2019/11/9
 * @Discription : �����м��
 * @hardware connect:
 *          -------------PC13(�ߵ�ƽ�����б�ȡ��)----------------------
 */

#include "Garbage_boxes_check.h"

static bool bGarbageBoxState=true;//������״̬ true ��ʾ���� false ȡ��
/**
  * @Description  �����м������
  * @param  ��
  * @retval ��
  */
void vGarbage_box_check_task(void *pvParameters)
{
	vGarbage_box_check_Init();

	while(1)
	{
		
		vGarbageBoxChecking();
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DustBoxAbn=!isGarbageBoxClose();//�ϱ�����״̬
		delay_ms(100);
	}
	
}


/**
  * @Description  �ж��������Ƿ����
  * @param  ��
  * @retval bGarbageBoxState��true���� false ȡ��
  */
bool isGarbageBoxClose(void)
{
  return   bGarbageBoxState;
}


/**
  * @Description  ������״̬���
  * @param  ��
  * @retval ��
  */
void vGarbageBoxChecking(void)
{
   if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13))//�����м��
    {
        //printf("�����б�ȡ����\r\n");
        bGarbageBoxState = false;
    }

    else
    {
        //printf("�����к�����\r\n");

        bGarbageBoxState = true;
    }
}

/**
  * @Description  �����м������IO��ʼ�� 
  * @param  ��
  * @retval ��
  */
void vGarbage_box_check_Init(void)
{
 	
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PB�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
}
 





