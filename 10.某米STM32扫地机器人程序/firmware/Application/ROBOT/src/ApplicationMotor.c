#include "applicationmotor.h"

/*
 * Filename   ��applicationmotor.c
 * Author     : pusheng       
 * Version    : 2.0
 * Date       : 2019.11.21 2020.1.14
 * Discription : 
 
 v1.0 �������Ӧ�ú���
 
 v2.0 �����رߴ���Ӧ�ú���
      ���ر߷������ر߲�ֵ���ı��ٶ� ��� ���ر߲�ֵ���ı�����������Ƕ� ��ֱ���ر�Ч������
*/	

/*���ת�� ��λ��ת/�룩*/
u32 g_FanRotationRate;

/***********************************   ����������  *****************************************/


//����ǰ�� ���ǰ�� Cycle��0-500  Խ��ʵ���ٶ�Խ��
   
#define LEFT_WHEEL_FORWARD(Cycle)   LEFT_WHEEL_T3CH4_PWM_CYCLE(Cycle);\
                                    CTRL_WHEEL_L=0;\
                                    g_tLeftWheel.Direction = WheelForward;  
//���ֺ��� ��Ǻ��� 
#define LEFT_WHEEL_RETREAT(Cycle)   LEFT_WHEEL_T3CH4_PWM_CYCLE(Cycle);\
                                    CTRL_WHEEL_L=1;\
                                    g_tLeftWheel.Direction = WheelRetreat;
//����ǰ�� ���ǰ��
#define RIGHT_WHEEL_FORWARD(Cycle)  RIGHT_WHEEL_T3CH3_PWM_CYCLE( Cycle );\
                                    CTRL_WHEEL_R=0;\
                                    g_tRightWheel.Direction = WheelForward;
//���ֺ��� ��Ǻ��� 
#define RIGHT_WHEEL_RETREAT(Cycle)  RIGHT_WHEEL_T3CH3_PWM_CYCLE( Cycle );\
                                    CTRL_WHEEL_R=1;\
                                    g_tRightWheel.Direction = WheelRetreat;
 
#define SLOW_ACCELE_MAX_NUM   (5) //�����ٲ�����λ mm/s


s16 g_sExpect_Angle=0;  //�����ĽǶ�
s16 g_sExpect_speed=200;//�ٶ� mm/s
s16 g_sExpect_range=10; //�Ƕ���Χ +- 1�� ��λ0.1��

/*�������ٶȻ�PID���� λ��ʽ SpeedOut= P*E[n] + I*(E[n]+E[n-1]+E[n-2].....+E[1]))*/
float P_V           = 1;
float I_V           = 0.1;
float D_V           = 0;

__IO float    SpeedLeftUiV;         //�����ٶȻ����ֽ��
__IO float    SpeedRightUiV;        //�����ٶȻ����ֽ��
 
/*�رߴ�����λ�û�PID����  ����ʽ Out = P*( E[n]-E[n-1] ) + I*E[n] + D*(E[n]- 2*E[n-1] + E[n-2])  */
//float P_AE           = 3;
//float I_AE           = 0.1;
//float D_AE           = 0.1;
float P_AE           = 0;
float I_AE           = 8;
float D_AE           = 0;
IncPID_t g_tIncPID={0};
/*******************************************************************************
* Function Name  : usSpeed2Cycle(s16 speed)
* Description    : �ٶ�ת��Ϊ��Ӧ��PWMռ�ձ�
* Input          : speed :mm/s  ��Χ -400~400
* Output         : None
* Return         : ���ռ�ձ� 450~50
*******************************************************************************/
__inline u16 usSpeed2Cycle(s16 loc_Speed)
{
    if(loc_Speed<10 && loc_Speed>-10)//�ٶȵ���10mm/s ������Ӧ����̫�� ֱ�Ӹ�0  ��ص���AD����3000 PWM������Ӧ450-0.
        return 500;//����500ռ�ձ� ���ͣת
    
    if(loc_Speed<-400 && loc_Speed>400)//�ٶȳ���400mm/s �����ٶ����ռ�ձ�
        return 450-400;
    
    if(loc_Speed>0)
        return 450-loc_Speed;
    else
        return 450+loc_Speed;
}

