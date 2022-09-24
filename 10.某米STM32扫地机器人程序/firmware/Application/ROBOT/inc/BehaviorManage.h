#ifndef BEHAVIORMANAGE_H
#define BEHAVIORMANAGE_H

#include "SystemTime.h"
#include "task.h"
#include "main.h"
#include "bmi160.h"
#include "Garbage_boxes_check.h"
#include "applicationmotor.h"
#include "Application_Ultrasonic.h"
#include "ApplicationDropCheck.h"



void vBehaviorManageTask(void *p);
void vErrorAbnormalCheckTask(void *p);

void DropCheckUpdate(void);
void UltraSonicCheckUpdate(void);
void AllMotorErrorCheck(void);

#endif
