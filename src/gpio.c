#include <gpiod.h>
#include "gpio.h"
#include "log.h"

#define GPIO_CHIP_PATH      ("/dev/gpiochip1")

static GHashTable *app_gpio_zone_map;
static struct gpiod_chip* app_gpio_chip;

static void reg_zone(control_zone_t* zone) {
    int status = 0;
    struct gpiod_line* line = gpiod_chip_get_line(app_gpio_chip, zone->pin);

    if (line) {
        status = gpiod_line_request_output(line, "threfd", 0);
    }

    if (status == 0) {
        g_hash_table_insert(app_gpio_zone_map, zone, line);
    }
}

static void close_zone_line(gpointer key __attribute__((unused)), gpointer value, gpointer user_data __attribute__((unused))) {
    struct gpiod_line *line = value;
    gpiod_line_release(line);
}

int gpio_init(void) {
    app_gpio_zone_map = g_hash_table_new(g_direct_hash, g_direct_equal);
    app_gpio_chip = gpiod_chip_open(GPIO_CHIP_PATH);
    return (app_gpio_chip != NULL) ? 0 : 1;
}

void gpio_deinit(void) {
    g_hash_table_foreach(app_gpio_zone_map, (GHFunc)close_zone_line, NULL);
    g_hash_table_destroy(app_gpio_zone_map);
    gpiod_chip_close(app_gpio_chip);
}

void gpio_write(const control_zone_t* zone, const uint8_t value) {
    struct gpiod_line *line = g_hash_table_lookup(app_gpio_zone_map, zone);

    if (line) {
        gpiod_line_set_value(line, value);
    }
}

void set_pins_modes(const GArray* control_zones) {
    for (size_t i = 0; i < control_zones->len; i++) {
        control_zone_t* zone = &g_array_index(control_zones, control_zone_t, i);

        if (zone->type == CONTROL_ZONE_TYPE_GPIO) {
            reg_zone(zone);
            gpio_write(zone, FALSE);

            g_log_structured(G_LOG_DOMAIN, G_LOG_LEVEL_MESSAGE,
                "MESSAGE", "Set OUTPUT mode for request control zone",
                "CONTROL_ZONE", zone->id,
                "CONTROL_ZONE_PIN", zone->pin);
        }
    }
}