/*******************************************************************************
* Function Name  : vMotionWheel(void)
* Description    : �����ӵ�ʵ��ת�ٿ�������ת�� ʹ�õ�ȫ�ֽṹ�� LeftWheel RightWheel
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vMotionWheel(void)
{
    int16_t PidTemp;
    PidTemp = sSpeed_L_PID( g_tLeftWheel.SoftSpeed, g_tLeftWheel.RealSpeed );//PID������һʱ���ٶ�
    if( PidTemp < 0 )//���ֵ���
    {                         
        LEFT_WHEEL_RETREAT(usSpeed2Cycle(PidTemp));
    } 
    else//����ǰ��
    {
        LEFT_WHEEL_FORWARD(usSpeed2Cycle(PidTemp));
    }
    
    PidTemp = sSpeed_R_PID( g_tRightWheel.SoftSpeed, g_tRightWheel.RealSpeed );

    if( PidTemp < 0 )//���ֺ���
    {               
        RIGHT_WHEEL_RETREAT(usSpeed2Cycle(PidTemp));
    } 
    else//����ǰ��
    {
        RIGHT_WHEEL_FORWARD(usSpeed2Cycle(PidTemp));
    }            
}
/*******************************************************************************
* Function Name  : sSpeed_R_PID(int16_t RefV,int16_t FdbV)
* Description    : �����ٶȻ�
* Input          : RefV-�����ٶ�ָ�FdbV-��ǰ�ٶȷ�������λmm/s
* Output         : None
* Return         : ������һ��ʱ��Ӧ�õ��ٶ�
*******************************************************************************/
int16_t sSpeed_R_PID(int16_t RefV,int16_t FdbV)
{
    int16_t  UpV=0;
    int16_t  ErrV,OutTempV;
    int16_t  OutputV;

    ErrV = RefV - FdbV;//����ٶ�

    UpV = P_V*ErrV;      // �������������
    
    SpeedRightUiV = SpeedRightUiV + ((float)(UpV*I_V));//����������ۼӲ���

    
    /*���ַ�Χ���ƣ����������ۼ�*/
    if(SpeedRightUiV>UIMAXV)
    {
        SpeedRightUiV=UIMAXV;
    }else
    if(SpeedRightUiV<UIMINV)
    {
        SpeedRightUiV=UIMINV;
    }
    
    OutTempV = UpV+(int16_t)SpeedRightUiV;  //PI

    /*����ٶȷ�Χ����*/
    if (OutTempV > OUTMAXV)
    {
        OutputV =  OUTMAXV;
    }
    else if (OutTempV < OUTMINV)
    {
        OutputV = OUTMINV;
    }
    else
    {
        OutputV = OutTempV;
    }

    if(RefV>=0)
    {
        if(OutputV<0) OutputV=1;
    }
    else if(RefV<0)
    {
        if(OutputV>0) OutputV=-1;
    }
    return OutputV;
}
/*******************************************************************************
* Function Name  : sSpeed_L_PID(int16_t RefV,int16_t FdbV)
* Description    : �����ٶȻ�
* Input          : RefV-�����ٶ�ָ�FdbV-��ǰ�ٶȷ�������λmm/s
* Output         : None
* Return         : ������һ��ʱ��Ӧ�õ��ٶ�
*******************************************************************************/
int16_t sSpeed_L_PID(int16_t RefV,int16_t FdbV)
{
    int16_t  UpV=0;
    int16_t  ErrV,OutTempV;
    int16_t  OutputV;

    ErrV = RefV - FdbV;//����ٶ�

    UpV = P_V*ErrV;      // �������������
    
    SpeedLeftUiV = SpeedLeftUiV + ((float)(UpV*I_V));//����������ۼӲ���

    /*���ַ�Χ���ƣ����������ۼ�*/
    if(SpeedLeftUiV>UIMAXV)
    {
        SpeedLeftUiV=UIMAXV;
    }else
    if(SpeedLeftUiV<UIMINV)
    {
        SpeedLeftUiV=UIMINV;
    }
    OutTempV = UpV+(int16_t)SpeedLeftUiV;  //PI

    /*����ٶȷ�Χ����*/
    if (OutTempV > OUTMAXV)
    {
        OutputV =  OUTMAXV;
    }
    else if (OutTempV < OUTMINV)
    {
        OutputV = OUTMINV;
    }
    else
    {
        OutputV = OutTempV;
    }
    
    if(RefV>=0)
    {
        if(OutputV<0) OutputV=1;
    }
    else if(RefV<0)
    {
        if(OutputV>0) OutputV=-1;
    }
    
    return OutputV;
}

/*******************************************************************************
* Function Name  : vSoftSpeedAdjust
* Description    : ������
* Input          :
* Output         : None
* Return         : None
*******************************************************************************/
void vSoftSpeedAdjust(void)//�������� 3ms
{
    if(g_tLeftWheel.ExpectSpeed != g_tLeftWheel.SoftSpeed)
    {
        if(g_tLeftWheel.ExpectSpeed > g_tLeftWheel.SoftSpeed)
            g_tLeftWheel.SoftSpeed+=SLOW_ACCELE_MAX_NUM;
        else 
            g_tLeftWheel.SoftSpeed-=SLOW_ACCELE_MAX_NUM;
    }
    if(g_tRightWheel.ExpectSpeed != g_tRightWheel.SoftSpeed)
    {
        if(g_tRightWheel.ExpectSpeed > g_tRightWheel.SoftSpeed)
            g_tRightWheel.SoftSpeed+=SLOW_ACCELE_MAX_NUM;
        else 
            g_tRightWheel.SoftSpeed-=SLOW_ACCELE_MAX_NUM;
    }    
}

