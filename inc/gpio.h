#ifndef GPIO_H
#define GPIO_H

#include "config.h"

/*
 * GPIO outputs are controlled using the libgpiod library, which is installed on the system.
 */

int gpio_init(void);

void gpio_deinit(void);

void gpio_write(const control_zone_t* zone, const uint8_t value);

void set_pins_modes(const GArray* control_zones);

#endif // GPIO_H