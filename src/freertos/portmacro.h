/*
 * Minimal portmacro.h for Cortex-M4
 */

#ifndef PORTMACRO_H
#define PORTMACRO_H

#include <stdint.h>

typedef uint32_t StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;
typedef uint32_t TickType_t;

#define portMAX_DELAY           (TickType_t)0xffffffffUL

#endif /* PORTMACRO_H */