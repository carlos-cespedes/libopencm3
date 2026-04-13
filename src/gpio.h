#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include <stdbool.h>

#define GPIOA_BASE 0x40020000UL
#define GPIOB_BASE 0x40020400UL
#define GPIOC_BASE 0x40020800UL
#define GPIOD_BASE 0x40020C00UL
#define GPIOE_BASE 0x40021000UL
#define GPIOF_BASE 0x40021400UL
#define GPIOG_BASE 0x40021800UL
#define GPIOH_BASE 0x40021C00UL
#define GPIOI_BASE 0x40022000UL

typedef enum { GPIO_INPUT, GPIO_OUTPUT, GPIO_AF, GPIO_ANALOG } gpio_mode_t;
typedef enum { GPIO_NOPULL, GPIO_PULLUP, GPIO_PULLDOWN } gpio_pull_t;
typedef enum { GPIO_SPEED_LOW, GPIO_SPEED_MED, GPIO_SPEED_HIGH, GPIO_SPEED_VERYHIGH } gpio_speed_t;

void gpio_init(uint32_t gpio_base, uint8_t pin, gpio_mode_t mode, gpio_pull_t pull, gpio_speed_t speed, uint8_t af);
void gpio_write(uint32_t gpio_base, uint8_t pin, bool value);
bool gpio_read(uint32_t gpio_base, uint8_t pin);
void gpio_toggle(uint32_t gpio_base, uint8_t pin);

void gpio_enable_exti(uint8_t pin, bool rising, bool falling, void (*callback)(uint8_t pin));

#endif /* GPIO_H */