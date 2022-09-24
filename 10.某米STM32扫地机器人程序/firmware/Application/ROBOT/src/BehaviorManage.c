#include "behaviormanage.h"
/*
 * Filename   ：behaviormanage.c
 * Author     : PuSheng HuangWenTao LiuHongChuan      
 * Version    : 2.3
 * Date       : 2019.11.28 
                2019.12.6  v2.0 
                2019.12.7  v2.1
                2019.12.9  v2.2
                2019.12.10 v2.3
                2019.12.11 v2.4
                2020. 1. 8 v2.5
 * Discription : 行为处理应用函数
 v1.0:完成状态切换基本框架
 v2.0:加入沿边状态
 v2.1:
      1.修正退出沿边模式的角度范围
      2.加入掉落传感器检测到悬崖后转向
      
 v2.2:
      1.将后退从固定时间改为 直接判断碰撞和掉落状态决定是否后退完毕
      2.删除绕轴转弯中触发碰撞后 需要保存角度 并执行完后退后 回到绕轴转弯状态。
        现在改成直接沿边
 v2.3:
      1.优化RetreatL 、RetreatR状态中转弯的角度比较。使得现在自转切换没有卡顿。
      
 v2.4:
      1.将沿边传感器的AD值转换成距离值。
  v2.5
      1.加入超声波检测到障碍后的延迟恢复速度  避开了超声波的3cm死区
      
*/
#define PuSheng


#ifdef PuSheng


#define  TURN_CORNER_SPEED   (180)  //转弯速度 mm/s
#define  ALONG_EDGE_SPEED    (180)  //沿边速度 mm/s
#define  FORWARD_SPEED       (260)  //前进速度
#define  RETREAT_SPEED       (-180) //后退速度
#define  HAVE_HINDER_SPEED   (100)   //有障碍物时的速度

#define  TURN_ANGLE          (200)   //避开障碍物 转向的角度 单位1/10度

/*限制角度 范围1800 ~ -1799*/
#define TRANSFORM_VALID_ANGLE( angle ) (angle>1800) ? (angle-3600):\
                                       ((angle<=-1800) ? angle+3600:angle)

s16 ForwardSpeed    =  FORWARD_SPEED;    //前进速度
s16 AlongEdgeSpeed  =  ALONG_EDGE_SPEED; //沿边速度

static s16  sConstAngle;//弓型清扫 前进基准角度 
static bool bSoonTurnL1_Or_R0;//弓型清扫 下次转向 1为左 0为右
static u8 ucClashBothSide=0;//碰撞传感器两边都碰撞了
static u8 ucHinderSource=0;// 障碍来源 1 掉落传感器  2碰撞传感器 

static OS_TimeStruct MainTimAuto;//auto的计时器

