#include "behaviormanage.h"
/*
 * Filename   ��behaviormanage.c
 * Author     : PuSheng HuangWenTao LiuHongChuan      
 * Version    : 2.3
 * Date       : 2019.11.28 
                2019.12.6  v2.0 
                2019.12.7  v2.1
                2019.12.9  v2.2
                2019.12.10 v2.3
                2019.12.11 v2.4
                2020. 1. 8 v2.5
 * Discription : ��Ϊ����Ӧ�ú���
 v1.0:���״̬�л��������
 v2.0:�����ر�״̬
 v2.1:
      1.�����˳��ر�ģʽ�ĽǶȷ�Χ
      2.������䴫������⵽���º�ת��
      
 v2.2:
      1.�����˴ӹ̶�ʱ���Ϊ ֱ���ж���ײ�͵���״̬�����Ƿ�������
      2.ɾ������ת���д�����ײ�� ��Ҫ����Ƕ� ��ִ������˺� �ص�����ת��״̬��
        ���ڸĳ�ֱ���ر�
 v2.3:
      1.�Ż�RetreatL ��RetreatR״̬��ת��ĽǶȱȽϡ�ʹ��������ת�л�û�п��١�
      
 v2.4:
      1.���رߴ�������ADֵת���ɾ���ֵ��
  v2.5
      1.���볬������⵽�ϰ�����ӳٻָ��ٶ�  �ܿ��˳�������3cm����
      
*/
#define PuSheng


#ifdef PuSheng


#define  TURN_CORNER_SPEED   (180)  //ת���ٶ� mm/s
#define  ALONG_EDGE_SPEED    (180)  //�ر��ٶ� mm/s
#define  FORWARD_SPEED       (260)  //ǰ���ٶ�
#define  RETREAT_SPEED       (-180) //�����ٶ�
#define  HAVE_HINDER_SPEED   (100)   //���ϰ���ʱ���ٶ�

#define  TURN_ANGLE          (200)   //�ܿ��ϰ��� ת��ĽǶ� ��λ1/10��

/*���ƽǶ� ��Χ1800 ~ -1799*/
#define TRANSFORM_VALID_ANGLE( angle ) (angle>1800) ? (angle-3600):\
                                       ((angle<=-1800) ? angle+3600:angle)

s16 ForwardSpeed    =  FORWARD_SPEED;    //ǰ���ٶ�
s16 AlongEdgeSpeed  =  ALONG_EDGE_SPEED; //�ر��ٶ�

static s16  sConstAngle;//������ɨ ǰ����׼�Ƕ� 
static bool bSoonTurnL1_Or_R0;//������ɨ �´�ת�� 1Ϊ�� 0Ϊ��
static u8 ucClashBothSide=0;//��ײ���������߶���ײ��
static u8 ucHinderSource=0;// �ϰ���Դ 1 ���䴫����  2��ײ������ 

static OS_TimeStruct MainTimAuto;//auto�ļ�ʱ��

