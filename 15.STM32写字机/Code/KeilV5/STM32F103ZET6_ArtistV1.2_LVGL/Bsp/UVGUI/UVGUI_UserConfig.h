/************************************************************************************
*  Copyright (c), 2019, LXG.
*
* FileName		:UVGUI_UserConfig.h
* Author		:firestaradmin
* Version		:1.0
* Date			:2020.7.25
* Description	:a universal GUI Lib, usually used in Screen display of Embedded System.
* History		:
*
*
*************************************************************************************/

#ifndef _UVGUI_USERCONFIG_H__
#define _UVGUI_USERCONFIG_H__
#include "UVGUI.h"
/* The number of using screen devices. In general, it should be set to 1.*/
#define UVGUI_DEV_NUM	2

/* Represents the size required for the color of a pixel   
   if your screen is mono, the screen_Color_bitNum need to be init as 1;*/
#define UVGUI_COLOR_DATA_BITS  16	//unit:bit

#if(UVGUI_COLOR_DATA_BITS == 1)
	#define UVGUI_COLOR_BLACK  0
	#define UVGUI_COLOR_WHITE  1
#elif (UVGUI_COLOR_DATA_BITS == 16)
	//������ɫ
	#define UVGUI_COLOR_WHITE         	 0xFFFF
	#define UVGUI_COLOR_BLACK         	 0x0000	  
	#define UVGUI_COLOR_BLUE         	 0x001F  
	#define UVGUI_COLOR_BRED             0XF81F
	#define UVGUI_COLOR_GRED 			 0XFFE0
	#define UVGUI_COLOR_GBLUE			 0X07FF
	#define UVGUI_COLOR_RED           	 0xF800
	#define UVGUI_COLOR_MAGENTA       	 0xF81F
	#define UVGUI_COLOR_GREEN         	 0x07E0
	#define UVGUI_COLOR_CYAN          	 0x7FFF
	#define UVGUI_COLOR_YELLOW        	 0xFFE0
	#define UVGUI_COLOR_BROWN 			 0XBC40 //��ɫ
	#define UVGUI_COLOR_BRRED 			 0XFC07 //�غ�ɫ
	#define UVGUI_COLOR_GRAY  			 0X8430 //��ɫ
	//GUI��ɫ
	#define UVGUI_COLOR_DARKBLUE      	 0X01CF	//����ɫ
	#define UVGUI_COLOR_LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
	#define UVGUI_COLOR_GRAYBLUE       	 0X5458 //����ɫ
	//������ɫΪPANEL����ɫ 
	#define UVGUI_COLOR_LIGHTGREEN     	 0X841F //ǳ��ɫ
	#define UVGUI_COLOR_LGRAY 			 0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ
	#define UVGUI_COLOR_LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
	#define UVGUI_COLOR_LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)	
#elif (UVGUI_COLOR_DATA_BITS == 18)

#elif (UVGUI_COLOR_DATA_BITS == 24)

#elif (UVGUI_COLOR_DATA_BITS == 32)
#endif

/* display default config	*/
#define UVGUI_DEFAULT_FRONT_COLOR		UVGUI_COLOR_BLACK
#define UVGUI_DEFAULT_BACK_COLOR		UVGUI_COLOR_WHITE
#define UVGUI_DEFAULT_FONTSIZE	UVGUI_FONT_Size_16







/*W25Q64
����Ϊ8M�ֽ�,����128��Block,2048��Sector 
4KbytesΪһ��Sector
16������Ϊ1��Block */

/* GB2312 Font Code start address in EX-FLASH(W25QXX) */
#define GB2312_FONT_SIZE8_ADDRESS	0
#define GB2312_FONT_SIZE12_ADDRESS	0
#define GB2312_FONT_SIZE16_ADDRESS	4*1024*16			/* address: 65536 ~ 348287 */
#define GB2312_FONT_SIZE24_ADDRESS	4*1024*16+282752 	/* address: 348288 ~ 984479 */
#define GB2312_FONT_SIZE32_ADDRESS 	0


#endif
