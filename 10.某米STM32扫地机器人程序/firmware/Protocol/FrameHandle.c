#include "framehandle.h"

/*
 * Filename   ��framehandle.c
 * Author     : PuSheng 
 * Version    : 1.0
 * Date       : 2019.12.25 
 * Discription : Э��֡������
*/


/*
* Function Name  : vFrameDataHandlerTask
* Description    : Э��֡���ݴ���������
* Input          : ���� 
* Output         : None 
* Return         : None
*/

u32  ProductAppPassword  =   PRODUCT_APP_PASSWORD;  //��ƷӦ����Կ
fCommand_e    g_tLastCommand;//�ϴν��յ�������
RouteCoord_t  g_tRouteCoord={0};// �����˵�����·������
/*
* Function Name  : vFrameDataHandlerTask
* Description    : ֡���������� �����ڽ��ܵ������ݷ���ΪЭ��֡���� 
* Input          : p ����
* Output         : None 
* Return         : None
*/
void vFrameDataHandlerTask(void *p)
{
    u8 length=0;
    while(1)
    {
        /*���ڽ��յ����� ����֡�����Ѿ�ȫ���������*/
        if(ucUSART1_ReceiveMessageNumber && FR_Rx.IsFrameNotProcess == false)
        {
            length=ucUSART1ReceiveBufferLength[ucUSART1ReadBufferIndex];//��ȡ�������ݵĳ���
            eFrame_Analy(ucUSART1ReceiveBuffer[ucUSART1ReadBufferIndex],length,&FR_Rx);//��������
            USART1ReceiveDataReadDone(  );//�������ݶ�ȡ���
            
            /*�ɹ����յ��Ĳ��ǻ�Ӧ֡*/
            if( FR_Rx.tFrame.FrameFunction != ffRespond  )
            {
                /*���û�Ӧ֡*/
                ucCode_RespondFrame(&FR_Tx,&FR_Rx,ucUSART1TrainsmitBuffer[ucWtiteDataToUSART1TransmitGrooveIndex]);
                /*���ͻ�Ӧ֡�����ڷ��ͻ�����*/
                ucUSART1TrainsmitLength[ucWtiteDataToUSART1TransmitGrooveIndex]=5+FR_Tx.tFrame.DataLength;
                WriteDataToUSART1TraismitBufferDone();
            }              
        }
        
        /*֡���ݴ���*/
        if(FR_Rx.IsFrameNotProcess)
        {
            switch(FR_Rx.tFrame.FrameFunction)
            {
                case ffRespond: //���յ����ǻ�Ӧ֡
                    FR_Tx.IsReceiveRespond=true;//��ʾ���յ���Ӧ֡
                    break;
                
                case ffCommand: //���յ���������֡
                    vCommandFrameHandle(&FR_Rx);
                    break;
                
                case ffData: //���յ���������֡
                    vDataFrameHandle(&FR_Rx);
                    break;
                
                case ffIdle: break;//���յ����ǿ���֡��������
                case ffMax: break;
            }
            FR_Rx.IsFrameNotProcess=false;//��������Ѿ�����
        }
        vFrameAutoResendByUsart1(&FR_Rx);
        vTaskDelay(5);
    }
}