/*******************************************************************************
* Function Name  : vAlongEdge
* Description    : �ر�״̬������
                    ��ײ������ر�״̬������ǰ���ǶȺ� �˳��ر�ģʽ
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
void vAlongEdge(void)
{
    static u8 AlongEdgeSte=0;//�Ƿ����ر�ģʽ��־λ
    MainTimAuto=GetOSRunTimeNow();//���ù�����ɨʱ��
    
    if(g_tRobotState.Robotstate.PreviousDirection != zAlongEdge )//��һ�ν���
    {
        /*�ϴ�����ײ*/
        if(g_tRobotState.Robotstate.PreviousDirection == zAuto)
        {
            AlongEdgeSte=0;//�ر�״̬����
        }
        if( ALONG_EDGE_DISTANCE() <100 && ucClashBothSide)//�ұ߾���С��10cm ����˫����ײ
        {
            DEBUG("\r\nת��40 ��ת��40��\r\n");
            g_sExpect_Angle=CURRENT_ANGLE+400;//����ת��40 ��
            
            if(bSoonTurnL1_Or_R0 == 1)//��һ�ν�����ת ˵����ʱ������ɨ��ǰ���Ƕ�Ӧ��ΪsConstAngle 
            {
                if(g_sExpect_Angle < sConstAngle-900)//��ǰ�Ƕȳ���������ɨ ǰ���Ƕ�
                    g_sExpect_Angle=sConstAngle-900;
                
                
            }else//������ɨ��ǰ���Ƕ�Ӧ��ΪsConstAngle-1800 
            {
                if( (g_sExpect_Angle < abs(sConstAngle)+900) && CURRENT_ANGLE> 0)//��ǰ�Ƕȳ���������ɨ ǰ���Ƕ�
                    g_sExpect_Angle=sConstAngle;
            }
            
            g_tRobotState.Robotstate.eDirection=zRotate;//��ת�ƹ� �����̽�⵽���ұ�����
            
            ucClashBothSide=0;//����˫����ײ��־λ
            AlongEdgeSte=1;//��ǽ����ر�
        }else
        {
            DEBUG("////////%d %d\r\n",ALONG_EDGE_VALUE(),ucClashBothSide);
        }
    }
    else 
    {
        if( ALONG_EDGE_DISTANCE() < 50)//����С��5CM
        {   
            AlongEdgeSte = 1;//�����ر�״̬
            
           //ֱ�ӿ�ʼ�ر�
            DEBUG("��");
            g_sExpect_speed=AlongEdgeSpeed;//��ֵ�ر��ٶ�
            ucRouteAdjustByAlongEdgeDistance(ALONG_EDGE_DISTANCE(),20,g_sExpect_speed);//�ر�
        }
        else//���ϰ������������5CM ������ת
        {
            if(AlongEdgeSte)//���ر�ģʽ
            {
                DEBUG("��");
                /*������ת�� ��ѰĿ��*/
                g_sExpect_Angle=CURRENT_ANGLE-900;//����ת��
                g_sExpect_Angle=TRANSFORM_VALID_ANGLE(g_sExpect_Angle);//�Ƕ���Ч��
                g_sExpect_speed=TURN_CORNER_SPEED;
                ucRotateAroundAxle(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range,LWheel);
            }else
            {
                DEBUG("��");
                /*��ת ��ѰĿ��*/
                g_sExpect_Angle=CURRENT_ANGLE+200;//����ת��
                g_sExpect_speed=TURN_CORNER_SPEED;
                ucTurn_Angle_Round(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range);
            }
        }
    }
    
    /*�жϽǶȷ�Χ �����Ƿ��˳��ر�*/
    if(bSoonTurnL1_Or_R0 == 1)//��һ�ν�����ת ˵����ʱ������ɨ��ǰ���Ƕ�Ӧ��ΪsConstAngle=0 
    {
        /* ��ʱ����ǰ���Ƕ�ΪsConstAngle=0  �������˽Ƕ���-100�ȵ�-170���� ��Ϊ�Ƕȳ�����Χ �˳��ر�*/
        if(CURRENT_ANGLE < sConstAngle-1000 && CURRENT_ANGLE>-1700)//��ǰ�Ƕȳ���������ɨ ǰ���Ƕ�
        {
            AlongEdgeSte=0;//�˳��ر�ģʽ
            g_tIncPID.LastData=0;
            g_tIncPID.PreData=0;
            DEBUG("\r\nȥAutoģʽ1ȥAutoģʽ1\r\n");
            g_tRobotState.Robotstate.eDirection=zAuto;//ȥ��Auto״̬
        }
    }else//������ɨ��ǰ���Ƕ�Ӧ��ΪsConstAngle-1800 
    {
        if( (CURRENT_ANGLE < abs(sConstAngle)+800) && CURRENT_ANGLE> 10)//��ǰ�Ƕȳ���������ɨ ǰ���Ƕ�
        {
            AlongEdgeSte=0;//�˳��ر�ģʽ
            g_tIncPID.LastData=0;
            g_tIncPID.PreData=0;
            DEBUG("\r\nȥAutoģʽ2ȥAutoģʽ2\r\n");
            g_tRobotState.Robotstate.eDirection=zAuto;
        }
    }
    
    g_tRobotState.Robotstate.PreviousDirection=zAlongEdge;//������AlongEdge״̬ ���AlongEdgeΪ�ϴη���
}
/*******************************************************************************
* Function Name  : vBehaviorStop
* Description    : ֹͣ״̬����
                   �����״̬�� ������ֹͣ������
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
void vBehaviorStop(void)
{
    if(g_tRobotState.Robotstate.PreviousDirection != zStop)
        DEBUG("zStop into\r\n");
    g_sExpect_speed=0;
    g_tLeftWheel.ExpectSpeed=g_sExpect_speed;
    g_tRightWheel.ExpectSpeed=g_sExpect_speed;
    g_tRobotState.Robotstate.PreviousDirection=zStop;//������Stop״̬ ���StopΪ�ϴη���
}
/*******************************************************************************
* Function Name  : vBehaviorForward
* Description    : ǰ��״̬����
                   �����״̬�� �������������ٶ�g_sExpect_speed �������Ƕ�g_sExpect_Angleǰ��
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
void vBehaviorForward(void)
{
//    if(g_tRobotState.Robotstate.PreviousDirection != zForward)
//        DEBUG("zForward into\r\n");
    g_sExpect_speed=ForwardSpeed;//��ֵǰ���ٶ�
    ucRoute_Adjust(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed);
    g_tRobotState.Robotstate.eDirection=zAuto;//ȥ��auto״̬
    g_tRobotState.Robotstate.PreviousDirection=zForward;//������Forward״̬ ���ForwardΪ�ϴη���
}
/*******************************************************************************
* Function Name  : vBehaviorRetreat
* Description    : ����״̬����
                   �����״̬�� �������������ٶ�g_sExpect_speed �������Ƕ�g_sExpect_Angle����
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
void vBehaviorRetreat(void)
{ 
    g_sExpect_speed=RETREAT_SPEED;//��ֵ�����ٶ�
    ucRoute_Adjust(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed);
    g_tRobotState.Robotstate.eDirection=zAuto;//ȥ��auto״̬
    g_tRobotState.Robotstate.PreviousDirection=zRetreat;//������Retreat״̬ ���RetreatΪ�ϴη���
}
/*******************************************************************************
* Function Name  : vBehaviorRetreatL
* Description    : ����Ȼ��ת�� ״̬����
                   �����״̬�� �������Ե�ǰ�Ƕ���������ٶ�RETREAT_SPEED����BACK_OFF_TIMEʱ��
                   Ȼ�����ٶ�TURN_CORNER_SPEED����תTURN_ANGLE��
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
void vBehaviorRetreatL(void)
{
    static bool IsFirstIn=true;//�Ƿ��һ�ν���
    static s16 TargeAngle;//Ŀ��Ƕ�
    
    /*��һ�ν���*/
    if(g_tRobotState.Robotstate.PreviousDirection != zRetreatL)
    {
        DEBUG("RetreatL RetreatL RetreatL into\r\n");
    }
    if(IsFirstIn == true)
    {
        g_sExpect_Angle=CURRENT_ANGLE;//��ȡ��һ�ν���ĽǶ�Ϊ��׼�Ƕ�
        TargeAngle=TRANSFORM_VALID_ANGLE(g_sExpect_Angle+TURN_ANGLE);//�����һ�ν���ĽǶ� ��ȡĿ��Ƕ�
        IsFirstIn=false;
    }

    /*����Ƿ�����ײ ���ߵ���*/
    if(g_tRobotState.Robotstate.unionAbnormal.AbnIden.RightCollisionAbn ||\
       g_tRobotState.Robotstate.unionAbnormal.AbnIden.LeftCollisionAbn  ||\
       g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLBAbn||\
       g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLFAbn||\
       g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRBAbn||\
       g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRFAbn)
    {
        g_sExpect_speed=RETREAT_SPEED;//��ֵ �����ٶ�
        ucRoute_Adjust(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed);//����
        
    }else /*����ʱ�䵽�� Ҫת����*/
    {
        g_sExpect_Angle=CURRENT_ANGLE+200;//�ı������Ƕ�Ϊ��ǰ�Ƕȼ�һ��20�� ��ת20.
        g_sExpect_speed=TURN_CORNER_SPEED;//�ٶ�Ϊת���ٶ�
        
        /*��ʼ��ת*/
        ucTurn_Angle_Round(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range);

        /*��ǰ�Ƕ� ����Ŀ��Ƕ� */
        if( TargeAngle <= CURRENT_ANGLE )
        {
            /*�Ѿ�����ָ���Ƕ�*/
            g_tRobotState.Robotstate.eDirection=zAuto;//ȥ��auto״̬
            IsFirstIn=true;//���±�ǵ�һ�ν���
            g_tLeftWheel.ExpectSpeed= 0;
            g_tRightWheel.ExpectSpeed= 0;
        } 
    }
    g_tRobotState.Robotstate.PreviousDirection=zRetreatL;//����RetreatL״̬ ��RetreatL�ϴη���
}
/*******************************************************************************
* Function Name  : vBehaviorRetreatR
* Description    : ����Ȼ��ת�� ״̬����
                   �����״̬�� �������Ե�ǰ�Ƕ���������ٶ�RETREAT_SPEED����BACK_OFF_TIMEʱ��
                   Ȼ�����ٶ�TURN_CORNER_SPEED����תTURN_ANGLE��
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
void vBehaviorRetreatR(void)
{
    static bool IsFirstIn=true;//�Ƿ��һ�ν���
    static s16 TargeAngle;//Ŀ��Ƕ�
    
    /*��һ�ν���*/
    if(g_tRobotState.Robotstate.PreviousDirection != zRetreatR)
    {
        DEBUG("RetreatR RetreatR RetreatR into\r\n");
    }
    
    if(IsFirstIn == true)
    {
        g_sExpect_Angle=CURRENT_ANGLE;//��ȡ��һ�ν���ĽǶ�Ϊ��׼�Ƕ�
        TargeAngle=TRANSFORM_VALID_ANGLE(g_sExpect_Angle-TURN_ANGLE);//�����һ�ν���ĽǶ� ��ȡĿ��Ƕ�
        IsFirstIn=false;
    }
    /*����Ƿ�����ײ ���ߵ���*/
    if(g_tRobotState.Robotstate.unionAbnormal.AbnIden.RightCollisionAbn ||\
       g_tRobotState.Robotstate.unionAbnormal.AbnIden.LeftCollisionAbn  ||\
       g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLBAbn||\
       g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLFAbn||\
       g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRBAbn||\
       g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRFAbn)
    {
        g_sExpect_speed=RETREAT_SPEED;//��ֵ�����ٶ�
        ucRoute_Adjust(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed);//����
    }else
    { 
        /*����ʱ�䵽�� Ҫת����*/
        g_sExpect_Angle=CURRENT_ANGLE-200;//�ı������Ƕ�Ϊ��ǰ�Ƕȼ�һ��20�� ��ת20.
        g_sExpect_speed=TURN_CORNER_SPEED;//�ٶ�Ϊת���ٶ�
        
        /*��ʼ��ת*/
        ucTurn_Angle_Round(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range);
        
        /*��ǰ�Ƕ� С��Ŀ��Ƕ� */
        if(TargeAngle >= CURRENT_ANGLE)
        {
            /*�Ѿ�����ָ���Ƕ�*/
            g_tRobotState.Robotstate.eDirection=zAuto;//ȥ��auto״̬
            IsFirstIn=true;//���±�ǵ�һ�ν���
            g_tLeftWheel.ExpectSpeed= 0;
            g_tRightWheel.ExpectSpeed= 0;
        }  
    }
    g_tRobotState.Robotstate.PreviousDirection=zRetreatR;//������RetreatR״̬ ���RetreatRΪ�ϴη���
}
/*******************************************************************************
* Function Name  : vBehaviorTurnLeft
* Description    : ������ת��
                   �����״̬�� �������������ٶ�TURN_CORNER_SPEED����ת����g_sExpect_Angle�Ƕ�
                   ִ�иú����Ḵλǰ��ʱ�� 
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
void vBehaviorTurnLeft(void)
{ 
    MainTimAuto=GetOSRunTimeNow();//��λǰ��ʱ��
    g_sExpect_speed=TURN_CORNER_SPEED;//��ֵת���ٶ�
    if(g_tRobotState.Robotstate.PreviousDirection != zTurnLeft)
        DEBUG("TurnLeft TurnLeft TurnLeft into\r\n");
    /*��������ת*/
    if(0 == ucRotateAroundAxle(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range,RWheel))
    {
        /*�Ѿ�����ָ���Ƕ�*/
        DEBUG("TurnLeft %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
        g_tRobotState.Robotstate.eDirection=zAuto;//ȥ��auto״̬
    } 
    
    g_tRobotState.Robotstate.PreviousDirection=zTurnLeft;//������TurnLeft״̬ ���TurnLeftΪ�ϴη���

}

