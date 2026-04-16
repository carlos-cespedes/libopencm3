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

#ifndef SPI_H
#define SPI_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* ===================================================================
 * Direcciones base oficiales (RM0090 / RM0385 - Sección 2.2 Memory map)
 * =================================================================== */
#define SPI1_BASE       0x40013000UL
#define SPI2_BASE       0x40003800UL
#define SPI3_BASE       0x40003C00UL
#define SPI4_BASE       0x40013400UL
#define SPI5_BASE       0x40015000UL
#define SPI6_BASE       0x40015400UL

/* ===================================================================
 * Flags de estado (RM0090 / RM0385 - SPI_SR, offset 0x08)
 * =================================================================== */
#define SPI_FLAG_RXNE   (1U << 0)
#define SPI_FLAG_TXE    (1U << 1)
#define SPI_FLAG_CRCERR (1U << 4)
#define SPI_FLAG_MODF   (1U << 5)
#define SPI_FLAG_OVR    (1U << 6)
#define SPI_FLAG_BSY    (1U << 7)
#define SPI_FLAG_FRE    (1U << 8)

/* Máscara de errores completa */
#define SPI_ERROR_MASK  (SPI_FLAG_CRCERR | SPI_FLAG_MODF | SPI_FLAG_OVR | SPI_FLAG_FRE)

/* ===================================================================
 * Frecuencias de reloj APB (el usuario debe sobrescribir según su RCC)
 * =================================================================== */
#ifndef PCLK1_FREQ
#define PCLK1_FREQ      42000000UL   /* APB1 (SPI2/SPI3) */
#endif
#ifndef PCLK2_FREQ
#define PCLK2_FREQ      84000000UL   /* APB2 (SPI1/SPI4/SPI5/SPI6) */
#endif

/* ===================================================================
 * API pública (estilo libopencm3: simple, eficiente y completa)
 * =================================================================== */
void spi_init(uint32_t spi_base, bool master, uint32_t desired_baud, uint8_t databits,
              uint8_t mode, bool nss_software);
void spi_set_baudrate(uint32_t spi_base, uint32_t desired_baud);
void spi_send_blocking(uint32_t spi_base, uint16_t data);
uint16_t spi_recv_blocking(uint32_t spi_base);
uint16_t spi_transfer_blocking(uint32_t spi_base, uint16_t data);
void spi_send_buffer_blocking(uint32_t spi_base, const void* tx_buf, void* rx_buf, uint32_t length);
void spi_enable_interrupt(uint32_t spi_base);
void spi_disable_interrupt(uint32_t spi_base);
void spi_dma_tx_start(uint32_t spi_base, const void* buffer, uint32_t length);
void spi_dma_rx_start(uint32_t spi_base, void* buffer, uint32_t length);
uint32_t spi_get_flag(uint32_t spi_base, uint32_t flag);

#endif /* SPI_H */