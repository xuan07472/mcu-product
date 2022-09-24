/************************************************************************************
*  Copyright (c), 2019, LXG.
*
* FileName		:UVGUI.c
* Author		:firestaradmin
* Version		:1.0
* Date			:2020.7.25
* Description	:a universal GUI Lib, usually used in Screen display of Embedded System.
* History		:
*
*
*************************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "UVGUI.h"


/* Ex_FLASH */
#include "../bsp/w25qxx/w25qxx.h" 

UVGUI_Screen_Device_t UVGUI_Devices[UVGUI_DEV_NUM];
uint8_t UVGUI_Current_DevIndex = 0;
static uint32_t UVGUI_Front_Color = UVGUI_DEFAULT_FRONT_COLOR;
static uint32_t UVGUI_Back_Color = UVGUI_DEFAULT_BACK_COLOR;
static UVGUI_Screen_Font_Size_enum_t Screen_Font_Size = UVGUI_DEFAULT_FONTSIZE;
	
/* 
	static void UVGUI_User_Read_FLASH( uint8_t *pBuffer, uint32_t address, uint32_t len)
	pBuffer:����BUF
	address����ȡ����ʼ��ַ
	len����ȡ�����ݳ���
 */
static void UVGUI_User_Read_FLASH( uint8_t *pBuffer, uint32_t address, uint32_t len)
{
	/* if you want to show GB2312 Char ,user need to config here */
	W25QXX_Read(pBuffer, address, len);  
}

void UVGUI_Device_Init(UVGUI_Screen_Device_t* dev, uint8_t devIndex)
{
	UVGUI_Devices[devIndex].drawPoint = dev->drawPoint;
	UVGUI_Devices[devIndex].screen_Height = dev->screen_Height;
	UVGUI_Devices[devIndex].screen_Width = dev->screen_Width;
	UVGUI_Devices[devIndex].screen_Direction = dev->screen_Direction;
	UVGUI_Devices[devIndex].screen_Device_Status = UVGUI_DEVICE_IDLE;
	UVGUI_Devices[devIndex].readPoint = dev->readPoint;
	UVGUI_Devices[devIndex].setWindow = dev->setWindow;
	UVGUI_Devices[devIndex].writeRam = dev->writeRam;
}

void UVGUI_SetCurrentDev(uint8_t devIndex)
{
	UVGUI_Current_DevIndex = devIndex;
}

void UVGUI_SetFrontColor(uint32_t color)
{
	UVGUI_Front_Color = color;
}

void UVGUI_SetBackColor(uint32_t color)
{
	UVGUI_Back_Color = color;
}
void UVGUI_SetFontSize(UVGUI_Screen_Font_Size_enum_t fontSize)
{
	Screen_Font_Size = fontSize;
}

void UVGUI_SetFont(UVGUI_Screen_Font_Size_enum_t fontSize, uint32_t color)
{
	Screen_Font_Size = fontSize;
	UVGUI_Front_Color = color;
}
//��ʾ�ַ���
//x,y:�������  
//*p:�ַ�����ʼ��ַ
//mode:0(OLED_DISPLAYCHAR_REVERSE)������ʾ;1(OLED_DISPLAYCHAR_NORMAL),������ʾ  				 
void UVGUI_ShowString(uint16_t x,uint16_t y,const uint8_t *pStr, UVGUI_String_Show_Mode_enum_t mode)
{
	uint16_t x0 = x;
	//uint8_t y0 = y;
	while(*pStr!='\0')
	{       
		if(*pStr < 128){
			if(*pStr == 13)
			{
				x = x0;
				y += Screen_Font_Size;
				
			}
			else
			{
				if(x>UVGUI_Devices[UVGUI_Current_DevIndex].screen_Width-(Screen_Font_Size/2)){x=0;y+=Screen_Font_Size;}
				if(y>UVGUI_Devices[UVGUI_Current_DevIndex].screen_Height-Screen_Font_Size){y=x=0;}
				UVGUI_ShowChar(x,y,*pStr,mode);  
				x+=Screen_Font_Size/2;
				
			}
			pStr++;
		}
		else{
			if(x>UVGUI_Devices[UVGUI_Current_DevIndex].screen_Width-(Screen_Font_Size)){x=0;y+=Screen_Font_Size;}
			if(y>UVGUI_Devices[UVGUI_Current_DevIndex].screen_Height-Screen_Font_Size){y=x=0;}
			UVGUI_ShowGB2312Char(x, y, (unsigned char*)pStr,mode);
			pStr += 2;
			x+=Screen_Font_Size;
		}
	
	} 
		
		

}     


 				 