/*******************************************************************************
* Function Name  : vBehaviorTurnRight
* Description    : ������ת��
                   �����״̬�� �������������ٶ�TURN_CORNER_SPEED����ת����g_sExpect_Angle�Ƕ�
                   ִ�иú����Ḵλǰ��ʱ�� 
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
void vBehaviorTurnRight(void)
{
    MainTimAuto=GetOSRunTimeNow();//��λǰ��ʱ��
    g_sExpect_speed=TURN_CORNER_SPEED;//��ֵת���ٶ�
    if(g_tRobotState.Robotstate.PreviousDirection != zTurnRight)
        DEBUG("TurnRight TurnRight TurnRight into\r\n");
    /*��������ת*/
    if(0 == ucRotateAroundAxle(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range,LWheel))
    {
        /*�Ѿ�����ָ���Ƕ�*/
        DEBUG("TurnRight %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
        g_tRobotState.Robotstate.eDirection=zAuto;//ȥ��auto״̬
    } 
    g_tRobotState.Robotstate.PreviousDirection=zTurnRight;//������TurnRight״̬ ���TurnRightΪ�ϴη���
}
/*******************************************************************************
* Function Name  : vBehaviorRotate
* Description    : ԭ����ת
                   �����״̬�� �������������ٶ�TURN_CORNER_SPEEDԭ��ת����g_sExpect_Angle�Ƕ�
                   ִ�иú����Ḵλǰ��ʱ�� 
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
void vBehaviorRotate(void)
{
    static bool IsAlongEdgeIn=false;//�Ƿ���ر�ģʽ����ģ�
    
    if(g_tRobotState.Robotstate.PreviousDirection != zRotate)
    {
        DEBUG("Rotate into\r\n");
        g_sExpect_speed=TURN_CORNER_SPEED;//��ֵת���ٶ�
        if(g_tRobotState.Robotstate.PreviousDirection == zAlongEdge)//�ϴ�״̬�ر�ģʽ
        {
            IsAlongEdgeIn=true;
        }
    }
    /*��ת*/
    if(0 == ucTurn_Angle_Round(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range))
    {
        /*�Ѿ�����ָ���Ƕ�*/
        if(IsAlongEdgeIn)//�ϴ��Ǵ��ر߹�����
        {
            DEBUG("to zAlongEdge \r\n");
            g_tRobotState.Robotstate.eDirection=zAlongEdge;
            IsAlongEdgeIn=false;
        }else
        {
            
            DEBUG("to zAuto \r\n");
            g_tRobotState.Robotstate.eDirection=zAuto;//ȥ��auto״̬
        }
    } 
    g_tRobotState.Robotstate.PreviousDirection=zRotate;//������Rotate״̬ ���RotateΪ�ϴη���
}
/*******************************************************************************
* Function Name  : vBehaviorAuto
* Description    : �Զ�״̬�л�����
                   �����״̬�� �����˿�ʼ�߹�����ɨ �������رߵĽ��� 
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
void vBehaviorAuto(void)
{
    static bool IsFirstIn=true;//�Ƿ��һ�ν���
    
    if(IsFirstIn == true)
    {
        g_sExpect_Angle=CURRENT_ANGLE;
        sConstAngle=00;
        bSoonTurnL1_Or_R0=1;//��һ������ת
        MainTimAuto=GetOSRunTimeNow();  //��ȡ��ǰʱ��
        IsFirstIn=false;
    }
    /*�ϴ�״̬�Ǻ��� ˵���ո������ϰ�����*/
    if(g_tRobotState.Robotstate.PreviousDirection == zRetreatL ||\
       g_tRobotState.Robotstate.PreviousDirection == zRetreatR )
    {
        if(ucHinderSource == 2)//�������ײ������
        {
            DEBUG("\r\nȥ�ر�ģʽȥ�ر�ģʽ\r\n");
            g_tRobotState.Robotstate.eDirection=zAlongEdge;//ȥ�ر�ģʽ
        }
        else if (ucHinderSource == 1)//��������´�����
        {
            bSoonTurnL1_Or_R0= !bSoonTurnL1_Or_R0;//��ͷ
            DEBUG("\r\n����ת������ת������ת�� %d\r\n",bSoonTurnL1_Or_R0);
        }
    } 
    else
    {
        DEBUG("auto");
//g_tRobotState.Robotstate.eDirection=  g_tRobotState.Robotstate.PreviousDirection;//֡����ʹ��  һֱִ�ж�Ӧ��״̬
        /*ǰ��ʱ�䵽��*/
        if( TimeOut == MultipleTimeoutCheck(&MainTimAuto,0,5,0))
        {
            if(bSoonTurnL1_Or_R0 == 1)
            {
                g_sExpect_Angle=sConstAngle+1800;
                g_tRobotState.Robotstate.eDirection=zTurnLeft;//ȥ����ת״̬
                bSoonTurnL1_Or_R0=0;//�´�Ϊ��ת
            }else
            {
                g_sExpect_Angle=sConstAngle;
                g_tRobotState.Robotstate.eDirection=zTurnRight;//ȥ����ת״̬
                bSoonTurnL1_Or_R0=1;//�´�Ϊ��ת
            }
        }else//����ֱ��
        {
            if(bSoonTurnL1_Or_R0 == 1)//����´�����ת
            {
                g_sExpect_Angle=sConstAngle;
            }else
            {
                g_sExpect_Angle=sConstAngle-1800;
            }
            g_sExpect_Angle=TRANSFORM_VALID_ANGLE(g_sExpect_Angle);
            g_tRobotState.Robotstate.eDirection=zForward;//ȥ��ǰ��״̬
        }
    }
    g_tRobotState.Robotstate.PreviousDirection=zAuto;//������Auto״̬ ���AutoΪ�ϴη���

}
/*******************************************************************************
* Function Name  : vBehaviorManageTask
* Description    : ��Ϊ�л�������
                   ���ݵ�ǰ����Ϊ״̬��ִ�ж�Ӧ�ĺ��� 
* Input          : p ������Ϣ δʹ��
* Output         : None 
* Return         : None
*******************************************************************************/
void vBehaviorManageTask(void *p)
{
    while(GYRO_WORKING != eGyro_State)//�ȴ������ǽ��빤��״̬
    {
        vTaskDelay(5);
    }
    if(g_tRobotState.Robotstate.eDirection != zCharging)
        g_tRobotState.Robotstate.eDirection=zAuto;
    
    while(1)
    {
        switch(g_tRobotState.Robotstate.eDirection)
        {
            case  zStop:
                vBehaviorStop();
                break;
            case  zForward:                //ǰ��
                vBehaviorForward();
                break;
            case  zRetreatL:               //���˺���ת
                vBehaviorRetreatL();
                break;
            case  zRetreatR:               //���˺���ת
                vBehaviorRetreatR();
                break;
            case  zTurnLeft :              //��ת
                vBehaviorTurnLeft();
                break;
            case  zTurnRight:              //��ת
                vBehaviorTurnRight();
                break;
            case  zRotate:                 //ԭ����ת
                vBehaviorRotate();
                break;
            case  zAuto :                  //�Զ�
                vBehaviorAuto();
                break;
            case  zChargeBack :            //�س�
                break;
            case  zCharging :              //���ڳ��
                break;
            case  zAlongEdge:              //�ر�
                vAlongEdge();
                break;
            case  zRetreat:                //����
                vBehaviorRetreat();
                break;
        }
        vTaskDelay(5);
    }
}

