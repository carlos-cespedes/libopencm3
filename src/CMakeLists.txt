# ================================================
# CMakeLists.txt - STM32F4/F7 Clean Room Drivers
# USART + SPI (Apache 2.0)
# ================================================

cmake_minimum_required(VERSION 3.16)
project(STM32_CleanRoom_HAL LANGUAGES C ASM)   # ← ASM es importante

set(TOOLCHAIN_PREFIX arm-none-eabi)
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}-gcc)

set(CPU_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")
set(CMAKE_C_FLAGS "${CPU_FLAGS} -ffunction-sections -fdata-sections -Wall -Wextra -Os -std=gnu11")
set(CMAKE_ASM_FLAGS "${CPU_FLAGS} -x assembler-with-cpp")

# ==================== DRIVERS (solo periféricos) ====================
add_library(hal STATIC
    usart.c
    spi.c
    i2c.c
    gpio.c
    timer.c
)

target_include_directories(hal PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/freertos
)

# ==================== EJECUTABLE FINAL ====================
add_executable(main.elf
    main.c
    startup_stm32f407xx.s   # startup entry point
    system_stm32f4xx.c      # system init
    freertos/port.c         # minimal freertos 
)

target_link_libraries(main.elf PRIVATE hal)

# Linker script
set_target_properties(main.elf PROPERTIES
    LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/STM32F407VG.ld -Wl,--gc-sections -Wl,-Map=main.map"
)

# Post-build: generar .bin y mostrar tamaño
add_custom_command(TARGET main.elf POST_BUILD
    COMMAND ${TOOLCHAIN_PREFIX}-objcopy -O binary main.elf main.bin
    COMMAND ${TOOLCHAIN_PREFIX}-size main.elf
    COMMENT "Generando main.bin y mostrando tamaño..."
)
    
# ================================================
# Mensajes finales
# ================================================
message(STATUS "===========================================")
message(STATUS "Proyecto: STM32 Clean Room Drivers (USART + SPI)")
message(STATUS "Build examples: ${BUILD_EXAMPLES}")
message(STATUS "===========================================")