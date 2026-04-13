/*
 * STM32F407VG.ld - Linker script clean-room
 * Copyright (c) 2026 Carlos Cespedes Moya
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * STM32F407VG (1 MB Flash, 192 KB RAM)
 * Compatible con STM32F4 series (ajusta LENGTH para otros modelos)
 */

MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 1M     /* Código y constantes */
  RAM   (xrw) : ORIGIN = 0x20000000, LENGTH = 192K   /* Variables, stack, heap */
}

/* Tamaño del stack y heap (puedes cambiarlos) */
STACK_SIZE = 8K;
HEAP_SIZE  = 4K;

SECTIONS
{
  .isr_vector : {
    . = ALIGN(4);
    KEEP(*(.isr_vector))     /* Tabla de vectores (startup) */
    . = ALIGN(4);
  } > FLASH

  .text : {
    . = ALIGN(4);
    *(.text*)                /* Código del programa */
    *(.rodata*)              /* Constantes */
    . = ALIGN(4);
  } > FLASH

  .data : {
    . = ALIGN(4);
    _sdata = .;
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } > RAM AT > FLASH

  .bss : {
    . = ALIGN(4);
    _sbss = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
  } > RAM

  ._heap : {
    . = ALIGN(8);
    _sheap = .;
    . = . + HEAP_SIZE;
    _eheap = .;
  } > RAM

  ._stack : {
    . = ALIGN(8);
    . = . + STACK_SIZE;
    _estack = .;
  } > RAM

  /* Símbolos para startup code */
  _sidata = LOADADDR(.data);
  _sdata = ADDR(.data);
  _edata = ADDR(.data) + SIZEOF(.data);
  _sbss  = ADDR(.bss);
  _ebss  = ADDR(.bss) + SIZEOF(.bss);
}

ENTRY(Reset_Handler)