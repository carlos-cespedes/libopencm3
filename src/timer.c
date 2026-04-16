/*
 * Copyright (c) 2026 Carlos Cespedes Moya
 * Licensed under Apache 2.0
 */

#include "timer.h"
#include "system_stm32f4xx.h"

/* SysTick registers (Cortex-M4) */
#define SysTick_BASE        0xE000E010UL
#define SysTick_CTRL        (*(volatile uint32_t*)(SysTick_BASE + 0x00))
#define SysTick_LOAD        (*(volatile uint32_t*)(SysTick_BASE + 0x04))
#define SysTick_VAL         (*(volatile uint32_t*)(SysTick_BASE + 0x08))

volatile uint32_t systick_ms = 0;

uint32_t SysTick_Config(uint32_t ticks)
{
    if (ticks == 0) return 1U;

    SysTick_LOAD = ticks - 1U;
    SysTick_VAL  = 0U;
    SysTick_CTRL = (1U << 2) | (1U << 1) | (1U << 0);  /* CLKSOURCE, TICKINT, ENABLE */
    return 0U;
}

void systick_init(void)
{
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000U);
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