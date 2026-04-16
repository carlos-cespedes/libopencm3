/*
 * Minimal queue.h for this project
 * Licensed under Apache 2.0
 */

#ifndef QUEUE_H
#define QUEUE_H

#include "FreeRTOSConfig.h"
#include "portmacro.h"

typedef void * QueueHandle_t;

QueueHandle_t xQueueCreate(const UBaseType_t uxQueueLength,
                           const UBaseType_t uxItemSize);

BaseType_t xQueueSend(QueueHandle_t xQueue,
                      const void * pvItemToQueue,
                      TickType_t xTicksToWait);

BaseType_t xQueueReceive(QueueHandle_t xQueue,
                         void * const pvBuffer,
                         TickType_t xTicksToWait);

#endif /* QUEUE_H */