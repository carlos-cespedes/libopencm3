# =====================================================
# CMakeLists.txt - STM32 Clean Room HAL (versión final)
# USART + SPI + I2C + GPIO + Timer + FreeRTOS
# Apache License 2.0
# =====================================================

cmake_minimum_required(VERSION 3.16)
project(STM32_CleanRoom_HAL LANGUAGES C ASM)  # ASM is important here

# ==================== Toolchain ====================
set(CMAKE_SYSTEM_PROCESSOR arm)

set(TOOLCHAIN_PREFIX arm-none-eabi)
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}-gcc)

# ==================== Flags comunes ====================
set(CPU_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")
set(COMMON_FLAGS "${CPU_FLAGS} -ffunction-sections -fdata-sections -fno-common -Os -Wall -Wextra -std=gnu11")
set(DEBUG_FLAGS  "${CPU_FLAGS} -ffunction-sections -fdata-sections -fno-common -O0 -Wall -Wextra -std=gnu11")

set(CMAKE_C_FLAGS "${COMMON_FLAGS}")
set(CMAKE_ASM_FLAGS "${CPU_FLAGS} -x assembler-with-cpp")

# ==================== Biblioteca HAL (drivers) ====================
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

# ====================== Syscalls (necesario para printf en ejemplos) ======================
add_library(syscalls STATIC
    syscalls.c
)

target_include_directories(syscalls PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# ==================== EJEMPLOS en carpeta examples/ ====================

# Función auxiliar para crear ejemplos fácilmente
function(add_example target_name source_file)
    add_executable(${target_name}
        ${source_file}
        startup_stm32f407xx.s
        system_stm32f4xx.c
        syscalls.c
    )
    target_link_libraries(${target_name} PRIVATE hal syscalls)
    target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

    set_target_properties(${target_name} PROPERTIES
        LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/STM32F407VG.ld \
                    -specs=nano.specs \
                    -Wl,--gc-sections -Wl,-Map=${target_name}.map -Wl,--print-memory-usage"
    )

    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${TOOLCHAIN_PREFIX}-objcopy -O binary ${target_name} ${target_name}.bin
        COMMAND ${TOOLCHAIN_PREFIX}-size ${target_name}
        COMMENT "=== ${target_name} → ${target_name}.bin ==="
    )
endfunction()


# ==================== EJECUTABLE PRINCIPAL (main.elf) ====================
add_executable(main.elf
    main.c
    startup_stm32f407xx.s   # startup entry point
    system_stm32f4xx.c      # system init
    freertos/port.c         # minimal freertos
    syscalls.c 
)

target_link_libraries(main.elf PRIVATE hal syscalls)

# Linker script
set_target_properties(main.elf PROPERTIES
    LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/STM32F407VG.ld \
                -specs=nano.specs \
                -Wl,--gc-sections -Wl,-Map=main.map -Wl,--print-memory-usage"
)

# Post-build: generar .bin y mostrar tamaño
add_custom_command(TARGET main.elf POST_BUILD
    COMMAND ${TOOLCHAIN_PREFIX}-objcopy -O binary main.elf main.bin
    COMMAND ${TOOLCHAIN_PREFIX}-size main.elf
    COMMENT "=== main.elf → main.bin ==="
)


# ====================== Ejemplos ======================
add_example(usart_example "examples/usart_example.c")
add_example(spi_example   "examples/spi_example.c")

# ==================== Mensaje final ====================
message(STATUS "===========================================")
message(STATUS "Proyecto STM32 Clean Room HAL compilado")
message(STATUS "Targets disponibles:")
message(STATUS "   • main.elf          (aplicación completa con FreeRTOS)")
message(STATUS "   • usart_example.elf (ejemplo solo USART)")
message(STATUS "   • spi_example.elf   (ejemplo solo SPI)")
message(STATUS "===========================================")