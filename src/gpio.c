#include "gpio.h"

void set_pins_modes(const GArray* control_zones) {
    for (size_t i = 0; i < control_zones->len; i++) {
        const control_zone_t* zone = &g_array_index(control_zones, control_zone_t, i);

        if (zone->type == CONTROL_ZONE_TYPE_GPIO) {
            pinMode (zone->pin, OUTPUT) ;
        }
    }
}