/*******************************************************************************
* Function Name  : s16 iIncPIDCalc(s16 NewData)
* Description    : �رߴ����� ����ʽPID ����
* Input          : NewData �µõ��رߴ�����de �������
* Output         : None
* Return         : �ٶȸı�ֵ
*******************************************************************************/
s16 iIncPIDCalc(s16 NewData)
{
    s16 loc_ChangeSpeed;


    loc_ChangeSpeed = P_AE*( NewData-g_tIncPID.LastData ) + \
                      I_AE*NewData + D_AE*(NewData- (2*g_tIncPID.LastData) + g_tIncPID.PreData);

    g_tIncPID.PreData=g_tIncPID.LastData;
    g_tIncPID.LastData=NewData;

    return loc_ChangeSpeed;
}
/*******************************************************************************
* Function Name  : ucRouteAdjustByAlongEdgeDistance(s16 loc_FbAngle,s16 loc_ExpAngle,s16 loc_Speed)
* Description    : �ر�·��΢�� 
* Input          : loc_FbValue ���رߴ������ľ���ֵ��mm�� loc_ExpValue �����ľ���ֵ��mm��  loc_Speed �ٶȣ�mm/s��
* Output         : None 
* Return         : 0 ����ִ�гɹ� 1 ԭ��ת��δ��� ����ʧ��
*******************************************************************************/
u8 ucRouteAdjustByAlongEdgeDistance(u16 loc_FbValue,u16 loc_ExpValue,s16 loc_Speed)
{
    s16 loc_DiffValue = loc_ExpValue - loc_FbValue;//��ֵ
    s16 loc_Divisor;//�Ƕ� ת �ٶ�ֵ 
    
    loc_Divisor=iIncPIDCalc(loc_DiffValue);//�ٶ�ֵ�ĵ�����׼ 

    
    if( abs(loc_Divisor) > 100)//�Ƕ�̫���� �ٶȵ���ֵ���ܳ�������ٶ�
    {
        if(loc_Divisor>0) 
            loc_Divisor=100;
        else
            loc_Divisor=-100;
    }
    /*
        ���loc_Diff_Angle С��0 ��ʾ��ʾ����תȥ�ӽ�Ŀ��Ƕ� �����ٶȼ�С �����ٶ�����
        ���loc_Diff_AngleС��0  ��loc_DivisorҲС��0 Ϊ����
        ����Ǻ��� ��loc_SpeedΪ����������ԽС ���ٶ�Խ�� 
    */
//		g_sExpect_Angle = CURRENT_ANGLE+loc_Divisor;
//		ucRoute_Adjust(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed);
g_tLeftWheel.ExpectSpeed=    loc_Speed - loc_Divisor;
g_tRightWheel.ExpectSpeed=   loc_Speed + loc_Divisor;
    return 0;
   
}
/*******************************************************************************
* Function Name  : ucRouteAdjustByAlongEdge(s16 loc_FbAngle,s16 loc_ExpAngle,s16 loc_Speed)
* Description    : �ر�·��΢�� 
* Input          : loc_FbValue ���رߴ�������ADֵ  loc_ExpValue ������ADֵ loc_Speed �ٶ�
* Output         : None 
* Return         : 0 ����ִ�гɹ� 1 ԭ��ת��δ��� ����ʧ��
*******************************************************************************/
u8 ucRouteAdjustByAlongEdge(u16 loc_FbValue,u16 loc_ExpValue,s16 loc_Speed)
{
    s16 loc_DiffValue = loc_ExpValue - loc_FbValue;//��ֵ
    s16 loc_ChangeSpeed;//��Ҫ�ı���ٶ�ֵ    

    if(loc_DiffValue> 500)
        loc_DiffValue=500;
    if(loc_DiffValue< -500)
        loc_DiffValue=-500;
        
    loc_ChangeSpeed=iIncPIDCalc(loc_DiffValue);
    
    if( abs(loc_ChangeSpeed) > 80)//�Ƕ�̫���� �ٶȵ���ֵ���ܳ�������ٶ�
    {
        if(loc_ChangeSpeed>0) 
            loc_ChangeSpeed=80;
        else
            loc_ChangeSpeed=-80;
    }
    /*
        ���loc_Diff_Angle С��0 ��ʾ��ʾ����תȥ�ӽ�Ŀ��Ƕ� �����ٶȼ�С �����ٶ�����
        ���loc_Diff_AngleС��0  ��loc_DivisorҲС��0 Ϊ����
        ����Ǻ��� ��loc_SpeedΪ����������ԽС ���ٶ�Խ�� 
    */
    g_tLeftWheel.ExpectSpeed=    loc_Speed + loc_ChangeSpeed;
    g_tRightWheel.ExpectSpeed=   loc_Speed - loc_ChangeSpeed;
       
    return 0;
   
}
/*******************************************************************************
* Function Name  : ucRoute_Adjust(s16 loc_FbAngle,s16 loc_ExpAngle,s16 loc_Speed)
* Description    : ·��΢�� ֧��ǰ������
* Input          : loc_FbAngle ��ǰ�Ƕ� loc_ExpAngle �����Ƕ�  ��Χ1800~-1790 ��λ1/10��  
                   ��ǰ�ǶȺ�Ŀ��Ƕ� ����30�������ԭ����ת����
                   loc_Speed ��λmm/s ��Χ-400-400 �ٶ���ֵ��ʾǰ�� ��ֵ��ʾ����
* Output         : None
* Return         : 0 ����ִ�гɹ� 1 ԭ��ת��δ��� ����ʧ��
*******************************************************************************/
u8 ucRoute_Adjust(s16 loc_FbAngle,s16 loc_ExpAngle,s16 loc_Speed)
{
    static u8 loc_ucIsTurn=0;//�Ƿ���Ҫԭ��ת�� 0 ����Ҫ
    s16 loc_Divisor;//�Ƕ�ת�ٶ�ֵ
    u8 ERR=0;
    
    s16 loc_Diff_Angle = loc_ExpAngle - loc_FbAngle;//�����ǶȺ͵�ǰʵ�ʽǶȵĲ�ֵ ����ֵ��ʾ���󿿽� �����ԭ�� 

    if( loc_ExpAngle>3600 || loc_ExpAngle< -3600)//����ĽǶ�
    {
        printf("ERROR!! %s, %d",__FILE__,__LINE__);
        return 2;//ֱ�ӷ���
    }
    
    if(loc_Diff_Angle<-1800)//������Ȧ �÷���Ƕȸ���
    {
        loc_Diff_Angle=loc_Diff_Angle+3600;
    }
    else if(loc_Diff_Angle>1800)
    {
        loc_Diff_Angle=loc_Diff_Angle-3600;
    }
        
    if( abs(loc_Diff_Angle) > 300)//�ǶȲ�����30��  
    {
        loc_ucIsTurn=1;//�����Ҫԭ��ת��        
    }
    
    if( loc_ucIsTurn )//ԭ��ת��
    {
        if(ucTurn_Angle_Round_NoCheak(loc_Diff_Angle,abs(loc_Speed),g_sExpect_range) == 0)//�Ƕ���Χ1��
        {
            loc_ucIsTurn=0;//ԭ��ת�����
            
            /*ת��������趨�����ٶȷ���  ucTurn_Angle_Round()����Ŀ��ǶȺ��ֹͣ������ 
            ��������ǰ���ǶȻ���һֱ�����ϴε�״̬��һ��ǰ��һ�����ˣ�,��������ֱ������Ϊȷ��״̬ �����������*/
            if(loc_Speed>=0)
            {
                g_tLeftWheel.Direction=WheelForward;//�ٶ���ֵ ��ʾǰ��
                g_tRightWheel.Direction=WheelForward;
            }else
            {
                g_tLeftWheel.Direction=WheelRetreat;//��ʾ����
                g_tRightWheel.Direction=WheelRetreat;
            }
        }else
            return 1;//ԭ��ת����δ����
    }else
    {
        loc_Divisor=loc_Diff_Angle/2;//���Ƕ�ֵ����2 ��Ϊ�ٶ�ֵ�ĵ�����׼ 
        
        if( abs(loc_Divisor) > abs(loc_Speed))//�Ƕ�̫���� �ٶȵ���ֵ���ܳ�������ٶ�
        {
            if(loc_Divisor)
                loc_Divisor=abs(loc_Speed);//������ֵ�����ֵ
            else
                loc_Divisor= - abs(loc_Speed);//������ֵ�����ֵ
            
            //printf("loc_Divisor %d,loc_Speed %d\r\n",loc_Divisor,loc_Speed);
            //printf("err! File Path:%s Line��%d\r\n ",__FILE__,__LINE__);
            ERR = 3;
        }
        /*
            ���loc_Diff_Angle С��0 ��ʾ��ʾ����תȥ�ӽ�Ŀ��Ƕ� �����ٶȼ�С �����ٶ�����
            ���loc_Diff_AngleС��0  ��loc_DivisorҲС��0 Ϊ����
            ����Ǻ��� ��loc_SpeedΪ����������ԽС ���ٶ�Խ�� 
        */
        g_tLeftWheel.ExpectSpeed=    loc_Speed - loc_Divisor;
        g_tRightWheel.ExpectSpeed=   loc_Speed + loc_Divisor;
 
 
    }        
    return ERR;
   
}
/*******************************************************************************
* Function Name  : ucTurn_Angle_Round(s16 loc_Current_Angle,s16 loc_Expect_Angle,u16 loc_Speed,u16 loc_AngRange)
* Description    : �����˴ӵ�ǰ��loc_Current_Angle �Ը����ٶ� loc_Speed ת�������Ƕȣ�ԭ��ת�� 
* Input          : current_Angle��ǰ�Ƕ� Expect_Angle�����Ƕ� ��λ1/10�� 1800~-1790  �ٶȵ�λmm/s ��Χ100-400
                   loc_AngRange �Ƕ�������Χ ��λ1/10��
* Output         : None
* Return         : 0 ���������Ƕ� 1 ��δ����  2 �Ƕȴ��� 3�ٶȴ���
*******************************************************************************/
u8 ucTurn_Angle_Round(s16 loc_Current_Angle,s16 loc_Expect_Angle,u16 loc_Speed,u16 loc_AngRange)
{
 
    s16 Diff_Angle = loc_Expect_Angle - loc_Current_Angle;//�����ǶȺ͵�ǰʵ�ʽǶȵĲ�ֵ ����ֵ��ʾ���󿿽� �����ԭ�� 
    
    if( loc_Expect_Angle>3600 || loc_Expect_Angle< -3600)//����ĽǶ�  �ᵼ�°�Ȧ�㷨��׼
    {
        printf("ERROR!! %s, %d",__FILE__,__LINE__);
        return 2;//ֱ�ӷ��ش���
    }
        
    if( loc_Speed>400)//������ٶ�
    {
        printf("ERROR!! %s, %d %d",__FILE__,__LINE__,loc_Speed);
        return 3;//ֱ�ӷ��ش���
    }
    
    if(Diff_Angle<-1800)//������Ȧ �÷���Ƕȸ���
    {
        Diff_Angle=Diff_Angle+3600;
    }
    else if(Diff_Angle>1800)
    {
        Diff_Angle=Diff_Angle-3600;
    }
        
    if(Diff_Angle < 0-loc_AngRange)//�����ֵ�Ƕ�С��1�� ��ʾ����תȥ�ӽ�Ŀ��Ƕ�    +-loc_AngRange��Χ���������
    {
        if(Diff_Angle > -100)//���Ƕ�С��10���� ����
        {
            g_tLeftWheel.ExpectSpeed=    -Diff_Angle;//100-0
            g_tRightWheel.ExpectSpeed=   Diff_Angle;
        }else 
        {
            g_tLeftWheel.ExpectSpeed=    loc_Speed;//ȫ�ٿ���
            g_tRightWheel.ExpectSpeed=  -loc_Speed;
        }
    }
    else if(Diff_Angle > 0+loc_AngRange)
    {
        if(Diff_Angle < 100)//���Ƕ�С��10���� ����
        {
            g_tLeftWheel.ExpectSpeed=   -Diff_Angle;//100-0
            g_tRightWheel.ExpectSpeed=  Diff_Angle;
        }else 
        {
            g_tLeftWheel.ExpectSpeed=  -loc_Speed;
            g_tRightWheel.ExpectSpeed=  loc_Speed;
        }
    }
    else//Ŀ��ǶȺ�ʵ�ʽǶ�����Χ�� ֹͣ���
    {   
        g_tLeftWheel.ExpectSpeed=  0;
        g_tRightWheel.ExpectSpeed= 0;
        return 0;
    }  
        return 1;
}
/*******************************************************************************
* Function Name  : u8 ucTurn_Angle_Round_NoCheak(s16 loc_Diff_Angle,u16 loc_Speed,u16 loc_AngRange)
* Description    : �������ж����Ƕ�Ȼ�󿿽�Ŀ��Ƕ� �������Ƕȣ�ԭ��ת��,�����Ƕȵ���Ч�� ����ע��
                   �������Ŀ��ǶȺ������Ƕȿ��Բο����������ucTurn_Angle_Round();

                   ת���ٶ���Ҫ���� ���ٽǶȣ�����speed�������200mm/s !!! Ҳ���Լ�С���ٽǶ�Ҫ�����С��ת���ٶ�

* Input          : loc_Diff_Angle���Ƕ� ��λ1/10�� 1800~-1790  ��ֵ ��ʾΪ��ת������ ��ֵ ��ת
                   loc_AngRange �Ƕ�������Χ ��λ1/10��  �ٶȵ�λmm/s ��Χ100-400
* Output         : None
* Return         : 0 ���������Ƕ� 1 ��δ���� 
*******************************************************************************/
u8 ucTurn_Angle_Round_NoCheak(s16 loc_Diff_Angle,u16 loc_Speed,u16 loc_AngRange)
{  
    if( (loc_Diff_Angle > -200) && (loc_Diff_Angle < 200) )//���Ƕ�С��20���� ����
    {
        /*loc_Diff_Angle ��ֵ Ϊ��ת�� ��ֵ ��ת*/
        g_tLeftWheel.ExpectSpeed=   -loc_Diff_Angle;//100-0
        g_tRightWheel.ExpectSpeed=   loc_Diff_Angle;
        
        //�����ֵ�Ƕ��ڷ�Χֵ  ��ʾ�Ѿ��ﵽĿ��Ƕ�   +-loc_AngRange��Χ���������
        if( (loc_Diff_Angle > -loc_AngRange) && (loc_Diff_Angle < +loc_AngRange))  
        {
            g_tLeftWheel.ExpectSpeed=  0;
            g_tRightWheel.ExpectSpeed= 0;
            return 0;
        }
    }else//����Ķ��أ��Ӵ����ȣ�����ٶ�ת��
    {
        if(loc_Diff_Angle < 0)//��ת
        {
            g_tLeftWheel.ExpectSpeed=    loc_Speed;
            g_tRightWheel.ExpectSpeed=  -loc_Speed;
        }
        else if(loc_Diff_Angle > 0)
        {
            g_tLeftWheel.ExpectSpeed=  -loc_Speed;
            g_tRightWheel.ExpectSpeed=  loc_Speed;
        } 
    }
        return 1;
}

