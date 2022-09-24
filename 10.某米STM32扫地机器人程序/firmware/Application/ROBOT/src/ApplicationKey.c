#include "applicationkey.h"

/*
 * Filename   ��applicationkey.c
 * Author     : pusheng       
 * Version    : 1.0
 * Date       : 2019.11.7
 * Discription : ��������Ӧ�ú���
*/	

/*******************************************************************************
* Function Name  : vButtonCheckOperation
* Description    : ��ⰴ����״̬ ������Ӧ�Ķ��� ������20ms�����ڵ���һ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vButtonCheckOperation(void)
{
    eButtonState buttonstate1 , buttonstate2;
    
    buttonstate1 = eKey1State();
    buttonstate2 = eKey2State();
    
    if( ButtonUp != buttonstate1)
    {
        DEBUG("buttonstate1 %d\r\n",buttonstate1);
        if(buttonstate1 == ButtonDoubleDown)
        {
            POWER_DOWN;//˫��ϵͳ����
            g_tRobotState.KeyCommand=keyStop;//�ϱ�������Ϣ
        }
        if(buttonstate1 == Button2sDown)
        {
            POWER_UP;//2�볤��ϵͳ�ϵ�
            g_tRobotState.KeyCommand=keyStart;//�ϱ�������Ϣ
        }
    }
    if( ButtonUp != buttonstate2)
    {
        DEBUG("buttonstate2 %d\r\n",buttonstate2);
    }
}
/*******************************************************************************
* Function Name  : eKey1State
* Description    : ���ذ�����ֵ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
eButtonState eKey1State(void)
{
    static uint8_t Button_PC = 0; //״ָ̬ʾ
    static OS_TimeStruct KeyDownTime = {0};//������ʱ��ṹ��

    switch( Button_PC )
    {
        case 0 :             
            if( KEY_1_PIN != ( uint8_t )Bit_RESET)
            {                
                return ButtonUp; //����û�а���       
            }
            KeyDownTime = GetOSRunTimeNow(  );//�������º� ��¼��ǰʱ��
            Button_PC = 1;              
        case 1:  
            if( KEY_1_PIN == ( uint8_t )Bit_RESET) //�ȴ������ͷ�
            {             
                return ButtonUp;//����һֱû���ͷ�        
            }
            /*�����Ѿ��ͷ�*/
            if( SingleTimeoutCheck( KeyDownTime, 0, 0, 20 ) == NotTimeOut )   //����
            {
                Button_PC = 0;//����ʱ��С��20ms  ��Ϊ�Ƕ��� ʶ��Ϊû�а�������
                return ButtonUp;  
            } 
            if( SingleTimeoutCheck( KeyDownTime, 0, 6, 0 ) == TimeOut ) //�Ƿ��³���6��
            {
                Button_PC = 0;
                return Button6sDown;
            }     
            else if( SingleTimeoutCheck( KeyDownTime, 0, 5, 0 ) == TimeOut )
            {
                Button_PC = 0;
                return Button5sDown;
            }
            else if( SingleTimeoutCheck( KeyDownTime, 0, 4, 0 ) == TimeOut )         
            {
                Button_PC = 0;
                return Button4sDown;
            }
            else if( SingleTimeoutCheck( KeyDownTime, 0, 3, 0 ) == TimeOut )           
            {
                Button_PC = 0;
                return Button3sDown;
            }
            else if( SingleTimeoutCheck( KeyDownTime, 0, 2, 0 ) == NotTimeOut )   //����С��2s            
            {
                KeyDownTime = GetOSRunTimeNow(  );//���»�ȡ��ǰʱ��
                Button_PC = 3;
                return ButtonUp;
            }
            else if( SingleTimeoutCheck( KeyDownTime, 0, 1, 0 ) == NotTimeOut ) 
            {
                KeyDownTime = GetOSRunTimeNow(  );
                Button_PC = 3;
                return ButtonUp;
            }
            Button_PC = 0;
       case 3://˫����� ����666
            if( KEY_1_PIN != ( uint8_t )Bit_RESET)//û�м�⵽����
            {                
                if(SingleTimeoutCheck( KeyDownTime, 0, 0, 300 ) == TimeOut)//300ms�� ��Ȼû�еȴ����ڶ��ΰ�������.
                {
                    Button_PC = 0;
                    return Button2sDown;//���ذ�������2S
                }
                return ButtonUp;        
            }
            else //300ms�ڼ�⵽�ڶ��ΰ�������
            {
                KeyDownTime = GetOSRunTimeNow(  );//���»�ȡ��ǰʱ��
                Button_PC = 4;                  
            }
           break;
       case 4://���˫��ʱ��
            if( KEY_1_PIN == ( uint8_t )Bit_RESET) //������û�ɿ�
            {             
                return ButtonUp;        
            }
            /*�����Ѿ��ͷ�*/
            if( SingleTimeoutCheck( KeyDownTime, 0, 2, 0 ) == NotTimeOut ) 
            {
                Button_PC = 0;
                return ButtonDoubleDown;//�ڶ��ΰ���ʱ����2����  ���˫�������� ����
            }
            else//����2��  ˫��ʧ�� ŷ����
            {
                Button_PC = 0;
                return ButtonUp;
            }
       default:  
           break;   
    }
    return ButtonUp;
}

