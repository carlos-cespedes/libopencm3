/*
 * Copyright (c) 2026 Carlos Cespedes Moya
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef SYSTEM_STM32F4XX_H
#define SYSTEM_STM32F4XX_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Registro base RCC (RM0090 Sección 7.3) */
#define RCC_BASE            0x40023800UL
#define RCC_CR              (*(volatile uint32_t*)(RCC_BASE + 0x00))
#define RCC_PLLCFGR         (*(volatile uint32_t*)(RCC_BASE + 0x04))
#define RCC_CFGR            (*(volatile uint32_t*)(RCC_BASE + 0x08))
#define RCC_CIR             (*(volatile uint32_t*)(RCC_BASE + 0x0C))
#define RCC_AHB1ENR         (*(volatile uint32_t*)(RCC_BASE + 0x30))
#define RCC_APB1ENR         (*(volatile uint32_t*)(RCC_BASE + 0x40))
#define RCC_APB2ENR         (*(volatile uint32_t*)(RCC_BASE + 0x44))

/* Flash (RM0090 Sección 3.7) */
#define FLASH_BASE          0x40023C00UL
#define FLASH_ACR           (*(volatile uint32_t*)(FLASH_BASE + 0x00))

/* ============================================================================
 * System Control Block (Cortex-M4) - Definición clean-room
 * periférico del núcleo Cortex-M4, header externo de CMSIS datasheet
 * ============================================================================ */
#define SCB_BASE        0xE000ED00UL
#define SCB_CPACR       (*(volatile uint32_t*)(SCB_BASE + 0x88UL))   /* Offset del CPACR */

/* Variable global estándar (CMSIS style) */
extern uint32_t SystemCoreClock;

/* Funciones públicas */
void SystemInit(void);
void SystemCoreClockUpdate(void);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_STM32F4XX_H */