/*
 * startup_stm32f407xx.s
 * Copyright (c) 2026 Carlos Cespedes Moya
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Clean-room startup code for STM32F407xx (Cortex-M4)
 * Basado exclusivamente en el Programming Manual PM0214 y RM0090
 */

.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.global  g_pfnVectors
.global  Default_Handler
.global  Reset_Handler

/* ================================================
 * Tabla de Vectores de Interrupción
 * ================================================ */
.section .isr_vector
.align 2
g_pfnVectors:
    .word  _estack                /* 0x00 - Initial Stack Pointer */
    .word  Reset_Handler          /* 0x04 - Reset Handler */
    .word  NMI_Handler            /* 0x08 - NMI */
    .word  HardFault_Handler      /* 0x0C - Hard Fault */
    .word  MemManage_Handler      /* 0x10 - Memory Management */
    .word  BusFault_Handler       /* 0x14 - Bus Fault */
    .word  UsageFault_Handler     /* 0x18 - Usage Fault */
    .word  0                      /* 0x1C - Reserved */
    .word  0                      /* 0x20 - Reserved */
    .word  0                      /* 0x24 - Reserved */
    .word  0                      /* 0x28 - Reserved */
    .word  SVC_Handler            /* 0x2C - SVCall */
    .word  DebugMon_Handler       /* 0x30 - Debug Monitor */
    .word  0                      /* 0x34 - Reserved */
    .word  PendSV_Handler         /* 0x38 - PendSV */
    .word  SysTick_Handler        /* 0x3C - SysTick */

    /* Interrupciones externas (STM32F4) - hasta IRQ 81 */
    .word  WWDG_IRQHandler
    .word  PVD_IRQHandler
    .word  TAMP_STAMP_IRQHandler
    .word  RTC_WKUP_IRQHandler
    .word  FLASH_IRQHandler
    .word  RCC_IRQHandler
    .word  EXTI0_IRQHandler
    .word  EXTI1_IRQHandler
    .word  EXTI2_IRQHandler
    .word  EXTI3_IRQHandler
    .word  EXTI4_IRQHandler
    .word  DMA1_Stream0_IRQHandler
    .word  DMA1_Stream1_IRQHandler
    .word  DMA1_Stream2_IRQHandler
    .word  DMA1_Stream3_IRQHandler
    .word  DMA1_Stream4_IRQHandler
    .word  DMA1_Stream5_IRQHandler
    .word  DMA1_Stream6_IRQHandler
    .word  ADC_IRQHandler
    .word  CAN1_TX_IRQHandler
    .word  CAN1_RX0_IRQHandler
    .word  CAN1_RX1_IRQHandler
    .word  CAN1_SCE_IRQHandler
    .word  EXTI9_5_IRQHandler
    .word  TIM1_BRK_TIM9_IRQHandler
    .word  TIM1_UP_TIM10_IRQHandler
    .word  TIM1_TRG_COM_TIM11_IRQHandler
    .word  TIM1_CC_IRQHandler
    .word  TIM2_IRQHandler
    .word  TIM3_IRQHandler
    .word  TIM4_IRQHandler
    .word  I2C1_EV_IRQHandler
    .word  I2C1_ER_IRQHandler
    .word  I2C2_EV_IRQHandler
    .word  I2C2_ER_IRQHandler
    .word  SPI1_IRQHandler
    .word  SPI2_IRQHandler
    .word  USART1_IRQHandler
    .word  USART2_IRQHandler
    .word  USART3_IRQHandler
    .word  EXTI15_10_IRQHandler
    .word  RTC_Alarm_IRQHandler
    .word  OTG_FS_WKUP_IRQHandler
    .word  TIM8_BRK_TIM12_IRQHandler
    .word  TIM8_UP_TIM13_IRQHandler
    .word  TIM8_TRG_COM_TIM14_IRQHandler
    .word  TIM8_CC_IRQHandler
    .word  DMA1_Stream7_IRQHandler
    .word  FSMC_IRQHandler
    .word  SDIO_IRQHandler
    .word  TIM5_IRQHandler
    .word  SPI3_IRQHandler
    .word  UART4_IRQHandler
    .word  UART5_IRQHandler
    .word  TIM6_DAC_IRQHandler
    .word  TIM7_IRQHandler
    .word  DMA2_Stream0_IRQHandler
    .word  DMA2_Stream1_IRQHandler
    .word  DMA2_Stream2_IRQHandler
    .word  DMA2_Stream3_IRQHandler
    .word  DMA2_Stream4_IRQHandler
    .word  0
    .word  0
    .word  CAN2_TX_IRQHandler
    .word  CAN2_RX0_IRQHandler
    .word  CAN2_RX1_IRQHandler
    .word  CAN2_SCE_IRQHandler
    .word  OTG_FS_IRQHandler
    .word  DMA2_Stream5_IRQHandler
    .word  DMA2_Stream6_IRQHandler
    .word  DMA2_Stream7_IRQHandler
    .word  USART6_IRQHandler
    .word  I2C3_EV_IRQHandler
    .word  I2C3_ER_IRQHandler
    .word  OTG_HS_EP1_OUT_IRQHandler
    .word  OTG_HS_EP1_IN_IRQHandler
    .word  OTG_HS_WKUP_IRQHandler
    .word  OTG_HS_IRQHandler
    .word  0
    .word  0
    .word  0
    .word  0
    .word  0
    .word  0
    .word  0
    .word  0
    .word  0
    .word  0
    .word  0
    .word  TIM1_BRK_TIM9_IRQHandler   /* Repetido por compatibilidad */
    /* ... (puedes extender si necesitas más IRQs) */

.size g_pfnVectors, .-g_pfnVectors

