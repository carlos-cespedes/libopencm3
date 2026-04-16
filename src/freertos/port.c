/*
 * Copyright (c) 2026 Carlos Cespedes Moya
 * Licensed under Apache 2.0
 */

#include "freertos.h"
#include "task.h"
#include "timer.h"

extern void xPortPendSVHandler(void);
extern void xPortSysTickHandler(void);
extern void vPortSVCHandler(void);

void vPortSetupTimerInterrupt(void)
{
    systick_init();
}

BaseType_t xTaskCreate(TaskFunction_t pxTaskCode,
                       const char * const pcName,
                       const uint16_t usStackDepth,
                       void * const pvParameters,
                       UBaseType_t uxPriority,
                       TaskHandle_t * const pxCreatedTask)
{
    (void)pcName; (void)usStackDepth; (void)pvParameters; (void)uxPriority; (void)pxCreatedTask;
    return pdPASS;   /* siempre ok para este ejemplo mínimo */
}

void vTaskStartScheduler(void)
{
    /* En este ejemplo mínimo solo habilitamos interrupciones y entramos en bucle */
    __asm volatile("cpsie i");
    while (1);
}

void vTaskDelay(const TickType_t xTicksToDelay)
{
    delay_ms(xTicksToDelay);
}

QueueHandle_t xQueueCreate(const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize)
{
    (void)uxQueueLength; (void)uxItemSize;
    return (QueueHandle_t)1;   /* dummy */
}

BaseType_t xQueueSend(QueueHandle_t xQueue,
                      const void * pvItemToQueue,
                      TickType_t xTicksToWait) 
{ 
    return pdPASS;
}

BaseType_t xQueueReceive(QueueHandle_t xQueue,
                         void * const pvBuffer,
                         TickType_t xTicksToWait) 
{
    return pdPASS;
}