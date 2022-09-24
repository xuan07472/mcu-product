/************************************************************************************
*  Copyright (c), 2019, LXG.
*
* FileName		:UVGUI_Touch.c
* Author		:firestaradmin
* Version		:1.0
* Date			:2020.8.1
* Description	:a universal GUI Lib, usually used in Screen display of Embedded System.
* History		:
*
*
*************************************************************************************/
#include <stdlib.h>
#include <math.h>

#include "UVGUI.h"
#include "delay.h"
#include "main.h"
extern UVGUI_Screen_Device_t UVGUI_Devices[];
extern uint8_t UVGUI_Current_DevIndex;
/* ����оƬ��ȡָ�� */
static u8 CMD_RDX=0XD0;
static u8 CMD_RDY=0X90;

UVGUI_Touch_Dev_Struct_t touch_Dev;


/* UVGUI��ȡƬ�ⴢ�����Ի�ȡ��ĻУ׼�������û���Ҫ�Լ�ʵ�ִ˺��� ������������ҪУ׼��*/
static uint32_t UVGUI_Touch_ReadStorageData(u32 address, unsigned char* pBuffer, u32 len)
{
	AT24CXX_Read((u16)address,pBuffer, (u16)len);
	return len;
}
/* UVGUIдƬ�ⴢ�����Դ�����ĻУ׼�������û���Ҫ�Լ�ʵ�ִ˺���������������ҪУ׼�� */
static uint32_t UVGUI_Touch_WriteStorageData(u32 address, unsigned char* pBuffer, u32 len)
{
	AT24CXX_Write((u16)address, pBuffer, (u16)len);
	return len;
}
//��������ʼ��  		    
//����ֵ:0,û�н���У׼
//       1,���й�У׼
u8 UVGUI_Touch_Init(void)
{	

	UVGUI_Touch_GPIO_Init();
	if(UVGUI_Touch_Get_Adjdata())
		return 0;//�Ѿ�У׼
	else		//δУ׼							    
		UVFUI_Touch_Adjust();//��ĻУ׼  
		
	UVGUI_Touch_Get_Adjdata();	
	
	return 1; 									 
}

//////////////////////////////////////////////////////////////////////////////////		  
//��������ɨ��
//tp:0,��Ļ����;1,��������(У׼�����ⳡ����)
//����ֵ:��ǰ����״̬.
//0,�����޴���;1,�����д���
u8 UVGUI_Touch_Scan(u8 tp)
{			   
	if(PEN==0)//�а�������
	{
		if(tp)UVGUI_Touch_Read_XY_Twice(&touch_Dev.xPos[0],&touch_Dev.yPos[0]);//��ȡ��������
		else if(UVGUI_Touch_Read_XY_Twice(&touch_Dev.xPos[0],&touch_Dev.yPos[0]))//��ȡ��Ļ����
		{
	 		touch_Dev.xPos[0]=touch_Dev.xfac*touch_Dev.xPos[0]+touch_Dev.xoff;//�����ת��Ϊ��Ļ����
			touch_Dev.yPos[0]=touch_Dev.yfac*touch_Dev.yPos[0]+touch_Dev.yoff;  
	 	} 
		if((touch_Dev.status&TP_PRES_DOWN)==0)//֮ǰû�б�����
		{		 
			touch_Dev.status=TP_PRES_DOWN|TP_CATH_PRES;//��������  
			touch_Dev.xPos[4]=touch_Dev.xPos[0];//��¼��һ�ΰ���ʱ������
			touch_Dev.yPos[4]=touch_Dev.yPos[0];  	   			 
		}			   
	}else
	{
		if(touch_Dev.status&TP_PRES_DOWN)//֮ǰ�Ǳ����µ�
		{
			touch_Dev.status&=~(1<<7);//��ǰ����ɿ�	
		}else//֮ǰ��û�б�����
		{
			touch_Dev.xPos[4]=0;
			touch_Dev.yPos[4]=0;
			touch_Dev.xPos[0]=0xffff;
			touch_Dev.yPos[0]=0xffff;
		}	    
	}
	return touch_Dev.status&TP_PRES_DOWN;//���ص�ǰ�Ĵ���״̬
}	  


