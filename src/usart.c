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
 * Driver USART/UART completo clean-room para STM32F4/F7
 * Basado EXCLUSIVAMENTE en RM0090 (STM32F4) y RM0385 (STM32F7)
 * 100% original - sin inspiración en CubeHAL, libopencm3 ni ningún otro código
 * Licencia Apache 2.0 - alternativa permisiva a libopencm3
 * ============================================================================ */

#include "usart.h"

/* Offsets de registros (RM0090 Sección 30.6 / RM0385 Sección 37.7) */
#define USART_SR_OFFSET    0x00
#define USART_DR_OFFSET    0x04
#define USART_BRR_OFFSET   0x08
#define USART_CR1_OFFSET   0x0C
#define USART_CR2_OFFSET   0x10
#define USART_CR3_OFFSET   0x14

/* Macros de acceso directo a registros (eficiente y portable) */
#define USART_REG(base, offset)     (*(volatile uint32_t*)((base) + (offset)))

/* ===================================================================
 * Cálculo interno de PCLK según base (APB1 o APB2)
 * =================================================================== */
static uint32_t usart_get_pclk(uint32_t usart_base)
{
    if (usart_base == USART1_BASE || usart_base == USART6_BASE) {
        return PCLK2_FREQ;
    }
    return PCLK1_FREQ;
}

/* ===================================================================
 * Configuración DMA interna (basada en tabla de requests RM0090 Sección 10.3.3)
 * Solo se usa cuando se llama a las funciones DMA
 * =================================================================== */
typedef struct {
    uint32_t dma_base;
    uint8_t  tx_stream;
    uint8_t  rx_stream;
    uint8_t  tx_channel;
    uint8_t  rx_channel;
} usart_dma_map_t;

static usart_dma_map_t usart_get_dma_map(uint32_t usart_base)
{
    usart_dma_map_t map = {0};

    if (usart_base == USART1_BASE) {
        map.dma_base = 0x40026400UL; /* DMA2 */
        map.tx_stream = 7; map.rx_stream = 5;
        map.tx_channel = 4; map.rx_channel = 4;
    } else if (usart_base == USART2_BASE) {
        map.dma_base = 0x40026000UL; /* DMA1 */
        map.tx_stream = 6; map.rx_stream = 5;
        map.tx_channel = 4; map.rx_channel = 4;
    } else if (usart_base == USART3_BASE) {
        map.dma_base = 0x40026000UL;
        map.tx_stream = 3; map.rx_stream = 1;
        map.tx_channel = 4; map.rx_channel = 4;
    } else if (usart_base == UART4_BASE) {
        map.dma_base = 0x40026000UL;
        map.tx_stream = 4; map.rx_stream = 2;
        map.tx_channel = 4; map.rx_channel = 4;
    } else if (usart_base == UART5_BASE) {
        map.dma_base = 0x40026000UL;
        map.tx_stream = 7; map.rx_stream = 0;
        map.tx_channel = 4; map.rx_channel = 4;
    } else if (usart_base == USART6_BASE) {
        map.dma_base = 0x40026400UL; /* DMA2 */
        map.tx_stream = 6; map.rx_stream = 2;
        map.tx_channel = 5; map.rx_channel = 5;
    }
    return map;
}

/* Offsets internos DMA (RM0090 Sección 10.5) */
#define DMA_SxCR_OFFSET   0x10
#define DMA_SxNDTR_OFFSET 0x14
#define DMA_SxPAR_OFFSET  0x18
#define DMA_SxM0AR_OFFSET 0x1C

/* ===================================================================
 * usart_init - Inicialización completa y limpia
 * =================================================================== */
