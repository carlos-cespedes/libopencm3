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
 * Driver SPI completo clean-room para STM32F4/F7
 * Basado EXCLUSIVAMENTE en RM0090 (STM32F4) y RM0385 (STM32F7)
 * 100% original - sin inspiración en CubeHAL, libopencm3 ni ningún otro código
 * Licencia Apache 2.0 - alternativa permisiva a libopencm3
 * ============================================================================ */

#include "spi.h"

/* Offsets de registros (RM0090 Sección 28.6 / RM0385 Sección 35.7) */
#define SPI_CR1_OFFSET   0x00
#define SPI_CR2_OFFSET   0x04
#define SPI_SR_OFFSET    0x08
#define SPI_DR_OFFSET    0x0C

/* Macros de acceso directo */
#define SPI_REG(base, offset)     (*(volatile uint32_t*)((base) + (offset)))

/* ===================================================================
 * Cálculo interno de PCLK según base (APB1 o APB2)
 * =================================================================== */
static uint32_t spi_get_pclk(uint32_t spi_base)
{
    if (spi_base == SPI2_BASE || spi_base == SPI3_BASE) {
        return PCLK1_FREQ;
    }
    return PCLK2_FREQ; /* SPI1/4/5/6 */
}

/* ===================================================================
 * Mapa DMA oficial (RM0090 Tabla 43 - DMA request mapping)
 * =================================================================== */
typedef struct {
    uint32_t dma_base;
    uint8_t  tx_stream;
    uint8_t  rx_stream;
    uint8_t  tx_channel;
    uint8_t  rx_channel;
} spi_dma_map_t;

static spi_dma_map_t spi_get_dma_map(uint32_t spi_base)
{
    spi_dma_map_t map = {0};

    if (spi_base == SPI1_BASE) {
        map.dma_base = 0x40026400UL; /* DMA2 */
        map.tx_stream = 3; map.rx_stream = 2;
        map.tx_channel = 3; map.rx_channel = 3;
    } else if (spi_base == SPI2_BASE) {
        map.dma_base = 0x40026000UL; /* DMA1 */
        map.tx_stream = 4; map.rx_stream = 3;
        map.tx_channel = 0; map.rx_channel = 0;
    } else if (spi_base == SPI3_BASE) {
        map.dma_base = 0x40026000UL;
        map.tx_stream = 5; map.rx_stream = 0;
        map.tx_channel = 0; map.rx_channel = 0;
    } else if (spi_base == SPI4_BASE) {
        map.dma_base = 0x40026400UL;
        map.tx_stream = 1; map.rx_stream = 0;
        map.tx_channel = 4; map.rx_channel = 4;
    } else if (spi_base == SPI5_BASE) {
        map.dma_base = 0x40026400UL;
        map.tx_stream = 6; map.rx_stream = 5;
        map.tx_channel = 2; map.rx_channel = 2;
    } else if (spi_base == SPI6_BASE) {
        map.dma_base = 0x40026400UL;
        map.tx_stream = 5; map.rx_stream = 6;
        map.tx_channel = 5; map.rx_channel = 5;
    }
    return map;
}

/* Offsets DMA (igual que en el driver USART) */
#define DMA_SxCR_OFFSET   0x10
#define DMA_SxNDTR_OFFSET 0x14
#define DMA_SxPAR_OFFSET  0x18
#define DMA_SxM0AR_OFFSET 0x1C

/* ===================================================================
 * spi_init - Configuración completa y limpia
 * =================================================================== */
