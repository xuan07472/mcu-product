#ifndef __GARBAGE_BOXES_CHECK_h
#define __GARBAGE_BOXES_CHECK_h
#include "sys.h"
#include "delay.h"
#include "main.h"
bool isGarbageBoxClose(void);

void vGarbage_box_check_task(void *pvParameters);
void vGarbage_box_check_Init(void);
void vGarbageBoxChecking(void);

#endif

