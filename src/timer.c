#include "timer.h"
#include "system_stm32f4xx.h"

volatile uint32_t systick_ms = 0;

void systick_init(void)
{
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000);   /* 1 ms tick */
}

void SysTick_Handler(void)
{
    systick_ms++;
}

void delay_ms(uint32_t ms)
{
    uint32_t start = systick_ms;
    while ((systick_ms - start) < ms);
}