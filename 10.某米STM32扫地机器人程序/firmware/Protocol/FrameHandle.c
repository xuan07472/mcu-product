#include "framehandle.h"

/*
 * Filename   ：framehandle.c
 * Author     : PuSheng 
 * Version    : 1.0
 * Date       : 2019.12.25 
 * Discription : 协议帧处理函数
*/


/*
* Function Name  : vFrameDataHandlerTask
* Description    : 协议帧数据处理任务函数
* Input          : 参数 
* Output         : None 
* Return         : None
*/

u32  ProductAppPassword  =   PRODUCT_APP_PASSWORD;  //产品应用秘钥
fCommand_e    g_tLastCommand;//上次接收到的命令
RouteCoord_t  g_tRouteCoord={0};// 机器人的行走路线坐标
/*
* Function Name  : vFrameDataHandlerTask
* Description    : 帧处理函数任务 将串口接受到的数据分析为协议帧数据 
* Input          : p 参数
* Output         : None 
* Return         : None
*/
void vFrameDataHandlerTask(void *p)
{
    u8 length=0;
    while(1)
    {
        /*串口接收到数据 并且帧数据已经全部处理完毕*/
        if(ucUSART1_ReceiveMessageNumber && FR_Rx.IsFrameNotProcess == false)
        {
            length=ucUSART1ReceiveBufferLength[ucUSART1ReadBufferIndex];//获取接收数据的长度
            eFrame_Analy(ucUSART1ReceiveBuffer[ucUSART1ReadBufferIndex],length,&FR_Rx);//解析数据
            USART1ReceiveDataReadDone(  );//串口数据读取完毕
            
            /*成功接收到的不是回应帧*/
            if( FR_Rx.tFrame.FrameFunction != ffRespond  )
            {
                /*配置回应帧*/
                ucCode_RespondFrame(&FR_Tx,&FR_Rx,ucUSART1TrainsmitBuffer[ucWtiteDataToUSART1TransmitGrooveIndex]);
                /*发送回应帧到串口发送缓冲区*/
                ucUSART1TrainsmitLength[ucWtiteDataToUSART1TransmitGrooveIndex]=5+FR_Tx.tFrame.DataLength;
                WriteDataToUSART1TraismitBufferDone();
            }              
        }
        
        /*帧数据处理*/
        if(FR_Rx.IsFrameNotProcess)
        {
            switch(FR_Rx.tFrame.FrameFunction)
            {
                case ffRespond: //接收到的是回应帧
                    FR_Tx.IsReceiveRespond=true;//表示接收到回应帧
                    break;
                
                case ffCommand: //接收到的是命令帧
                    vCommandFrameHandle(&FR_Rx);
                    break;
                
                case ffData: //接收到的是数据帧
                    vDataFrameHandle(&FR_Rx);
                    break;
                
                case ffIdle: break;//接收到的是空闲帧，不处理
                case ffMax: break;
            }
            FR_Rx.IsFrameNotProcess=false;//标记数据已经处理
        }
        vFrameAutoResendByUsart1(&FR_Rx);
        vTaskDelay(5);
    }
}


