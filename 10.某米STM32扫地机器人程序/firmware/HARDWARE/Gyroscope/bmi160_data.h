#ifndef BMI160_DATA_H
#define BMI160_DATA_H
#include "stm32f10x.h"


#define CURRENT_ANGLE ((s16)(GetGyroValue_Z()*10))

//陀螺仪读取周期
typedef enum{
    Start = 0,//开始采集
    Watting,  //等待状态
}OperationSlave_e;


typedef struct 
{
	//陀螺仪【XYZ】轴的当前姿态角
	float X;
	float Y;
	float Z;//偏航角【左加右减】
}gyrovalue_t;

typedef struct 
{
	//【XYZ】轴的倾斜角
	signed char X;//前后倾斜角度【前翘为负  尾翘为正】
	signed char Y;//左右倾斜角度【左翘为负  右翘为正】
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
