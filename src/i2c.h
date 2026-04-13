/*
 * Copyright (c) 2026 Carlos Cespedes Moya
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <stdbool.h>

#define I2C1_BASE       0x40005400UL
#define I2C2_BASE       0x40005800UL
#define I2C3_BASE       0x40005C00UL

#define I2C_FLAG_TXE    (1U << 7)
#define I2C_FLAG_RXNE   (1U << 6)
#define I2C_FLAG_BTF    (1U << 2)
#define I2C_FLAG_ADDR   (1U << 1)
#define I2C_FLAG_STOPF  (1U << 4)
#define I2C_ERROR_MASK  (0x0F00U)   /* AF, ARLO, BERR, OVR */

void i2c_init(uint32_t i2c_base, uint32_t clock_speed_hz);
void i2c_master_transmit(uint32_t i2c_base, uint8_t dev_addr, const uint8_t* data, uint16_t len, bool stop);
void i2c_master_receive(uint32_t i2c_base, uint8_t dev_addr, uint8_t* data, uint16_t len);
void i2c_enable_interrupt(uint32_t i2c_base);
void i2c_disable_interrupt(uint32_t i2c_base);
void i2c_dma_tx_start(uint32_t i2c_base, const void* buffer, uint16_t length);
void i2c_dma_rx_start(uint32_t i2c_base, void* buffer, uint16_t length);
uint32_t i2c_get_flag(uint32_t i2c_base, uint32_t flag);

#endif /* I2C_H */