static void UVGUI_ShowChar(uint16_t x,uint16_t y,uint8_t chr,UVGUI_String_Show_Mode_enum_t mode)
{      			 
	const u8 *pFontCodeBuf;
	u8 charByteTemp;
	u16 y0 = y;
	u16 bytesOfOneChar = (Screen_Font_Size/8+((Screen_Font_Size%8)?1:0))*(Screen_Font_Size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���
	chr=chr-' ';//�õ�ƫ�ƺ�ĵ�ַ
	
	if(Screen_Font_Size == 8){
		bytesOfOneChar = 4;	
		pFontCodeBuf = (u8 *)UVGUI_ASCII_4X8;
	}else if(Screen_Font_Size == 12){
		pFontCodeBuf = (u8 *)UVGUI_ASCII_6X12;
	}else if(Screen_Font_Size == 16){
		pFontCodeBuf = (u8 *)UVGUI_ASCII_8X16;
	}else if(Screen_Font_Size == 24){
		pFontCodeBuf = (u8 *)UVGUI_ASCII_12X24;
	}else if(Screen_Font_Size == 32){
		pFontCodeBuf = (u8 *)UVGUI_ASCII_16X32;
	}else{
		return ;	/* There is no font Code of this size */
	}
	
	
    for(u16 i=0; i < bytesOfOneChar; i++)
    {   
		charByteTemp = *(pFontCodeBuf + chr * bytesOfOneChar + i);
        for(u8 j=0; j < 8; j++)
		{
			if(charByteTemp&0x80)
				UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x,y,UVGUI_Front_Color);
			else if(mode)
				UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x,y,UVGUI_Back_Color);
			charByteTemp<<=1;
			y++;
			if((y-y0)==Screen_Font_Size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}

//--------------------------------------------------------------
// Prototype      : void UVGUI_ShowINT(uint8_t x, uint8_t y, int num)
// Calls          : 
// Parameters     : x,y -- ��ʼ������; int num ��ʾ��int���� 
// Description    : 
//--------------------------------------------------------------
void UVGUI_ShowINT(uint16_t x, uint16_t y, long num, uint8_t width, UVGUI_String_Alignment_Mode_enum_t alignment)
{
	unsigned char str[50];
	uint8_t n = 0;
	n = my_itoa(str, num);
	if(n < width)
	{
		if(alignment == UVGUI_STRING_ALIGN_LEFT){
			for(u8 i = 0; i < width - n; i++){
				str[n + i] = ' ';
			}
		}else if(alignment == UVGUI_STRING_ALIGN_RIGHT){
			for(int8_t i = n - 1; i >= 0; i--){
				str[i + width - n] = str[i];
			}
			for(u8 i = 0; i < width - n; i++){
				str[i] = ' ';
			}
		}
		str[width] = '\0';
	}
	
	UVGUI_ShowString(x, y, (unsigned char*)str, UVGUI_STRING_SHOW_OVERLAY);
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowFLOAT(u8 x, u8 y, float num, u8 pointNum,u8 size, u8 mode)
// Calls          : 
// Parameters     : x,y -- ��ʼ������(x:0~127, y:0~63); float num ��ʾ��float��; 
// pointNum		  : С�������λ��(0~5)
// Size:�ֺ�(12/16/24)  
// mode:0(OLED_DISPLAYCHAR_REVERSE)������ʾ;1(OLED_DISPLAYCHAR_NORMAL),������ʾ  
// Description    : 
//--------------------------------------------------------------
void UVGUI_ShowFLOAT(u16 x, u16 y, float num, u8 pointNum)
{
	char strBuf[50];
	sprintf(strBuf, "%f", num);
	UVGUI_ShowString(x, y, (unsigned char*)strBuf, UVGUI_STRING_SHOW_OVERLAY);
}

static void UVGUI_ShowGB2312Char(uint8_t x,uint8_t y,uint8_t *pGB2312Char,UVGUI_String_Show_Mode_enum_t mode)
{
	u8 charByteTemp;
	u16 y0 = y;
//	u16 x0 = x;
	u16 bytesOfOneChar = Screen_Font_Size*Screen_Font_Size/8;		//�õ�����һ���ַ���Ӧ������ռ���ֽ���
	u8* pFontCodeBuf ;
	pFontCodeBuf = malloc(sizeof(u8));
	UVGUI_GetGBKCode_from_EXFlash(pFontCodeBuf, pGB2312Char, Screen_Font_Size);



    for(u16 i=0; i < bytesOfOneChar; i++)
    {   
		charByteTemp = *(pFontCodeBuf + i);
        for(u8 j=0; j < 8; j++)
		{
			if(charByteTemp&0x80)
				UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x,y,UVGUI_Front_Color);
			else if(mode)
				UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x,y,UVGUI_Back_Color);
			charByteTemp<<=1;
			y++;
			if((y-y0)==Screen_Font_Size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          


	
	free(pFontCodeBuf);
	
}


void UVGUI_Slow_FillRect(u16 x1, u16 y1, u16 x2, u16 y2,u32 color)
{
	for(u16 x = x1; x <= x2; x++){
		for(u16 y = y1; y <= y2; y ++){
			UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x,y,color);//���� 
		}
	}
}

void UVGUI_Fast_FillRect(u16 x1, u16 y1, u16 x2, u16 y2,u32 color)
{
	UVGUI_Devices[UVGUI_Current_DevIndex].setWindow(x1,y1,x2,y2);
	for(u32 i = 0; i < (x2-x1+1)*(y2-y1+1) ; i++){
		UVGUI_Devices[UVGUI_Current_DevIndex].writeRam(color);
	}
}

void UVGUI_Slow_Clear(u32 color)
{
	UVGUI_Slow_FillRect(0,0,UVGUI_Devices[UVGUI_Current_DevIndex].screen_Width-1,UVGUI_Devices[UVGUI_Current_DevIndex].screen_Height-1,color);
}
void UVGUI_Fast_Clear(u32 color)
{
	UVGUI_Fast_FillRect(0,0,UVGUI_Devices[UVGUI_Current_DevIndex].screen_Width-1,UVGUI_Devices[UVGUI_Current_DevIndex].screen_Height-1,color);
}
   




void UVGUI_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) 
{
  // Update in subclasses if desired!
  if(x0 == x1)  {
    if(y0 > y1) swap(y0, y1);
    UVGUI_DrawVLine(x0, y0, y1 - y0 ,UVGUI_Front_Color);
  } 
  else if(y0 == y1) {
    if(x0 > x1)
      swap(x0, x1);
    UVGUI_DrawHLine(x0, y0, x1 - x0 ,UVGUI_Front_Color);
  }
  else
    UVGUI_WriteLine(x0, y0, x1, y1);
}
  
void UVGUI_DrawColoredLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,uint32_t color) 
{
  // Update in subclasses if desired!
  if(x0 == x1)  {
    if(y0 > y1) swap(y0, y1);
    UVGUI_DrawVLine(x0, y0, y1 - y0 ,color);
  } 
  else if(y0 == y1) {
    if(x0 > x1)
      swap(x0, x1);
    UVGUI_DrawHLine(x0, y0, x1 - x0 ,color);
  }
  else
    UVGUI_WriteColoredLine(x0, y0, x1, y1,color);
}
  
void  UVGUI_DrawWideColoredLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t with, uint32_t color)
{  int32_t   dx;						// ֱ��x���ֵ����
   int32_t   dy;          			// ֱ��y���ֵ����
   int16_t    dx_sym;					// x����������Ϊ-1ʱ��ֵ����Ϊ1ʱ��ֵ����
   int16_t    dy_sym;					// y����������Ϊ-1ʱ��ֵ����Ϊ1ʱ��ֵ����
   int32_t   dx_x2;					// dx*2ֵ���������ڼӿ������ٶ�
   int32_t   dy_x2;					// dy*2ֵ���������ڼӿ������ٶ�
   int32_t   di;						// ���߱���
   
   int32_t   wx, wy;					// �߿����
   int32_t   draw_a, draw_b;
   
   /* �������� */
   if(with==0) return;
   if(with>50) with = 50;
   
   dx = x1-x0;						// ��ȡ����֮��Ĳ�ֵ
   dy = y1-y0;
   
   wx = with/2;
   wy = with-wx-1;
   
   /* �ж��������򣬻��Ƿ�Ϊˮƽ�ߡ���ֱ�ߡ��� */
   if(dx>0)							// �ж�x�᷽��
   {  dx_sym = 1;					// dx>0������dx_sym=1
   }
   else
   {  if(dx<0)
      {  dx_sym = -1;				// dx<0������dx_sym=-1
      }
      else
      {  /* dx==0������ֱ�ߣ���һ�� */
         wx = x0-wx;
         if(wx<0) wx = 0;
         wy = x0+wy;
         
         while(1)
         {  x0 = wx;
					 
						if(y0 > y1)
							swap(y0, y1);
            UVGUI_DrawVLine(x0, y0, y1 - y0, color);
            if(wx>=wy) break;
            wx++;
         }
         
      	 return;
      }
   }
   
   if(dy>0)							// �ж�y�᷽��
   {  dy_sym = 1;					// dy>0������dy_sym=1
   }
   else
   {  if(dy<0)
      {  dy_sym = -1;				// dy<0������dy_sym=-1
      }
      else
      {  /* dy==0����ˮƽ�ߣ���һ�� */
         wx = y0-wx;
         if(wx<0) wx = 0;
         wy = y0+wy;
         
         while(1)
         {  y0 = wx;
						if(x0 > x1)
							swap(x0, x1);
							UVGUI_DrawHLine(x0, y0, x1 - x0 ,color);
            if(wx>=wy) break;
            wx++;
         }
      	 return;
      }
   }
    
   /* ��dx��dyȡ����ֵ */
   dx = dx_sym * dx;
   dy = dy_sym * dy;
 
   /* ����2����dx��dyֵ */
   dx_x2 = dx*2;
   dy_x2 = dy*2;
   
   /* ʹ��Bresenham�����л�ֱ�� */
   if(dx>=dy)						// ����dx>=dy����ʹ��x��Ϊ��׼
   {  di = dy_x2 - dx;
      while(x0!=x1)
      {  /* x����������������y���򣬼�����ֱ�� */
         draw_a = y0-wx;
         if(draw_a<0) draw_a = 0;
         draw_b = y0+wy;
				if(draw_a > draw_b)
					swap(draw_a, draw_b);
         UVGUI_DrawVLine(x0, draw_a, draw_b - draw_a, color);
         
         x0 += dx_sym;				
         if(di<0)
         {  di += dy_x2;			// �������һ���ľ���ֵ
         }
         else
         {  di += dy_x2 - dx_x2;
            y0 += dy_sym;
         }
      }
      draw_a = y0-wx;
      if(draw_a<0) draw_a = 0;
      draw_b = y0+wy;
				if(draw_a > draw_b)
					swap(draw_a, draw_b);
      UVGUI_DrawVLine(x0, draw_a, draw_b - draw_a, color);
   }
   else								// ����dx<dy����ʹ��y��Ϊ��׼
   {  di = dx_x2 - dy;
      while(y0!=y1)
      {  /* y����������������x���򣬼���ˮƽ�� */
         draw_a = x0-wx;
         if(draw_a<0) draw_a = 0;
         draw_b = x0+wy;
				if(draw_a > draw_b)
					swap(draw_a, draw_b);
         UVGUI_DrawHLine(draw_a, y0, draw_b - draw_a, color);
         
         y0 += dy_sym;
         if(di<0)
         {  di += dx_x2;
         }
         else
         {  di += dx_x2 - dy_x2;
            x0 += dx_sym;
         }
      }
      draw_a = x0-wx;
      if(draw_a<0) draw_a = 0;
      draw_b = x0+wy;
				if(draw_a > draw_b)
					swap(draw_a, draw_b);
      UVGUI_DrawHLine(draw_a, y0, draw_b - draw_a, color);
   } 
  
}


  // Draw a rectangle
