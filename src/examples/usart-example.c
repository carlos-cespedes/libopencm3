/*
 * Ejemplo de uso del driver USART Clean Room
 * Apache License 2.0
 */

#include "usart.h"
#include <stdio.h>

int main(void)
{
    /* Inicializar USART2 a 115200 8N1 sin flow control */
    usart_init(USART2_BASE, 115200, 8, 'N', 1, false);

    /* Redirección de printf a USART2 */
    usart_stdio_base = USART2_BASE;

    printf("=== STM32 USART Clean Room Driver ===\r\n");
    printf("Hola mundo desde USART2!\r\n");

    char buffer[64];

    while (1)
    {
        /* Eco con bloqueante */
        uint8_t c = usart_recv_blocking(USART2_BASE);
        usart_send_blocking(USART2_BASE, c);

        if (c == '\r') {
            printf("\r\nRecibido comando!\r\n");
        }
    }

    return 0;
}