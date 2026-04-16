/*
 * Ejemplo de uso del driver SPI Clean Room
 * Apache License 2.0
 */

#include "common_headers.h"
#include <stdio.h>

int main(void)
{
    /* Inicializar SPI1 como Master, 8 MHz, Modo 0, NSS software */
    spi_init(SPI1_BASE, true, 8000000UL, 8, 0, true);

    printf("=== STM32 SPI Clean Room Driver ===\r\n");

    uint8_t tx_buf[16] = {0xA5, 0x5A, 0x01, 0x02, 0x03};
    uint8_t rx_buf[16] = {0};

    while (1)
    {
        /* Transferencia full-duplex */
        spi_send_buffer_blocking(SPI1_BASE, tx_buf, rx_buf, 5);

        printf("Enviado: %02X %02X | Recibido: %02X %02X\r\n",
               tx_buf[0], tx_buf[1], rx_buf[0], rx_buf[1]);

        /* Ejemplo de transferencia simple */
        uint16_t response = spi_transfer_blocking(SPI1_BASE, 0xAA);
        printf("Respuesta SPI: 0x%04X\r\n", response);

        /* Pequeño delay (en producción usa timer o RTOS) */
        for (volatile uint32_t i = 0; i < 2000000; i++);
    }

    return 0;
}