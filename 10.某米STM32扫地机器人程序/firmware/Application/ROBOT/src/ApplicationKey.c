#include "applicationkey.h"

/*
 * Filename   ：applicationkey.c
 * Author     : pusheng       
 * Version    : 1.0
 * Date       : 2019.11.7
 * Discription : 按键处理应用函数
*/	

/*******************************************************************************
* Function Name  : vButtonCheckOperation
* Description    : 检测按键的状态 并做相应的动作 至少以20ms的周期调用一次
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vButtonCheckOperation(void)
{
    eButtonState buttonstate1 , buttonstate2;
    
    buttonstate1 = eKey1State();
    buttonstate2 = eKey2State();
    
    if( ButtonUp != buttonstate1)
    {
        DEBUG("buttonstate1 %d\r\n",buttonstate1);
        if(buttonstate1 == ButtonDoubleDown)
        {
            POWER_DOWN;//双击系统掉电
            g_tRobotState.KeyCommand=keyStop;//上报按键信息
        }
        if(buttonstate1 == Button2sDown)
        {
            POWER_UP;//2秒长按系统上电
            g_tRobotState.KeyCommand=keyStart;//上报按键信息
        }
    }
    if( ButtonUp != buttonstate2)
    {
        DEBUG("buttonstate2 %d\r\n",buttonstate2);
    }
}
/*******************************************************************************
* Function Name  : eKey1State
* Description    : 返回按键键值
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
eButtonState eKey1State(void)
{
    static uint8_t Button_PC = 0; //状态指示
    static OS_TimeStruct KeyDownTime = {0};//非阻塞时间结构体

    switch( Button_PC )
    {
        case 0 :             
            if( KEY_1_PIN != ( uint8_t )Bit_RESET)
            {                
                return ButtonUp; //按键没有按下       
            }
            KeyDownTime = GetOSRunTimeNow(  );//按键按下后 记录当前时间
            Button_PC = 1;              
        case 1:  
            if( KEY_1_PIN == ( uint8_t )Bit_RESET) //等待按键释放
            {             
                return ButtonUp;//按键一直没有释放        
            }
            /*按键已经释放*/
            if( SingleTimeoutCheck( KeyDownTime, 0, 0, 20 ) == NotTimeOut )   //消抖
            {
                Button_PC = 0;//按下时间小于20ms  认为是抖动 识别为没有按键按下
                return ButtonUp;  
            } 
            if( SingleTimeoutCheck( KeyDownTime, 0, 6, 0 ) == TimeOut ) //是否按下超过6秒
            {
                Button_PC = 0;
                return Button6sDown;
            }     
            else if( SingleTimeoutCheck( KeyDownTime, 0, 5, 0 ) == TimeOut )
            {
                Button_PC = 0;
                return Button5sDown;
            }
            else if( SingleTimeoutCheck( KeyDownTime, 0, 4, 0 ) == TimeOut )         
            {
                Button_PC = 0;
                return Button4sDown;
            }
            else if( SingleTimeoutCheck( KeyDownTime, 0, 3, 0 ) == TimeOut )           
            {
                Button_PC = 0;
                return Button3sDown;
            }
            else if( SingleTimeoutCheck( KeyDownTime, 0, 2, 0 ) == NotTimeOut )   //按下小于2s            
            {
                KeyDownTime = GetOSRunTimeNow(  );//重新获取当前时间
                Button_PC = 3;
                return ButtonUp;
            }
            else if( SingleTimeoutCheck( KeyDownTime, 0, 1, 0 ) == NotTimeOut ) 
            {
                KeyDownTime = GetOSRunTimeNow(  );
                Button_PC = 3;
                return ButtonUp;
            }
            Button_PC = 0;
       case 3://双击检测 老铁666
            if( KEY_1_PIN != ( uint8_t )Bit_RESET)//没有检测到按下
            {                
                if(SingleTimeoutCheck( KeyDownTime, 0, 0, 300 ) == TimeOut)//300ms内 依然没有等待到第二次按键按下.
                {
                    Button_PC = 0;
                    return Button2sDown;//返回按键单击2S
                }
                return ButtonUp;        
            }
            else //300ms内检测到第二次按键按下
            {
                KeyDownTime = GetOSRunTimeNow(  );//重新获取当前时间
                Button_PC = 4;                  
            }
           break;
       case 4://检测双击时间
            if( KEY_1_PIN == ( uint8_t )Bit_RESET) //按键还没松开
            {             
                return ButtonUp;        
            }
            /*按键已经释放*/
            if( SingleTimeoutCheck( KeyDownTime, 0, 2, 0 ) == NotTimeOut ) 
            {
                Button_PC = 0;
                return ButtonDoubleDown;//第二次按键时间在2秒内  完成双击！给劲 老铁
            }
            else//超过2秒  双击失败 欧力给
            {
                Button_PC = 0;
                return ButtonUp;
            }
       default:  
           break;   
    }
    return ButtonUp;
}