/* ================================================
 * Reset Handler - Punto de entrada principal
 * ================================================ */
.section .text.Reset_Handler
.weak Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
    /* 1. Copiar sección .data desde Flash a RAM */
    ldr r0, =_sidata    /* Origen en Flash */
    ldr r1, =_sdata     /* Destino en RAM */
    ldr r2, =_edata     /* Fin de .data */
    1:  cmp r1, r2
        ittt lo
        ldrlo r3, [r0], #4
        strlo r3, [r1], #4
        blo 1b

    /* 2. Inicializar sección .bss a cero */
    ldr r1, =_sbss
    ldr r2, =_ebss
    movs r3, #0
    2:  cmp r1, r2
        itt lo
        strlo r3, [r1], #4
        blo 2b

    /* 3. Llamar a SystemInit() (configuración de reloj) */
    bl SystemInit

    /* 4. Saltar a main() */
    bl main

    /* Si main() retorna (no debería), entrar en bucle infinito */
Infinite_Loop:
    b Infinite_Loop
.size Reset_Handler, .-Reset_Handler

/* ================================================
 * Handlers por defecto (Weak - se pueden sobrescribir)
 * ================================================ */
.macro Default_Handler handler_name
.weak \handler_name
.type \handler_name, %function
\handler_name:
    b .
.size \handler_name, .-\handler_name
.endm

Default_Handler NMI_Handler
Default_Handler HardFault_Handler
Default_Handler MemManage_Handler
Default_Handler BusFault_Handler
Default_Handler UsageFault_Handler
Default_Handler SVC_Handler
Default_Handler DebugMon_Handler
Default_Handler PendSV_Handler
Default_Handler SysTick_Handler

/* Interrupciones externas por defecto */
Default_Handler WWDG_IRQHandler
Default_Handler PVD_IRQHandler
Default_Handler TAMP_STAMP_IRQHandler
Default_Handler RTC_WKUP_IRQHandler
Default_Handler FLASH_IRQHandler
Default_Handler RCC_IRQHandler
Default_Handler EXTI0_IRQHandler
Default_Handler EXTI1_IRQHandler
Default_Handler EXTI2_IRQHandler
Default_Handler EXTI3_IRQHandler
Default_Handler EXTI4_IRQHandler
Default_Handler DMA1_Stream0_IRQHandler
Default_Handler DMA1_Stream1_IRQHandler
Default_Handler DMA1_Stream2_IRQHandler
Default_Handler DMA1_Stream3_IRQHandler
Default_Handler DMA1_Stream4_IRQHandler
Default_Handler DMA1_Stream5_IRQHandler
Default_Handler DMA1_Stream6_IRQHandler
Default_Handler ADC_IRQHandler
Default_Handler CAN1_TX_IRQHandler
Default_Handler CAN1_RX0_IRQHandler
Default_Handler CAN1_RX1_IRQHandler
Default_Handler CAN1_SCE_IRQHandler
Default_Handler EXTI9_5_IRQHandler
Default_Handler TIM1_BRK_TIM9_IRQHandler
Default_Handler TIM1_UP_TIM10_IRQHandler
Default_Handler TIM1_TRG_COM_TIM11_IRQHandler
Default_Handler TIM1_CC_IRQHandler
Default_Handler TIM2_IRQHandler
Default_Handler TIM3_IRQHandler
Default_Handler TIM4_IRQHandler
Default_Handler I2C1_EV_IRQHandler
Default_Handler I2C1_ER_IRQHandler
Default_Handler I2C2_EV_IRQHandler
Default_Handler I2C2_ER_IRQHandler
Default_Handler SPI1_IRQHandler
Default_Handler SPI2_IRQHandler
Default_Handler USART1_IRQHandler
Default_Handler USART2_IRQHandler
Default_Handler USART3_IRQHandler
Default_Handler EXTI15_10_IRQHandler
Default_Handler RTC_Alarm_IRQHandler
Default_Handler OTG_FS_WKUP_IRQHandler
Default_Handler TIM8_BRK_TIM12_IRQHandler
Default_Handler TIM8_UP_TIM13_IRQHandler
Default_Handler TIM8_TRG_COM_TIM14_IRQHandler
Default_Handler TIM8_CC_IRQHandler
Default_Handler DMA1_Stream7_IRQHandler
Default_Handler FSMC_IRQHandler
Default_Handler SDIO_IRQHandler
Default_Handler TIM5_IRQHandler
Default_Handler SPI3_IRQHandler
Default_Handler UART4_IRQHandler
Default_Handler UART5_IRQHandler
Default_Handler TIM6_DAC_IRQHandler
Default_Handler TIM7_IRQHandler
Default_Handler DMA2_Stream0_IRQHandler
Default_Handler DMA2_Stream1_IRQHandler
Default_Handler DMA2_Stream2_IRQHandler
Default_Handler DMA2_Stream3_IRQHandler
Default_Handler DMA2_Stream4_IRQHandler
Default_Handler CAN2_TX_IRQHandler
Default_Handler CAN2_RX0_IRQHandler
Default_Handler CAN2_RX1_IRQHandler
Default_Handler CAN2_SCE_IRQHandler
Default_Handler OTG_FS_IRQHandler
Default_Handler DMA2_Stream5_IRQHandler
Default_Handler DMA2_Stream6_IRQHandler
Default_Handler DMA2_Stream7_IRQHandler
Default_Handler USART6_IRQHandler
Default_Handler I2C3_EV_IRQHandler
Default_Handler I2C3_ER_IRQHandler
Default_Handler OTG_HS_EP1_OUT_IRQHandler
Default_Handler OTG_HS_EP1_IN_IRQHandler
Default_Handler OTG_HS_WKUP_IRQHandler
Default_Handler OTG_HS_IRQHandler

.end