/*******************************************************************************
* Function Name  : vAlongEdge
* Description    : 沿边状态处理函数
                    碰撞后进入沿边状态，超过前进角度后 退出沿边模式
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
void vAlongEdge(void)
{
    static u8 AlongEdgeSte=0;//是否在沿边模式标志位
    MainTimAuto=GetOSRunTimeNow();//重置弓型清扫时间
    
    if(g_tRobotState.Robotstate.PreviousDirection != zAlongEdge )//第一次进入
    {
        /*上次是碰撞*/
        if(g_tRobotState.Robotstate.PreviousDirection == zAuto)
        {
            AlongEdgeSte=0;//沿边状态清零
        }
        if( ALONG_EDGE_DISTANCE() <100 && ucClashBothSide)//右边距离小于10cm 且是双边碰撞
        {
            DEBUG("\r\n转向40 度转向40度\r\n");
            g_sExpect_Angle=CURRENT_ANGLE+400;//往左转向40 度
            
            if(bSoonTurnL1_Or_R0 == 1)//下一次将向左转 说明此时弓型清扫的前进角度应该为sConstAngle 
            {
                if(g_sExpect_Angle < sConstAngle-900)//当前角度超过弓型清扫 前进角度
                    g_sExpect_Angle=sConstAngle-900;
                
                
            }else//弓型清扫的前进角度应该为sConstAngle-1800 
            {
                if( (g_sExpect_Angle < abs(sConstAngle)+900) && CURRENT_ANGLE> 0)//当前角度超过弓型清扫 前进角度
                    g_sExpect_Angle=sConstAngle;
            }
            
            g_tRobotState.Robotstate.eDirection=zRotate;//旋转绕过 进入就探测到的右边物体
            
            ucClashBothSide=0;//清零双边碰撞标志位
            AlongEdgeSte=1;//标记进入沿边
        }else
        {
            DEBUG("////////%d %d\r\n",ALONG_EDGE_VALUE(),ucClashBothSide);
        }
    }
    else 
    {
        if( ALONG_EDGE_DISTANCE() < 50)//距离小于5CM
        {   
            AlongEdgeSte = 1;//进入沿边状态
            
           //直接开始沿边
            DEBUG("沿");
            g_sExpect_speed=AlongEdgeSpeed;//赋值沿边速度
            ucRouteAdjustByAlongEdgeDistance(ALONG_EDGE_DISTANCE(),20,g_sExpect_speed);//沿边
        }
        else//与障碍物距离间隔大于5CM 继续旋转
        {
            if(AlongEdgeSte)//在沿边模式
            {
                DEBUG("右");
                /*绕右轴转向 搜寻目标*/
                g_sExpect_Angle=CURRENT_ANGLE-900;//往右转向
                g_sExpect_Angle=TRANSFORM_VALID_ANGLE(g_sExpect_Angle);//角度有效化
                g_sExpect_speed=TURN_CORNER_SPEED;
                ucRotateAroundAxle(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range,LWheel);
            }else
            {
                DEBUG("自");
                /*自转 搜寻目标*/
                g_sExpect_Angle=CURRENT_ANGLE+200;//往左转向
                g_sExpect_speed=TURN_CORNER_SPEED;
                ucTurn_Angle_Round(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range);
            }
        }
    }
    
    /*判断角度范围 决定是否退出沿边*/
    if(bSoonTurnL1_Or_R0 == 1)//下一次将向左转 说明此时弓型清扫的前进角度应该为sConstAngle=0 
    {
        /* 此时正常前进角度为sConstAngle=0  当机器人角度在-100度到-170度是 认为角度超过范围 退出沿边*/
        if(CURRENT_ANGLE < sConstAngle-1000 && CURRENT_ANGLE>-1700)//当前角度超过弓型清扫 前进角度
        {
            AlongEdgeSte=0;//退出沿边模式
            g_tIncPID.LastData=0;
            g_tIncPID.PreData=0;
            DEBUG("\r\n去Auto模式1去Auto模式1\r\n");
            g_tRobotState.Robotstate.eDirection=zAuto;//去到Auto状态
        }
    }else//弓型清扫的前进角度应该为sConstAngle-1800 
    {
        if( (CURRENT_ANGLE < abs(sConstAngle)+800) && CURRENT_ANGLE> 10)//当前角度超过弓型清扫 前进角度
        {
            AlongEdgeSte=0;//退出沿边模式
            g_tIncPID.LastData=0;
            g_tIncPID.PreData=0;
            DEBUG("\r\n去Auto模式2去Auto模式2\r\n");
            g_tRobotState.Robotstate.eDirection=zAuto;
        }
    }
    
    g_tRobotState.Robotstate.PreviousDirection=zAlongEdge;//即将出AlongEdge状态 标记AlongEdge为上次方向
}
/*******************************************************************************
* Function Name  : vBehaviorStop
* Description    : 停止状态函数
                   进入该状态后 机器人停止驱动轮
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
void vBehaviorStop(void)
{
    if(g_tRobotState.Robotstate.PreviousDirection != zStop)
        DEBUG("zStop into\r\n");
    g_sExpect_speed=0;
    g_tLeftWheel.ExpectSpeed=g_sExpect_speed;
    g_tRightWheel.ExpectSpeed=g_sExpect_speed;
    g_tRobotState.Robotstate.PreviousDirection=zStop;//即将出Stop状态 标记Stop为上次方向
}
/*******************************************************************************
* Function Name  : vBehaviorForward
* Description    : 前进状态函数
                   进入该状态后 机器人以期望速度g_sExpect_speed 向期望角度g_sExpect_Angle前进
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
void vBehaviorForward(void)
{
//    if(g_tRobotState.Robotstate.PreviousDirection != zForward)
//        DEBUG("zForward into\r\n");
    g_sExpect_speed=ForwardSpeed;//赋值前进速度
    ucRoute_Adjust(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed);
    g_tRobotState.Robotstate.eDirection=zAuto;//去到auto状态
    g_tRobotState.Robotstate.PreviousDirection=zForward;//即将出Forward状态 标记Forward为上次方向
}
/*******************************************************************************
* Function Name  : vBehaviorRetreat
* Description    : 后退状态函数
                   进入该状态后 机器人以期望速度g_sExpect_speed 向期望角度g_sExpect_Angle后退
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
void vBehaviorRetreat(void)
{ 
    g_sExpect_speed=RETREAT_SPEED;//赋值后退速度
    ucRoute_Adjust(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed);
    g_tRobotState.Robotstate.eDirection=zAuto;//去到auto状态
    g_tRobotState.Robotstate.PreviousDirection=zRetreat;//即将出Retreat状态 标记Retreat为上次方向
}
/*******************************************************************************
* Function Name  : vBehaviorRetreatL
* Description    : 后退然后转左 状态函数
                   进入该状态后 机器人以当前角度配合期望速度RETREAT_SPEED后退BACK_OFF_TIME时间
                   然后以速度TURN_CORNER_SPEED向左转TURN_ANGLE度
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
void vBehaviorRetreatL(void)
{
    static bool IsFirstIn=true;//是否第一次进入
    static s16 TargeAngle;//目标角度
    
    /*第一次进入*/
    if(g_tRobotState.Robotstate.PreviousDirection != zRetreatL)
    {
        DEBUG("RetreatL RetreatL RetreatL into\r\n");
    }
    if(IsFirstIn == true)
    {
        g_sExpect_Angle=CURRENT_ANGLE;//获取第一次进入的角度为基准角度
        TargeAngle=TRANSFORM_VALID_ANGLE(g_sExpect_Angle+TURN_ANGLE);//保存第一次进入的角度 获取目标角度
        IsFirstIn=false;
    }

    /*检测是否还在碰撞 或者掉落*/
    if(g_tRobotState.Robotstate.unionAbnormal.AbnIden.RightCollisionAbn ||\
       g_tRobotState.Robotstate.unionAbnormal.AbnIden.LeftCollisionAbn  ||\
       g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLBAbn||\
       g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLFAbn||\
       g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRBAbn||\
       g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRFAbn)
    {
        g_sExpect_speed=RETREAT_SPEED;//赋值 后退速度
        ucRoute_Adjust(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed);//后退
        
    }else /*后退时间到了 要转弯了*/
    {
        g_sExpect_Angle=CURRENT_ANGLE+200;//改变期望角度为当前角度加一个20度 左转20.
        g_sExpect_speed=TURN_CORNER_SPEED;//速度为转弯速度
        
        /*开始自转*/
        ucTurn_Angle_Round(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range);

        /*当前角度 大于目标角度 */
        if( TargeAngle <= CURRENT_ANGLE )
        {
            /*已经到达指定角度*/
            g_tRobotState.Robotstate.eDirection=zAuto;//去到auto状态
            IsFirstIn=true;//重新标记第一次进入
            g_tLeftWheel.ExpectSpeed= 0;
            g_tRightWheel.ExpectSpeed= 0;
        } 
    }
    g_tRobotState.Robotstate.PreviousDirection=zRetreatL;//即将RetreatL状态 标RetreatL上次方向
}
/*******************************************************************************
* Function Name  : vBehaviorRetreatR
* Description    : 后退然后转右 状态函数
                   进入该状态后 机器人以当前角度配合期望速度RETREAT_SPEED后退BACK_OFF_TIME时间
                   然后以速度TURN_CORNER_SPEED向右转TURN_ANGLE度
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
void vBehaviorRetreatR(void)
{
    static bool IsFirstIn=true;//是否第一次进入
    static s16 TargeAngle;//目标角度
    
    /*第一次进入*/
    if(g_tRobotState.Robotstate.PreviousDirection != zRetreatR)
    {
        DEBUG("RetreatR RetreatR RetreatR into\r\n");
    }
    
    if(IsFirstIn == true)
    {
        g_sExpect_Angle=CURRENT_ANGLE;//获取第一次进入的角度为基准角度
        TargeAngle=TRANSFORM_VALID_ANGLE(g_sExpect_Angle-TURN_ANGLE);//保存第一次进入的角度 获取目标角度
        IsFirstIn=false;
    }
    /*检测是否还在碰撞 或者掉落*/
    if(g_tRobotState.Robotstate.unionAbnormal.AbnIden.RightCollisionAbn ||\
       g_tRobotState.Robotstate.unionAbnormal.AbnIden.LeftCollisionAbn  ||\
       g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLBAbn||\
       g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLFAbn||\
       g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRBAbn||\
       g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRFAbn)
    {
        g_sExpect_speed=RETREAT_SPEED;//赋值后退速度
        ucRoute_Adjust(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed);//后退
    }else
    { 
        /*后退时间到了 要转弯了*/
        g_sExpect_Angle=CURRENT_ANGLE-200;//改变期望角度为当前角度减一个20度 右转20.
        g_sExpect_speed=TURN_CORNER_SPEED;//速度为转弯速度
        
        /*开始自转*/
        ucTurn_Angle_Round(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range);
        
        /*当前角度 小于目标角度 */
        if(TargeAngle >= CURRENT_ANGLE)
        {
            /*已经到达指定角度*/
            g_tRobotState.Robotstate.eDirection=zAuto;//去到auto状态
            IsFirstIn=true;//重新标记第一次进入
            g_tLeftWheel.ExpectSpeed= 0;
            g_tRightWheel.ExpectSpeed= 0;
        }  
    }
    g_tRobotState.Robotstate.PreviousDirection=zRetreatR;//即将出RetreatR状态 标记RetreatR为上次方向
}
/*******************************************************************************
* Function Name  : vBehaviorTurnLeft
* Description    : 绕轴左转弯
                   进入该状态后 机器人以期望速度TURN_CORNER_SPEED向左转动到g_sExpect_Angle角度
                   执行该函数会复位前进时间 
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
void vBehaviorTurnLeft(void)
{ 
    MainTimAuto=GetOSRunTimeNow();//复位前进时间
    g_sExpect_speed=TURN_CORNER_SPEED;//赋值转弯速度
    if(g_tRobotState.Robotstate.PreviousDirection != zTurnLeft)
        DEBUG("TurnLeft TurnLeft TurnLeft into\r\n");
    /*绕左轮旋转*/
    if(0 == ucRotateAroundAxle(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range,RWheel))
    {
        /*已经到达指定角度*/
        DEBUG("TurnLeft %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
        g_tRobotState.Robotstate.eDirection=zAuto;//去到auto状态
    } 
    
    g_tRobotState.Robotstate.PreviousDirection=zTurnLeft;//即将出TurnLeft状态 标记TurnLeft为上次方向

}

