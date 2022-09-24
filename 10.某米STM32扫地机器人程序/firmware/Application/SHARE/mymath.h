#ifndef __MYMATH_H
#define __MYMATH_H
#include "sys.h"
#include "main.h"

float fMath_FindMaxDiff(float *data , u8 length);
u8 ucMath_Tolerance(float *data,u8 length,float allowance);
void vMath_BubbleSort_f(float *a,int len);//float 


#endif 