/*******************************************************************************
* Function Name  : ucRotateAroundAxle(s16 loc_Current_Angle,s16 loc_Expect_Angle,u16 loc_Speed,u16 loc_AngRange)
* Description    : �����˴ӵ�ǰ��loc_Current_Angle �Ը����ٶ� loc_Speed ת�������Ƕ�
                   ��������ΪԲ��ת�� һ�����Ӷ� һ�������� 
* Input          : current_Angle��ǰ�Ƕ� Expect_Angle�����Ƕ� ��λ1/10�� 1800~-1790  �ٶȵ�λmm/s ��Χ100-400
                   loc_AngRange �Ƕ�������Χ ��λ1/10��
                   Wnum 0 LWheel ����ת�� 1 ����ת��
* Output         : None
* Return         : 0 ���������Ƕ� 1 ��δ����  2 ����5������  3 �Ƕȴ��� 4 �ٶȴ���
*******************************************************************************/
u8 ucRotateAroundAxle(s16 loc_CurrAngle,s16 loc_ExpAngle,u16 loc_Speed,u16 loc_AngRange,WheelNum_e Wnum)
{
#define NEED_RETREAT 0 //�Ƿ���Ҫ���� 0 ����Ҫ

#if (NEED_RETREAT == 1)
    s16 Diff_Angle = loc_ExpAngle - loc_CurrAngle;//�����ǶȺ͵�ǰʵ�ʽǶȵĲ�ֵ ����ֵ��ʾ���󿿽� �����ԭ�� 
    
    if( loc_ExpAngle>3600 || loc_ExpAngle< -3600)//����ĽǶ�  �ᵼ�°�Ȧ�㷨��׼
        return 3;//ֱ�ӷ��ش��� 
    
    if( loc_Speed>400)//������ٶ�
    {
        printf("ERROR!! %s, %d",__FILE__,__LINE__);
        return 4;//ֱ�ӷ��ش���
    }
    
    if(Diff_Angle<-1800)//������Ȧ �÷���Ƕȸ���
    {
        Diff_Angle=Diff_Angle+3600;
    }
    else if(Diff_Angle>1800)
    {
        Diff_Angle=Diff_Angle-3600;
    }
   
    if( (Diff_Angle > -loc_Speed) && (Diff_Angle < loc_Speed) )//���Ƕ�С��10���� ���١��ٶ�Խ�󣬼��ٽǶ�Խ�� 
    {
        /*Diff_Angle ��ֵ Ϊ��ת�� ��ֵ ��ת*/
        if(LWheel == Wnum)
        {
            g_tLeftWheel.ExpectSpeed=   -Diff_Angle;//100-0
            g_tRightWheel.ExpectSpeed=   0;
        }else
        {
            g_tLeftWheel.ExpectSpeed=    0;//100-0
            g_tRightWheel.ExpectSpeed=   Diff_Angle;
        }
        //�����ֵ�Ƕ��ڷ�Χֵ  ��ʾ�Ѿ��ﵽĿ��Ƕ�   +-loc_AngRange��Χ���������
        if( (Diff_Angle > -loc_AngRange) && (Diff_Angle < +loc_AngRange))  
        {
            g_tLeftWheel.ExpectSpeed=  0;
            g_tRightWheel.ExpectSpeed= 0;
            return 0;
        }
        if((Diff_Angle > -50) && (Diff_Angle < 50))//�����5������
        {
            return 2;
        }
    }else//����Ķ��أ��Ӵ����ȣ�����ٶ�ת��
    {
        if(LWheel == Wnum)
        {
            if(Diff_Angle>=0)//��ת
            {
                g_tLeftWheel.ExpectSpeed=    -loc_Speed;
                g_tRightWheel.ExpectSpeed=   0;
            }else
            {
                g_tLeftWheel.ExpectSpeed=    loc_Speed;
                g_tRightWheel.ExpectSpeed=   0;
            }
        }else
        {
            if(Diff_Angle>=0)//��ת
            {
                g_tLeftWheel.ExpectSpeed=    0;
                g_tRightWheel.ExpectSpeed=   loc_Speed;
            }else
            {
                g_tLeftWheel.ExpectSpeed=    0;
                g_tRightWheel.ExpectSpeed=   -loc_Speed;
            }
        }
    }
    return 1;    
#endif
    
#if (NEED_RETREAT == 0)
    s16 Diff_Angle = loc_ExpAngle - loc_CurrAngle;//�����ǶȺ͵�ǰʵ�ʽǶȵĲ�ֵ ����ֵ��ʾ���󿿽� �����ԭ�� 
    
    if( loc_ExpAngle>3600 || loc_ExpAngle< -3600)//����ĽǶ�  
        return 3;//ֱ�ӷ��ش��� 
    
    if( loc_Speed>400)//������ٶ�
    {
        printf("ERROR!! %s, %d",__FILE__,__LINE__);
        return 4;//ֱ�ӷ��ش���
    }
    if(Wnum == LWheel)//������ǰת�� ���Ƕ�ȫ��ת���ɸ�ֵ����ʾ��ת
    {
        if(Diff_Angle>0)
        {
            Diff_Angle=Diff_Angle-3600;
        }
    }else
    {
        if(Diff_Angle<0)
        {
            Diff_Angle=Diff_Angle+3600;
        }
    }

    if( (Diff_Angle > -loc_Speed) && (Diff_Angle < loc_Speed) )//���Ƕ�С��10���� ���١��ٶ�Խ�󣬼��ٽǶ�Խ�� 
    {
        /*Diff_Angle ��ֵ Ϊ��ת�� ��ֵ ��ת*/
        if(LWheel == Wnum)
        {
            g_tLeftWheel.ExpectSpeed=    abs(Diff_Angle);//100-0
            g_tRightWheel.ExpectSpeed=   0;
        }else
        {
            g_tLeftWheel.ExpectSpeed=    0;
            g_tRightWheel.ExpectSpeed=   abs(Diff_Angle);
        }
        //�����ֵ�Ƕ��ڷ�Χֵ  ��ʾ�Ѿ��ﵽĿ��Ƕ�   +-loc_AngRange��Χ���������
        if( (Diff_Angle > -loc_AngRange) && (Diff_Angle < +loc_AngRange))  
        {
            g_tLeftWheel.ExpectSpeed=  0;
            g_tRightWheel.ExpectSpeed= 0;
            return 0;
        }
        if((Diff_Angle > -50) && (Diff_Angle < 50))//�����5������
        {
            return 2;
        }
    }else//����Ķ��أ��Ӵ����ȣ�����ٶ�ת��
    {
        if(LWheel == Wnum)
        {
            g_tLeftWheel.ExpectSpeed=    loc_Speed;
            g_tRightWheel.ExpectSpeed=   0;

        }else
        {
            g_tLeftWheel.ExpectSpeed=    0;
            g_tRightWheel.ExpectSpeed=   loc_Speed;

        }
    }
    return 1;    
#endif
}