void UVGUI_DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h) 
{
    
  UVGUI_DrawHLine(x, y, w, UVGUI_Front_Color);
  UVGUI_DrawHLine(x, y+h, w, UVGUI_Front_Color);
  UVGUI_DrawVLine(x, y, h, UVGUI_Front_Color);
  UVGUI_DrawVLine(x+w, y, h, UVGUI_Front_Color);
}
  // Draw a rectangle
void UVGUI_DrawColoredRectangle(int16_t x, int16_t y, int16_t w, int16_t h,uint32_t color) 
{
    
  UVGUI_DrawHLine(x, y, w, color);
  UVGUI_DrawHLine(x, y+h, w, color);
  UVGUI_DrawVLine(x, y, h, color);
  UVGUI_DrawVLine(x+w, y, h, color);
}



void UVGUI_FillColoredCircle(int16_t x0, int16_t y0, int16_t r,uint32_t color) 
{
  
  UVGUI_DrawVLine(x0, y0-r, 2*r, color);
  UVGUI_FillCircle_Helper(x0, y0, r, 3, 0, color);
}

// Used to do circles and roundrects
static void UVGUI_FillCircle_Helper(int16_t x0, int16_t y0, int16_t r, uint8_t corner, int16_t delta,uint32_t color)  
{

  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while(x<y)  {
    if(f >= 0)  {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    if(corner & 0x1)  {
      UVGUI_DrawVLine(x0+x, y0-y, 2*y+delta, color);
      UVGUI_DrawVLine(x0+y, y0-x, 2*x+delta, color);
    }
    if(corner & 0x2)  {
      UVGUI_DrawVLine(x0-x, y0-y, 2*y+delta, color);
      UVGUI_DrawVLine(x0-y, y0-x, 2*x+delta, color);
    }
  }
}




// Draw a circle outline
void UVGUI_DrawCircle(int16_t x0, int16_t y0, int16_t r) 
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0  , y0+r, UVGUI_Front_Color);
  UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0  , y0-r, UVGUI_Front_Color);
  UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0+r, y0  ,UVGUI_Front_Color);
  UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0-r, y0  ,UVGUI_Front_Color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 + x, y0 + y, UVGUI_Front_Color);
    UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 - x, y0 + y, UVGUI_Front_Color);
    UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 + x, y0 - y, UVGUI_Front_Color);
    UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 - x, y0 - y, UVGUI_Front_Color);
    UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 + y, y0 + x, UVGUI_Front_Color);
    UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 - y, y0 + x, UVGUI_Front_Color);
    UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 + y, y0 - x, UVGUI_Front_Color);
    UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 - y, y0 - x, UVGUI_Front_Color);
    }
}