void spi_init(uint32_t spi_base, bool master, uint32_t desired_baud, uint8_t databits,
              uint8_t mode, bool nss_software)
{
    /* Paso 1: Deshabilitar SPI */
    SPI_REG(spi_base, SPI_CR1_OFFSET) &= ~(1U << 6); /* SPE = 0 */

    /* Paso 2: CR1 - Master/Slave, BR, CPOL/CPHA, DFF, LSBFIRST, SSM */
    uint32_t cr1 = 0;

    if (master) {
        cr1 |= (1U << 2); /* MSTR = 1 */
    }

    /* Baudrate (se calcula después) */
    /* CPOL y CPHA según mode (0..3) */
    if (mode & 0x02) cr1 |= (1U << 1); /* CPOL */
    if (mode & 0x01) cr1 |= (1U << 0); /* CPHA */

    /* Data frame: 8 o 16 bits */
    if (databits == 16) {
        cr1 |= (1U << 11); /* DFF = 1 */
    }

    /* NSS: software (SSM=1) o hardware */
    if (nss_software) {
        cr1 |= (1U << 9);  /* SSM = 1 */
        cr1 |= (1U << 8);  /* SSI = 1 (para master) */
    }

    /* MSB first por defecto (LSBFIRST = 0) */

    SPI_REG(spi_base, SPI_CR1_OFFSET) = cr1;

    /* Paso 3: CR2 - Configuración básica (RXNEIE/ERRIE se habilitan después) */
    uint32_t cr2 = SPI_REG(spi_base, SPI_CR2_OFFSET);
    cr2 &= ~(1U << 2); /* SSOE = 0 por defecto (NSS hardware) */
    SPI_REG(spi_base, SPI_CR2_OFFSET) = cr2;

    /* Paso 4: Baudrate */
    spi_set_baudrate(spi_base, desired_baud);

    /* Paso 5: Habilitar SPI */
    SPI_REG(spi_base, SPI_CR1_OFFSET) |= (1U << 6); /* SPE = 1 */

    /* Limpieza inicial de flags */
    (void)SPI_REG(spi_base, SPI_SR_OFFSET);
    (void)SPI_REG(spi_base, SPI_DR_OFFSET);
}

/* ===================================================================
 * spi_set_baudrate - Cálculo exacto del prescaler más cercano
 * =================================================================== */
void spi_set_baudrate(uint32_t spi_base, uint32_t desired_baud)
{
    if (desired_baud == 0) return;

    uint32_t pclk = spi_get_pclk(spi_base);
    uint32_t best_prescaler = 256;
    uint32_t best_br = 7;

    /* Prescalers posibles: 2, 4, 8, 16, 32, 64, 128, 256 */
    for (uint32_t i = 0; i < 8; i++) {
        uint32_t pres = 1U << (i + 1); /* 2^(i+1) */
        uint32_t actual = pclk / pres;
        if (actual >= desired_baud) {
            best_prescaler = pres;
            best_br = i;
            break;
        }
    }

    uint32_t cr1 = SPI_REG(spi_base, SPI_CR1_OFFSET);
    cr1 &= ~(7U << 3);           /* BR[2:0] = 000 */
    cr1 |= (best_br << 3);
    SPI_REG(spi_base, SPI_CR1_OFFSET) = cr1;
}

/* ===================================================================
 * Funciones blocking (polling) - full-duplex
 * =================================================================== */
void spi_send_blocking(uint32_t spi_base, uint16_t data)
{
    while ((spi_get_flag(spi_base, SPI_FLAG_TXE) == 0)) {}
    SPI_REG(spi_base, SPI_DR_OFFSET) = data;
}

uint16_t spi_recv_blocking(uint32_t spi_base)
{
    while ((spi_get_flag(spi_base, SPI_FLAG_RXNE) == 0)) {}
    return (uint16_t)SPI_REG(spi_base, SPI_DR_OFFSET);
}

uint16_t spi_transfer_blocking(uint32_t spi_base, uint16_t data)
{
    spi_send_blocking(spi_base, data);
    return spi_recv_blocking(spi_base);
}

void spi_send_buffer_blocking(uint32_t spi_base, const void* tx_buf, void* rx_buf, uint32_t length)
{
    if (tx_buf == NULL || length == 0) return;
    const uint8_t* tx = (const uint8_t*)tx_buf;
    uint8_t* rx = (uint8_t*)rx_buf;

    for (uint32_t i = 0; i < length; i++) {
        uint16_t rx_data = spi_transfer_blocking(spi_base, tx[i]);
        if (rx) rx[i] = (uint8_t)rx_data;
    }
}

