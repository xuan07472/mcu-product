
#ifndef BMI160_H_
#define BMI160_H_
 
#include "sys.h"
#include "math.h"
#include "mymath.h"
#include "delay.h"
#include "spi.h"
#include "bmi160_data.h"
#include "main.h"


struct anglefloat_t
{
    float x;
    float y;
    float z;
};

enum GyroState_e
{
    GYRO_INIT,			//初始化状态
    GYRO_GETOFFSET,	    //计算偏移量状态
    GYRO_WORKING,		//正常工作状态
};

extern enum GyroState_e eGyro_State;

void vGyroscopse_task(void *pvParameters);
void BMI160_Init(void);
void vBmi160_InitAttAngle(void);
u8 BMI160_SPI_GetID(void);
void BMI160_SPI_GetOffset(void);
void BMI160_SPI_GetACC(void);
void BMI160_SPI_GetGyro(void);



void vBMI160CalcOffset_Gyro(float data_add_x , float data_add_y , float data_add_z);
u8 ucBmi160_Shake_Reset(float fnewdata,u8 ucResetthreshold);
u8 ucBMI160deal_Shake_Reset(float fnewdata);
float ucBmi160_ToleranceFail_Deal(float offset);


#endif /* BMI160_H_ */