/*******************************************************************************
* Function Name  : eKey2State
* Description    : 返回按键键值
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
eButtonState eKey2State(void)
{
    static uint8_t Button_PC = 0; //状态指示
    static OS_TimeStruct KeyDownTime = {0};//非阻塞时间结构体

    switch( Button_PC )
    {
        case 0 :             
            if( KEY_2_PIN != ( uint8_t )Bit_RESET)
            {                
                return ButtonUp; //按键没有按下       
            }
            KeyDownTime = GetOSRunTimeNow(  );//按键按下后 记录当前时间
            Button_PC = 1;              
        case 1:  
            if( KEY_2_PIN == ( uint8_t )Bit_RESET) //按键释放检测
            {             
                return ButtonUp;//按键一直没有释放        
            }
            /*按键已经释放*/
            if( SingleTimeoutCheck( KeyDownTime, 0, 0, 20 ) == NotTimeOut )   //消抖
            {
                Button_PC = 0;//按下时间小于20ms  认为是抖动 识别为没有按键按下
                return ButtonUp;  
            } 
            if( SingleTimeoutCheck( KeyDownTime, 0, 6, 0 ) == TimeOut ) //是否按下超过6秒
            {
                Button_PC = 0;
                return Button6sDown;
            }     
            else if( SingleTimeoutCheck( KeyDownTime, 0, 5, 0 ) == TimeOut )
            {
                Button_PC = 0;
                return Button5sDown;
            }
            else if( SingleTimeoutCheck( KeyDownTime, 0, 4, 0 ) == TimeOut )         
            {
                Button_PC = 0;
                return Button4sDown;
            }
            else if( SingleTimeoutCheck( KeyDownTime, 0, 3, 0 ) == TimeOut )           
            {
                Button_PC = 0;
                return Button3sDown;
            }
            else if( SingleTimeoutCheck( KeyDownTime, 0, 2, 0 ) == NotTimeOut )   //按下小于2s            
            {
                KeyDownTime = GetOSRunTimeNow(  );//重新获取当前时间
                Button_PC = 3;
                return ButtonUp;
            }
            else if( SingleTimeoutCheck( KeyDownTime, 0, 1, 0 ) == NotTimeOut ) 
            {
                KeyDownTime = GetOSRunTimeNow(  );
                Button_PC = 3;
                return ButtonUp;
            }
            Button_PC = 0;
       case 3://双击检测 老铁666
            if( KEY_2_PIN != ( uint8_t )Bit_RESET)//没有检测到按下
            {                
                if(SingleTimeoutCheck( KeyDownTime, 0, 0, 300 ) == TimeOut)//300ms内 依然没有等待到第二次按键按下.
                {
                    Button_PC = 0;
                    return Button2sDown;//返回按键单击2S
                }
                return ButtonUp;        
            }
            else //300ms内检测到第二次按键按下
            {
                KeyDownTime = GetOSRunTimeNow(  );//重新获取当前时间
                Button_PC = 4;                  
            }
           break;
       case 4://检测双击时间
            if( KEY_2_PIN == ( uint8_t )Bit_RESET) //按键还没松开
            {             
                return ButtonUp;        
            }
            /*按键已经释放*/
            if( SingleTimeoutCheck( KeyDownTime, 0, 2, 0 ) == NotTimeOut ) 
            {
                Button_PC = 0;
                return ButtonDoubleDown;//第二次按键时间在2秒内  完成双击！给劲 老铁
            }
            else//超过2秒  双击失败 欧力给
            {
                Button_PC = 0;
                return ButtonUp;
            }
       default:  
           break;   
    }
    return ButtonUp;
}
