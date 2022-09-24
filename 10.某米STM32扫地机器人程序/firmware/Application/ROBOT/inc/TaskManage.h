#ifndef __TASKMANAGE_H
#define __TASKMANAGE_H

#include "SystemTime.h"
#include "task.h"
#include "applicationkey.h"
#include "applicationled.h"
#include "applicationPowerChip.h"
#include "applicationadc.h"
#include "bmi160.h"
#include "Garbage_boxes_check.h"
#include "applicationmotor.h"
#include "Application_Ultrasonic.h"
#include "ApplicationDropCheck.h"
#include "behaviormanage.h"
#include "framehandle.h"

#include "usart1.h"
#include "usart.h"

void vTaskCreateFunction(void);
void vSystemHardwareWorkTask(void *p);
    
#endif

