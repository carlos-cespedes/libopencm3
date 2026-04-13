/*
 * Copyright (c) 2026 Carlos Cespedes Moya
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* ============================================================================
 * system_stm32f4xx.c - Clean-room System Clock Initialization
 * Basado EXCLUSIVAMENTE en RM0090 (STM32F4) y RM0385 (STM32F7)
 * Compatible con los drivers USART y SPI generados anteriormente
 * ============================================================================ */

#include <stdint.h>

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

/* Frecuencia del sistema (ajusta según tu MCU) */
uint32_t SystemCoreClock = 168000000UL;   /* 168 MHz por defecto (STM32F4) */

/* ===================================================================
 * SystemInit - Configuración completa del reloj
 * =================================================================== */
void SystemInit(void)
{
    /* 1. Deshabilitar interrupciones durante la configuración */
    __asm volatile("cpsid i");

    /* 2. Reset del reloj (limpieza) */
    RCC_CR |= (1U << 16);        /* HSEON = 1 */
    while ((RCC_CR & (1U << 17)) == 0); /* Esperar HSERDY */

    /* 3. Configurar PLL: fuente HSE, M=8, N=336, P=2 → 168 MHz */
    RCC_PLLCFGR = (1U << 22) |       /* PLLSRC = HSE */
                  (8U << 0) |        /* PLLM = 8 */
                  (336U << 6) |      /* PLLN = 336 */
                  (0U << 16);        /* PLLP = 2 (00) */

    /* 4. Habilitar PLL */
    RCC_CR |= (1U << 24);        /* PLLON = 1 */
    while ((RCC_CR & (1U << 25)) == 0); /* Esperar PLLRDY */

    /* 5. Configurar prescalers */
    RCC_CFGR = (0U << 10) |      /* PPRE1 = 4 (APB1 = 42 MHz) */
               (0U << 13) |      /* PPRE2 = 2 (APB2 = 84 MHz) */
               (0U << 4);        /* HPRE  = 1 (AHB  = 168 MHz) */

    /* 6. Seleccionar PLL como fuente de sistema */
    RCC_CFGR |= (2U << 0);       /* SW = PLL */
    while ((RCC_CFGR & (3U << 2)) != (2U << 2)); /* Esperar SWS == PLL */

    /* 7. Configurar Flash latency (5 wait states @ 168 MHz) */
    FLASH_ACR = (5U << 0) |      /* LATENCY = 5 */
                (1U << 8) |      /* PRFTEN */
                (1U << 9);       /* ICEN + DCEN */

    /* 8. Habilitar caches y prefetch */
    FLASH_ACR |= (1U << 10);     /* DCEN */

    /* 9. Habilitar FPU (Cortex-M4/M7) */
    SCB->CPACR |= (0xF << 20);   /* CP10 y CP11 full access */

    /* 10. Actualizar variable SystemCoreClock */
    SystemCoreClock = 168000000UL;

    /* Restaurar interrupciones */
    __asm volatile("cpsie i");
}

/* ===================================================================
 * SystemCoreClockUpdate - Para uso en runtime (ej. FreeRTOS)
 * =================================================================== */
void SystemCoreClockUpdate(void)
{
    /* En este ejemplo mantenemos 168 MHz fijo.
       Si cambias el reloj dinámicamente, implementa aquí el cálculo. */
    SystemCoreClock = 168000000UL;
}