/*******************************************************************************
* Function Name  : vDataMemoryCopy
* Description    : ���ݿ���
* Input          : *pusTarget��Ŀ�껺���ַ��*pusSource��Դ��ַ��ucLength�����ݳ���
* Output         : None
* Return         : None
*******************************************************************************/
void vDataMemoryCopy( uint16_t *pusTarget, uint16_t *pusSource, uint8_t ucLength )
{
    uint8_t i;
    
    for(i=0;i<ucLength;i++)
    {
        pusTarget[i] = pusSource[i];
    }
    
}
/*******************************************************************************
* Function Name  : usGetMiddleValue
* Description    : ��ȡһά�����С�м�ֵ���� �����һ����������
* Input          : *pusBuffer�������׵�ַ��ucLength�����ݳ���
* Output         : None
* Return         : usMiddle���м�ֵ
*******************************************************************************/
__inline uint16_t usGetMiddleValue( uint16_t *pusBuffer, uint8_t ucLength )
{
    uint8_t i;
    uint16_t usMiddle,usMax,usCount = 0,usPoint;
    
    while( usCount <= (ucLength/2) )
    {
        usPoint = 0;
        usMax = pusBuffer[0];
        for( i=1;i<ucLength;i++ ) 
        {      
            if( pusBuffer[i] > usMax ) 
            {
                usMax = pusBuffer[i];
                usPoint = i;
            }                
            
        }
        usMiddle = usMax;
        pusBuffer[usPoint] = 0;   
        usCount++;       
    }
    
    return usMiddle;
}
/*******************************************************************************
* Function Name  : usGetWheelSpeed
* Description    : ��ȡ�����ٶȣ�������������С�м�ֵ��Ϊ����ʱ��
* Input          :
* Output         : None
* Return         : None
*******************************************************************************/
int16_t usGetWheelSpeed( uint16_t usBuffer[], WheelDirection_e Direction ) 
{ 

    int16_t usSpeed;     
    uint16_t usArray[EncodeNumber];
        
    vDataMemoryCopy( usArray, usBuffer, EncodeNumber );               
    usSpeed = SINGLE_ENCODE_DISTANCE/usGetMiddleValue( usArray, EncodeNumber );

    if( Direction == WheelRetreat )
    {
        usSpeed = -usSpeed;
    }
    else if( Direction == WheelStop )
    {
        usSpeed = 0;
    }
    
    return usSpeed;
} 