// Draw a circle outline
void UVGUI_DrawColoredCircle(int16_t x0, int16_t y0, int16_t r,uint32_t color) 
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0  , y0+r, color);
  UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0  , y0-r, color);
  UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0+r, y0  , color);
  UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0-r, y0  , color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 + x, y0 + y, color);
    UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 - x, y0 + y, color);
    UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 + x, y0 - y, color);
    UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 - x, y0 - y, color);
    UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 + y, y0 + x, color);
    UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 - y, y0 + x, color);
    UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 + y, y0 - x, color);
    UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 - y, y0 - x, color);
    }
}

void UVGUI_DrawQuarterCircle(int16_t x0, int16_t y0, int16_t r,UVGUI_DrawQuarterCircle_Part_enum_t part,uint32_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;
	switch(part)
	{
		case UCGUI_CIRCLE_PART_LOWRIGHT:
				UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0  , y0+r, color);
				UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0+r, y0  , color);break;//����
		case UCGUI_CIRCLE_PART_LOWLEFT:
				UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0  , y0+r, color);
				UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0-r, y0  , color);break;//����
		case UCGUI_CIRCLE_PART_UPRIGHT:
				UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0  , y0-r, color);
				UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0+r, y0  , color);break;//����
		case UCGUI_CIRCLE_PART_UPLEFT:
				UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0-r, y0  , color);
				UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0  , y0-r, color);break;//����
	}

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

	switch(part)
	{
		case UCGUI_CIRCLE_PART_LOWRIGHT:
    UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 + x, y0 + y, color);
    UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 + y, y0 + x, color);break;
		case UCGUI_CIRCLE_PART_LOWLEFT:
    UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 - x, y0 + y, color);
    UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 - y, y0 + x, color);break;
		case UCGUI_CIRCLE_PART_UPRIGHT:
    UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 + x, y0 - y, color);
    UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 + y, y0 - x, color);break;
		case UCGUI_CIRCLE_PART_UPLEFT:
    UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 - y, y0 - x, color);
    UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 - x, y0 - y, color);break;
	}
		
		
		
    }
}


