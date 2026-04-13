# ================================================
# CMakeLists.txt - STM32F4/F7 Clean Room Drivers
# USART + SPI (Apache 2.0)
# ================================================

cmake_minimum_required(VERSION 3.16)
project(STM32_CleanRoom_Drivers LANGUAGES C)

# ================================================
# Configuración de toolchain ARM (ajusta según tu instalación)
# ================================================
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(TOOLCHAIN_PREFIX arm-none-eabi)
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}-objcopy)
set(CMAKE_OBJDUMP ${TOOLCHAIN_PREFIX}-objdump)
set(CMAKE_SIZE ${TOOLCHAIN_PREFIX}-size)

# Flags comunes para STM32F4/F7
set(CPU_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")
set(COMMON_FLAGS "${CPU_FLAGS} -ffunction-sections -fdata-sections -fno-common -Wall -Wextra -std=gnu11")

set(CMAKE_C_FLAGS "${COMMON_FLAGS} -Os")
set(CMAKE_EXE_LINKER_FLAGS "${CPU_FLAGS} -Wl,--gc-sections -Wl,-Map=output.map")

# ================================================
# Opciones del proyecto
# ================================================
option(BUILD_EXAMPLES "Build example applications" ON)

# ================================================
# Drivers (biblioteca estática)
# ================================================
add_library(drivers STATIC
    usart.c
    spi.c
)

target_include_directories(drivers PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(drivers PUBLIC
    PCLK1_FREQ=42000000UL
    PCLK2_FREQ=84000000UL
)

# ================================================
# Ejemplos
# ================================================
if(BUILD_EXAMPLES)
    # --- USART Example ---
    add_executable(usart_example
        examples/usart_example.c
    )
    target_link_libraries(usart_example PRIVATE drivers)
    target_include_directories(usart_example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

    # --- SPI Example ---
    add_executable(spi_example
        examples/spi_example.c
    )
    target_link_libraries(spi_example PRIVATE drivers)
    target_include_directories(spi_example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

    # Post-build: generar .bin y mostrar tamaño
    foreach(target usart_example spi_example)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} -O binary ${target} ${target}.bin
            COMMAND ${CMAKE_SIZE} ${target}
            COMMENT "Generando binario y mostrando tamaño..."
        )
    endforeach()
endif()

# Startup + System
add_library(startup OBJECT
    startup_stm32f407xx.s
    system_stm32f4xx.c
)

target_include_directories(startup PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Linkear todo correctamente
target_link_libraries(drivers PRIVATE startup)

# Linker script para los ejemplos
set_target_properties(usart_example spi_example PROPERTIES
    LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/STM32F407VG.ld -Wl,--gc-sections"

# ================================================
# Mensajes finales
# ================================================
message(STATUS "===========================================")
message(STATUS "Proyecto: STM32 Clean Room Drivers (USART + SPI)")
message(STATUS "Build examples: ${BUILD_EXAMPLES}")
message(STATUS "===========================================")