void usart_init(uint32_t usart_base, uint32_t baudrate, uint8_t databits, char parity, uint8_t stopbits, bool flow_control)
{
    /* Paso 1: Deshabilitar USART (obligatorio antes de configurar - RM0090 30.4.1) */
    USART_REG(usart_base, USART_CR1_OFFSET) &= ~(1U << 13); /* UE = 0 */

    /* Paso 2: Configurar CR1 (word length, parity, oversampling 16x por defecto) */
    uint32_t cr1 = 0;
    cr1 |= (1U << 3);  /* TE */
    cr1 |= (1U << 2);  /* RE */

    /* Word length: 8 o 9 bits */
    if (databits == 9) {
        cr1 |= (1U << 12); /* M = 1 */
    }

    /* Parity */
    if (parity == 'E' || parity == 'e') {
        cr1 |= (1U << 10); /* PCE = 1 */
        cr1 &= ~(1U << 9); /* PS = 0 (even) */
    } else if (parity == 'O' || parity == 'o') {
        cr1 |= (1U << 10); /* PCE = 1 */
        cr1 |= (1U << 9);  /* PS = 1 (odd) */
    }
    /* 'N' o cualquier otro = parity off */

    /* Oversampling por defecto 16x (OVER8 = 0) - se puede cambiar después si se necesita 8x */
    cr1 &= ~(1U << 15);

    USART_REG(usart_base, USART_CR1_OFFSET) = cr1;

    /* Paso 3: CR2 - Stop bits */
    uint32_t cr2 = USART_REG(usart_base, USART_CR2_OFFSET) & ~(3U << 12);
    if (stopbits == 2) {
        cr2 |= (2U << 12); /* STOP = 10 */
    } /* 1 stop = 00 por defecto */
    USART_REG(usart_base, USART_CR2_OFFSET) = cr2;

    /* Paso 4: CR3 - Flow control */
    uint32_t cr3 = USART_REG(usart_base, USART_CR3_OFFSET) & ~( (1U << 8) | (1U << 7) );
    if (flow_control) {
        cr3 |= (1U << 8); /* CTSE */
        cr3 |= (1U << 7); /* RTSE */
    }
    USART_REG(usart_base, USART_CR3_OFFSET) = cr3;

    /* Paso 5: Baudrate */
    usart_set_baudrate(usart_base, baudrate);

    /* Paso 6: Habilitar USART */
    USART_REG(usart_base, USART_CR1_OFFSET) |= (1U << 13); /* UE = 1 */

    /* Limpieza inicial de flags de error */
    (void)USART_REG(usart_base, USART_SR_OFFSET);
    (void)USART_REG(usart_base, USART_DR_OFFSET);
}

/* ===================================================================
 * usart_set_baudrate - Cálculo exacto según fórmula del datasheet
 * =================================================================== */
void usart_set_baudrate(uint32_t usart_base, uint32_t baudrate)
{
    if (baudrate == 0) return;

    uint32_t pclk = usart_get_pclk(usart_base);

    /* Oversampling 16x (por defecto) - RM0090 30.3.4 */
    uint32_t divisor = 16UL * baudrate;
    uint32_t mantissa = pclk / divisor;
    uint32_t remainder = pclk % divisor;
    uint32_t fraction = (remainder * 16UL) / divisor;

    uint32_t brr = (mantissa << 4) | fraction;

    USART_REG(usart_base, USART_BRR_OFFSET) = brr;
}

/* ===================================================================
 * Funciones blocking (polling) - seguras y eficientes
 * =================================================================== */
void usart_send_blocking(uint32_t usart_base, uint8_t data)
{
    while ((usart_get_flag(usart_base, USART_FLAG_TXE) == 0)) {
        /* polling TXE */
    }
    USART_REG(usart_base, USART_DR_OFFSET) = data;
}

uint8_t usart_recv_blocking(uint32_t usart_base)
{
    /* Esperar dato o error */
    while ((usart_get_flag(usart_base, USART_FLAG_RXNE) == 0)) {
        /* polling RXNE */
    }

    /* Manejo automático de errores (RM0090 30.4.2) */
    uint32_t sr = USART_REG(usart_base, USART_SR_OFFSET);
    if (sr & USART_ERROR_MASK) {
        /* Lectura dummy para limpiar flags ORE/FE/NF/PE */
        (void)USART_REG(usart_base, USART_DR_OFFSET);
        /* Se puede extender aquí con callback de error si se desea */
        return 0x00; /* dato inválido en caso de error */
    }

    return (uint8_t)USART_REG(usart_base, USART_DR_OFFSET);
}

void usart_send_string(uint32_t usart_base, const char* str)
{
    if (str == NULL) return;
    while (*str) {
        usart_send_blocking(usart_base, (uint8_t)*str++);
    }
}

/* ===================================================================
 * Interrupciones (solo periférico - usuario debe habilitar NVIC)
 * =================================================================== */
void usart_enable_interrupt(uint32_t usart_base)
{
    /* Habilitamos interrupciones típicas para RX/TX y errores */
    uint32_t cr1 = USART_REG(usart_base, USART_CR1_OFFSET);
    cr1 |= (1U << 5);  /* RXNEIE */
    cr1 |= (1U << 7);  /* TXEIE */
    cr1 |= (1U << 6);  /* TCIE */
    cr1 |= (1U << 9);  /* PEIE */
    USART_REG(usart_base, USART_CR1_OFFSET) = cr1;

    /* Error interrupt global en CR3 */
    uint32_t cr3 = USART_REG(usart_base, USART_CR3_OFFSET);
    cr3 |= (1U << 0);  /* EIE */
    USART_REG(usart_base, USART_CR3_OFFSET) = cr3;
}

void usart_disable_interrupt(uint32_t usart_base)
{
    uint32_t cr1 = USART_REG(usart_base, USART_CR1_OFFSET);
    cr1 &= ~((1U << 5) | (1U << 7) | (1U << 6) | (1U << 9));
    USART_REG(usart_base, USART_CR1_OFFSET) = cr1;

    uint32_t cr3 = USART_REG(usart_base, USART_CR3_OFFSET);
    cr3 &= ~(1U << 0);
    USART_REG(usart_base, USART_CR3_OFFSET) = cr3;
}