// Draw a rounded rectangle
void UVGUI_DrawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r) 
{
    // smarter version

    UVGUI_DrawHLine(x+r  , y    , w-2*r, UVGUI_Front_Color); // Top
    UVGUI_DrawHLine(x+r  , y+h-1, w-2*r, UVGUI_Front_Color); // Bottom
    UVGUI_DrawVLine(x    , y+r  , h-2*r, UVGUI_Front_Color); // Left
    UVGUI_DrawVLine(x+w-1, y+r  , h-2*r, UVGUI_Front_Color); // Right
    // draw four corners
    UVGUI_DrawCircle_Helper(x+r    , y+r    , r, 1);
    UVGUI_DrawCircle_Helper(x+w-r-1, y+r    , r, 2);
    UVGUI_DrawCircle_Helper(x+w-r-1, y+h-r-1, r, 4);
    UVGUI_DrawCircle_Helper(x+r    , y+h-r-1, r, 8);
}
// Draw a rounded rectangle
void UVGUI_DrawColoredRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r,uint32_t color) 
{
    // smarter version

    UVGUI_DrawHLine(x+r  , y    , w-2*r, color); // Top
    UVGUI_DrawHLine(x+r  , y+h-1, w-2*r, color); // Bottom
    UVGUI_DrawVLine(x    , y+r  , h-2*r, color); // Left
    UVGUI_DrawVLine(x+w-1, y+r  , h-2*r, color); // Right
    // draw four corners
    UVGUI_DrawColoredCircle_Helper(x+r    , y+r    , r, 1, color);
    UVGUI_DrawColoredCircle_Helper(x+w-r-1, y+r    , r, 2, color);
    UVGUI_DrawColoredCircle_Helper(x+w-r-1, y+h-r-1, r, 4, color);
    UVGUI_DrawColoredCircle_Helper(x+r    , y+h-r-1, r, 8, color);
}