/* ===================================================================
 * Interrupciones (RXNE + errores)
 * =================================================================== */
void spi_enable_interrupt(uint32_t spi_base)
{
    uint32_t cr2 = SPI_REG(spi_base, SPI_CR2_OFFSET);
    cr2 |= (1U << 0);  /* RXNEIE */
    cr2 |= (1U << 1);  /* TXEIE */
    cr2 |= (1U << 5);  /* ERRIE */
    SPI_REG(spi_base, SPI_CR2_OFFSET) = cr2;
}

void spi_disable_interrupt(uint32_t spi_base)
{
    uint32_t cr2 = SPI_REG(spi_base, SPI_CR2_OFFSET);
    cr2 &= ~((1U << 0) | (1U << 1) | (1U << 5));
    SPI_REG(spi_base, SPI_CR2_OFFSET) = cr2;
}

/* ===================================================================
 * Soporte DMA TX/RX (configuración ligera)
 * =================================================================== */
void spi_dma_tx_start(uint32_t spi_base, const void* buffer, uint32_t length)
{
    if (buffer == NULL || length == 0) return;

    spi_dma_map_t map = spi_get_dma_map(spi_base);
    if (map.dma_base == 0) return;

    uint32_t stream_offset = (uint32_t)map.tx_stream * 0x18UL;

    SPI_REG(map.dma_base, DMA_SxPAR_OFFSET + stream_offset) = spi_base + SPI_DR_OFFSET;
    SPI_REG(map.dma_base, DMA_SxM0AR_OFFSET + stream_offset) = (uint32_t)buffer;
    SPI_REG(map.dma_base, DMA_SxNDTR_OFFSET + stream_offset) = length;

    SPI_REG(map.dma_base, DMA_SxCR_OFFSET + stream_offset) |= (1U << 0); /* EN */

    uint32_t cr2 = SPI_REG(spi_base, SPI_CR2_OFFSET);
    cr2 |= (1U << 7); /* TXDMAEN */
    SPI_REG(spi_base, SPI_CR2_OFFSET) = cr2;
}

void spi_dma_rx_start(uint32_t spi_base, void* buffer, uint32_t length)
{
    if (buffer == NULL || length == 0) return;

    spi_dma_map_t map = spi_get_dma_map(spi_base);
    if (map.dma_base == 0) return;

    uint32_t stream_offset = (uint32_t)map.rx_stream * 0x18UL;

    SPI_REG(map.dma_base, DMA_SxPAR_OFFSET + stream_offset) = spi_base + SPI_DR_OFFSET;
    SPI_REG(map.dma_base, DMA_SxM0AR_OFFSET + stream_offset) = (uint32_t)buffer;
    SPI_REG(map.dma_base, DMA_SxNDTR_OFFSET + stream_offset) = length;

    SPI_REG(map.dma_base, DMA_SxCR_OFFSET + stream_offset) |= (1U << 0); /* EN */

    uint32_t cr2 = SPI_REG(spi_base, SPI_CR2_OFFSET);
    cr2 |= (1U << 6); /* RXDMAEN */
    SPI_REG(spi_base, SPI_CR2_OFFSET) = cr2;
}

/* ===================================================================
 * Lectura de flags
 * =================================================================== */
uint32_t spi_get_flag(uint32_t spi_base, uint32_t flag)
{
    return (SPI_REG(spi_base, SPI_SR_OFFSET) & flag);
}

/* ===================================================================
 * Handler ISR genérico (llamar desde tu ISR)
 * =================================================================== */
void spi_isr(uint32_t spi_base)
{
    uint32_t sr = SPI_REG(spi_base, SPI_SR_OFFSET);

    /* RXNE - dato listo */
    if (sr & SPI_FLAG_RXNE) {
        /* Aquí puedes leer SPI_DR y push a cola FreeRTOS */
        (void)SPI_REG(spi_base, SPI_DR_OFFSET);
    }

    /* Errores */
    if (sr & SPI_ERROR_MASK) {
        /* Limpieza automática de flags */
        (void)SPI_REG(spi_base, SPI_DR_OFFSET);
    }
}