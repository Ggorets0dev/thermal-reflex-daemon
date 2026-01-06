#include "gpio.h"
#include "log.h"

#define RESET_GPIO_PIN(pin)     digitalWrite((pin), LOW)

void set_pins_modes(const GArray* control_zones) {
    for (size_t i = 0; i < control_zones->len; i++) {
        const control_zone_t* zone = &g_array_index(control_zones, control_zone_t, i);

        if (zone->type == CONTROL_ZONE_TYPE_GPIO) {
            pinMode (zone->pin, OUTPUT) ;
            RESET_GPIO_PIN(zone->pin);

            g_log_structured(G_LOG_DOMAIN, G_LOG_LEVEL_MESSAGE,
                "MESSAGE", "Set OUTPUT mode for request control zone",
                "CONTROL_ZONE", zone->id,
                "CONTROL_ZONE_PIN", zone->pin);
        }
    }
}