/************************************************************************************
*  Copyright (c), 2019, LXG.
*
* FileName		:UVGUI_Animation.h
* Author		:firestaradmin
* Version		:1.0
* Date			:2020.7.31
* Description	:a universal GUI Lib, usually used in Screen display of Embedded System.
* History		:
*
*
*************************************************************************************/

#ifndef _UVGUI_ANIMATION_H__
#define _UVGUI_ANIMATION_H__
#include "stm32f10x.h"

#include "UVGUI.h"
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	UVGUI enum define
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
typedef enum UVGUI_Animation_Object_OutSide_enum
{
	UVGUI_ANIMATION_OBJECT_INSIDE = 0,
	UVGUI_ANIMATION_OBJECT_OUTSIDE = 1
}UVGUI_Animation_Object_OutSide_enum;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	UVGUI struct define
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

typedef struct UVGUI_Animation_Object_struct
{
	float x;		//���������
	float y;		//����������
	float dirx;	//����������˶���
	float diry;	//�����������˶���
	u16 r;		//����뾶
	u32 color;	//������ɫ
}UVGUI_Animation_Object_struct_t;


void UVGUI_Animation_Init(void);
void UVGUI_Animation_Mind(void);


UVGUI_Animation_Object_OutSide_enum UVGUI_Animation_UpdataCoordinate(u8 Index);
static u32 UVGUI_Animation_GetObejctDistanceSquare(u16 x0,u16 y0,u16 x1,u16 y1);
#endif