/*******************************************************************************
* Function Name  : eKey2State
* Description    : ���ذ�����ֵ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
eButtonState eKey2State(void)
{
    static uint8_t Button_PC = 0; //״ָ̬ʾ
    static OS_TimeStruct KeyDownTime = {0};//������ʱ��ṹ��

    switch( Button_PC )
    {
        case 0 :             
            if( KEY_2_PIN != ( uint8_t )Bit_RESET)
            {                
                return ButtonUp; //����û�а���       
            }
            KeyDownTime = GetOSRunTimeNow(  );//�������º� ��¼��ǰʱ��
            Button_PC = 1;              
        case 1:  
            if( KEY_2_PIN == ( uint8_t )Bit_RESET) //�����ͷż��
            {             
                return ButtonUp;//����һֱû���ͷ�        
            }
            /*�����Ѿ��ͷ�*/
            if( SingleTimeoutCheck( KeyDownTime, 0, 0, 20 ) == NotTimeOut )   //����
            {
                Button_PC = 0;//����ʱ��С��20ms  ��Ϊ�Ƕ��� ʶ��Ϊû�а�������
                return ButtonUp;  
            } 
            if( SingleTimeoutCheck( KeyDownTime, 0, 6, 0 ) == TimeOut ) //�Ƿ��³���6��
            {
                Button_PC = 0;
                return Button6sDown;
            }     
            else if( SingleTimeoutCheck( KeyDownTime, 0, 5, 0 ) == TimeOut )
            {
                Button_PC = 0;
                return Button5sDown;
            }
            else if( SingleTimeoutCheck( KeyDownTime, 0, 4, 0 ) == TimeOut )         
            {
                Button_PC = 0;
                return Button4sDown;
            }
            else if( SingleTimeoutCheck( KeyDownTime, 0, 3, 0 ) == TimeOut )           
            {
                Button_PC = 0;
                return Button3sDown;
            }
            else if( SingleTimeoutCheck( KeyDownTime, 0, 2, 0 ) == NotTimeOut )   //����С��2s            
            {
                KeyDownTime = GetOSRunTimeNow(  );//���»�ȡ��ǰʱ��
                Button_PC = 3;
                return ButtonUp;
            }
            else if( SingleTimeoutCheck( KeyDownTime, 0, 1, 0 ) == NotTimeOut ) 
            {
                KeyDownTime = GetOSRunTimeNow(  );
                Button_PC = 3;
                return ButtonUp;
            }
            Button_PC = 0;
       case 3://˫����� ����666
            if( KEY_2_PIN != ( uint8_t )Bit_RESET)//û�м�⵽����
            {                
                if(SingleTimeoutCheck( KeyDownTime, 0, 0, 300 ) == TimeOut)//300ms�� ��Ȼû�еȴ����ڶ��ΰ�������.
                {
                    Button_PC = 0;
                    return Button2sDown;//���ذ�������2S
                }
                return ButtonUp;        
            }
            else //300ms�ڼ�⵽�ڶ��ΰ�������
            {
                KeyDownTime = GetOSRunTimeNow(  );//���»�ȡ��ǰʱ��
                Button_PC = 4;                  
            }
           break;
       case 4://���˫��ʱ��
            if( KEY_2_PIN == ( uint8_t )Bit_RESET) //������û�ɿ�
            {             
                return ButtonUp;        
            }
            /*�����Ѿ��ͷ�*/
            if( SingleTimeoutCheck( KeyDownTime, 0, 2, 0 ) == NotTimeOut ) 
            {
                Button_PC = 0;
                return ButtonDoubleDown;//�ڶ��ΰ���ʱ����2����  ���˫�������� ����
            }
            else//����2��  ˫��ʧ�� ŷ����
            {
                Button_PC = 0;
                return ButtonUp;
            }
       default:  
           break;   
    }
    return ButtonUp;
}