//SPIд����
//������ICд��1byte����    
//num:Ҫд�������
void UVGUI_Touch_Write_Byte(u8 num)    
{  
	u8 count=0;   
	for(count=0;count<8;count++)  
	{ 	  
		if(num&0x80)TDIN=1;  
		else TDIN=0;   
		num<<=1;    
		TCLK=0; 
		DelayUs(1);
		TCLK=1;		//��������Ч	        
	}		 			    
} 		 
//SPI������ 
//�Ӵ�����IC��ȡadcֵ
//CMD:ָ��
//����ֵ:����������	   
u16 UVGUI_Touch_Read_AD(u8 CMD)	  
{ 	 
	u8 count=0; 	  
	u16 Num=0; 
	TCLK=0;		//������ʱ�� 	 
	TDIN=0; 	//����������
	TCS=0; 		//ѡ�д�����IC
	UVGUI_Touch_Write_Byte(CMD);//����������
	DelayUs(6);//ADS7846��ת��ʱ���Ϊ6us
	TCLK=0; 	     	    
	DelayUs(1);    	   
	TCLK=1;		//��1��ʱ�ӣ����BUSY
	DelayUs(1);    
	TCLK=0; 	     	    
	for(count=0;count<16;count++)//����16λ����,ֻ�и�12λ��Ч 
	{ 				  
		Num<<=1; 	 
		TCLK=0;	//�½�����Ч  	    	   
		DelayUs(1);    
 		TCLK=1;
 		if(DOUT)Num++; 		 
	}  	
	Num>>=4;   	//ֻ�и�12λ��Ч.
	TCS=1;		//�ͷ�Ƭѡ	 
	return(Num);   
}
//��ȡһ������ֵ(x����y)
//������ȡREAD_TIMES������,����Щ������������,
//Ȼ��ȥ����ͺ����LOST_VAL����,ȡƽ��ֵ 
//xy:ָ�CMD_RDX/CMD_RDY��
//����ֵ:����������
#define READ_TIMES 5 	//��ȡ����
#define LOST_VAL 1	  	//����ֵ
u16 UVGUI_Touch_Read_XOY(u8 xy)
{
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum=0;
	u16 temp;
	for(i=0;i<READ_TIMES;i++)buf[i]=UVGUI_Touch_Read_AD(xy);		 		    
	for(i=0;i<READ_TIMES-1; i++)//����
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//��������
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 
//��ȡx,y����
//��Сֵ��������100.
//x,y:��ȡ��������ֵ
//����ֵ:0,ʧ��;1,�ɹ���
u8 UVGUI_Touch_Read_XY(u16 *x,u16 *y)
{
	u16 xtemp,ytemp;			 	 		  
	xtemp=UVGUI_Touch_Read_XOY(CMD_RDX);
	ytemp=UVGUI_Touch_Read_XOY(CMD_RDY);	  												   
	//if(xtemp<100||ytemp<100)return 0;//����ʧ��
	*x=xtemp;
	*y=ytemp;
	return 1;//�����ɹ�
}
//����2�ζ�ȡ������IC,�������ε�ƫ��ܳ���
//ERR_RANGE,��������,����Ϊ������ȷ,�����������.	   
//�ú����ܴ�����׼ȷ��
//x,y:��ȡ��������ֵ
//����ֵ:0,ʧ��;1,�ɹ���
#define ERR_RANGE 50 //��Χ 
u8 UVGUI_Touch_Read_XY_Twice(u16 *x,u16 *y) 
{
	u16 x1,y1;
 	u16 x2,y2;
 	u8 flag;    
    flag=UVGUI_Touch_Read_XY(&x1,&y1);   
    if(flag==0)return(0);
    flag=UVGUI_Touch_Read_XY(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//ǰ�����β�����+-50��
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else return 0;	  
}  

//����У׼����										    
static void UVGUI_Touch_Save_Adjdata(void)
{
	u8 val = 0x0A;
	UVGUI_Touch_WriteStorageData(SAVE_ADDR_BASE,(u8*)&touch_Dev.xfac,14);	//ǿ�Ʊ���&touch_Dev.xfac��ַ��ʼ��14���ֽ����ݣ������浽touch_Dev.touchtype
 	UVGUI_Touch_WriteStorageData(SAVE_ADDR_BASE+14, &val , 1);		//�����д0X0A���У׼����
}

//�õ������ڴ����������У׼ֵ
//����ֵ��1���ɹ���ȡ����
//        0����ȡʧ�ܣ�Ҫ����У׼
static u8 UVGUI_Touch_Get_Adjdata(void)
{					  
	u8 temp;
	UVGUI_Touch_ReadStorageData(SAVE_ADDR_BASE+14, &temp, 1);//��ȡ�����,���Ƿ�У׼���� 		 
	if(temp==0X0A)//�������Ѿ�У׼����			   
 	{ 
		UVGUI_Touch_ReadStorageData(SAVE_ADDR_BASE, (u8*)&touch_Dev.xfac, 14);//��ȡ֮ǰ�����У׼���� 
		if(	UVGUI_Devices[UVGUI_Current_DevIndex].screen_Direction == SCREEN_HORIZONTAL ||
			UVGUI_Devices[UVGUI_Current_DevIndex].screen_Direction == SCREEN_HORIZONTAL_180)
		{
			CMD_RDX=0X90;
			CMD_RDY=0XD0;	 
		}else if(	UVGUI_Devices[UVGUI_Current_DevIndex].screen_Direction == SCREEN_VERTICAL ||
					UVGUI_Devices[UVGUI_Current_DevIndex].screen_Direction == SCREEN_VERTICAL_180)	
		{
			CMD_RDX=0XD0;
			CMD_RDY=0X90;	 
		}		 
		return 1;	 
	}
	return 0;
}	 

static void UVGUI_Touch_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOF, ENABLE);	 //ʹ��PB,PF�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 // PB1�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//B1�������
	GPIO_SetBits(GPIOB,GPIO_Pin_1);//����

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				 // PB2�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);//B2��������
	GPIO_SetBits(GPIOF,GPIO_Pin_7);//����		

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_9;				 // F9��PF11�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStructure);//PF9,PF11�������
	GPIO_SetBits(GPIOF, GPIO_Pin_11|GPIO_Pin_9);//����

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				 // PF10�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //��������
	GPIO_Init(GPIOF, &GPIO_InitStructure);//PF10��������
	GPIO_SetBits(GPIOF,GPIO_Pin_10);//����		
}
//��ʾУ׼���(��������)
void UVGUI_Touch_Adj_Info_Show(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u16 x3,u16 y3,u16 fac)
{	  
	UVGUI_SetFrontColor(UVGUI_COLOR_RED);
	UVGUI_ShowString(40,160,(u8*)"x1:", UVGUI_STRING_SHOW_OVERLAY);
 	UVGUI_ShowString(40+80,160,(u8*)"y1:", UVGUI_STRING_SHOW_OVERLAY);
 	UVGUI_ShowString(40,180,(u8*)"x2:", UVGUI_STRING_SHOW_OVERLAY);
 	UVGUI_ShowString(40+80,180,(u8*)"y2:", UVGUI_STRING_SHOW_OVERLAY);
	UVGUI_ShowString(40,200,(u8*)"x3:", UVGUI_STRING_SHOW_OVERLAY);
 	UVGUI_ShowString(40+80,200,(u8*)"y3:", UVGUI_STRING_SHOW_OVERLAY);
	UVGUI_ShowString(40,220,(u8*)"x4:", UVGUI_STRING_SHOW_OVERLAY);
 	UVGUI_ShowString(40+80,220,(u8*)"y4:", UVGUI_STRING_SHOW_OVERLAY);  
 	UVGUI_ShowString(40,240,(u8*)"fac is:", UVGUI_STRING_SHOW_OVERLAY);   

	UVGUI_ShowINT(40+24,160,x0,4,UVGUI_STRING_ALIGN_RIGHT);		//��ʾ��ֵ
	UVGUI_ShowINT(40+24+80,160,y0,4,UVGUI_STRING_ALIGN_RIGHT);	//��ʾ��ֵ
	UVGUI_ShowINT(40+24,180,x1,4,UVGUI_STRING_ALIGN_RIGHT);		//��ʾ��ֵ
	UVGUI_ShowINT(40+24+80,180,y1,4,UVGUI_STRING_ALIGN_RIGHT);	//��ʾ��ֵ
	UVGUI_ShowINT(40+24,200,x2,4,UVGUI_STRING_ALIGN_RIGHT);		//��ʾ��ֵ
	UVGUI_ShowINT(40+24+80,200,y2,4,UVGUI_STRING_ALIGN_RIGHT);	//��ʾ��ֵ
	UVGUI_ShowINT(40+24,220,x3,4,UVGUI_STRING_ALIGN_RIGHT);		//��ʾ��ֵ
	UVGUI_ShowINT(40+24+80,220,y3,4,UVGUI_STRING_ALIGN_RIGHT);	//��ʾ��ֵ
 	UVGUI_ShowINT(40+56,240,fac,3,UVGUI_STRING_ALIGN_RIGHT); 	//��ʾ��ֵ,����ֵ������95~105��Χ֮��.

}

