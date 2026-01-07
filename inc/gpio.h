#ifndef GPIO_H
#define GPIO_H

#include "config.h"

/*
 * GPIO outputs are controlled using the libgpiod library, which is installed on the system.
 */

typedef enum {
    GPIO_VOTE_DECREMENT,
    GPIO_VOTE_INCREMENT,
} gpio_vote_e;

int gpio_init(void);

void gpio_deinit(void);

void gpio_write(int pin, uint8_t value);

void gpio_vote(int pin, gpio_vote_e vote);

void gpio_apply_vote(int pin);

void set_pins_modes(const GArray* control_zones);

#endif // GPIO_H