/*
* Function Name  : vCommandFrameHandle
* Description    : 命令帧处理函数 
* Input          : Rx_FrameRecord 接收帧结构体 
* Output         : None 
* Return         : None
*/
void vCommandFrameHandle(Rx_FrameRecord *R)
{
    
    if( R->tFrame.DataLength==1 )//命令字节只有一字节
    {
        switch(R->tFrame.FrameData[0])//查看命令
        {
            case UpdateFirmwareStart: //是开始更新固件命令
                g_tLastCommand=UpdateFirmwareStart;//标记命令
                ProductAppPassword=0;//产品密钥清理
                STMFLASH_Write(PASSWORD_ADDR,(u16*)&ProductAppPassword,2);//擦除产品秘钥
                ((void (*)())(*((u32 *)(0x08000004))))();//复位 进入bootloader程序
                break;
            
            case UpdateFirmwareDone: //是结束更新固件命令 firmware程序不会用到
                g_tLastCommand=UpdateFirmwareDone;
                break;
            
            case BehaviorControl: //是行为控制命令
                g_tLastCommand=BehaviorControl;//记录命令
                break;
            
            case RequestState://请求状态命令
                g_tLastCommand=RequestState;
                vRequestStateCommandHandle(&FR_Tx);//处理 请求状态命令
                break;
            
            case RouteCoord://线路坐标命令
                g_tLastCommand=RouteCoord;
                break;
        }
        
    }
}
/*
* Function Name  : vDataFrameHandle
* Description    : 数据帧处理函数
* Input          : Rx_FrameRecord 接收帧结构体 
* Output         : None 
* Return         : None
*/
void vDataFrameHandle(Rx_FrameRecord *R)
{
    switch(g_tLastCommand)
    {
        case UpdateFirmwareStart://命令为开始更新固件 不会执行
            break;
        
        case UpdateFirmwareDone://命令为固件更新完毕 不会执行
            break;  
        
        case RequestState://命令为请求状态 不会执行
            break;
        
        case BehaviorControl://命令为行为控制
            vBehaviorControlHandle(R);//运行 行为控制处理函数
            break;

        case RouteCoord://线路坐标命令
            vRouteCoordHandle(R);//运行 线路坐标处理函数
            break;        
    }
}

/*
* Function Name  : vBehaviorControlHandle
* Description    : 接收到数据为行为控制 的处理函数 
* Input          : 参数 Rx_FrameRecord 接收帧结构体
* Output         : None 
* Return         : None
*/
void vBehaviorControlHandle(Rx_FrameRecord *R)
{
    BehaviorControl_t *pBC = (BehaviorControl_t *)R->DataBuf;//数据赋值给结构体成员
    
    switch(pBC->Com)//控制类型
    {
        case bccStop ://停止  
            g_tRobotState.Robotstate.eDirection = zStop;
            break;
        
        case bccForward ://前进
            
            g_sExpect_Angle = pBC->TargetAngle*10;//获取目标角度 
            g_tRobotState.Robotstate.eDirection = zForward;
            break;
        
        case bccRetreat ://后退
            g_sExpect_Angle = pBC->TargetAngle*10;//获取目标角度 
            g_tRobotState.Robotstate.eDirection = zRetreat;
            break;
        
        case bccTurnRotate ://原地转弯
            g_sExpect_Angle = pBC->TargetAngle*10;//获取目标角度 
            g_tRobotState.Robotstate.eDirection = zRotate;
            break;
        
        case bccTurnLeft ://向左绕轴转弯
            g_sExpect_Angle = pBC->TargetAngle*10;//获取目标角度   
            g_tRobotState.Robotstate.eDirection = zTurnLeft;
            break;
            
        case bccTurnRight ://向右绕轴转弯
            g_sExpect_Angle = pBC->TargetAngle*10;//获取目标角度 
            g_tRobotState.Robotstate.eDirection = zTurnRight;
            break;
    }
}
/*
* Function Name  : vRouteCoordHandle
* Description    : 接收到线路坐标数据 的处理函数 
* Input          : 参数 Rx_FrameRecord 接收帧结构体
* Output         : None 
* Return         : None
*/
void vRouteCoordHandle(Rx_FrameRecord *R)
{
    /*每次都会接受到5个 坐标点 。第一个是当前机器人坐标 一个 坐标X Y为8字节*/
    memcmp(&g_tRouteCoord,R->DataBuf,5*8);
}

