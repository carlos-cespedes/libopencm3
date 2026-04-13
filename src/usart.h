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

#ifndef USART_H
#define USART_H

#include <stdint.h>
#include <stdbool.h>

/* ===================================================================
 * Direcciones base oficiales (RM0090 / RM0385 - Sección 2.2 Memory map)
 * =================================================================== */
#define USART1_BASE     0x40011000UL
#define USART2_BASE     0x40004400UL
#define USART3_BASE     0x40004800UL
#define UART4_BASE      0x40004C00UL
#define UART5_BASE      0x40005000UL
#define USART6_BASE     0x40011400UL

/* ===================================================================
 * Flags de estado (RM0090 / RM0385 - USART_SR, offset 0x00)
 * =================================================================== */
#define USART_FLAG_TXE   (1U << 7)
#define USART_FLAG_TC    (1U << 6)
#define USART_FLAG_RXNE  (1U << 5)
#define USART_FLAG_IDLE  (1U << 4)
#define USART_FLAG_ORE   (1U << 3)
#define USART_FLAG_NF    (1U << 2)
#define USART_FLAG_FE    (1U << 1)
#define USART_FLAG_PE    (1U << 0)

/* Error mask completo para manejo seguro */
#define USART_ERROR_MASK (USART_FLAG_ORE | USART_FLAG_NF | USART_FLAG_FE | USART_FLAG_PE)

/* ===================================================================
 * Frecuencias de reloj APB (el usuario debe sobrescribir según su RCC)
 * Valores por defecto típicos STM32F4 168 MHz / F7 216 MHz
 * =================================================================== */
#ifndef PCLK1_FREQ
#define PCLK1_FREQ      42000000UL   /* APB1 (USART2/3/4/5) */
#endif
#ifndef PCLK2_FREQ
#define PCLK2_FREQ      84000000UL   /* APB2 (USART1/6) */
#endif

/* ===================================================================
 * API pública (exactamente la solicitada - no se modifican nombres ni firmas)
 * =================================================================== */
void usart_init(uint32_t usart_base, uint32_t baudrate, uint8_t databits, char parity, uint8_t stopbits, bool flow_control);
void usart_set_baudrate(uint32_t usart_base, uint32_t baudrate);
void usart_send_blocking(uint32_t usart_base, uint8_t data);
uint8_t usart_recv_blocking(uint32_t usart_base);
void usart_send_string(uint32_t usart_base, const char* str);
void usart_enable_interrupt(uint32_t usart_base);
void usart_disable_interrupt(uint32_t usart_base);
void usart_dma_tx_start(uint32_t usart_base, const void* buffer, uint32_t length);
void usart_dma_rx_start(uint32_t usart_base, void* buffer, uint32_t length);
uint32_t usart_get_flag(uint32_t usart_base, uint32_t flag);

#endif /* USART_H */