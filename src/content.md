libopencm3 stm32 

src/
├── usart.h / usart.c
├── spi.h   / spi.c
├── system_stm32f4xx.c
├── i2c.h   / i2c.c          ← NUEVO
├── gpio.h  / gpio.c         ← NUEVO
├── timer.h / timer.c        ← NUEVO
├── freertos/
│   ├── FreeRTOSConfig.h
│   ├── port.c               ← NUEVO (port limpio para Cortex-M4)
│   └── portmacro.h
├── examples/
│   ├── usart_example.c
│   ├── spi_example.c
│   └── main.c               ← NUEVO (unificado)
├── STM32F407VG.ld
├── startup_stm32f407xx.s
├── examples/
│   ├── usart_example.c
│   └── spi_example.c
├── CMakeLists.txt         
└── Makefile      
└── main.c                   ← (opcional, si prefieres usarlo directamente)         