/*
* Function Name  : vCommandFrameHandle
* Description    : ����֡������ 
* Input          : Rx_FrameRecord ����֡�ṹ�� 
* Output         : None 
* Return         : None
*/
void vCommandFrameHandle(Rx_FrameRecord *R)
{
    
    if( R->tFrame.DataLength==1 )//�����ֽ�ֻ��һ�ֽ�
    {
        switch(R->tFrame.FrameData[0])//�鿴����
        {
            case UpdateFirmwareStart: //�ǿ�ʼ���¹̼�����
                g_tLastCommand=UpdateFirmwareStart;//�������
                ProductAppPassword=0;//��Ʒ��Կ����
                STMFLASH_Write(PASSWORD_ADDR,(u16*)&ProductAppPassword,2);//������Ʒ��Կ
                ((void (*)())(*((u32 *)(0x08000004))))();//��λ ����bootloader����
                break;
            
            case UpdateFirmwareDone: //�ǽ������¹̼����� firmware���򲻻��õ�
                g_tLastCommand=UpdateFirmwareDone;
                break;
            
            case BehaviorControl: //����Ϊ��������
                g_tLastCommand=BehaviorControl;//��¼����
                break;
            
            case RequestState://����״̬����
                g_tLastCommand=RequestState;
                vRequestStateCommandHandle(&FR_Tx);//���� ����״̬����
                break;
            
            case RouteCoord://��·��������
                g_tLastCommand=RouteCoord;
                break;
        }
        
    }
}
/*
* Function Name  : vDataFrameHandle
* Description    : ����֡������
* Input          : Rx_FrameRecord ����֡�ṹ�� 
* Output         : None 
* Return         : None
*/
void vDataFrameHandle(Rx_FrameRecord *R)
{
    switch(g_tLastCommand)
    {
        case UpdateFirmwareStart://����Ϊ��ʼ���¹̼� ����ִ��
            break;
        
        case UpdateFirmwareDone://����Ϊ�̼�������� ����ִ��
            break;  
        
        case RequestState://����Ϊ����״̬ ����ִ��
            break;
        
        case BehaviorControl://����Ϊ��Ϊ����
            vBehaviorControlHandle(R);//���� ��Ϊ���ƴ�����
            break;

        case RouteCoord://��·��������
            vRouteCoordHandle(R);//���� ��·���괦����
            break;        
    }
}

/*
* Function Name  : vBehaviorControlHandle
* Description    : ���յ�����Ϊ��Ϊ���� �Ĵ����� 
* Input          : ���� Rx_FrameRecord ����֡�ṹ��
* Output         : None 
* Return         : None
*/
void vBehaviorControlHandle(Rx_FrameRecord *R)
{
    BehaviorControl_t *pBC = (BehaviorControl_t *)R->DataBuf;//���ݸ�ֵ���ṹ���Ա
    
    switch(pBC->Com)//��������
    {
        case bccStop ://ֹͣ  
            g_tRobotState.Robotstate.eDirection = zStop;
            break;
        
        case bccForward ://ǰ��
            
            g_sExpect_Angle = pBC->TargetAngle*10;//��ȡĿ��Ƕ� 
            g_tRobotState.Robotstate.eDirection = zForward;
            break;
        
        case bccRetreat ://����
            g_sExpect_Angle = pBC->TargetAngle*10;//��ȡĿ��Ƕ� 
            g_tRobotState.Robotstate.eDirection = zRetreat;
            break;
        
        case bccTurnRotate ://ԭ��ת��
            g_sExpect_Angle = pBC->TargetAngle*10;//��ȡĿ��Ƕ� 
            g_tRobotState.Robotstate.eDirection = zRotate;
            break;
        
        case bccTurnLeft ://��������ת��
            g_sExpect_Angle = pBC->TargetAngle*10;//��ȡĿ��Ƕ�   
            g_tRobotState.Robotstate.eDirection = zTurnLeft;
            break;
            
        case bccTurnRight ://��������ת��
            g_sExpect_Angle = pBC->TargetAngle*10;//��ȡĿ��Ƕ� 
            g_tRobotState.Robotstate.eDirection = zTurnRight;
            break;
    }
}
/*
* Function Name  : vRouteCoordHandle
* Description    : ���յ���·�������� �Ĵ����� 
* Input          : ���� Rx_FrameRecord ����֡�ṹ��
* Output         : None 
* Return         : None
*/
void vRouteCoordHandle(Rx_FrameRecord *R)
{
    /*ÿ�ζ�����ܵ�5�� ����� ����һ���ǵ�ǰ���������� һ�� ����X YΪ8�ֽ�*/
    memcmp(&g_tRouteCoord,R->DataBuf,5*8);
}

