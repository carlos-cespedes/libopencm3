#include "FreeRTOS.h"
#include "task.h"

extern void xPortPendSVHandler(void);
extern void xPortSysTickHandler(void);
extern void vPortSVCHandler(void);

void vPortSetupTimerInterrupt(void)
{
    systick_init();
}