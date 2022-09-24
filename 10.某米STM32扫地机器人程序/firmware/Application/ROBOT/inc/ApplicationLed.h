#ifndef __APP_LED_H
#define __APP_LED_H


#include "led.h"
#include "SystemTime.h"
#include "applicationPowerChip.h"

void vLedStateDisplay(void);


void vRedLedBrightness(u8 Bright);
void vWhiteLedBrightness(u8 Bright);
void vYellowLedBrightness(u8 Bright);


#endif