/*
* Function Name  : vRequestStateCommandHandle
* Description    : ���յ�����Ϊ����״̬ �Ĵ����� 
* Input          : ���� Rx_FrameRecord ����֡�ṹ��
* Output         : None 
* Return         : None
*/
void vRequestStateCommandHandle(Tx_FrameRecord *T)
{
    s16 loc_CurrentAngle;//��ǰ�Ƕ�
    u16 loc_Size;//���ڼ�¼���ݴ�С
    
    if(T->IsSendOut == false)//�������ݷ��� ����û���յ���Ӧ����ȴ����ջ�Ӧ֡
    {
//        g_tRobotState.ucUserNumber=0xaa;
//        g_tRobotState.ucFirmVersion=0x00;
//        g_tRobotState.Robotstate.ucRemainBattery=1;
//        g_tRobotState.Robotstate.eDirection=zCharging;
//        g_tRobotState.Robotstate.PreviousDirection =zCharging;
//        g_tRobotState.Robotstate.unionAbnormal.AbnIden.BatteryAbn=1;
//        g_tRobotState.Robotstate.unionEEROR.ErrorSta  =0xFF;
        
        /*д�������ȫ����Ϣ*/
        loc_Size=sizeof g_tRobotState;
        memcpy(T->DataBuf,(void *)&g_tRobotState,loc_Size);

        /*д������˵�ǰ�Ƕ���Ϣ*/
        loc_CurrentAngle=CURRENT_ANGLE;
        loc_Size=sizeof loc_CurrentAngle;
        memcpy(T->DataBuf+sizeof g_tRobotState,&loc_CurrentAngle,loc_Size);
        
        /*д�����ݵ�����BUF �ȴ����ڷ���*/  
        loc_Size=(sizeof g_tRobotState)+(sizeof loc_CurrentAngle);//��ȡ�������ֽ�
        if(loc_Size > FRAME_DATA_MAX_LENGTH)
        {
            loc_Size=FRAME_DATA_MAX_LENGTH;//������� ���Ϊ��������ֽ�
            DEBUG("Tx Frame Over!\r\n");//�����Ϣ
        }
        ucCode_DataFrame(&FR_Tx,T->DataBuf,loc_Size,ucUSART1TrainsmitBuffer[ucWtiteDataToUSART1TransmitGrooveIndex]); 
        ucUSART1TrainsmitLength[ucWtiteDataToUSART1TransmitGrooveIndex]=loc_Size+5;//�����ֽ�+5���ֽ�֡��Ϣ
        WriteDataToUSART1TraismitBufferDone();
        
        T->IsSendOut=true;//��� �Ѿ�������һ֡����
    }
}


/*
* Function Name  : vFrameAutoResendByUsart1
* Description    : ֡�Զ��ط������ô���1��ΪЭ����Ϣ����ʱ��
* Input          : R ֡���ռ�¼�ṹ��
* Output         : None 
* Return         : None
*/
void vFrameAutoResendByUsart1(Rx_FrameRecord *R)
{
    static  bool  IsFirstIn=true;
    static  OS_TimeStruct TimFrameAutoResend;
    static  bool  IsRespondErr=false;
    
    if(FR_Tx.IsSendOut == true)//�з�������
    {
        if(true == IsFirstIn)
        {
            TimFrameAutoResend=GetOSRunTimeNow();//��һ�ν���  ��ý���ʱ��
            IsFirstIn = false;
        }
        
        if(FR_Tx.IsReceiveRespond == true)//���յ���Ӧ֡
        {
            //��Ӧ֡�������� ֡��ȷ
            if(R->tFrame.FrameFunction == ffRespond && R->tFrame.FrameData[0] == FrameSuccess)
            {
                FR_Tx.IsReceiveRespond=false;//���� ���յ���Ӧ֡
                FR_Tx.IsSendOut=false;//���㷢�ͱ�־λ
                IsFirstIn= true;//�����±�ǵ�һ�ν���
            }else//�������� ��Ҫ���·���
            {
                IsRespondErr= true;
            }
        }
        /*��ʱ��û�н��յ���Ӧ֡ 1�� ���߻�Ӧ֡��ʾ���ݳ���*/
        if(TimeOut == MultipleTimeoutCheck( &TimFrameAutoResend,0, 5, 0 ) || IsRespondErr)
        {
            /*����ָ����ƶ�һλ  ���·��͸ոջ�����������*/
            ucDMA1_Stream4TransmitGrooveIndex = ( (ucDMA1_Stream4TransmitGrooveIndex-1) > \
            USART1_TRANSMIT_GROOVE_SIZE ) ? USART1_TRANSMIT_GROOVE_SIZE : (ucDMA1_Stream4TransmitGrooveIndex-1);
            
            ucUSART1_TransmitMessageNumber++;//���ڷ������ݼ�1  ��ʾ�ط�����
            
            FR_Tx.IsReceiveRespond=false;//���� ���յ���Ӧ֡
            FR_Tx.IsSendOut=false;//���㷢�ͱ�־λ
            IsFirstIn= true;//�����±�ǵ�һ�ν���
        }
    }
}

