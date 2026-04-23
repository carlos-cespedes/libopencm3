# libopencm3
**Librería de código abierto, Apache2, liviana para el ARM Cortex-M3 (STM32, NXP LPC)**

Esta es una librería HAL (Hardware Abstraction Layer) que contiene drivers para el microcontrolador ARM Cortex®-M3, enfocado para aplicaciones embebidas.
Publicado bajo una licencia más permisiva Apache2.
El microcontrolador Cortex-M combina bajo consumo de potencia con una conveniente integración de periféricos, protocolos de comunicación y facilidad de desarollo.

## Cómo compilar

- mkdir build && cd build
- cmake ..
- make

### Cómo compilar solo los ejemplos:

- make usart_example
- make spi_example


## ¿Para qué sirve cada archivo?

**system_stm32f4xx.c**
Este archivo es el inicializador del sistema del microcontrolador.

Se ejecuta antes de main() (lo llama el código de startup).
Configura el reloj principal del MCU (System Clock):
Activa el oscilador externo HSE (8 MHz típico).
Configura el PLL para obtener la frecuencia máxima (168 MHz en F4 / 216 MHz en F7).
Configura los prescalers AHB, APB1 y APB2.
Configura la latencia de Flash (Flash wait states) según la frecuencia.

Define la variable global SystemCoreClock (usada por FreeRTOS, delay functions, etc.).
Proporciona SystemCoreClockUpdate() para recalcular el reloj si cambias la configuración en runtime.
Limpia y habilita el FPU (Floating Point Unit) en Cortex-M4/M7.

Sin este archivo no puedes arrancar correctamente el MCU (el reloj quedaría en HSI 16 MHz interno y todo iría muy lento o inestable).

**Linker Script (STM32F407VG.ld)**
Este archivo le dice al linker (el programa ld dentro de arm-none-eabi-gcc) cómo colocar el código y los datos en la memoria física del microcontrolador.

Define las dos memorias principales:
FLASH → donde va el código y las constantes.
RAM → donde van las variables, stack y heap.

Coloca la tabla de vectores de interrupción en la dirección correcta (0x08000000).
Define secciones especiales (.text, .data, .bss, .rodata, heap, stack).
Reserva espacio para el stack y el heap.
Especifica el punto de entrada (Reset_Handler).

Sin un linker script correcto el compilador no sabría dónde poner tu programa y el binario sería inválido (el MCU no arrancaría).