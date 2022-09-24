
#include "main.h"
#include "grbl.h"
#include <stdio.h>
#include "ff.h"
#include "diskio.h"
#include "exfuns.h"
#include "fattester.h"

#include "lvgl_grbl_app.h"


/******************************* ȫ�ֱ������� ************************************/
u8 g_Off_Line_Flag = 0;	//�ѻ�ִ��nc�ļ���־

//��ť�ص�����
static void event_handler(lv_obj_t * obj, lv_event_t event)
{
    
    if(event == LV_EVENT_CLICKED) {//�����ť����
        
        //����һ����Ϣ��
        lv_obj_t * mbox = lv_msgbox_create(lv_scr_act(), NULL);//������Ϣ��
        lv_obj_set_width(mbox, 200);//���ÿ��
        static const char * mbox_btns[] = {"ok","cancel", ""};//���ð�ť����
        lv_msgbox_add_btns(mbox, mbox_btns);    /*The default action is close*///����Ϣ�����һ����ť,ĬȻ��ť�¼���ر���Ϣ��
        lv_obj_align(mbox, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 0, LV_DPI / 2);//��ť����
    }

}

void lvgl_create_page(void)
{
	//����һ��ť
    lv_obj_t * label;

    lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(btn1, event_handler);//��ť���»ص����������Բ������ã����Զఴťͬһ���ص�����
    lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, 0, -40);

    label = lv_label_create(btn1, NULL);
    lv_label_set_text(label, "Button");

	lv_obj_t * slider1 = lv_slider_create(lv_scr_act(), NULL);
}