/*******************************************************************************
* Function Name  : vBehaviorTurnRight
* Description    : 绕轴右转弯
                   进入该状态后 机器人以期望速度TURN_CORNER_SPEED向右转动到g_sExpect_Angle角度
                   执行该函数会复位前进时间 
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
void vBehaviorTurnRight(void)
{
    MainTimAuto=GetOSRunTimeNow();//复位前进时间
    g_sExpect_speed=TURN_CORNER_SPEED;//赋值转弯速度
    if(g_tRobotState.Robotstate.PreviousDirection != zTurnRight)
        DEBUG("TurnRight TurnRight TurnRight into\r\n");
    /*绕右轮旋转*/
    if(0 == ucRotateAroundAxle(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range,LWheel))
    {
        /*已经到达指定角度*/
        DEBUG("TurnRight %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
        g_tRobotState.Robotstate.eDirection=zAuto;//去到auto状态
    } 
    g_tRobotState.Robotstate.PreviousDirection=zTurnRight;//即将出TurnRight状态 标记TurnRight为上次方向
}
/*******************************************************************************
* Function Name  : vBehaviorRotate
* Description    : 原地旋转
                   进入该状态后 机器人以期望速度TURN_CORNER_SPEED原地转动到g_sExpect_Angle角度
                   执行该函数会复位前进时间 
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
void vBehaviorRotate(void)
{
    static bool IsAlongEdgeIn=false;//是否从沿边模式进入的？
    
    if(g_tRobotState.Robotstate.PreviousDirection != zRotate)
    {
        DEBUG("Rotate into\r\n");
        g_sExpect_speed=TURN_CORNER_SPEED;//赋值转弯速度
        if(g_tRobotState.Robotstate.PreviousDirection == zAlongEdge)//上次状态沿边模式
        {
            IsAlongEdgeIn=true;
        }
    }
    /*自转*/
    if(0 == ucTurn_Angle_Round(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range))
    {
        /*已经到达指定角度*/
        if(IsAlongEdgeIn)//上次是从沿边过来的
        {
            DEBUG("to zAlongEdge \r\n");
            g_tRobotState.Robotstate.eDirection=zAlongEdge;
            IsAlongEdgeIn=false;
        }else
        {
            
            DEBUG("to zAuto \r\n");
            g_tRobotState.Robotstate.eDirection=zAuto;//去到auto状态
        }
    } 
    g_tRobotState.Robotstate.PreviousDirection=zRotate;//即将出Rotate状态 标记Rotate为上次方向
}
/*******************************************************************************
* Function Name  : vBehaviorAuto
* Description    : 自动状态切换函数
                   进入该状态后 机器人开始走弓型清扫 并处理沿边的进入 
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
void vBehaviorAuto(void)
{
    static bool IsFirstIn=true;//是否第一次进入
    
    if(IsFirstIn == true)
    {
        g_sExpect_Angle=CURRENT_ANGLE;
        sConstAngle=00;
        bSoonTurnL1_Or_R0=1;//第一次先左转
        MainTimAuto=GetOSRunTimeNow();  //获取当前时间
        IsFirstIn=false;
    }
    /*上次状态是后退 说明刚刚碰到障碍物了*/
    if(g_tRobotState.Robotstate.PreviousDirection == zRetreatL ||\
       g_tRobotState.Robotstate.PreviousDirection == zRetreatR )
    {
        if(ucHinderSource == 2)//如果是碰撞传感器
        {
            DEBUG("\r\n去沿边模式去沿边模式\r\n");
            g_tRobotState.Robotstate.eDirection=zAlongEdge;//去沿边模式
        }
        else if (ucHinderSource == 1)//如果是悬崖传感器
        {
            bSoonTurnL1_Or_R0= !bSoonTurnL1_Or_R0;//掉头
            DEBUG("\r\n悬崖转向悬崖转向悬崖转向 %d\r\n",bSoonTurnL1_Or_R0);
        }
    } 
    else
    {
        DEBUG("auto");
//g_tRobotState.Robotstate.eDirection=  g_tRobotState.Robotstate.PreviousDirection;//帧控制使用  一直执行对应的状态
        /*前进时间到了*/
        if( TimeOut == MultipleTimeoutCheck(&MainTimAuto,0,5,0))
        {
            if(bSoonTurnL1_Or_R0 == 1)
            {
                g_sExpect_Angle=sConstAngle+1800;
                g_tRobotState.Robotstate.eDirection=zTurnLeft;//去到左转状态
                bSoonTurnL1_Or_R0=0;//下次为右转
            }else
            {
                g_sExpect_Angle=sConstAngle;
                g_tRobotState.Robotstate.eDirection=zTurnRight;//去到右转状态
                bSoonTurnL1_Or_R0=1;//下次为左转
            }
        }else//继续直行
        {
            if(bSoonTurnL1_Or_R0 == 1)//如果下次是左转
            {
                g_sExpect_Angle=sConstAngle;
            }else
            {
                g_sExpect_Angle=sConstAngle-1800;
            }
            g_sExpect_Angle=TRANSFORM_VALID_ANGLE(g_sExpect_Angle);
            g_tRobotState.Robotstate.eDirection=zForward;//去到前进状态
        }
    }
    g_tRobotState.Robotstate.PreviousDirection=zAuto;//即将出Auto状态 标记Auto为上次方向

}
/*******************************************************************************
* Function Name  : vBehaviorManageTask
* Description    : 行为切换任务函数
                   根据当前的行为状态来执行对应的函数 
* Input          : p 任务消息 未使用
* Output         : None 
* Return         : None
*******************************************************************************/
void vBehaviorManageTask(void *p)
{
    while(GYRO_WORKING != eGyro_State)//等待陀螺仪进入工作状态
    {
        vTaskDelay(5);
    }
    if(g_tRobotState.Robotstate.eDirection != zCharging)
        g_tRobotState.Robotstate.eDirection=zAuto;
    
    while(1)
    {
        switch(g_tRobotState.Robotstate.eDirection)
        {
            case  zStop:
                vBehaviorStop();
                break;
            case  zForward:                //前进
                vBehaviorForward();
                break;
            case  zRetreatL:               //后退后左转
                vBehaviorRetreatL();
                break;
            case  zRetreatR:               //后退后右转
                vBehaviorRetreatR();
                break;
            case  zTurnLeft :              //左转
                vBehaviorTurnLeft();
                break;
            case  zTurnRight:              //右转
                vBehaviorTurnRight();
                break;
            case  zRotate:                 //原地旋转
                vBehaviorRotate();
                break;
            case  zAuto :                  //自动
                vBehaviorAuto();
                break;
            case  zChargeBack :            //回充
                break;
            case  zCharging :              //正在充电
                break;
            case  zAlongEdge:              //沿边
                vAlongEdge();
                break;
            case  zRetreat:                //后退
                vBehaviorRetreat();
                break;
        }
        vTaskDelay(5);
    }
}

