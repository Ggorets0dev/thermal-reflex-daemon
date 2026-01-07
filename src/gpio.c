#include <gpiod.h>
#include "gpio.h"
#include "log.h"

#define GPIO_CHIP_PATH      ("/dev/gpiochip1")

typedef struct {
    struct gpiod_line* line;
    uint8_t vote_counter;
} gpio_pin_state_t;

static GHashTable *app_gpio_zone_map;
static struct gpiod_chip* app_gpio_chip;

static void pin_state_free(gpointer data) {
    // Clear inner fields
    gpio_pin_state_t* state = data;
    gpiod_line_release(state->line);

    g_free(state);
}

static void reg_pin(const int pin) {
    if (g_hash_table_contains(app_gpio_zone_map, GINT_TO_POINTER(pin))) {
        return;
    }

    int status = 0;
    gpio_pin_state_t* state = g_new(gpio_pin_state_t, 1);
    struct gpiod_line* line = gpiod_chip_get_line(app_gpio_chip, pin);

    if (line) {
        status = gpiod_line_request_output(line, "threfd", 0);
    }

    if (status == 0) {
        state->vote_counter = 0;
        state->line = line;
        g_hash_table_insert(app_gpio_zone_map, GINT_TO_POINTER(pin), state);
    }
}

int gpio_init(void) {
    app_gpio_zone_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, pin_state_free);
    app_gpio_chip = gpiod_chip_open(GPIO_CHIP_PATH);
    return (app_gpio_chip != NULL) ? 0 : 1;
}

void gpio_deinit(void) {
    if (app_gpio_zone_map)      g_hash_table_destroy(app_gpio_zone_map);
    if (app_gpio_chip)          gpiod_chip_close(app_gpio_chip);
}

void gpio_write(const int pin, const uint8_t value) {
    gpio_pin_state_t* state = g_hash_table_lookup(app_gpio_zone_map, GINT_TO_POINTER(pin));

    if (state->line) {
        gpiod_line_set_value(state->line, value);
    }
}

int gpio_read(const int pin) {
    const gpio_pin_state_t* state = g_hash_table_lookup(app_gpio_zone_map, GINT_TO_POINTER(pin));

    if (state->line) {
        return gpiod_line_get_value(state->line);
    }

    return -1;
}

void gpio_vote(const int pin, const gpio_vote_e vote) {
    gpio_pin_state_t* state = g_hash_table_lookup(app_gpio_zone_map, GINT_TO_POINTER(pin));

    if (state->line) {
        if (vote == GPIO_VOTE_DECREMENT && state->vote_counter > 0)     --state->vote_counter;
        else if (vote == GPIO_VOTE_INCREMENT)                           ++state->vote_counter;
    }
}

void gpio_apply_vote(const int pin) {
    const gpio_pin_state_t* state = g_hash_table_lookup(app_gpio_zone_map, GINT_TO_POINTER(pin));

    if (state->line) {
        const int pin_value = gpio_read(pin);
        if (state->vote_counter && pin_value == 0)     gpio_write(pin, TRUE);
        else if (!state->vote_counter && pin_value)    gpio_write(pin, FALSE);
    }
}

void set_pins_modes(const GArray* control_zones) {
    for (size_t i = 0; i < control_zones->len; i++) {
        const control_zone_t* zone = &g_array_index(control_zones, control_zone_t, i);

        if (zone->type == CONTROL_ZONE_TYPE_GPIO) {
            reg_pin(zone->pin);
            gpio_write(zone->pin, FALSE);

            g_log_structured(G_LOG_DOMAIN, G_LOG_LEVEL_MESSAGE,
                "MESSAGE", "Set OUTPUT mode for request control zone",
                "CONTROL_ZONE", zone->id,
                "CONTROL_ZONE_PIN", zone->pin);
        }
    }
}