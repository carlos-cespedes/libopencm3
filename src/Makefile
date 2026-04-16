# ================================================
# Makefile alternativo (sin CMake) - STM32 Clean Room Drivers
# USART + SPI + System + Startup
# Apache License 2.0
# ================================================

# Toolchain
TOOLCHAIN = arm-none-eabi-
CC        = $(TOOLCHAIN)gcc
OBJCOPY   = $(TOOLCHAIN)objcopy
SIZE      = $(TOOLCHAIN)size

# CPU y flags (STM32F4 Cortex-M4)
CPU       = -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16
COMMON    = $(CPU) -ffunction-sections -fdata-sections -fno-common -Wall -Wextra -std=gnu11 -Os
CFLAGS    = $(COMMON) -I. 
LDFLAGS   = $(CPU) -Wl,--gc-sections -Wl,-Map=output.map -T STM32F407VG.ld

# Archivos fuente
SOURCES = usart.c spi.c system_stm32f4xx.c startup_stm32f407xx.s
OBJECTS = $(SOURCES:.c=.o)
OBJECTS := $(OBJECTS:.s=.o)

# Targets principales
all: usart_example.bin spi_example.bin main.elf

# Biblioteca drivers (estática)
drivers.a: $(OBJECTS)
	$(TOOLCHAIN)ar rcs $@ $^

# Ejemplo USART
usart_example.elf: examples/usart_example.c drivers.a
	$(CC) $(CFLAGS) -c examples/usart_example.c -o usart_example.o
	$(CC) $(LDFLAGS) usart_example.o drivers.a -o $@

usart_example.bin: usart_example.elf
	$(OBJCOPY) -O binary $< $@
	@echo "=== USART EXAMPLE ==="
	$(SIZE) usart_example.elf

# Ejemplo SPI
spi_example.elf: examples/spi_example.c drivers.a
	$(CC) $(CFLAGS) -c examples/spi_example.c -o spi_example.o
	$(CC) $(LDFLAGS) spi_example.o drivers.a -o $@

spi_example.bin: spi_example.elf
	$(OBJCOPY) -O binary $< $@
	@echo "=== SPI EXAMPLE ==="
	$(SIZE) spi_example.elf

# En lugar de linkear solo drivers.a
main.elf: main.c startup_stm32f407xx.o system_stm32f4xx.o drivers.a
	$(CC) $(CFLAGS) main.c startup_stm32f407xx.o system_stm32f4xx.o drivers.a \
	      $(LDFLAGS) -o $@

# Compilación de objetos
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

# Limpieza
clean:
	rm -f *.o *.a *.elf *.bin *.map output.map
	rm -f examples/*.o
	@echo "Limpieza completada."

# Phony targets
.PHONY: all clean