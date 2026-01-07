#ifndef GPIO_H
#define GPIO_H

#include <stdbool.h>
#include "config.h"

/*
 * GPIO outputs are controlled using the libgpiod library, which is installed on the system.
 */

/**
 * @brief GPIO vote
 */
typedef enum {
    GPIO_VOTE_DECREMENT,
    GPIO_VOTE_INCREMENT,
} gpio_vote_e;

/**
 * @brief Init GPIO lib (gpiod) and inner components
 * @return Status of init (0 - success)
 */
int gpio_init(void);

/**
 * @brief Deinit GPIo lib (gpiod) and inner components
 */
void gpio_deinit(void);

/**
 * @brief Write state to GPIO pin
 * @param pin GPIO pin
 * @param value Value (0 or 1)
 */
void gpio_write(int pin, bool value);

/**
 * @brief Read state from GPIO pin
 * @param pin GPIO pin
 * @return GPIO status (0 or 1 or -1 if error)
 */
int gpio_read(int pin);

/**
 * @brief Vote for GPIO pin state
 * @param pin GPIO pin
 * @param vote Vote for pin
 */
void gpio_vote(int pin, gpio_vote_e vote);

/**
 * @brief Apply vote for GPIO pin state
 * @param pin GPIO pin
*/
void gpio_apply_vote(int pin);

/**
 * @brief Set modes for pins and reset their states
 * @param control_zones Control zones
*/
void set_pins_modes(const GArray* control_zones);

#endif // GPIO_H