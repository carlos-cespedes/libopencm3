/*
 * Minimal task.h for this project (clean-room compatible)
 * Licensed under Apache 2.0
 */

#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#include "FreeRTOSConfig.h"
#include "portmacro.h"

typedef void (*TaskFunction_t)(void *);

/* Tipos básicos */
typedef void * TaskHandle_t;

/* Funciones usadas en main.c */
BaseType_t xTaskCreate(TaskFunction_t pxTaskCode,
                       const char * const pcName,
                       const uint16_t usStackDepth,
                       void * const pvParameters,
                       UBaseType_t uxPriority,
                       TaskHandle_t * const pxCreatedTask);

void vTaskStartScheduler(void);
void vTaskDelay(const TickType_t xTicksToDelay);

#define pdMS_TO_TICKS(xTimeInMs)    ((TickType_t)(((TickType_t)(xTimeInMs) * (TickType_t)configTICK_RATE_HZ) / (TickType_t)1000))

/* Constantes */
#define pdPASS          (1)
#define pdFAIL          (0)

#endif /* TASK_H */