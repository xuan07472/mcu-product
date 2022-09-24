#ifndef BMI160_DATA_H
#define BMI160_DATA_H
#include "stm32f10x.h"


#define CURRENT_ANGLE ((s16)(GetGyroValue_Z()*10))

//�����Ƕ�ȡ����
typedef enum{
    Start = 0,//��ʼ�ɼ�
    Watting,  //�ȴ�״̬
}OperationSlave_e;


typedef struct 
{
	//�����ǡ�XYZ����ĵ�ǰ��̬��
	float X;
	float Y;
	float Z;//ƫ���ǡ�����Ҽ���
}gyrovalue_t;

typedef struct 
{
	//��XYZ�������б��
	signed char X;//ǰ����б�Ƕȡ�ǰ��Ϊ��  β��Ϊ����
	signed char Y;//������б�Ƕȡ�����Ϊ��  ����Ϊ����
	signed char Z;
}accvalue_t;



void SetGyroValue_X(float data);
void SetGyroValue_Y(float data);
void SetGyroValue_Z(float data);

float GetGyroValue_X(void);
float GetGyroValue_Y(void);
float GetGyroValue_Z(void);

void SetAccValue_X(float data);
void SetAccValue_Y(float data);
void SetAccValue_Z(float data);

signed char GetAccValue_X(void);
signed char GetAccValue_Y(void);
signed char GetAccValue_Z(void);

#endif
