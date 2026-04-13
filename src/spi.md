# SPI Driver Clean-Room para STM32F4/F7 (Apache 2.0)

**Implementación 100% original y limpia** basada exclusivamente en los datasheets oficiales:
- RM0090 (STM32F407/417/427/437/429/439)
- RM0385 (STM32F7xx)

**¿Por qué este driver?**  
Elegí **SPI** porque es uno de los periféricos **más utilizados y de mayor valor** en el mercado embedded actual. Ingenieros que usan libopencm3 lo necesitan constantemente para:
- Memorias Flash externas (QSPI no siempre disponible)
- Pantallas TFT / OLED
- Sensores de alto rendimiento (IMU, ADC externos)
- Tarjetas SD / eMMC
- Ethernet PHYs, etc.

Es más crítico en rendimiento que I2C y complementa perfectamente al driver USART que ya creamos.

**Características completas:**
- Master / Slave completo
- Baudrate calculado automáticamente (prescalers 2..256)
- Modos 0..3 (CPOL/CPHA)
- 8/16 bits por frame
- NSS software / hardware
- Polling, interrupciones y **DMA** (TX + RX)
- Manejo completo de errores (OVR, MODF, CRCERR, FRE)
- Soporte multi-instancia (SPI1..SPI6)
- Buffer blocking full-duplex
- Integración lista con FreeRTOS (ISR con colas)

**Licencia:** Apache License 2.0 (permisiva) – alternativa limpia y moderna a libopencm3.

## Cómo usar

```c
#include "spi.h"

// Ejemplo SPI1 Master 10 MHz, modo 0, 8 bits, NSS software
spi_init(SPI1_BASE, true, 10000000, 8, 0, true);

// Envío simple
spi_send_blocking(SPI1_BASE, 0xAA);

// Transferencia full-duplex
uint16_t rx = spi_transfer_blocking(SPI1_BASE, 0x55);

// Buffer completo (TX y RX simultáneo)
uint8_t tx_buf[64] = {0};
uint8_t rx_buf[64];
spi_send_buffer_blocking(SPI1_BASE, tx_buf, rx_buf, 64);

// DMA (previamente configura el stream)
uint8_t dma_tx_buf[128];
spi_dma_tx_start(SPI1_BASE, dma_tx_buf, 128);

// Interrupciones + FreeRTOS
spi_enable_interrupt(SPI1_BASE);
// En tu ISR:
void SPI1_IRQHandler(void) {
    spi_isr(SPI1_BASE);
}