static void UVGUI_DrawCircle_Helper( int16_t x0, int16_t y0, int16_t r, uint8_t corner) 
{
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    if (corner & 0x4) {
      UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 + x, y0 + y, UVGUI_Front_Color);
      UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 + y, y0 + x, UVGUI_Front_Color);
    }
    if (corner & 0x2) {
      UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 + x, y0 - y, UVGUI_Front_Color);
      UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 + y, y0 - x, UVGUI_Front_Color);
    }
    if (corner & 0x8) {
      UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 - y, y0 + x, UVGUI_Front_Color);
      UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 - x, y0 + y, UVGUI_Front_Color);
    }
    if (corner & 0x1) {
      UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 - y, y0 - x, UVGUI_Front_Color);
      UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 - x, y0 - y, UVGUI_Front_Color);
    }
  }
}


static void UVGUI_DrawColoredCircle_Helper( int16_t x0, int16_t y0, int16_t r, uint8_t corner,uint32_t color) 
{
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    if (corner & 0x4) {
      UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 + x, y0 + y, color);
      UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 + y, y0 + x, color);
    }
    if (corner & 0x2) {
      UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 + x, y0 - y, color);
      UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 + y, y0 - x, color);
    }
    if (corner & 0x8) {
      UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 - y, y0 + x, color);
      UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 - x, y0 + y, color);
    }
    if (corner & 0x1) {
      UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 - y, y0 - x, color);
      UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0 - x, y0 - y, color);
    }
  }
}