/*
* Function Name  : vRequestStateCommandHandle
* Description    : 接收到数据为请求状态 的处理函数 
* Input          : 参数 Rx_FrameRecord 接收帧结构体
* Output         : None 
* Return         : None
*/
void vRequestStateCommandHandle(Tx_FrameRecord *T)
{
    s16 loc_CurrentAngle;//当前角度
    u16 loc_Size;//用于记录数据大小
    
    if(T->IsSendOut == false)//若有数据发送 但是没有收到回应，则等待接收回应帧
    {
//        g_tRobotState.ucUserNumber=0xaa;
//        g_tRobotState.ucFirmVersion=0x00;
//        g_tRobotState.Robotstate.ucRemainBattery=1;
//        g_tRobotState.Robotstate.eDirection=zCharging;
//        g_tRobotState.Robotstate.PreviousDirection =zCharging;
//        g_tRobotState.Robotstate.unionAbnormal.AbnIden.BatteryAbn=1;
//        g_tRobotState.Robotstate.unionEEROR.ErrorSta  =0xFF;
        
        /*写入机器人全局信息*/
        loc_Size=sizeof g_tRobotState;
        memcpy(T->DataBuf,(void *)&g_tRobotState,loc_Size);

        /*写入机器人当前角度信息*/
        loc_CurrentAngle=CURRENT_ANGLE;
        loc_Size=sizeof loc_CurrentAngle;
        memcpy(T->DataBuf+sizeof g_tRobotState,&loc_CurrentAngle,loc_Size);
        
        /*写入数据到串口BUF 等待串口发送*/  
        loc_Size=(sizeof g_tRobotState)+(sizeof loc_CurrentAngle);//获取总数据字节
        if(loc_Size > FRAME_DATA_MAX_LENGTH)
        {
            loc_Size=FRAME_DATA_MAX_LENGTH;//数据溢出 标记为发送最大字节
            DEBUG("Tx Frame Over!\r\n");//输出信息
        }
        ucCode_DataFrame(&FR_Tx,T->DataBuf,loc_Size,ucUSART1TrainsmitBuffer[ucWtiteDataToUSART1TransmitGrooveIndex]); 
        ucUSART1TrainsmitLength[ucWtiteDataToUSART1TransmitGrooveIndex]=loc_Size+5;//数据字节+5个字节帧信息
        WriteDataToUSART1TraismitBufferDone();
        
        T->IsSendOut=true;//标记 已经发送了一帧数据
    }
}


/*
* Function Name  : vFrameAutoResendByUsart1
* Description    : 帧自动重发任务（用串口1作为协议信息交流时）
* Input          : R 帧接收记录结构体
* Output         : None 
* Return         : None
*/
void vFrameAutoResendByUsart1(Rx_FrameRecord *R)
{
    static  bool  IsFirstIn=true;
    static  OS_TimeStruct TimFrameAutoResend;
    static  bool  IsRespondErr=false;
    
    if(FR_Tx.IsSendOut == true)//有发送数据
    {
        if(true == IsFirstIn)
        {
            TimFrameAutoResend=GetOSRunTimeNow();//第一次进入  获得进入时间
            IsFirstIn = false;
        }
        
        if(FR_Tx.IsReceiveRespond == true)//接收到回应帧
        {
            //回应帧反馈的是 帧正确
            if(R->tFrame.FrameFunction == ffRespond && R->tFrame.FrameData[0] == FrameSuccess)
            {
                FR_Tx.IsReceiveRespond=false;//清理 接收到回应帧
                FR_Tx.IsSendOut=false;//清零发送标志位
                IsFirstIn= true;//；重新标记第一次进入
            }else//数据有误 需要重新发送
            {
                IsRespondErr= true;
            }
        }
        /*超时还没有接收到回应帧 1秒 或者回应帧表示数据出错*/
        if(TimeOut == MultipleTimeoutCheck( &TimFrameAutoResend,0, 5, 0 ) || IsRespondErr)
        {
            /*发送指针回移动一位  重新发送刚刚缓存区的数据*/
            ucDMA1_Stream4TransmitGrooveIndex = ( (ucDMA1_Stream4TransmitGrooveIndex-1) > \
            USART1_TRANSMIT_GROOVE_SIZE ) ? USART1_TRANSMIT_GROOVE_SIZE : (ucDMA1_Stream4TransmitGrooveIndex-1);
            
            ucUSART1_TransmitMessageNumber++;//串口发送数据加1  表示重发数据
            
            FR_Tx.IsReceiveRespond=false;//清理 接收到回应帧
            FR_Tx.IsSendOut=false;//清零发送标志位
            IsFirstIn= true;//；重新标记第一次进入
        }
    }
}