//������У׼����
//�õ��ĸ�У׼����
static void UVFUI_Touch_Adjust(void)
{								 
	u16 pos_temp[4][2];//���껺��ֵ
	u8  cnt=0;	
	u16 d1,d2;
	u32 tem1,tem2;
	double fac; 	
	u16 outtime=0;
 	cnt=0;			
	UVGUI_SetFrontColor(UVGUI_COLOR_BLUE);
	UVGUI_SetBackColor(UVGUI_COLOR_WHITE);
	UVGUI_Slow_Clear(UVGUI_COLOR_WHITE);//���� 
	UVGUI_SetFrontColor(UVGUI_COLOR_BLACK);
	UVGUI_SetFontSize(UVGUI_FONT_Size_16);
	UVGUI_ShowString(40,40,(u8*)"screen adjust, please touch point appear", UVGUI_STRING_SHOW_OVERLAY);//��ʾ��ʾ��Ϣ
	UVGUI_Touch_Draw_Touch_Point(20,20,UVGUI_COLOR_RED);//����1 
	touch_Dev.status = 0;	//���������ź� 
	touch_Dev.xfac = 0;		//xfac��������Ƿ�У׼��,����У׼֮ǰ�������!�������	 
	while(1)//�������10����û�а���,���Զ��˳�
	{
		UVGUI_Touch_Scan(1);//ɨ����������
		if((touch_Dev.status&0xc0)==TP_CATH_PRES)//����������һ��(��ʱ�����ɿ���.)
		{	
			outtime=0;		
			touch_Dev.status&=~(1<<6);//��ǰ����Ѿ����������.
						   			   
			pos_temp[cnt][0]=touch_Dev.xPos[0];
			pos_temp[cnt][1]=touch_Dev.yPos[0];
			cnt++;	  
			switch(cnt)
			{			   
				case 1:						 
					UVGUI_Touch_Draw_Touch_Point(20,20,UVGUI_COLOR_WHITE);				//�����1 
					UVGUI_Touch_Draw_Touch_Point(UVGUI_Devices[UVGUI_Current_DevIndex].screen_Width-20,20,UVGUI_COLOR_RED);	//����2
					break;
				case 2:
 					UVGUI_Touch_Draw_Touch_Point(UVGUI_Devices[UVGUI_Current_DevIndex].screen_Width-20,20,UVGUI_COLOR_WHITE);	//�����2
					UVGUI_Touch_Draw_Touch_Point(20,UVGUI_Devices[UVGUI_Current_DevIndex].screen_Height-20,UVGUI_COLOR_RED);	//����3
					break;
				case 3:
 					UVGUI_Touch_Draw_Touch_Point(20,UVGUI_Devices[UVGUI_Current_DevIndex].screen_Height-20,UVGUI_COLOR_WHITE);			//�����3
 					UVGUI_Touch_Draw_Touch_Point(UVGUI_Devices[UVGUI_Current_DevIndex].screen_Width-20,UVGUI_Devices[UVGUI_Current_DevIndex].screen_Height-20,UVGUI_COLOR_RED);	//����4
					break;
				case 4:	 //ȫ���ĸ����Ѿ��õ�
	    		    //�Ա����
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,2�ľ���
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�3,4�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05||d1==0||d2==0)//���ϸ�
					{
						cnt=0;
 				    	UVGUI_Touch_Draw_Touch_Point(UVGUI_Devices[UVGUI_Current_DevIndex].screen_Width-20,UVGUI_Devices[UVGUI_Current_DevIndex].screen_Height-20,UVGUI_COLOR_WHITE);	//�����4
   	 					UVGUI_Touch_Draw_Touch_Point(20,20,UVGUI_COLOR_RED);								//����1
 						UVGUI_Touch_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
 						continue;
					}
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,3�ľ���
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,4�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						cnt=0;
 				    	UVGUI_Touch_Draw_Touch_Point(UVGUI_Devices[UVGUI_Current_DevIndex].screen_Width-20,UVGUI_Devices[UVGUI_Current_DevIndex].screen_Height-20,UVGUI_COLOR_WHITE);	//�����4
   	 					UVGUI_Touch_Draw_Touch_Point(20,20,UVGUI_COLOR_RED);								//����1
 						UVGUI_Touch_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
						continue;
					}//��ȷ��
								   
					//�Խ������
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,4�ľ���
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,3�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						cnt=0;
 				    	UVGUI_Touch_Draw_Touch_Point(UVGUI_Devices[UVGUI_Current_DevIndex].screen_Width-20,UVGUI_Devices[UVGUI_Current_DevIndex].screen_Height-20,UVGUI_COLOR_WHITE);	//�����4
   	 					UVGUI_Touch_Draw_Touch_Point(20,20,UVGUI_COLOR_RED);								//����1
 						UVGUI_Touch_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
						continue;
					}//��ȷ��
					//������
					touch_Dev.xfac=(float)(UVGUI_Devices[UVGUI_Current_DevIndex].screen_Width-40)/(pos_temp[1][0]-pos_temp[0][0]);//�õ�xfac		 
					touch_Dev.xoff=(UVGUI_Devices[UVGUI_Current_DevIndex].screen_Width-touch_Dev.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//�õ�xoff
						  
					touch_Dev.yfac=(float)(UVGUI_Devices[UVGUI_Current_DevIndex].screen_Height-40)/(pos_temp[2][1]-pos_temp[0][1]);//�õ�yfac
					touch_Dev.yoff=(UVGUI_Devices[UVGUI_Current_DevIndex].screen_Height-touch_Dev.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//�õ�yoff  
					if(abs((int32_t)touch_Dev.xfac)>2||abs((int32_t)touch_Dev.yfac)>2)//������Ԥ����෴��.
					{
						cnt=0;
 				    	UVGUI_Touch_Draw_Touch_Point(UVGUI_Devices[UVGUI_Current_DevIndex].screen_Width-20,UVGUI_Devices[UVGUI_Current_DevIndex].screen_Height-20,UVGUI_COLOR_WHITE);	//�����4
   	 					UVGUI_Touch_Draw_Touch_Point(20,20,UVGUI_COLOR_RED);								//����1
						UVGUI_ShowString(40,26,(u8*)"TP Need readjust!",UVGUI_STRING_SHOW_OVERLAY);
						touch_Dev.touchtype=!touch_Dev.touchtype;//�޸Ĵ�������.
						if(touch_Dev.touchtype)//X,Y��������Ļ�෴
						{
							CMD_RDX=0X90;
							CMD_RDY=0XD0;	 
						}else				   //X,Y��������Ļ��ͬ
						{
							CMD_RDX=0XD0;
							CMD_RDY=0X90;	 
						}			    
						continue;
					}		
					UVGUI_SetFrontColor(UVGUI_COLOR_BLUE);
					UVGUI_Fast_Clear(UVGUI_COLOR_WHITE);//����
					UVGUI_ShowString(35,110, (u8*)"Touch Screen Adjust OK!", UVGUI_STRING_SHOW_OVERLAY);//У�����
					DelayMs(1000);
					UVGUI_Touch_Save_Adjdata();  
 					UVGUI_Fast_Clear(UVGUI_COLOR_WHITE);//����   
					return;//У�����				 
			}
		}
		DelayMs(10);
		outtime++;
		if(outtime>1000)
		{
			UVGUI_Touch_Get_Adjdata();
			break;
	 	} 
 	}
}	 





//��һ��������
//����У׼�õ�
//x,y:����
//color:��ɫ
static void UVGUI_Touch_Draw_Touch_Point(u16 x,u16 y,u32 color)
{
	UVGUI_SetFrontColor(color);

	UVGUI_DrawLine(x-12,y,x+13,y);//����
	UVGUI_DrawLine(x,y-12,x,y+13);//����
	UVGUI_DrawPoint(x+1,y+1);
	UVGUI_DrawPoint(x-1,y+1);
	UVGUI_DrawPoint(x+1,y-1);
	UVGUI_DrawPoint(x-1,y-1);
	UVGUI_DrawCircle(x,y,6);//������Ȧ
}	  






