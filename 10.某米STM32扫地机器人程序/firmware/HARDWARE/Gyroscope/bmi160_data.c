/**
 * @Filename   ：bmi160_data.c
 * @Author     : huangwentao       
 * @Version    : 1.0
 * @Date       : 2019/11/9
 * @Discription : 陀螺仪全局数据，外部通过这里的函数访问陀螺仪数据
 * @hardware connect:
 *          -------------------------------------
 */

#include "bmi160_data.h"
gyrovalue_t GyroValue_t;//陀螺仪角度数据【XYZ】
accvalue_t AccValue_t;//加速度计数据【XYZ】


/**
  * @Description  SetValue更新陀螺仪数据
  * @param  data  新数据
  * @retval 无
  */
void SetGyroValue_X(float data)
{
	GyroValue_t.X = data;
}

void SetGyroValue_Y(float data)
{
	GyroValue_t.Y = data;
}

void SetGyroValue_Z(float data)
{
	GyroValue_t.Z = data;
}

void SetAccValue_X(float data)
{
	AccValue_t.X = data;
}

void SetAccValue_Y(float data)
{
	AccValue_t.Y = data;
}

void SetAccValue_Z(float data)
{
	AccValue_t.Z = data;
}

/**
  * @Description  GetValue获取陀螺仪数据
  * @param  无
  * @retval 旋转角度/倾斜角度数据
  */
float GetGyroValue_X(void)
{
	return GyroValue_t.X;
}

float GetGyroValue_Y(void)
{
	return GyroValue_t.Y;
}

float GetGyroValue_Z(void)
{
	return GyroValue_t.Z;
}

signed char GetAccValue_X(void)
{
	return AccValue_t.X;
}

signed char GetAccValue_Y(void)
{
	return AccValue_t.Y;
}

signed char GetAccValue_Z(void)
{
	return AccValue_t.Z;
}


