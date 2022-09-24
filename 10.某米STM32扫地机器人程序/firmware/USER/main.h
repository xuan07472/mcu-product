#ifndef __MAIN_H_
#define __MAIN_H_

#include "sys.h"

#define  MYDEBUG 

#ifdef   MYDEBUG     //�Ƿ���봮�ڵ��� ������д���1��Ϣ��� ��������usmart����
#define  DEBUG       printf
#else
#define  DEBUG(...)  (void)(0)
#endif

/* �������� */
typedef enum {
    keyStop      =0,//ֹͣ
    keyStart     =1,//���� 
    keyBackCharg =2,//�س�
    
}KeyCommand_t;

/* �������˶�״̬ */
typedef enum {
    zStop      =0,           //ֹͣ
    zForward   =1,           //ǰ��
    zRetreatL  =2,           //���˺���ת
    zRetreatR  =3,           //���˺���ת
    zTurnLeft  =4,           //��ת
    zTurnRight =5,           //��ת
    zRotate    =6,           //ԭ����ת
    zAuto      =7,           //�Զ�
    zChargeBack=8,           //�س�
    zCharging  =9,           //���ڳ��
    zAlongEdge =10,          //�ر�ģʽ
    zRetreat   =11,          //����
}RobotMotionState_t;

__packed typedef struct{
    
    u8     ucRemainBattery;                //���� 0~100
    RobotMotionState_t  eDirection;        //����
    RobotMotionState_t  PreviousDirection; //�ϴη���
    /*ϵͳ����*/
    __packed union 
    {
        u32 ErrorSta;
        __packed struct 
        {
            u32 BatteryErr    : 1;//��ش���
            u32 LeftWheelErr  : 1;//���ִ���
            u32 RightWheelErr : 1;//���ִ���
            u32 DropLFErr  : 1;//��ǰ���մ���������
            u32 DropLBErr  : 1;//��ǰ���մ���������
            u32 DropRFErr  : 1;//��ǰ���մ���������
            u32 DropRBErr  : 1;//��ǰ���մ���������
            u32 UltraSonicErr  : 1;//����������
            u32 CrashErr: 1;//��ײ��������ס�ȴ���
            u32 RollBrushErr: 1;//��ˢ�����ƺ�ģ���쳣��
            u32 SideBrushErr: 1;//��ˢ�����ƺ�ģ���쳣��
            u32 StrainerExistence: 1;//�����ڲ���
            u32 StrainerDirty: 1;//�����಻��
            u32 EdgeErr: 1;//�رߴ�����
            u32 RobotTilt: 1;//����ʱ��������б
            u32 FanErr: 1;//�����

        }ErrIden;
    }unionEEROR;
    
    /*ϵͳ�쳣*/
    __packed union 
    {
        u32 AbnormalSta;
        __packed struct 
        {
            u32 BatteryAbn    : 1;//��ص͵����쳣
            u32 LeftWheelAbn  : 1;//���������쳣
            u32 RightWheelAbn : 1;//���������쳣
            u32 DropLFAbn  : 1;//��ǰ���մ����������쳣
            u32 DropLBAbn  : 1;//��ǰ���մ����������쳣
            u32 DropRFAbn  : 1;//��ǰ���մ����������쳣
            u32 DropRBAbn  : 1;//��ǰ���մ����������쳣
            u32 UltraSonicAbn     : 1;//��������⵽�ϰ����쳣
            u32 LeftCollisionAbn  : 1;//����ײ
            u32 RightCollisionAbn : 1;//����ײ
            u32 DustBoxAbn        : 1;//���в��� 
            u32 InRechargeStand   : 1;//�Ƿ��ڳ����
        }AbnIden;
    }unionAbnormal;

}RobotState_t;

/***************ͨ��Э���û����ݽṹ**************/                  
__packed typedef struct{    
    uint8_t     ucUserNumber;        //�ͻ����
    uint8_t     ucFirmVersion;       //�̼��汾��
    RobotState_t  Robotstate;        //������״̬
    KeyCommand_t  KeyCommand;        //��������
}UserData;  


extern volatile UserData g_tRobotState;



#endif

