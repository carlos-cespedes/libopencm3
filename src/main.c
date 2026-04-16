/*
 * main.c - Aplicación unificada con todos los drivers + FreeRTOS
 * Apache License 2.0
 */

#include "system_stm32f4xx.h"
#include "usart.h"
#include "spi.h"
#include "i2c.h"
#include "gpio.h"
#include "timer.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/freertos.h"
#include "freertos/task.h"
#include "freertos/queue.h"

QueueHandle_t uart_queue;

static void blink_task(void *arg)
{
    gpio_init(GPIOD_BASE, 12, GPIO_OUTPUT, GPIO_NOPULL, GPIO_SPEED_HIGH, 0);
    gpio_init(GPIOD_BASE, 13, GPIO_OUTPUT, GPIO_NOPULL, GPIO_SPEED_HIGH, 0);

    while (1) {
        gpio_toggle(GPIOD_BASE, 12);
        gpio_toggle(GPIOD_BASE, 13);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

static void uart_task(void *arg)
{
    usart_init(USART2_BASE, 115200, 8, 'N', 1, false);
    char msg[] = "FreeRTOS + Clean Room HAL running!\r\n";
    usart_send_string(USART2_BASE, msg);

    while (1) {
        uint8_t c = usart_recv_blocking(USART2_BASE);
        usart_send_blocking(USART2_BASE, c);
        if (c == '\r') usart_send_string(USART2_BASE, "\r\n");
    }
}

int main(void)
{
    SystemInit();

    /* Periféricos */
    systick_init();

    /* Crear tareas */
    xTaskCreate(blink_task, "Blink", 256, NULL, 2, NULL);
    xTaskCreate(uart_task,  "UART",  512, NULL, 3, NULL);

    uart_queue = xQueueCreate(64, sizeof(uint8_t));

    vTaskStartScheduler();

    while (1);
}