/*******************************************************************************
* Function Name  : vErrorAbnormalCheckTask
* Description    : 异常错误检测函数
                   检测包括 驱动电机 滚刷电机 边刷电机 风机 掉落传感器 碰撞传感器  
                   电池 等异常错误的信息 并根据信息类型转动对应的机器人状态
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
void vErrorAbnormalCheckTask(void *p)
{
    while(1)
    {
        for(;;)
        {
            vTaskDelay(5);
            DropCheckUpdate();//掉落传感器数据更新
            UltraSonicCheckUpdate();//超声波传感器数据更新
            AllMotorErrorCheck();//所有电机错误检测
            if(g_tRobotState.Robotstate.unionAbnormal.AbnormalSta != 0)//有异常情况
            {
                /*轮子悬空*/
                if(g_tRobotState.Robotstate.unionAbnormal.AbnIden.LeftWheelAbn ||\
                   g_tRobotState.Robotstate.unionAbnormal.AbnIden.RightWheelAbn  )
                {
                    g_tRobotState.Robotstate.eDirection=zStop;
                    break;
                }else
                {
                    //g_tRobotState.Robotstate.eDirection=zAuto;
                }
                
                /*停止与充电状态不判断碰撞和超声波*/
                if(g_tRobotState.Robotstate.eDirection == zStop ||\
                   g_tRobotState.Robotstate.eDirection == zCharging )
                {
                    break;
                }
                /*检测到碰撞*/
                if(g_tRobotState.Robotstate.unionAbnormal.AbnIden.LeftCollisionAbn||\
                   g_tRobotState.Robotstate.unionAbnormal.AbnIden.RightCollisionAbn )
                {
                    MainTimAuto=GetOSRunTimeNow();//复位前进时间
                    ucHinderSource=2;//标记障碍来源
                    /*是否两边同时碰撞*/
                    if(g_tRobotState.Robotstate.unionAbnormal.AbnIden.LeftCollisionAbn &&\
                       g_tRobotState.Robotstate.unionAbnormal.AbnIden.RightCollisionAbn )
                    {
                        ucClashBothSide=1;
                        DEBUG("6");
                    }else
                    {
                        //ucClashBothSide=0;
                        //DEBUG("5");
                    }
                    g_tRobotState.Robotstate.eDirection=zRetreatL;

                }
                /*检测到跌落*/
                if(g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLBAbn||\
                   g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLFAbn||\
                   g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRBAbn||\
                   g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRFAbn)
                {
                    MainTimAuto=GetOSRunTimeNow();//复位前进时间
                    ucHinderSource=1;//标记障碍来源
                    if(bSoonTurnL1_Or_R0 == 1 )
                    {
                        g_tRobotState.Robotstate.eDirection=zRetreatL;
                    }
                    else 
                    {
                        g_tRobotState.Robotstate.eDirection=zRetreatR;
                    }
                }
            }
        }
    }
}
/*******************************************************************************
* Function Name  : DropCheckUpdate
* Description    : 掉落传感器数据检测函数
                   检测掉落传感器的数据 上报传感器的数值
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
void DropCheckUpdate(void)
{
    if(tDropcheck.bAnyoneFall)
    {
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLBAbn=tDropcheck.bBackLFall;
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLFAbn=tDropcheck.bFrontLFall;
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRBAbn=tDropcheck.bBackRFall;
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRFAbn=tDropcheck.bFrontRFall;
        //DEBUG("DropCheck\r\n");
    }else
    {
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLBAbn=0;
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLFAbn=0;
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRBAbn=0;
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRFAbn=0;
    }
}
/*******************************************************************************
* Function Name  : UltraSonicCheckUpdate
* Description    : 超声波传感器数据检测函数
                   检测超声波传感器的数据 上报传感器的数值 若检测到障碍物 前进速度减半
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
void UltraSonicCheckUpdate(void)
{
    static OS_TimeStruct UltraSonicHinderTim;
    
    g_tRobotState.Robotstate.unionAbnormal.AbnIden.UltraSonicAbn=bGetClashFlag();//得到超声波状态
    if(g_tRobotState.Robotstate.unionAbnormal.AbnIden.UltraSonicAbn)
    {
        //DEBUG("UlSonicClash\r\n");
        
        UltraSonicHinderTim=GetOSRunTimeNow();//获取当前时间
        ForwardSpeed    =  HAVE_HINDER_SPEED;    //前进速度
        AlongEdgeSpeed  =  HAVE_HINDER_SPEED;    //沿边速度
    }
    else
    {
        /*没有检测到障碍物 也等待1s 才恢复速度*/
        if(TimeOut == SingleTimeoutCheck( UltraSonicHinderTim,0, 1, 0 ))
        {
            ForwardSpeed    =  FORWARD_SPEED;    //前进速度
            AlongEdgeSpeed  =  ALONG_EDGE_SPEED; //沿边速度
        }
    }
}



/*******************************************************************************
* Function Name  : AllMotorErrorCheck
* Description    : 所有电机错误检测
                   检测所有电机的电流AD值，超过阈值则停止扫地机
* Input          : None
* Output         : None 
* Return         : None
*******************************************************************************/
extern uint16_t g_usInChipAdcChannelAverageValue[ADC_CHANNEL_NUM]; //ADC 各通道平均值
static OS_TimeStruct MotorErrorCheckTime;//检查电机错误的时间
void AllMotorErrorCheck(void)
{
    #define MotorAdStopThreshold 1200 //电机AD阈值，1m内AD均值超过此阈值则让电机停下
    static u8  Count= 0;//AD累加计数值
    static uint16_t LeftWheelAd = 0,RightWheelAd = 0,SideBrushAd = 0,RollBrushAd = 0;
   
    if( TimeOut == MultipleTimeoutCheck(&MotorErrorCheckTime,0,0,100))//100ms计一个值
    {

        LeftWheelAd += g_usInChipAdcChannelAverageValue[adcLeftWheel]/10;//左轮
        RightWheelAd += g_usInChipAdcChannelAverageValue[adcRightWheel]/10;//右轮
        SideBrushAd += g_usInChipAdcChannelAverageValue[adcSideBrush]/10;//边刷
        RollBrushAd += g_usInChipAdcChannelAverageValue[adcRollBrush]/10;//滚刷
        Count++;
    }
    if(Count>=10)//累计达到10个值
    {
        
        if((LeftWheelAd>MotorAdStopThreshold)| (RightWheelAd>MotorAdStopThreshold)| (SideBrushAd>MotorAdStopThreshold)| (RollBrushAd>MotorAdStopThreshold))
        {
            g_tRobotState.Robotstate.eDirection=zStop;
            DEBUG("电机卡住了\r\n");
        }
       
        LeftWheelAd = 0;
        RightWheelAd = 0;
        SideBrushAd = 0;
        RollBrushAd = 0;
        Count = 0;
    }
    
     
    
}


#endif

#ifdef HongChuan 



#define MYDEBUG
#ifdef MYDEBUG
#define DEBUG  printf
#else
#define DEBUG(...)  (void)(0)
#endif

#define  TURN_CORNER_SPEED   (180) //转弯速度 mm/s
#define  FORWARD_SPEED       (300) //前进速度
#define  RETREAT_SPEED       (-200) //后退速度
#define  ALONGEDGE_SPEED       (150) //沿边速度

#define  TURN_ANGLE         (200) //避开障碍物 转向角度 单位1/10度
#define  BACK_OFF_TIME      (200)//后退时间 单位ms
#define  FRONT_OFF_TIME      (100)//前进时间 单位ms


s16 ForwardSpeed  =  FORWARD_SPEED; //前进速度

void vBehaviorStop(void)
{
    if(g_tRobotState.Robotstate.PreviousDirection != zStop)
        DEBUG("zStop into\r\n");
    g_sExpect_speed=0;
    g_tLeftWheel.ExpectSpeed=g_sExpect_speed;
    g_tRightWheel.ExpectSpeed=g_sExpect_speed;
    g_tRobotState.Robotstate.PreviousDirection=zStop;//即将出Stop状态 标记Stop为上次方向
}
/*前进*/
void vBehaviorForward(void)
{
    if(g_tRobotState.Robotstate.PreviousDirection != zForward)
        DEBUG("zForward into\r\n");
    g_sExpect_speed=ForwardSpeed;//赋值前进速度
    ucRoute_Adjust(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed);
    g_tRobotState.Robotstate.eDirection=zAuto;//去到auto状态
    g_tRobotState.Robotstate.PreviousDirection=zForward;//即将出Forward状态 标记Forward为上次方向
}

