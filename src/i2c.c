/*
 * Copyright (c) 2026 Carlos Cespedes Moya
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 */

#include "i2c.h"

#define I2C_CR1   0x00
#define I2C_CR2   0x04
#define I2C_SR1   0x14
#define I2C_SR2   0x18
#define I2C_DR    0x10
#define I2C_CCR   0x1C
#define I2C_TRISE 0x20

#define I2C_REG(base, off) (*(volatile uint32_t*)((base)+(off)))

static uint32_t i2c_pclk(void) { return 42000000UL; }

void i2c_init(uint32_t i2c_base, uint32_t clock_speed_hz)
{
    I2C_REG(i2c_base, I2C_CR1) &= ~(1U);
    uint32_t ccr = i2c_pclk() / (clock_speed_hz * 2);
    if (ccr < 4) ccr = 4;
    I2C_REG(i2c_base, I2C_CCR) = ccr;
    I2C_REG(i2c_base, I2C_TRISE) = (i2c_pclk()/1000000U) + 1;
    I2C_REG(i2c_base, I2C_CR1) |= 1U;   /* PE = 1 */
}

void i2c_master_transmit(uint32_t i2c_base, uint8_t dev_addr, const uint8_t* data, uint16_t len, bool stop)
{
    I2C_REG(i2c_base, I2C_CR1) |= (1U << 8);                /* START */
    while (!(I2C_REG(i2c_base, I2C_SR1) & (1U << 0)));

    I2C_REG(i2c_base, I2C_DR) = (dev_addr << 1);
    while (!(I2C_REG(i2c_base, I2C_SR1) & I2C_FLAG_TXE));

    while (len--) {
        I2C_REG(i2c_base, I2C_DR) = *data++;
        while (!(I2C_REG(i2c_base, I2C_SR1) & I2C_FLAG_TXE));
    }
    if (stop) I2C_REG(i2c_base, I2C_CR1) |= (1U << 9);     /* STOP */
}

void i2c_master_receive(uint32_t i2c_base, uint8_t dev_addr, uint8_t* data, uint16_t len)
{
    I2C_REG(i2c_base, I2C_CR1) |= (1U << 8);                /* START */
    while (!(I2C_REG(i2c_base, I2C_SR1) & (1U << 0)));

    I2C_REG(i2c_base, I2C_DR) = (dev_addr << 1) | 1U;

    while (len--) {
        while (!(I2C_REG(i2c_base, I2C_SR1) & I2C_FLAG_RXNE));
        *data++ = I2C_REG(i2c_base, I2C_DR);
    }
    I2C_REG(i2c_base, I2C_CR1) |= (1U << 9);                /* STOP */
}

/* Interrupciones y DMA (implementación ligera) */
void i2c_enable_interrupt(uint32_t i2c_base)
{
    I2C_REG(i2c_base, I2C_CR2) |= (1U<<9) | (1U<<10) | (1U<<11); /* ITBUFEN, ITEVTEN, ITERREN */
}

void i2c_dma_tx_start(uint32_t i2c_base, const void* buffer, uint16_t length)
{
    /* Configuración DMA similar a SPI (implementación completa en proyecto real) */
}