/*******************************************************************************
* Function Name  : vMotorSpeedUpdateTask(void *p)
* Description    : ����ٶȸ�������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vMotorSpeedUpdateTask(void *p)
{
    while(1)
    {
        if(g_tLeftWheel.TimeOut++ > 10) //����10�� ��û�в�������
            g_tLeftWheel.RealSpeed=0;  //�ٶȱ��Ϊ0
        else        
            g_tLeftWheel.RealSpeed = usGetWheelSpeed( g_tLeftWheel.usEncodeBuffer, g_tLeftWheel.Direction );

        if(g_tRightWheel.TimeOut++ > 10) 
            g_tRightWheel.RealSpeed=0;
        else  
            g_tRightWheel.RealSpeed = usGetWheelSpeed( g_tRightWheel.usEncodeBuffer, g_tRightWheel.Direction );
        
        g_FanRotationRate=g_FanEncodeCount*1000/SPEED_UPDATE_CYCLE/FAN_ENCODE_COUNT;//���ת��
        g_FanEncodeCount=0;
        
        vTaskDelay(SPEED_UPDATE_CYCLE);
    }    
}
/*******************************************************************************
* Function Name  : vAllMotorMotionTask(void *p)
* Description    : ɨ�ػ����������еĵ�����˶�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vAllMotorMotionTask(void *p)
{
    while(1)
    {
//        DataScope_Get_Channel_Data(g_tLeftWheel.RealSpeed,1);
//        DataScope_Get_Channel_Data(g_tRightWheel.RealSpeed,2);
//        DataScope_Get_Channel_Data(g_tLeftWheel.SoftSpeed,3);
//        DataScope_Get_Channel_Data(g_tRightWheel.SoftSpeed,4);
//        vWaveFromOut();
        vMotionWheel();//�������ٶȵ���
        vTaskDelay(10);
    }
}

/*******************************************************************************
* Function Name  : vSoftSpeedAdvanceTask(void *p)
* Description    : ���������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vSoftSpeedAdvanceTask(void *p)
{
    while(1)
    {
        vSoftSpeedAdjust();
        vTaskDelay(2);
    }
}

/*************************   ���  *******************************/