void vBehaviorRetreatL(void)
{
    static bool IsFirstIn=true;//是否第一次进入
    static OS_TimeStruct Loc_TimRetreatL;
    static s16 TurnAngle;//转弯后的角度
    static s16 SaveGlobalAngle;//保存进入RetreatL状态前的角度
    static u8 IsSave=0;//是否保存之前的角度 0 不保存  1 上次是左转 2上次是右转
    
    if(g_tRobotState.Robotstate.PreviousDirection != zRetreatL)
        DEBUG("RetreatL RetreatL RetreatL into\r\n");
    /*如果上一个状态是在绕轴转弯，则有可能还没有转完，所以先保存下上次的期望角度*/
    if(g_tRobotState.Robotstate.PreviousDirection == zTurnLeft )
    {
        IsSave=1;
        SaveGlobalAngle=g_sExpect_Angle;//保存全局角度
        DEBUG("SaveGlobalAngle %d\r\n",SaveGlobalAngle);        
    }
    if( g_tRobotState.Robotstate.PreviousDirection == zTurnRight )
    {
        IsSave=2;
        SaveGlobalAngle=g_sExpect_Angle;//保存全局角度
    }
    if(IsFirstIn == true)
    {
        g_sExpect_Angle=CURRENT_ANGLE;//获取第一次进入的角度为基准角度
        TurnAngle=g_sExpect_Angle+TURN_ANGLE;//转弯的期望角度  左转+
        Loc_TimRetreatL=GetOSRunTimeNow();  //获取当前时间
        IsFirstIn=false;
    }
    if( NotTimeOut == SingleTimeoutCheck(Loc_TimRetreatL,0,0,BACK_OFF_TIME))
    {
        g_sExpect_speed=RETREAT_SPEED;//赋值 后退速度
        ucRoute_Adjust(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed);//后退
        
    }else /*后退时间到了 要转弯了*/
    {
        g_sExpect_Angle=TurnAngle;//改变期望角度为 转向后角度
        g_sExpect_speed=TURN_CORNER_SPEED;//速度为转弯速度
        /*自转*/
        if(0 == ucTurn_Angle_Round(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range))
        {
            /*已经到达指定角度*/
            if(1 == IsSave )//上次状态是左转 继续左转
            {
                g_sExpect_Angle=SaveGlobalAngle;//恢复全局角度
                g_tRobotState.Robotstate.eDirection=zTurnLeft;//去到TurnLeft状态
                DEBUG("1 TurnLeft %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
            }else
            if(2 == IsSave )//上次状态是右转 继续右转
            {
                g_sExpect_Angle=SaveGlobalAngle;//恢复全局角度
                g_tRobotState.Robotstate.eDirection=zTurnRight;//去到TurnRight状态
                DEBUG("2 TurnLeft %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
            }else
            {
                g_tRobotState.Robotstate.eDirection=zAuto;//去到auto状态
            }
            IsFirstIn=true;//重新标记第一次进入
        } 
    }
    g_tRobotState.Robotstate.PreviousDirection=zRetreatL;//即将RetreatL状态 标RetreatL上次方向
}
void vBehaviorRetreatR(void)
{
    static bool IsFirstIn=true;//是否第一次进入
    static OS_TimeStruct Loc_TimRetreatR;
    static s16 TurnAngle;//转弯后的角度
    static s16 SaveGlobalAngle;//保存进入RetreatL状态前的角度
    static u8 IsSave=0;//是否保存之前的角度 0 不保存  1 上次是左转 2上次是右转
    
    if(g_tRobotState.Robotstate.PreviousDirection != zRetreatR)
        DEBUG("RetreatR RetreatR RetreatR into\r\n");
    /*如果上一个状态是在绕轴转弯，则有可能还没有转完，所以先保存下上次的期望角度*/
    if(g_tRobotState.Robotstate.PreviousDirection == zTurnLeft )
    {
        IsSave=1;
        SaveGlobalAngle=g_sExpect_Angle;//保存全局角度    
    }
    if( g_tRobotState.Robotstate.PreviousDirection == zTurnRight )
    {
        IsSave=2;
        SaveGlobalAngle=g_sExpect_Angle;//保存全局角度
    }
    
    if(IsFirstIn == true)
    {
        g_sExpect_Angle=CURRENT_ANGLE;//获取第一次进入的角度为基准角度
        TurnAngle=g_sExpect_Angle-TURN_ANGLE;//转弯的期望角度  右转-
        Loc_TimRetreatR=GetOSRunTimeNow();  //获取当前时间
        IsFirstIn=false;
    }
    
    if( NotTimeOut == SingleTimeoutCheck(Loc_TimRetreatR,0,0,BACK_OFF_TIME))
    {
        g_sExpect_speed=RETREAT_SPEED;//赋值后退速度
        ucRoute_Adjust(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed);//后退
    }else
    { 
        /*后退时间到了 要转弯了*/
        g_sExpect_Angle=TurnAngle;//改变期望角度为 转向后角度
        g_sExpect_speed=TURN_CORNER_SPEED;//速度为转弯速度
        /*自转*/
        if(0 == ucTurn_Angle_Round(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range))
        {
            /*已经到达指定角度*/
            if(1 == IsSave )//上次状态是左转 继续左转
            {
                g_sExpect_Angle=SaveGlobalAngle;//恢复全局角度
                g_tRobotState.Robotstate.eDirection=zTurnLeft;//去到TurnLeft状态
            }else
            if(2 == IsSave )//上次状态是右转 继续右转
            {
                g_sExpect_Angle=SaveGlobalAngle;//恢复全局角度
                g_tRobotState.Robotstate.eDirection=zTurnRight;//去到TurnRight状态
            }else
            {
                g_tRobotState.Robotstate.eDirection=zAuto;//去到auto状态
            }
            IsFirstIn=true;//重新标记第一次进入
        }  
    }
    g_tRobotState.Robotstate.PreviousDirection=zRetreatR;//即将出RetreatR状态 标记RetreatR为上次方向
}

void vBehaviorTurnLeft(void)
{
    g_sExpect_speed=TURN_CORNER_SPEED;//赋值转弯速度

    if(g_tRobotState.Robotstate.PreviousDirection != zTurnLeft)
        DEBUG("TurnLeft TurnLeft TurnLeft into\r\n");
    /*绕左轮旋转*/
    if(0 == RotateAroundAxle(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range,RWheel))
    {
        /*已经到达指定角度*/
        DEBUG("TurnLeft %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
        g_tRobotState.Robotstate.eDirection=zAuto;//去到auto状态
    } 
    
    g_tRobotState.Robotstate.PreviousDirection=zTurnLeft;//即将出TurnLeft状态 标记TurnLeft为上次方向

}
void vBehaviorTurnRight(void)
{
    g_sExpect_speed=TURN_CORNER_SPEED;//赋值转弯速度
    if(g_tRobotState.Robotstate.PreviousDirection != zTurnRight)
        DEBUG("TurnRight TurnRight TurnRight into\r\n");
    /*绕右轮旋转*/
    if(0 == RotateAroundAxle(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range,LWheel))
    {
        /*已经到达指定角度*/
        DEBUG("TurnRight %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
        g_tRobotState.Robotstate.eDirection=zAuto;//去到auto状态
    } 
    g_tRobotState.Robotstate.PreviousDirection=zTurnRight;//即将出TurnRight状态 标记TurnRight为上次方向
}
void vBehaviorRotate(void)
{

    static bool IsFirstIn=true;//是否第一次进入
    if(g_tRobotState.Robotstate.PreviousDirection != zRotate)
        DEBUG("Rotate into\r\n");
    if(IsFirstIn == true)
    {
        g_sExpect_speed=TURN_CORNER_SPEED;//赋值转弯速度
        IsFirstIn=false;
    }
    /*自转*/
    if(0 == ucTurn_Angle_Round(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range))
    {
        /*已经到达指定角度*/
        g_tRobotState.Robotstate.eDirection=zAuto;//去到auto状态
        IsFirstIn=true;//重新标记第一次进入
    } 
    g_tRobotState.Robotstate.PreviousDirection=zRotate;//即将出Rotate状态 标记Rotate为上次方向
}
void vBehaviorAuto(void)
{
        
    
    static bool IsFirstIn=true;//是否第一次进入
    static OS_TimeStruct Loc_TimAuto;
    static s16  constAngle;
    int i=0;
    
     g_sExpect_speed=ALONGEDGE_SPEED;//赋值转弯速度
// g_tRobotState.Robotstate.eDirection=zForward;
//    g_tRobotState.Robotstate.eDirection=zStop;

    if(g_tRobotState.Robotstate.PreviousDirection != zAuto)
        DEBUG("Auto into\r\n");
    if(IsFirstIn == true)
    {
        g_sExpect_Angle=CURRENT_ANGLE;
        constAngle=g_sExpect_Angle;
        Loc_TimAuto=GetOSRunTimeNow();  //获取当前时间
        IsFirstIn=false;
    }
    /*前进时间到了*/
    if( TimeOut == MultipleTimeoutCheck(&Loc_TimAuto,0,10,0))
    {
        if(abs(CURRENT_ANGLE-constAngle) <TURN_ANGLE+g_sExpect_range+10)
        {
            g_sExpect_Angle=constAngle+1785;
            DEBUG("Auto TurnLeft g_sExpect_Angle %d\r\n ",g_sExpect_Angle);
            g_tRobotState.Robotstate.eDirection=zTurnLeft;//去到左转状态
            DEBUG("左 %d %d \r\n",CURRENT_ANGLE,g_sExpect_Angle);
        }else
        {
            g_sExpect_Angle=constAngle+20;
            g_tRobotState.Robotstate.eDirection=zTurnRight;//去到右转状态
            DEBUG("右 %d %d \r\n",CURRENT_ANGLE,g_sExpect_Angle);
        }
    }else
    {
        if(abs(CURRENT_ANGLE-constAngle) <TURN_ANGLE+g_sExpect_range+10)
        {
            g_sExpect_Angle=constAngle;
        }else
        {
            g_sExpect_Angle=constAngle-1800;
        }
        g_tRobotState.Robotstate.eDirection=zForward;//去到前进状态
    }
    g_tRobotState.Robotstate.PreviousDirection=zAuto;//即将出Auto状态 标记Auto为上次方向

}
/*行为转换任务*/
void vBehaviorManageTask(void *p)
{
    while(GYRO_WORKING != eGyro_State)
    {
        vTaskDelay(5);
    }
    g_tRobotState.Robotstate.eDirection=zAuto;
    while(1)
    {
        switch(g_tRobotState.Robotstate.eDirection)
        {
            case  zStop:
                vBehaviorStop();
                break;
            case  zForward:                //前进
                vBehaviorForward();
                break;
            case  zRetreatL:               //后退后左转
                vBehaviorRetreatL();
                break;
            case  zRetreatR:               //后退后右转
                vBehaviorRetreatR();
                break;
            case  zTurnLeft :              //左转
                vBehaviorTurnLeft();
                break;
            case  zTurnRight:              //右转
                vBehaviorTurnRight();
                break;
            case  zRotate:                 //原地旋转
                vBehaviorRotate();
                break;
            case  zAuto :                  //自动
                vBehaviorAuto();
                break;
            case  zChargeBack :            //回充
                break;
            case  zCharging :              //正在充电
                break;
            case  zAlongEdge :              //正在充电
                EdgeChrashCnt(0,0); 
                break;
              
        }
        vTaskDelay(5);
    }
}

/*异常和错误检测*/
void vErrorAbnormalCheckTask(void *p)
{
    
    
    static s16  constAngle; 
    static bool IsCrashToEdge=true; //碰撞后转转沿边标志
    static OS_TimeStruct Loc_TimCrash;

    while(1)
    {
        for(;;)
        {
            vTaskDelay(5);
            DropCheckUpdate();
            UltraSonicCheckUpdate();
            if(g_tRobotState.Robotstate.unionAbnormal.AbnormalSta != 0)//有异常情况
            {
                /*轮子悬空*/
                if(g_tRobotState.Robotstate.unionAbnormal.AbnIden.LeftWheelAbn ||\
                   g_tRobotState.Robotstate.unionAbnormal.AbnIden.RightWheelAbn  )
                {
                    g_tRobotState.Robotstate.eDirection=zStop;
                    break;
                }else
                {
                    //g_tRobotState.Robotstate.eDirection=zAuto;
                }
                /*检测到碰撞*/
               if(g_tRobotState.Robotstate.unionAbnormal.AbnIden.LeftCollisionAbn||\
                     g_tRobotState.Robotstate.unionAbnormal.AbnIden.RightCollisionAbn)
                {
                    if(g_tRobotState.Robotstate.eDirection == zForward ||\
                       g_tRobotState.Robotstate.eDirection == zTurnLeft||\
                       g_tRobotState.Robotstate.eDirection == zAuto ||\
                       g_tRobotState.Robotstate.eDirection == zAlongEdge
                    )
                    {
                        g_tRobotState.Robotstate.eDirection=zRetreatL;
                        
                    }
                    else if(g_tRobotState.Robotstate.eDirection == zTurnRight)
                    {
                        g_tRobotState.Robotstate.eDirection=zRetreatR;
                    }

                      if(IsCrashToEdge)
                    {
                        IsCrashToEdge=false;
                        constAngle=CURRENT_ANGLE;  
                    }
                   Loc_TimCrash=GetOSRunTimeNow();  //获取当前时间
                    
                    
                }

                
                /*检测到跌落*/
                if(g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLBAbn||\
                   g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLFAbn||\
                   g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRBAbn||\
                   g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRFAbn)
                {
                    if(g_tRobotState.Robotstate.eDirection == zForward ||\
                       g_tRobotState.Robotstate.eDirection == zTurnLeft||\
                       g_tRobotState.Robotstate.eDirection == zAuto )
                    {
                        g_tRobotState.Robotstate.eDirection=zRetreatL;
                    }
                    else if(g_tRobotState.Robotstate.eDirection == zTurnRight)
                    {
                        g_tRobotState.Robotstate.eDirection=zRetreatR;
                    }
                }
            }

                
            if( TimeOut==SingleTimeoutCheck(Loc_TimCrash,0,0,650))
            {
                 if(!IsCrashToEdge)
                 {
                     g_tRobotState.Robotstate.eDirection=zAlongEdge;
                     IsCrashToEdge=true;
                     EdgeChrashCnt(constAngle,1); 
                     
                 }
              
            }
  
        }
    }
}


//int Edge_Chrash_CNT(s16 FirstAng)
//{
//    
//    static bool IsChrashFirstIn=true;//是否第一次进入
//    static OS_TimeStruct Loc_TimALongEdge;
//    static s16  constAngle,CurrentAng,LastCurrentAng;
//    static uint16_t AlongEdgeAD,lastAlongEdgeAD; //沿边AD值
//    static s16 TurnAngle;//转弯后的角度
//    static uint8_t ucTurnCount;
//    if(IsChrashFirstIn == true)
//    {
//        
//        lastAlongEdgeAD=g_usAnalogSwitch[adcAlong_Edge];
//        constAngle=FirstAng;
//        ucTurnCount=0;
//        IsChrashFirstIn=false;
//    }
//    
//    
//    g_tRobotState.Robotstate.eDirection=zAlongEdge;
//    g_sExpect_speed=TURN_CORNER_SPEED;//赋值沿边速度
//    AlongEdgeAD=g_usAnalogSwitch[adcAlong_Edge];
//    CurrentAng=CURRENT_ANGLE; //现在角度
//    
////     if(ucTurnCount>100)
////    {
////        IsChrashFirstIn =true;
////        g_tRobotState.Robotstate.eDirection=zAuto;
////        return 0;
////    }
////    if(abs(constAngle-CurrentAng)<100)
////    {
////        IsFirstIn =true;
////        g_tRobotState.Robotstate.eDirection=zAuto;
////        return 1;
////    }
////    
//    
//    
//    if(AlongEdgeAD>800)
//    {
//         g_sExpect_speed=ALONGEDGE_SPEED;//赋值沿边速度
//         if(AlongEdgeAD>1000)
//         {
//              g_sExpect_Angle=CURRENT_ANGLE+300;
//                /*绕左旋转*/
//                if(0 == ucTurn_Angle_Round(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,10))
//                {
//                /*已经到达指定角度*/
//                DEBUG("TurnRight %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
//                } 
//         }
//        
//         if(abs(AlongEdgeAD-lastAlongEdgeAD)>300)
//        {
//          
//            if(AlongEdgeAD>lastAlongEdgeAD)
//            {
//                printf("左转\r\n");
//                g_sExpect_Angle=CURRENT_ANGLE+100;
//                /*绕左旋转*/
//                if(0 == ucRoute_Adjust(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed))
//                {
//                /*已经到达指定角度*/
//                DEBUG("TurnRight %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
//                } 
//            }
//            else
//            {
//                 printf("右转\r\n");
//                g_sExpect_Angle=CURRENT_ANGLE-100;
//                /*绕右轮旋转*/
//                if(0 == ucRoute_Adjust(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed))
//                {
//                /*已经到达指定角度*/
//                DEBUG("TurnRight %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
//                }  
//            }
//            
//        }
//        else 
//        {
//            //直行
//            g_sExpect_speed=ALONGEDGE_SPEED;//赋值沿边速度
//            ucRoute_Adjust(CURRENT_ANGLE,CURRENT_ANGLE,g_sExpect_speed);
//        }
//    }
//    else
//    {
//       if(abs(AlongEdgeAD-lastAlongEdgeAD)>400&&AlongEdgeAD<1500)
//        {
//            ucTurnCount=200;
//            Loc_TimALongEdge=GetOSRunTimeNow();  //获取当前时间
//            while(ucTurnCount--)
//            {
//                if( NotTimeOut == SingleTimeoutCheck(Loc_TimALongEdge,0,0,FRONT_OFF_TIME))
//                {
//                    g_sExpect_speed=FORWARD_SPEED;//赋值前进速度
//                    ucRoute_Adjust(CURRENT_ANGLE,CURRENT_ANGLE,g_sExpect_speed);//角度注意
//                }else
//                { 
//                    TurnAngle=CURRENT_ANGLE-450;
//                    /*前进时间到了 要转弯了*/
//                    g_sExpect_Angle=TurnAngle;//改变期望角度为 转向后角度
//                    g_sExpect_speed=TURN_CORNER_SPEED;//速度为转弯速度
//                    /*自转*/
//                    if(0 == RotateAroundAxle(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range,0))
//                    {
//                    }  
//                }
//            }
//        }
//        
//        printf("小1500\r\n");
//        ucTurnCount++;
//        g_sExpect_speed=ALONGEDGE_SPEED;//赋值沿边速度
//        g_sExpect_Angle=CURRENT_ANGLE+100;
//        /*绕左旋转*/
//        if(0 == ucTurn_Angle_Round(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,30))
//        {
//        /*已经到达指定角度*/
//            DEBUG("TurnRight %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
//        }  

//            
//    }
//    

//    
//    lastAlongEdgeAD=AlongEdgeAD;
//    return 1;
//  
//}

/*******************************************************************************
* Function Name  : int EdgeChrashCnt(s16 FirstAng, bool IsCrash)
* Description    : 沿边
* Input          : FirstAng 基准角度  IsCrash 1为碰撞后进入 
* Output         : None 
* Return         : 0 函数继续执行 1 退出
*******************************************************************************/
int EdgeChrashCnt(s16 FirstAng, bool IsCrash)
{
    static bool IsFirstIn=true;//是否第一次进入
    static OS_TimeStruct Loc_TimALongEdge; // 
    static OS_TimeStruct Loc_TimTurn;  //转弯计时
    static OS_TimeStruct Loc_TimCheck;  //采集计时
    static s16  constAngle,CurrentAng;
    static uint16_t AlongEdgeAD,lastAlongEdgeAD; //沿边AD值
    static uint16_t ucTurnCount=0,ucCrashCouont=0; //转弯计数（秒），碰撞计数
    static uint8_t step=1;
    
    vTaskDelay(5);
    
    g_sExpect_speed=ALONGEDGE_SPEED;//赋值沿边速度
    //第一次进入
    if(IsFirstIn == true)
    {
        DEBUG("第一次 %d \r\n",ucTurnCount);
        //左转直到离墙5cm
        ucTurnCount=1;
        step=6;
        Loc_TimTurn=GetOSRunTimeNow();  //获取当前时间 
        Loc_TimCheck=GetOSRunTimeNow();  //获取当前时间 
        lastAlongEdgeAD=g_usAnalogSwitch[adcAlong_Edge];
        AlongEdgeAD=g_usAnalogSwitch[adcAlong_Edge];
        constAngle=FirstAng; 
        IsFirstIn=false;
    }
    //15ms采集一次数据
    if( TimeOut == MultipleTimeoutCheck(&Loc_TimCheck,0,0,15))
    {
         AlongEdgeAD=g_usAnalogSwitch[adcAlong_Edge];
        CurrentAng=CURRENT_ANGLE;
    }

     //碰撞检测 防墙角卡死
     if(IsCrash&&g_usAnalogSwitch[adcAlong_Edge]>750)//7cm
    {
        ucCrashCouont++;
        Loc_TimALongEdge=GetOSRunTimeNow();  //获取当前时间
        //离墙太近
        if(g_usAnalogSwitch[adcAlong_Edge]>1800)
        {
            g_tLeftWheel.ExpectSpeed=-180;
            g_tRightWheel.ExpectSpeed=-80;
            while( NotTimeOut == SingleTimeoutCheck(Loc_TimALongEdge,0,0,FRONT_OFF_TIME))
            {
                vTaskDelay(5);
                
            }       
        }
        //左转300度后检查沿边值
        g_sExpect_Angle=CURRENT_ANGLE+300;
        while((AlongEdgeAD>lastAlongEdgeAD+50)&&IsCrash&&g_usAnalogSwitch[adcAlong_Edge]>750)
        {
              vTaskDelay(15);
             if((ucTurn_Angle_Round(CURRENT_ANGLE,g_sExpect_Angle,ALONGEDGE_SPEED,10))==0)
            {
                DEBUG("TurnRight %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
               break;
            }
        }
        step=6;
        ucTurnCount=1;
        
    }
    
    if(0==ucTurnCount)
    {
        ucCrashCouont=0;
    }
    if(ucCrashCouont>3)
    {
        ucCrashCouont=0;
        IsFirstIn=true;
    }
  
//    if(abs(constAngle-CurrentAng)<100)
//    {
//        step=9;
//    }
    
    /* 离墙距离对应AD值 unit ：cm
    
    0   2450
    1   1900
    2   1550
    3   1320
    4   1150
    5   1000
    6   900
    7   830
    */
    
    //直行沿边中 离墙2到3cm
    if(0==ucTurnCount)
    {
      //沿边直行
        if(AlongEdgeAD>1500) //2cm
        {
            //直行左转
           step=2;
        }
        else if(AlongEdgeAD>1300)
        {
            //直行
            step=5;
        }
        else if(AlongEdgeAD>1200&&AlongEdgeAD<1300)
        {
            //直行右转
            step=1;
        }
       
        //沿边转弯
         if(AlongEdgeAD<1200&&AlongEdgeAD>1000)
        {
            step=1;
            if(AlongEdgeAD>lastAlongEdgeAD+100)
            {
                step=8;
            }
            if(AlongEdgeAD<lastAlongEdgeAD-100)
            {
               step=7;
 
            } 
        }
        else if(AlongEdgeAD<1000)
        {

             if(AlongEdgeAD+200<lastAlongEdgeAD)
            {
                //前进然后右转
                step=4;
                ucTurnCount=1;
                Loc_TimALongEdge=GetOSRunTimeNow();  //获取当前时间
                Loc_TimTurn=GetOSRunTimeNow();  //获取当前时间 
            }
            else
            {
                //右转
                step=3;
                ucTurnCount=1;
                Loc_TimTurn=GetOSRunTimeNow();  //获取当前时间 
            }

        }

//          //沿边直行
//            if(AlongEdgeAD>1200)
//            {
//                //直行左转
//                ucRouteAdjustByAlongEdge(CurrentAng,1500,ALONGEDGE_SPEED);
//            }
//            
//            

//            if(AlongEdgeAD<1000)
//            {

//                 if(AlongEdgeAD+300<lastAlongEdgeAD)
//                {
//                    step=4;
//                    ucTurnCount=1;
//                    Loc_TimALongEdge=GetOSRunTimeNow();  //获取当前时间
//                    Loc_TimTurn=GetOSRunTimeNow();  //获取当前时间 
//                }
//                else
//                {
//                    step=3;
//                    ucTurnCount=1;
//                    Loc_TimTurn=GetOSRunTimeNow();  //获取当前时间 
//                }
//  
//            }

    }
    
        
    //转弯计时 unit:s
    if(ucTurnCount>8)
    {
        step=9;
    }  
    
    //沿边运动
    switch(step)
    {
        case 1:  //直行右转
        {

            DEBUG("直行右转\r\n");
            g_sExpect_Angle=CURRENT_ANGLE-100;
            /*绕右轮旋转*/
            if(0 == ucRoute_Adjust(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed))
            {
            /*已经到达指定角度*/
            DEBUG("TurnRight and Along %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
            }  
        }
        break;
        
        case 2://直行左转
        {
            DEBUG("直行左转\r\n");
            g_sExpect_Angle=CURRENT_ANGLE+100;
            /*绕左旋转*/
            if(0 == ucRoute_Adjust(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed))
            {
            /*已经到达指定角度*/
            DEBUG("TurnRight and Along%d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
            } 
            
        }
        break;
        
        case 3:
        {
            //原地右转
//            DEBUG("原地右转\r\n");
            if( TimeOut == MultipleTimeoutCheck(&Loc_TimTurn,0,1,0))
            {
                 ucTurnCount++;
            }
            g_sExpect_Angle=CURRENT_ANGLE-100;
            if((ucTurn_Angle_Round(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,10))==0)
            {
             DEBUG("TurnRight %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
            }
        }
        break;
        
        case 4:     //大幅度右转，前进然后右转
        {
//             DEBUG("大幅右转\r\n");
            if( TimeOut == MultipleTimeoutCheck(&Loc_TimTurn,0,1,0))
            {
                 ucTurnCount++;
            }
            if(ucCrashCouont>1||ucTurnCount>3)
            {
                step=8;
                ucCrashCouont=1;
                ucTurnCount=1;
            }
            
            if( NotTimeOut == SingleTimeoutCheck(Loc_TimALongEdge,0,0,FRONT_OFF_TIME))
            {
                DEBUG("大转直行\r\n");
                ucRoute_Adjust(CURRENT_ANGLE,CURRENT_ANGLE,g_sExpect_speed);//角度注意
            }else
            { 
                /*前进时间到了 要转弯了*/
                g_sExpect_Angle=CURRENT_ANGLE-450;;//改变期望角度为 转向后角度
                g_sExpect_speed=ALONGEDGE_SPEED;//速度为转弯速度
                /*自转*/
                DEBUG("自转\r\n");
                if(0 == RotateAroundAxle(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range,LWheel))
                {   
                } 
            }
        }
        break;
        
        case 5: //直行
        {
            ucRoute_Adjust(CURRENT_ANGLE,CURRENT_ANGLE,g_sExpect_speed);
        }
        break;
        
        case 6: 
        {   //原地左转
             DEBUG("原地左转\r\n");
            g_sExpect_Angle=CURRENT_ANGLE+100;
            if( TimeOut == MultipleTimeoutCheck(&Loc_TimTurn,0,1,0))
            {
                 ucTurnCount++;
            }         
            if((ucTurn_Angle_Round(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,10))==0)
            {
             DEBUG("TurnRight %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);               
            }
        }
        break;
        
        case 7:
        {
            //右转
//             DEBUG("单轮原地右转\r\n");
             g_sExpect_Angle=CURRENT_ANGLE-100;
            if(0 == RotateAroundAxle(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range,LWheel))
            {
                /*已经到达指定角度*/
                DEBUG("TurnRight %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE); 
            } 
        }
        break;
        case 8:
        {
            //左转
//              DEBUG("单轮原地左转\r\n");
            g_sExpect_Angle=CURRENT_ANGLE+100;
            if(0 == RotateAroundAxle(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed,g_sExpect_range,RWheel))
            {
                /*已经到达指定角度*/
                DEBUG("TurnLeft %d %d \r\n",g_sExpect_Angle,CURRENT_ANGLE);
            } 
        }
        break;
        case 9:     //退出
        {
            ucTurnCount=0;
            IsFirstIn=true;
            g_tRobotState.Robotstate.eDirection=zAuto;
            return 1;
        }
    }
    
    //转弯超时重置
    if(g_usAnalogSwitch[adcAlong_Edge]>1000)
    { 
        ucTurnCount=0;
    }     
    lastAlongEdgeAD=AlongEdgeAD;
    return 0;
  
}


void DropCheckUpdate(void)
{
    if(tDropcheck.bAnyoneFall)
    {
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLBAbn=tDropcheck.bBackLFall;
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLFAbn=tDropcheck.bFrontLFall;
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRBAbn=tDropcheck.bBackRFall;
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRFAbn=tDropcheck.bFrontRFall;
    }else
    {
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLBAbn=0;
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropLFAbn=0;
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRBAbn=0;
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DropRFAbn=0;
    }
}
void UltraSonicCheckUpdate(void)
{
    
    
    g_tRobotState.Robotstate.unionAbnormal.AbnIden.UltraSonicAbn=bGetClashFlag();
    if(g_tRobotState.Robotstate.unionAbnormal.AbnIden.UltraSonicAbn)
        ForwardSpeed=FORWARD_SPEED/2;
    else
        ForwardSpeed=FORWARD_SPEED;
}


#endif

#ifdef WenTao

#endif