// Draw a triangle
void UVGUI_DrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2)  
{
  UVGUI_DrawLine(x0, y0, x1, y1);
  UVGUI_DrawLine(x1, y1, x2, y2);
  UVGUI_DrawLine(x2, y2, x0, y0);
}
// Draw a triangle
void UVGUI_DrawColoredTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2,uint32_t color)  
{
  UVGUI_DrawColoredLine(x0, y0, x1, y1, color);
  UVGUI_DrawColoredLine(x1, y1, x2, y2, color);
  UVGUI_DrawColoredLine(x2, y2, x0, y0, color);
}


static void UVGUI_DrawHLine(int16_t x, int16_t y, int16_t length,uint32_t color) 
{
  // Bounds check
		int16_t x0=x;
    do
    {
        UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x, y,color);   // �����ʾ�������ֱ��
        x++;
    }
    while(x0+length>=x);
}


static void UVGUI_DrawVLine(int16_t x, int16_t y, int16_t length,uint32_t color)  
{
  // Bounds check
		int16_t y0=y;
    do
    {
        UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x, y,color);   // �����ʾ�������ֱ��
        y++;
    }
    while(y0+length>=y);
}
  
static void UVGUI_WriteLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1)  
{
    
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    
  if (steep)  {
    swap(x0, y0);
    swap(x1, y1);
  }
  if (x0 > x1)  {
    swap(x0, x1);
    swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if(y0 < y1) {
    ystep = 1;
  } 
  else  {
    ystep = -1;
  }

  for(; x0<=x1; x0++) {
    if(steep)
      UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(y0, x0, UVGUI_Front_Color);
    else
      UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0, y0, UVGUI_Front_Color);
    err -= dy;
      
    if(err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

static void UVGUI_WriteColoredLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,uint32_t color)  
{
    
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    
  if (steep)  {
    swap(x0, y0);
    swap(x1, y1);
  }
  if (x0 > x1)  {
    swap(x0, x1);
    swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if(y0 < y1) {
    ystep = 1;
  } 
  else  {
    ystep = -1;
  }

  for(; x0<=x1; x0++) {
    if(steep)
      UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(y0, x0,color);
    else
      UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x0, y0,color);
    err -= dy;
      
    if(err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void UVGUI_DrawPoint(int16_t x, int16_t y)
{
	UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x, y, UVGUI_Front_Color);
}
void UVGUI_DrawColoredPoint(int16_t x, int16_t y, uint32_t color)
{
	UVGUI_Devices[UVGUI_Current_DevIndex].drawPoint(x, y, color);
}

/**
* @brief ��ȡ������ʾ�ֿ�����
* @param pBuffer:�洢�ֿ����Ļ�����
* @param c �� Ҫ��ȡ��GB2312����
* @param size �� �����С
* @retval None.
*/
static void UVGUI_GetGBKCode_from_EXFlash( uint8_t *pBuffer, uint8_t *pGB2312_Char, UVGUI_Screen_Font_Size_enum_t font_Size)
{
	unsigned char High8bit,Low8bit;
	unsigned int pos;
	uint32_t address;
	//static uint8_t everRead=0;

	High8bit= pGB2312_Char[0]; /* ȡ�� 8 λ���� */
	Low8bit= pGB2312_Char[1]; /* ȡ�� 8 λ���� */

	/*GB2312 ��ʽ*/
	pos = ((High8bit-0xa1)*94+Low8bit-0xa1)*font_Size*font_Size/8;
	
	if(font_Size == 8)
		address = GB2312_FONT_SIZE8_ADDRESS;
	else if(font_Size == 16)
		address = GB2312_FONT_SIZE16_ADDRESS;
	else if(font_Size == 24)
		address = GB2312_FONT_SIZE24_ADDRESS;
	else if(font_Size == 32)
		address = GB2312_FONT_SIZE32_ADDRESS;

	//��ȡ�ֿ�����
	UVGUI_User_Read_FLASH(pBuffer, address+pos, font_Size*font_Size/8);


}

/*********************************************
Function:	unsigned char *reverse(unsigned char *s)
Description:���ַ���˳��ߵ�
Input:		unsigned char * ��Ҫ�ߵ����ַ���
Return:		unsigned char* :ת������ַ���ָ��
Author:		firestaradmin
**********************************************/
static unsigned char *reverse(unsigned char *s)
{
    unsigned char temp;
    unsigned char *p = s;    //pָ��s��ͷ��
    unsigned char *q = s;    //qָ��s��β��
    while(*q)
        ++q;
    q--;
    
    //�����ƶ�ָ�룬ֱ��p��q����
    while(q > p)
    {
        temp = *p;
        *p++ = *q;
        *q-- = temp;
    }
    return s;
}


/*********************************************
Function:	static uint8_t *my_itoa(u8* str, long n)
Description:��int��ת��Ϊunsigned char*�ַ���
Input:		int n ��Ҫת������
Return:		ת�����ַ����ĳ���
Calls:		unsigned char *reverse(unsigned char *s)
Author:		firestaradmin
**********************************************/
static uint8_t my_itoa(u8* str, long n)
{
    int i = 0,isNegative = 0;
	uint8_t width = 0;
    if((isNegative = n) < 0) //����Ǹ�������תΪ����
    {
        n = -n;
		width ++;
    }
    do      //�Ӹ�λ��ʼ��Ϊ�ַ���ֱ�����λ�����Ӧ�÷�ת
    {
        str[i++] = n%10 + '0';
        n = n/10;
		width ++;
    }while(n > 0);
    
    if(isNegative < 0)   //����Ǹ��������ϸ���
    {
        str[i++] = '-';
    }
    str[i] = '\0';    //�������ַ���������
	reverse(str);
    return width;
}
/*********************************************
Function:	unsigned char *my_strcat(u8 * str1, u8 * str2)
Description:��str2ƴ�ӵ�str1ĩβ
Input:		str1 str2
Return:		unsigned char* :ת������ַ���ָ��
Calls:		
Author:		firestaradmin
**********************************************/
//static unsigned char *my_strcat(u8 * str1, u8 * str2)
//{
//	u8* pt = str1;
//	while(*str1 != '\0') str1++;
//	while(*str2 != '\0') *str1++ = *str2++;
//	*str1 = '\0';
//	return pt;

//}