/*******************************************************************************
* Function Name  : uiGetFanRotationRate(void)
* Description    : ��ȡ���ת��
* Input          : None
* Output         : ת�� ��λ r/s
* Return         : None
*******************************************************************************/
u32 uiGetFanRotationRate(void)
{
    return g_FanRotationRate;
}
/*******************************************************************************
* Function Name  : vSetFanSpeed(void)
* Description    : ���÷��ת��
* Input          : None
* Output         : PWMռ�ձ� 0-500  Խ��Խ��
* Return         : None
*******************************************************************************/
void vSetFanSpeed(u16 PWMPulse)
{
    if(PWMPulse)
        FAN_POWER_UP
    else     
        FAN_POWER_DOWN
    
    FAN_T5CH2_PWM_CYCLE(PWMPulse); 
}
    /*************************   ��ˢ  *******************************/

/*******************************************************************************
* Function Name  : vSetSideBrushSpeed(void)
* Description    : ���ñ�ˢת��
* Input          : None
* Output         : PWMռ�ձ� 0-500  Խ��Խ�� ,Dir���� 1Ϊ��
* Return         : None
*******************************************************************************/
void vSetSideBrushSpeed(u8 Dir,u16 PWMPulse)
{
    if(Dir)
        SIDE_BRUSH_CTRL=1;
    else     
        SIDE_BRUSH_CTRL=0;
    
    SIDE_BRUSH_T4CH3_PWM_CYCLE(PWMPulse); 
}

    /*************************   ��ˢ  *******************************/
/*******************************************************************************
* Function Name  : vSetRollBrushSpeed(void)
* Description    : ���ù�ˢת��
* Input          : None
* Output         : PWMռ�ձ� 0-500  Խ��Խ�� ,Dir���� 1Ϊ��
* Return         : None
*******************************************************************************/
void vSetRollBrushSpeed(u8 Dir,u16 PWMPulse)
{
    if(Dir)
        ROLL_BRUSH_CTRL=0;
    else     
        ROLL_BRUSH_CTRL=1;
    
    ROLL_BRUSH_T4CH2_PWM_CYCLE(PWMPulse); 
}