/*******************************************************************************
* Function Name  : vErrorAbnormalCheckTask
* Description    : �쳣�����⺯��
                   ������ ������� ��ˢ��� ��ˢ��� ��� ���䴫���� ��ײ������  
                   ��� ���쳣�������Ϣ ��������Ϣ����ת����Ӧ�Ļ�����״̬
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
void vErrorAbnormalCheckTask(void *p)
{
    while(1)
    {
        for(;;)
        {
            vTaskDelay(5);
            DropCheckUpdate();//���䴫�������ݸ���
            UltraSonicCheckUpdate();//���������������ݸ���
            AllMotorErrorCheck();//���е��������
            if(g_tRobotState.Robotstate.unionAbnormal.AbnormalSta != 0)//���쳣���
            {
                /*��������*/
                if(g_tRobotState.Robotstate.unionAbnormal.AbnIden.LeftWheelAbn ||\
                   g_tRobotState.Robotstate.unionAbnormal.AbnIden.RightWheelAbn  )
                {
                    g_tRobotState.Robotstate.eDirection=zStop;
                    break;
                }else
                {
                    //g_tRobotState.Robotstate.eDirection=zAuto;
                }
                
                /*ֹͣ����״̬���ж���ײ�ͳ�����*/
                if(g_tRobotState.Robotstate.eDirection == zStop ||\
                   g_tRobotState.Robotstate.eDirection == zCharging )
                {
                    break;
                }
                /*��⵽��ײ*/
                if(g_tRobotState.Robotstate.unionAbnormal.AbnIden.LeftCollisionAbn||\
                   g_tRobotState.Robotstate.unionAbnormal.AbnIden.RightCollisionAbn )
                {
                    MainTimAuto=GetOSRunTimeNow();//��λǰ��ʱ��
                    ucHinderSource=2;//����ϰ���Դ
                    /*�Ƿ�����ͬʱ��ײ*/
                    if(g_tRobotState.Robotstate.unionAbnormal.AbnIden.LeftCollisionAbn &&\
                       g_tRobotState.Robotstate.unionAbnormal.AbnIden.RightCollisionAbn )
                    {
                        ucClashBothSide=1;
                        DEBUG("6");
                    }else
                    {
                        //ucClashBothSide=0;
                        //DEBUG("5");
                    }
                    g_tRobotState.Robotstate.eDirection=zRetreatL;

                }
                /*��⵽����*/
                if(g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLBAbn||\
                   g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLFAbn||\
                   g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRBAbn||\
                   g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRFAbn)
                {
                    MainTimAuto=GetOSRunTimeNow();//��λǰ��ʱ��
                    ucHinderSource=1;//����ϰ���Դ
                    if(bSoonTurnL1_Or_R0 == 1 )
                    {
                        g_tRobotState.Robotstate.eDirection=zRetreatL;
                    }
                    else 
                    {
                        g_tRobotState.Robotstate.eDirection=zRetreatR;
                    }
                }
            }
        }
    }
}
/*******************************************************************************
* Function Name  : DropCheckUpdate
* Description    : ���䴫�������ݼ�⺯��
                   �����䴫���������� �ϱ�����������ֵ
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
void DropCheckUpdate(void)
{
    if(tDropcheck.bAnyoneFall)
    {
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLBAbn=tDropcheck.bBackLFall;
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLFAbn=tDropcheck.bFrontLFall;
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRBAbn=tDropcheck.bBackRFall;
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRFAbn=tDropcheck.bFrontRFall;
        //DEBUG("DropCheck\r\n");
    }else
    {
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLBAbn=0;
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLFAbn=0;
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRBAbn=0;
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRFAbn=0;
    }
}
/*******************************************************************************
* Function Name  : UltraSonicCheckUpdate
* Description    : ���������������ݼ�⺯��
                   ��ⳬ���������������� �ϱ�����������ֵ ����⵽�ϰ��� ǰ���ٶȼ���
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
void UltraSonicCheckUpdate(void)
{
    static OS_TimeStruct UltraSonicHinderTim;
    
    g_tRobotState.Robotstate.unionAbnormal.AbnIden.UltraSonicAbn=bGetClashFlag();//�õ�������״̬
    if(g_tRobotState.Robotstate.unionAbnormal.AbnIden.UltraSonicAbn)
    {
        //DEBUG("UlSonicClash\r\n");
        
        UltraSonicHinderTim=GetOSRunTimeNow();//��ȡ��ǰʱ��
        ForwardSpeed    =  HAVE_HINDER_SPEED;    //ǰ���ٶ�
        AlongEdgeSpeed  =  HAVE_HINDER_SPEED;    //�ر��ٶ�
    }
    else
    {
        /*û�м�⵽�ϰ��� Ҳ�ȴ�1s �Żָ��ٶ�*/
        if(TimeOut == SingleTimeoutCheck( UltraSonicHinderTim,0, 1, 0 ))
        {
            ForwardSpeed    =  FORWARD_SPEED;    //ǰ���ٶ�
            AlongEdgeSpeed  =  ALONG_EDGE_SPEED; //�ر��ٶ�
        }
    }
}



/*******************************************************************************
* Function Name  : AllMotorErrorCheck
* Description    : ���е��������
                   ������е���ĵ���ADֵ��������ֵ��ֹͣɨ�ػ�
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
extern uint16_t g_usInChipAdcChannelAverageValue[ADC_CHANNEL_NUM]; //ADC ��ͨ��ƽ��ֵ
static OS_TimeStruct MotorErrorCheckTime;//����������ʱ��
void AllMotorErrorCheck(void)
{
    #define MotorAdStopThreshold 1200 //���AD��ֵ��1m��AD��ֵ��������ֵ���õ��ͣ��
    static u8  Count= 0;//AD�ۼӼ���ֵ
    static uint16_t LeftWheelAd = 0,RightWheelAd = 0,SideBrushAd = 0,RollBrushAd = 0;
   
    if( TimeOut == MultipleTimeoutCheck(&MotorErrorCheckTime,0,0,100))//100ms��һ��ֵ
    {

        LeftWheelAd += g_usInChipAdcChannelAverageValue[adcLeftWheel]/10;//����
        RightWheelAd += g_usInChipAdcChannelAverageValue[adcRightWheel]/10;//����
        SideBrushAd += g_usInChipAdcChannelAverageValue[adcSideBrush]/10;//��ˢ
        RollBrushAd += g_usInChipAdcChannelAverageValue[adcRollBrush]/10;//��ˢ
        Count++;
    }
    if(Count>=10)//�ۼƴﵽ10��ֵ
    {
        
        if((LeftWheelAd>MotorAdStopThreshold)| (RightWheelAd>MotorAdStopThreshold)| (SideBrushAd>MotorAdStopThreshold)| (RollBrushAd>MotorAdStopThreshold))
        {
            g_tRobotState.Robotstate.eDirection=zStop;
            DEBUG("�����ס��\r\n");
        }
       
        LeftWheelAd = 0;
        RightWheelAd = 0;
        SideBrushAd = 0;
        RollBrushAd = 0;
        Count = 0;
    }
    
     
    
}


#endif

#ifdef HongChuan 



#define MYDEBUG
#ifdef MYDEBUG
#define DEBUG  printf
#else
#define DEBUG(...)  (void)(0)
#endif

#define  TURN_CORNER_SPEED   (180) //ת���ٶ� mm/s
#define  FORWARD_SPEED       (300) //ǰ���ٶ�
#define  RETREAT_SPEED       (-200) //�����ٶ�
#define  ALONGEDGE_SPEED       (150) //�ر��ٶ�

#define  TURN_ANGLE         (200) //�ܿ��ϰ��� ת��Ƕ� ��λ1/10��
#define  BACK_OFF_TIME      (200)//����ʱ�� ��λms
#define  FRONT_OFF_TIME      (100)//ǰ��ʱ�� ��λms


s16 ForwardSpeed  =  FORWARD_SPEED; //ǰ���ٶ�

void vBehaviorStop(void)
{
    if(g_tRobotState.Robotstate.PreviousDirection != zStop)
        DEBUG("zStop into\r\n");
    g_sExpect_speed=0;
    g_tLeftWheel.ExpectSpeed=g_sExpect_speed;
    g_tRightWheel.ExpectSpeed=g_sExpect_speed;
    g_tRobotState.Robotstate.PreviousDirection=zStop;//������Stop״̬ ���StopΪ�ϴη���
}
/*ǰ��*/
void vBehaviorForward(void)
{
    if(g_tRobotState.Robotstate.PreviousDirection != zForward)
        DEBUG("zForward into\r\n");
    g_sExpect_speed=ForwardSpeed;//��ֵǰ���ٶ�
    ucRoute_Adjust(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed);
    g_tRobotState.Robotstate.eDirection=zAuto;//ȥ��auto״̬
    g_tRobotState.Robotstate.PreviousDirection=zForward;//������Forward״̬ ���ForwardΪ�ϴη���
}

