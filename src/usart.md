# USART/UART Driver Clean-Room para STM32F4/F7 (Apache 2.0)

**Implementación 100% original y limpia** basada exclusivamente en los datasheets oficiales:
- RM0090 (STM32F407/417/427/437/429/439)
- RM0385 (STM32F7xx)

**Características completas:**
- Inicialización completa (baudrate, 8/9 bits, parity, stop bits, flow control)
- Polling, interrupciones y DMA (TX/RX)
- Cálculo preciso de baudrate con oversampling 8/16 (APB1/APB2)
- Manejo completo de errores (ORE, FE, NF, PE)
- Soporte multi-instancia (USART1..6 + UART4/5)
- Redirección `printf` vía `_write` débil
- Integración básica con FreeRTOS (colas y semáforos vía `usart_isr`)
- Código ligero, eficiente y seguro

**Licencia:** Apache License 2.0 (permisiva) – alternativa limpia a libopencm3.

## Cómo usar

```c
#include "usart.h"

// Ejemplo inicialización USART2 115200 8N1 sin flow control
usart_init(USART2_BASE, 115200, 8, 'N', 1, false);

// Envío bloqueante
usart_send_string(USART2_BASE, "Hola mundo\r\n");

// Recepción bloqueante
uint8_t dato = usart_recv_blocking(USART2_BASE);

// DMA (previamente configura el stream DMA con el canal correcto)
uint8_t tx_buf[64] = "Datos DMA";
usart_dma_tx_start(USART2_BASE, tx_buf, 64);

// Interrupciones + FreeRTOS
usart_enable_interrupt(USART2_BASE);
// En tu ISR:
void USART2_IRQHandler(void) {
    usart_isr(USART2_BASE);
}