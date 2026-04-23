# libopencm3
**Light, Apache2, Open Source Library for ARM Cortex-M3 (STM32, NXP LPC)**

This is a HAL Library with drivers for ARM Cortex®-M3 microcontroller, targeted for embedded applications.
It is released under permissive license Apache2.
The Cortex-M combines low power comsumption with convenient integration of peripherals, communication protocols and ease of development.

## How to build

- mkdir build && cd build
- cmake ..
- make

### How to build the examples only

- make usart_example
- make spi_example


## What is the purpose of each file?

**system_stm32f4xx.c**
This file has the system init for the micro-controller

It gets executed before main () (invoked by startup).
Configs the main clock of MCU: System Clock.
Sets HSE external oscillator (8 Mhz typically).
Configs PLL to get max frequency (168 Mhz on F4, 216 MHz on F7).
Sets prescalers AHB, APB1, APB2.
Configs Flash wait states according to frequency.

Defines global var SystemCoreClock (used by FreeRTOS, delay functions, etc.).
Provides SystemCoreClockUpdate() to recalculate the clock if configuration is changed during runtime.
Cleans and enables the FPU (Floating Point Unit) on Cortex-M4/M7

Without this file the MCU cannot start properly (clock would stick to 16Mhz internal HSI, things would run either very slow or unstable).

**Linker Script (STM32F407VG.ld)**
This file tells the linker (ld executable from arm-none-eabi-gcc) how to place the code and the data on physical memory of the microcontroller.

Defines the two main memories:
FLASH → where code and constants are found.
RAM → where variables, stack and heap are found.

Creates the Interupt's vector table at the proper address (0x08000000).
Defines special sections (.text, .data, .bss, .rodata, heap, stack).
Reserves memory for the stack and heap.
Specifies the code entry point (Reset_Handler).

Without a correct linker script, the compiler would not know where to place your program and the binary would be invalid (MCU would not start).