void vBehaviorRetreatL(void)
{
    static bool IsFirstIn=true;//�Ƿ��һ�ν���
    static OS_TimeStruct Loc_TimRetreatL;
    static s16 TurnAngle;//ת���ĽǶ�
    static s16 SaveGlobalAngle;//�������RetreatL״̬ǰ�ĽǶ�
    static u8 IsSave=0;//�Ƿ񱣴�֮ǰ�ĽǶ� 0 ������  1 �ϴ�����ת 2�ϴ�����ת
    
    if(g_tRobotState.Robotstate.PreviousDirection != zRetreatL)
        DEBUG("RetreatL RetreatL RetreatL into\r\n");
    /*�����һ��״̬��������ת�䣬���п��ܻ�û��ת�꣬�����ȱ������ϴε������Ƕ�*/
    if(g_tRobotState.Robotstate.PreviousDirection == zTurnLeft )
    {
        IsSave=1;
        SaveGlobalAngle=g_sExpect_Angle;//����ȫ�ֽǶ�
        DEBUG("SaveGlobalAngle %d\r\n",SaveGlobalAngle);        
    }
    if( g_tRobotState.Robotstate.PreviousDirection == zTurnRight )
    {
        IsSave=2;
        SaveGlobalAngle=g_sExpect_Angle;//����ȫ�ֽǶ�
    }
    if(IsFirstIn == true)
    {
        g_sExpect_Angle=CURRENT_ANGLE;//��ȡ��һ�ν���ĽǶ�Ϊ��׼�Ƕ�
        TurnAngle=g_sExpect_Angle+TURN_ANGLE;//ת��������Ƕ�  ��ת+
        Loc_TimRetreatL=GetOSRunTimeNow();  //��ȡ��ǰʱ��
        IsFirstIn=false;
    }
    if( NotTimeOut == SingleTimeoutCheck(Loc_TimRetreatL,0,0,BACK_OFF_TIME))
    {
        g_sExpect_speed=RETREAT_SPEED;//��ֵ �����ٶ�
        ucRoute_Adjust(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed);//����
        
    }else /*����ʱ�䵽�� Ҫת����*/
    {
        g_sExpect_Angle=TurnAngle;//�ı������Ƕ�Ϊ ת���Ƕ�
        g_sExpect_speed=TURN_CORNER_SPEED;//�ٶ�Ϊת���ٶ�
        /*��ת*/
        if(0 == ucTurn_Angle_Round(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range))
        {
            /*�Ѿ�����ָ���Ƕ�*/
            if(1 == IsSave )//�ϴ�״̬����ת ������ת
            {
                g_sExpect_Angle=SaveGlobalAngle;//�ָ�ȫ�ֽǶ�
                g_tRobotState.Robotstate.eDirection=zTurnLeft;//ȥ��TurnLeft״̬
                DEBUG("1 TurnLeft %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
            }else
            if(2 == IsSave )//�ϴ�״̬����ת ������ת
            {
                g_sExpect_Angle=SaveGlobalAngle;//�ָ�ȫ�ֽǶ�
                g_tRobotState.Robotstate.eDirection=zTurnRight;//ȥ��TurnRight״̬
                DEBUG("2 TurnLeft %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
            }else
            {
                g_tRobotState.Robotstate.eDirection=zAuto;//ȥ��auto״̬
            }
            IsFirstIn=true;//���±�ǵ�һ�ν���
        } 
    }
    g_tRobotState.Robotstate.PreviousDirection=zRetreatL;//����RetreatL״̬ ��RetreatL�ϴη���
}
void vBehaviorRetreatR(void)
{
    static bool IsFirstIn=true;//�Ƿ��һ�ν���
    static OS_TimeStruct Loc_TimRetreatR;
    static s16 TurnAngle;//ת���ĽǶ�
    static s16 SaveGlobalAngle;//�������RetreatL״̬ǰ�ĽǶ�
    static u8 IsSave=0;//�Ƿ񱣴�֮ǰ�ĽǶ� 0 ������  1 �ϴ�����ת 2�ϴ�����ת
    
    if(g_tRobotState.Robotstate.PreviousDirection != zRetreatR)
        DEBUG("RetreatR RetreatR RetreatR into\r\n");
    /*�����һ��״̬��������ת�䣬���п��ܻ�û��ת�꣬�����ȱ������ϴε������Ƕ�*/
    if(g_tRobotState.Robotstate.PreviousDirection == zTurnLeft )
    {
        IsSave=1;
        SaveGlobalAngle=g_sExpect_Angle;//����ȫ�ֽǶ�    
    }
    if( g_tRobotState.Robotstate.PreviousDirection == zTurnRight )
    {
        IsSave=2;
        SaveGlobalAngle=g_sExpect_Angle;//����ȫ�ֽǶ�
    }
    
    if(IsFirstIn == true)
    {
        g_sExpect_Angle=CURRENT_ANGLE;//��ȡ��һ�ν���ĽǶ�Ϊ��׼�Ƕ�
        TurnAngle=g_sExpect_Angle-TURN_ANGLE;//ת��������Ƕ�  ��ת-
        Loc_TimRetreatR=GetOSRunTimeNow();  //��ȡ��ǰʱ��
        IsFirstIn=false;
    }
    
    if( NotTimeOut == SingleTimeoutCheck(Loc_TimRetreatR,0,0,BACK_OFF_TIME))
    {
        g_sExpect_speed=RETREAT_SPEED;//��ֵ�����ٶ�
        ucRoute_Adjust(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed);//����
    }else
    { 
        /*����ʱ�䵽�� Ҫת����*/
        g_sExpect_Angle=TurnAngle;//�ı������Ƕ�Ϊ ת���Ƕ�
        g_sExpect_speed=TURN_CORNER_SPEED;//�ٶ�Ϊת���ٶ�
        /*��ת*/
        if(0 == ucTurn_Angle_Round(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range))
        {
            /*�Ѿ�����ָ���Ƕ�*/
            if(1 == IsSave )//�ϴ�״̬����ת ������ת
            {
                g_sExpect_Angle=SaveGlobalAngle;//�ָ�ȫ�ֽǶ�
                g_tRobotState.Robotstate.eDirection=zTurnLeft;//ȥ��TurnLeft״̬
            }else
            if(2 == IsSave )//�ϴ�״̬����ת ������ת
            {
                g_sExpect_Angle=SaveGlobalAngle;//�ָ�ȫ�ֽǶ�
                g_tRobotState.Robotstate.eDirection=zTurnRight;//ȥ��TurnRight״̬
            }else
            {
                g_tRobotState.Robotstate.eDirection=zAuto;//ȥ��auto״̬
            }
            IsFirstIn=true;//���±�ǵ�һ�ν���
        }  
    }
    g_tRobotState.Robotstate.PreviousDirection=zRetreatR;//������RetreatR״̬ ���RetreatRΪ�ϴη���
}

void vBehaviorTurnLeft(void)
{
    g_sExpect_speed=TURN_CORNER_SPEED;//��ֵת���ٶ�

    if(g_tRobotState.Robotstate.PreviousDirection != zTurnLeft)
        DEBUG("TurnLeft TurnLeft TurnLeft into\r\n");
    /*��������ת*/
    if(0 == RotateAroundAxle(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range,RWheel))
    {
        /*�Ѿ�����ָ���Ƕ�*/
        DEBUG("TurnLeft %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
        g_tRobotState.Robotstate.eDirection=zAuto;//ȥ��auto״̬
    } 
    
    g_tRobotState.Robotstate.PreviousDirection=zTurnLeft;//������TurnLeft״̬ ���TurnLeftΪ�ϴη���

}
void vBehaviorTurnRight(void)
{
    g_sExpect_speed=TURN_CORNER_SPEED;//��ֵת���ٶ�
    if(g_tRobotState.Robotstate.PreviousDirection != zTurnRight)
        DEBUG("TurnRight TurnRight TurnRight into\r\n");
    /*��������ת*/
    if(0 == RotateAroundAxle(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range,LWheel))
    {
        /*�Ѿ�����ָ���Ƕ�*/
        DEBUG("TurnRight %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
        g_tRobotState.Robotstate.eDirection=zAuto;//ȥ��auto״̬
    } 
    g_tRobotState.Robotstate.PreviousDirection=zTurnRight;//������TurnRight״̬ ���TurnRightΪ�ϴη���
}
void vBehaviorRotate(void)
{

    static bool IsFirstIn=true;//�Ƿ��һ�ν���
    if(g_tRobotState.Robotstate.PreviousDirection != zRotate)
        DEBUG("Rotate into\r\n");
    if(IsFirstIn == true)
    {
        g_sExpect_speed=TURN_CORNER_SPEED;//��ֵת���ٶ�
        IsFirstIn=false;
    }
    /*��ת*/
    if(0 == ucTurn_Angle_Round(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range))
    {
        /*�Ѿ�����ָ���Ƕ�*/
        g_tRobotState.Robotstate.eDirection=zAuto;//ȥ��auto״̬
        IsFirstIn=true;//���±�ǵ�һ�ν���
    } 
    g_tRobotState.Robotstate.PreviousDirection=zRotate;//������Rotate״̬ ���RotateΪ�ϴη���
}
void vBehaviorAuto(void)
{
        
    
    static bool IsFirstIn=true;//�Ƿ��һ�ν���
    static OS_TimeStruct Loc_TimAuto;
    static s16  constAngle;
    int i=0;
    
     g_sExpect_speed=ALONGEDGE_SPEED;//��ֵת���ٶ�
// g_tRobotState.Robotstate.eDirection=zForward;
//    g_tRobotState.Robotstate.eDirection=zStop;

    if(g_tRobotState.Robotstate.PreviousDirection != zAuto)
        DEBUG("Auto into\r\n");
    if(IsFirstIn == true)
    {
        g_sExpect_Angle=CURRENT_ANGLE;
        constAngle=g_sExpect_Angle;
        Loc_TimAuto=GetOSRunTimeNow();  //��ȡ��ǰʱ��
        IsFirstIn=false;
    }
    /*ǰ��ʱ�䵽��*/
    if( TimeOut == MultipleTimeoutCheck(&Loc_TimAuto,0,10,0))
    {
        if(abs(CURRENT_ANGLE-constAngle) <TURN_ANGLE+g_sExpect_range+10)
        {
            g_sExpect_Angle=constAngle+1785;
            DEBUG("Auto TurnLeft g_sExpect_Angle %d\r\n ",g_sExpect_Angle);
            g_tRobotState.Robotstate.eDirection=zTurnLeft;//ȥ����ת״̬
            DEBUG("�� %d %d \r\n",CURRENT_ANGLE,g_sExpect_Angle);
        }else
        {
            g_sExpect_Angle=constAngle+20;
            g_tRobotState.Robotstate.eDirection=zTurnRight;//ȥ����ת״̬
            DEBUG("�� %d %d \r\n",CURRENT_ANGLE,g_sExpect_Angle);
        }
    }else
    {
        if(abs(CURRENT_ANGLE-constAngle) <TURN_ANGLE+g_sExpect_range+10)
        {
            g_sExpect_Angle=constAngle;
        }else
        {
            g_sExpect_Angle=constAngle-1800;
        }
        g_tRobotState.Robotstate.eDirection=zForward;//ȥ��ǰ��״̬
    }
    g_tRobotState.Robotstate.PreviousDirection=zAuto;//������Auto״̬ ���AutoΪ�ϴη���

}
/*��Ϊת������*/
void vBehaviorManageTask(void *p)
{
    while(GYRO_WORKING != eGyro_State)
    {
        vTaskDelay(5);
    }
    g_tRobotState.Robotstate.eDirection=zAuto;
    while(1)
    {
        switch(g_tRobotState.Robotstate.eDirection)
        {
            case  zStop:
                vBehaviorStop();
                break;
            case  zForward:                //ǰ��
                vBehaviorForward();
                break;
            case  zRetreatL:               //���˺���ת
                vBehaviorRetreatL();
                break;
            case  zRetreatR:               //���˺���ת
                vBehaviorRetreatR();
                break;
            case  zTurnLeft :              //��ת
                vBehaviorTurnLeft();
                break;
            case  zTurnRight:              //��ת
                vBehaviorTurnRight();
                break;
            case  zRotate:                 //ԭ����ת
                vBehaviorRotate();
                break;
            case  zAuto :                  //�Զ�
                vBehaviorAuto();
                break;
            case  zChargeBack :            //�س�
                break;
            case  zCharging :              //���ڳ��
                break;
            case  zAlongEdge :              //���ڳ��
                EdgeChrashCnt(0,0); 
                break;
              
        }
        vTaskDelay(5);
    }
}

/*�쳣�ʹ�����*/
void vErrorAbnormalCheckTask(void *p)
{
    
    
    static s16  constAngle; 
    static bool IsCrashToEdge=true; //��ײ��תת�ر߱�־
    static OS_TimeStruct Loc_TimCrash;

    while(1)
    {
        for(;;)
        {
            vTaskDelay(5);
            DropCheckUpdate();
            UltraSonicCheckUpdate();
            if(g_tRobotState.Robotstate.unionAbnormal.AbnormalSta != 0)//���쳣���
            {
                /*��������*/
                if(g_tRobotState.Robotstate.unionAbnormal.AbnIden.LeftWheelAbn ||\
                   g_tRobotState.Robotstate.unionAbnormal.AbnIden.RightWheelAbn  )
                {
                    g_tRobotState.Robotstate.eDirection=zStop;
                    break;
                }else
                {
                    //g_tRobotState.Robotstate.eDirection=zAuto;
                }
                /*��⵽��ײ*/
               if(g_tRobotState.Robotstate.unionAbnormal.AbnIden.LeftCollisionAbn||\
                     g_tRobotState.Robotstate.unionAbnormal.AbnIden.RightCollisionAbn)
                {
                    if(g_tRobotState.Robotstate.eDirection == zForward ||\
                       g_tRobotState.Robotstate.eDirection == zTurnLeft||\
                       g_tRobotState.Robotstate.eDirection == zAuto ||\
                       g_tRobotState.Robotstate.eDirection == zAlongEdge
                    )
                    {
                        g_tRobotState.Robotstate.eDirection=zRetreatL;
                        
                    }
                    else if(g_tRobotState.Robotstate.eDirection == zTurnRight)
                    {
                        g_tRobotState.Robotstate.eDirection=zRetreatR;
                    }

                      if(IsCrashToEdge)
                    {
                        IsCrashToEdge=false;
                        constAngle=CURRENT_ANGLE;  
                    }
                   Loc_TimCrash=GetOSRunTimeNow();  //��ȡ��ǰʱ��
                    
                    
                }

                
                /*��⵽����*/
                if(g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLBAbn||\
                   g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLFAbn||\
                   g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRBAbn||\
                   g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRFAbn)
                {
                    if(g_tRobotState.Robotstate.eDirection == zForward ||\
                       g_tRobotState.Robotstate.eDirection == zTurnLeft||\
                       g_tRobotState.Robotstate.eDirection == zAuto )
                    {
                        g_tRobotState.Robotstate.eDirection=zRetreatL;
                    }
                    else if(g_tRobotState.Robotstate.eDirection == zTurnRight)
                    {
                        g_tRobotState.Robotstate.eDirection=zRetreatR;
                    }
                }
            }

                
            if( TimeOut==SingleTimeoutCheck(Loc_TimCrash,0,0,650))
            {
                 if(!IsCrashToEdge)
                 {
                     g_tRobotState.Robotstate.eDirection=zAlongEdge;
                     IsCrashToEdge=true;
                     EdgeChrashCnt(constAngle,1); 
                     
                 }
              
            }
  
        }
    }
}


//int Edge_Chrash_CNT(s16 FirstAng)
//{
//    
//    static bool IsChrashFirstIn=true;//�Ƿ��һ�ν���
//    static OS_TimeStruct Loc_TimALongEdge;
//    static s16  constAngle,CurrentAng,LastCurrentAng;
//    static uint16_t AlongEdgeAD,lastAlongEdgeAD; //�ر�ADֵ
//    static s16 TurnAngle;//ת���ĽǶ�
//    static uint8_t ucTurnCount;
//    if(IsChrashFirstIn == true)
//    {
//        
//        lastAlongEdgeAD=g_usAnalogSwitch[adcAlong_Edge];
//        constAngle=FirstAng;
//        ucTurnCount=0;
//        IsChrashFirstIn=false;
//    }
//    
//    
//    g_tRobotState.Robotstate.eDirection=zAlongEdge;
//    g_sExpect_speed=TURN_CORNER_SPEED;//��ֵ�ر��ٶ�
//    AlongEdgeAD=g_usAnalogSwitch[adcAlong_Edge];
//    CurrentAng=CURRENT_ANGLE; //���ڽǶ�
//    
////     if(ucTurnCount>100)
////    {
////        IsChrashFirstIn =true;
////        g_tRobotState.Robotstate.eDirection=zAuto;
////        return 0;
////    }
////    if(abs(constAngle-CurrentAng)<100)
////    {
////        IsFirstIn =true;
////        g_tRobotState.Robotstate.eDirection=zAuto;
////        return 1;
////    }
////    
//    
//    
//    if(AlongEdgeAD>800)
//    {
//         g_sExpect_speed=ALONGEDGE_SPEED;//��ֵ�ر��ٶ�
//         if(AlongEdgeAD>1000)
//         {
//              g_sExpect_Angle=CURRENT_ANGLE+300;
//                /*������ת*/
//                if(0 == ucTurn_Angle_Round(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,10))
//                {
//                /*�Ѿ�����ָ���Ƕ�*/
//                DEBUG("TurnRight %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
//                } 
//         }
//        
//         if(abs(AlongEdgeAD-lastAlongEdgeAD)>300)
//        {
//          
//            if(AlongEdgeAD>lastAlongEdgeAD)
//            {
//                printf("��ת\r\n");
//                g_sExpect_Angle=CURRENT_ANGLE+100;
//                /*������ת*/
//                if(0 == ucRoute_Adjust(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed))
//                {
//                /*�Ѿ�����ָ���Ƕ�*/
//                DEBUG("TurnRight %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
//                } 
//            }
//            else
//            {
//                 printf("��ת\r\n");
//                g_sExpect_Angle=CURRENT_ANGLE-100;
//                /*��������ת*/
//                if(0 == ucRoute_Adjust(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed))
//                {
//                /*�Ѿ�����ָ���Ƕ�*/
//                DEBUG("TurnRight %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
//                }  
//            }
//            
//        }
//        else 
//        {
//            //ֱ��
//            g_sExpect_speed=ALONGEDGE_SPEED;//��ֵ�ر��ٶ�
//            ucRoute_Adjust(CURRENT_ANGLE,CURRENT_ANGLE,g_sExpect_speed);
//        }
//    }
//    else
//    {
//       if(abs(AlongEdgeAD-lastAlongEdgeAD)>400&&AlongEdgeAD<1500)
//        {
//            ucTurnCount=200;
//            Loc_TimALongEdge=GetOSRunTimeNow();  //��ȡ��ǰʱ��
//            while(ucTurnCount--)
//            {
//                if( NotTimeOut == SingleTimeoutCheck(Loc_TimALongEdge,0,0,FRONT_OFF_TIME))
//                {
//                    g_sExpect_speed=FORWARD_SPEED;//��ֵǰ���ٶ�
//                    ucRoute_Adjust(CURRENT_ANGLE,CURRENT_ANGLE,g_sExpect_speed);//�Ƕ�ע��
//                }else
//                { 
//                    TurnAngle=CURRENT_ANGLE-450;
//                    /*ǰ��ʱ�䵽�� Ҫת����*/
//                    g_sExpect_Angle=TurnAngle;//�ı������Ƕ�Ϊ ת���Ƕ�
//                    g_sExpect_speed=TURN_CORNER_SPEED;//�ٶ�Ϊת���ٶ�
//                    /*��ת*/
//                    if(0 == RotateAroundAxle(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range,0))
//                    {
//                    }  
//                }
//            }
//        }
//        
//        printf("С1500\r\n");
//        ucTurnCount++;
//        g_sExpect_speed=ALONGEDGE_SPEED;//��ֵ�ر��ٶ�
//        g_sExpect_Angle=CURRENT_ANGLE+100;
//        /*������ת*/
//        if(0 == ucTurn_Angle_Round(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,30))
//        {
//        /*�Ѿ�����ָ���Ƕ�*/
//            DEBUG("TurnRight %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
//        }  

//            
//    }
//    

//    
//    lastAlongEdgeAD=AlongEdgeAD;
//    return 1;
//  
//}

/*******************************************************************************
* Function Name  : int EdgeChrashCnt(s16 FirstAng, bool IsCrash)
* Description    : �ر�
* Input          : FirstAng ��׼�Ƕ�  IsCrash 1Ϊ��ײ����� 
* Output         : None 
* Return         : 0 ��������ִ�� 1 �˳�
*******************************************************************************/
int EdgeChrashCnt(s16 FirstAng, bool IsCrash)
{
    static bool IsFirstIn=true;//�Ƿ��һ�ν���
    static OS_TimeStruct Loc_TimALongEdge; // 
    static OS_TimeStruct Loc_TimTurn;  //ת���ʱ
    static OS_TimeStruct Loc_TimCheck;  //�ɼ���ʱ
    static s16  constAngle,CurrentAng;
    static uint16_t AlongEdgeAD,lastAlongEdgeAD; //�ر�ADֵ
    static uint16_t ucTurnCount=0,ucCrashCouont=0; //ת��������룩����ײ����
    static uint8_t step=1;
    
    vTaskDelay(5);
    
    g_sExpect_speed=ALONGEDGE_SPEED;//��ֵ�ر��ٶ�
    //��һ�ν���
    if(IsFirstIn == true)
    {
        DEBUG("��һ�� %d \r\n",ucTurnCount);
        //��תֱ����ǽ5cm
        ucTurnCount=1;
        step=6;
        Loc_TimTurn=GetOSRunTimeNow();  //��ȡ��ǰʱ�� 
        Loc_TimCheck=GetOSRunTimeNow();  //��ȡ��ǰʱ�� 
        lastAlongEdgeAD=g_usAnalogSwitch[adcAlong_Edge];
        AlongEdgeAD=g_usAnalogSwitch[adcAlong_Edge];
        constAngle=FirstAng; 
        IsFirstIn=false;
    }
    //15ms�ɼ�һ������
    if( TimeOut == MultipleTimeoutCheck(&Loc_TimCheck,0,0,15))
    {
         AlongEdgeAD=g_usAnalogSwitch[adcAlong_Edge];
        CurrentAng=CURRENT_ANGLE;
    }

     //��ײ��� ��ǽ�ǿ���
     if(IsCrash&&g_usAnalogSwitch[adcAlong_Edge]>750)//7cm
    {
        ucCrashCouont++;
        Loc_TimALongEdge=GetOSRunTimeNow();  //��ȡ��ǰʱ��
        //��ǽ̫��
        if(g_usAnalogSwitch[adcAlong_Edge]>1800)
        {
            g_tLeftWheel.ExpectSpeed=-180;
            g_tRightWheel.ExpectSpeed=-80;
            while( NotTimeOut == SingleTimeoutCheck(Loc_TimALongEdge,0,0,FRONT_OFF_TIME))
            {
                vTaskDelay(5);
                
            }       
        }
        //��ת300�Ⱥ����ر�ֵ
        g_sExpect_Angle=CURRENT_ANGLE+300;
        while((AlongEdgeAD>lastAlongEdgeAD+50)&&IsCrash&&g_usAnalogSwitch[adcAlong_Edge]>750)
        {
              vTaskDelay(15);
             if((ucTurn_Angle_Round(CURRENT_ANGLE,g_sExpect_Angle,ALONGEDGE_SPEED,10))==0)
            {
                DEBUG("TurnRight %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
               break;
            }
        }
        step=6;
        ucTurnCount=1;
        
    }
    
    if(0==ucTurnCount)
    {
        ucCrashCouont=0;
    }
    if(ucCrashCouont>3)
    {
        ucCrashCouont=0;
        IsFirstIn=true;
    }
  
//    if(abs(constAngle-CurrentAng)<100)
//    {
//        step=9;
//    }
    
    /* ��ǽ�����ӦADֵ unit ��cm
    
    0   2450
    1   1900
    2   1550
    3   1320
    4   1150
    5   1000
    6   900
    7   830
    */
    
    //ֱ���ر��� ��ǽ2��3cm
    if(0==ucTurnCount)
    {
      //�ر�ֱ��
        if(AlongEdgeAD>1500) //2cm
        {
            //ֱ����ת
           step=2;
        }
        else if(AlongEdgeAD>1300)
        {
            //ֱ��
            step=5;
        }
        else if(AlongEdgeAD>1200&&AlongEdgeAD<1300)
        {
            //ֱ����ת
            step=1;
        }
       
        //�ر�ת��
         if(AlongEdgeAD<1200&&AlongEdgeAD>1000)
        {
            step=1;
            if(AlongEdgeAD>lastAlongEdgeAD+100)
            {
                step=8;
            }
            if(AlongEdgeAD<lastAlongEdgeAD-100)
            {
               step=7;
 
            } 
        }
        else if(AlongEdgeAD<1000)
        {

             if(AlongEdgeAD+200<lastAlongEdgeAD)
            {
                //ǰ��Ȼ����ת
                step=4;
                ucTurnCount=1;
                Loc_TimALongEdge=GetOSRunTimeNow();  //��ȡ��ǰʱ��
                Loc_TimTurn=GetOSRunTimeNow();  //��ȡ��ǰʱ�� 
            }
            else
            {
                //��ת
                step=3;
                ucTurnCount=1;
                Loc_TimTurn=GetOSRunTimeNow();  //��ȡ��ǰʱ�� 
            }

        }

//          //�ر�ֱ��
//            if(AlongEdgeAD>1200)
//            {
//                //ֱ����ת
//                ucRouteAdjustByAlongEdge(CurrentAng,1500,ALONGEDGE_SPEED);
//            }
//            
//            

//            if(AlongEdgeAD<1000)
//            {

//                 if(AlongEdgeAD+300<lastAlongEdgeAD)
//                {
//                    step=4;
//                    ucTurnCount=1;
//                    Loc_TimALongEdge=GetOSRunTimeNow();  //��ȡ��ǰʱ��
//                    Loc_TimTurn=GetOSRunTimeNow();  //��ȡ��ǰʱ�� 
//                }
//                else
//                {
//                    step=3;
//                    ucTurnCount=1;
//                    Loc_TimTurn=GetOSRunTimeNow();  //��ȡ��ǰʱ�� 
//                }
//  
//            }

    }
    
        
    //ת���ʱ unit:s
    if(ucTurnCount>8)
    {
        step=9;
    }  
    
    //�ر��˶�
    switch(step)
    {
        case 1:  //ֱ����ת
        {

            DEBUG("ֱ����ת\r\n");
            g_sExpect_Angle=CURRENT_ANGLE-100;
            /*��������ת*/
            if(0 == ucRoute_Adjust(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed))
            {
            /*�Ѿ�����ָ���Ƕ�*/
            DEBUG("TurnRight and Along %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
            }  
        }
        break;
        
        case 2://ֱ����ת
        {
            DEBUG("ֱ����ת\r\n");
            g_sExpect_Angle=CURRENT_ANGLE+100;
            /*������ת*/
            if(0 == ucRoute_Adjust(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed))
            {
            /*�Ѿ�����ָ���Ƕ�*/
            DEBUG("TurnRight and Along%d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
            } 
            
        }
        break;
        
        case 3:
        {
            //ԭ����ת
//            DEBUG("ԭ����ת\r\n");
            if( TimeOut == MultipleTimeoutCheck(&Loc_TimTurn,0,1,0))
            {
                 ucTurnCount++;
            }
            g_sExpect_Angle=CURRENT_ANGLE-100;
            if((ucTurn_Angle_Round(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,10))==0)
            {
             DEBUG("TurnRight %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
            }
        }
        break;
        
        case 4:     //�������ת��ǰ��Ȼ����ת
        {
//             DEBUG("�����ת\r\n");
            if( TimeOut == MultipleTimeoutCheck(&Loc_TimTurn,0,1,0))
            {
                 ucTurnCount++;
            }
            if(ucCrashCouont>1||ucTurnCount>3)
            {
                step=8;
                ucCrashCouont=1;
                ucTurnCount=1;
            }
            
            if( NotTimeOut == SingleTimeoutCheck(Loc_TimALongEdge,0,0,FRONT_OFF_TIME))
            {
                DEBUG("��תֱ��\r\n");
                ucRoute_Adjust(CURRENT_ANGLE,CURRENT_ANGLE,g_sExpect_speed);//�Ƕ�ע��
            }else
            { 
                /*ǰ��ʱ�䵽�� Ҫת����*/
                g_sExpect_Angle=CURRENT_ANGLE-450;;//�ı������Ƕ�Ϊ ת���Ƕ�
                g_sExpect_speed=ALONGEDGE_SPEED;//�ٶ�Ϊת���ٶ�
                /*��ת*/
                DEBUG("��ת\r\n");
                if(0 == RotateAroundAxle(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range,LWheel))
                {   
                } 
            }
        }
        break;
        
        case 5: //ֱ��
        {
            ucRoute_Adjust(CURRENT_ANGLE,CURRENT_ANGLE,g_sExpect_speed);
        }
        break;
        
        case 6: 
        {   //ԭ����ת
             DEBUG("ԭ����ת\r\n");
            g_sExpect_Angle=CURRENT_ANGLE+100;
            if( TimeOut == MultipleTimeoutCheck(&Loc_TimTurn,0,1,0))
            {
                 ucTurnCount++;
            }         
            if((ucTurn_Angle_Round(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,10))==0)
            {
             DEBUG("TurnRight %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);               
            }
        }
        break;
        
        case 7:
        {
            //��ת
//             DEBUG("����ԭ����ת\r\n");
             g_sExpect_Angle=CURRENT_ANGLE-100;
            if(0 == RotateAroundAxle(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range,LWheel))
            {
                /*�Ѿ�����ָ���Ƕ�*/
                DEBUG("TurnRight %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE); 
            } 
        }
        break;
        case 8:
        {
            //��ת
//              DEBUG("����ԭ����ת\r\n");
            g_sExpect_Angle=CURRENT_ANGLE+100;
            if(0 == RotateAroundAxle(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range,RWheel))
            {
                /*�Ѿ�����ָ���Ƕ�*/
                DEBUG("TurnLeft %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
            } 
        }
        break;
        case 9:     //�˳�
        {
            ucTurnCount=0;
            IsFirstIn=true;
            g_tRobotState.Robotstate.eDirection=zAuto;
            return 1;
        }
    }
    
    //ת�䳬ʱ����
    if(g_usAnalogSwitch[adcAlong_Edge]>1000)
    { 
        ucTurnCount=0;
    }     
    lastAlongEdgeAD=AlongEdgeAD;
    return 0;
  
}


void DropCheckUpdate(void)
{
    if(tDropcheck.bAnyoneFall)
    {
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLBAbn=tDropcheck.bBackLFall;
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLFAbn=tDropcheck.bFrontLFall;
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRBAbn=tDropcheck.bBackRFall;
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRFAbn=tDropcheck.bFrontRFall;
    }else
    {
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLBAbn=0;
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLFAbn=0;
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRBAbn=0;
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRFAbn=0;
    }
}
void UltraSonicCheckUpdate(void)
{
    
    
    g_tRobotState.Robotstate.unionAbnormal.AbnIden.UltraSonicAbn=bGetClashFlag();
    if(g_tRobotState.Robotstate.unionAbnormal.AbnIden.UltraSonicAbn)
        ForwardSpeed=FORWARD_SPEED/2;
    else
        ForwardSpeed=FORWARD_SPEED;
}


#endif

#ifdef WenTao

#endif


