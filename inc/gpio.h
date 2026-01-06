#ifndef GPIO_H
#define GPIO_H

#include "config.h"

/*
 * GPIO outputs are controlled using the WiringOP library, which is installed on the system.
 */

#include "wiringPi.h"

void set_pins_modes(const GArray* control_zones);

#endif // GPIO_H