/* ===================================================================
 * Soporte DMA TX/RX (configuración ligera del lado USART + DMA)
 * El usuario debe configurar previamente el stream DMA (canal, dirección, prioridad)
 * Esta función solo actualiza buffer/longitud e inicia la transferencia
 * =================================================================== */
void usart_dma_tx_start(uint32_t usart_base, const void* buffer, uint32_t length)
{
    if (buffer == NULL || length == 0) return;

    usart_dma_map_t map = usart_get_dma_map(usart_base);
    if (map.dma_base == 0) return; /* USART sin DMA */

    uint32_t stream_offset = (uint32_t)map.tx_stream * 0x18UL;

    /* Configuración rápida del stream (PAR, M0AR, NDTR) */
    USART_REG(map.dma_base, DMA_SxPAR_OFFSET + stream_offset) = usart_base + USART_DR_OFFSET;
    USART_REG(map.dma_base, DMA_SxM0AR_OFFSET + stream_offset) = (uint32_t)buffer;
    USART_REG(map.dma_base, DMA_SxNDTR_OFFSET + stream_offset) = length;

    /* Habilitar stream DMA */
    USART_REG(map.dma_base, DMA_SxCR_OFFSET + stream_offset) |= (1U << 0); /* EN */

    /* Habilitar request DMA en USART */
    uint32_t cr3 = USART_REG(usart_base, USART_CR3_OFFSET);
    cr3 |= (1U << 6); /* DMAT */
    USART_REG(usart_base, USART_CR3_OFFSET) = cr3;
}

void usart_dma_rx_start(uint32_t usart_base, void* buffer, uint32_t length)
{
    if (buffer == NULL || length == 0) return;

    usart_dma_map_t map = usart_get_dma_map(usart_base);
    if (map.dma_base == 0) return;

    uint32_t stream_offset = (uint32_t)map.rx_stream * 0x18UL;

    USART_REG(map.dma_base, DMA_SxPAR_OFFSET + stream_offset) = usart_base + USART_DR_OFFSET;
    USART_REG(map.dma_base, DMA_SxM0AR_OFFSET + stream_offset) = (uint32_t)buffer;
    USART_REG(map.dma_base, DMA_SxNDTR_OFFSET + stream_offset) = length;

    USART_REG(map.dma_base, DMA_SxCR_OFFSET + stream_offset) |= (1U << 0); /* EN */

    uint32_t cr3 = USART_REG(usart_base, USART_CR3_OFFSET);
    cr3 |= (1U << 5); /* DMAR */
    USART_REG(usart_base, USART_CR3_OFFSET) = cr3;
}

/* ===================================================================
 * Lectura de flags (útil para polling y manejo de errores)
 * =================================================================== */
uint32_t usart_get_flag(uint32_t usart_base, uint32_t flag)
{
    return (USART_REG(usart_base, USART_SR_OFFSET) & flag);
}

/* ===================================================================
 * Redirección débil de printf / _write (newlib)
 * Por defecto usa USART1 - reasignable cambiando usart_stdio_base
 * =================================================================== */
static uint32_t usart_stdio_base = USART1_BASE;

int _write(int file, char *ptr, int len) __attribute__((weak));
int _write(int file, char *ptr, int len)
{
    (void)file;
    if (ptr == NULL || len <= 0) return 0;

    for (int i = 0; i < len; i++) {
        usart_send_blocking(usart_stdio_base, (uint8_t)ptr[i]);
    }
    return len;
}

/* ===================================================================
 * Ejemplo de handler ISR (llamar desde tu ISR real)
 * Incluye soporte básico FreeRTOS vía colas (opcional)
 * =================================================================== */
void usart_isr(uint32_t usart_base)
{
    uint32_t sr = USART_REG(usart_base, USART_SR_OFFSET);

    /* RXNE - dato recibido */
    if (sr & USART_FLAG_RXNE) {
        uint8_t data = (uint8_t)USART_REG(usart_base, USART_DR_OFFSET);
        /* Aquí puedes push a cola FreeRTOS si defines CONFIG_USE_FREERTOS */
        /* Ejemplo: xQueueSendFromISR(rx_queue, &data, &higherPriority); */
    }

    /* TXE/TC - transmisión lista (opcional para half-duplex) */
    if (sr & (USART_FLAG_TXE | USART_FLAG_TC)) {
        /* Puedes pop de cola TX si usas FreeRTOS */
    }

    /* Errores - limpieza automática */
    if (sr & USART_ERROR_MASK) {
        (void)USART_REG(usart_base, USART_DR_OFFSET);
    }
}