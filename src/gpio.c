#include "gpio.h"

#define GPIO_REG(base, off) (*(volatile uint32_t*)((base)+(off)))

void gpio_init(uint32_t gpio_base, uint8_t pin, gpio_mode_t mode, gpio_pull_t pull, gpio_speed_t speed, uint8_t af)
{
    uint8_t port = (gpio_base - GPIOA_BASE) / 0x400;

    /* Habilitar reloj GPIO */
    *(volatile uint32_t*)0x40023830 |= (1U << port);

    uint32_t pin2 = pin * 2;
    GPIO_REG(gpio_base, 0x00) &= ~(3U << pin2);
    GPIO_REG(gpio_base, 0x00) |= ((uint32_t)mode << pin2);

    /* Speed, Pull, AF */
    GPIO_REG(gpio_base, 0x08) |= (speed << (pin*2));
    GPIO_REG(gpio_base, 0x0C) |= (pull << (pin*2));

    if (mode == GPIO_AF) {
        uint8_t idx = pin / 8;
        uint8_t shift = (pin % 8) * 4;
        GPIO_REG(gpio_base, 0x20 + idx*4) &= ~(0xFU << shift);
        GPIO_REG(gpio_base, 0x20 + idx*4) |= ((uint32_t)af << shift);
    }
}