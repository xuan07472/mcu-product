#ifndef __APP_KEY_H
#define __APP_KEY_H


#include "key.h"
#include "SystemTime.h"
#include "iooutput.h"

typedef enum
{
    ButtonUp=0,
    Button1sDown,
    Button2sDown,
    Button3sDown,
    Button4sDown,
    Button5sDown,
    Button6sDown,
    ButtonDoubleDown,
}eButtonState;

/*public*/
void vButtonCheckOperation(void);

/*private*/
eButtonState eKey1State(void);
eButtonState eKey2State(void);

#endif

