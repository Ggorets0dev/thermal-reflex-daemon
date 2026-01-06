#include <stdio.h>

#include "thermal_monitor_task.h"
#include "config.h"
#include "gpio.h"
#include "log.h"

static GArray* app_ctrl_zones_states;

static void deinit_zones_states(void) __attribute__((destructor));

static void init_zones_states(const threfd_config_t* config) {
    app_ctrl_zones_states = g_array_new(FALSE, TRUE, sizeof(control_zone_state_t));

    for (size_t i = 0; i < config->control_zones->len; i++) {
        const control_zone_t* zone = &g_array_index(config->control_zones, control_zone_t, i);
        control_zone_state_t state = {0};

        state.zone = zone;
        state.current_temp = 0.0f;
        state.reaction = CONTROL_ZONE_REACTION_NONE;

        g_array_append_val(app_ctrl_zones_states, state);
    }
}

static float get_temp_from_zone(const control_zone_t* zone) {
    FILE *file = NULL;
    int temp = 0;
    char temp_file_path[MAX_ZONE_PATH_LENGTH + 20] = {0};

    strcpy(temp_file_path, zone->path);
    strcat(temp_file_path, "/temp");

    file = fopen(temp_file_path, "r");

    if (file == NULL) {
        g_log_structured(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "MESSAGE", "Failed to open temperature file for control zone");
        return 1;
    }

    if (fscanf(file, "%d", &temp) == 1) {

    } else {
        g_log_structured(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "MESSAGE", "Failed to read temperature for control zone");
    }

    fclose(file);

    // MicroCelsius --> Celsius
    return (float)temp / 1000.0f;
}

static void deinit_zones_states(void) {
    g_array_free(app_ctrl_zones_states, TRUE);
}

gboolean monitor_temp_task(gpointer data) {
    const threfd_config_t* config = (threfd_config_t *)data;

    if (!app_ctrl_zones_states) {
        init_zones_states(config);
    }

    // Parse temp for all zones and apply reaction if needed
    for (size_t i = 0; i < app_ctrl_zones_states->len; i++) {
        control_zone_state_t* state = &g_array_index(app_ctrl_zones_states, control_zone_state_t, i);
        state->current_temp = get_temp_from_zone(state->zone);

        if (state->current_temp > state->zone->temp_max && state->reaction == CONTROL_ZONE_REACTION_NONE) {
            state->reaction = CONTROL_ZONE_REACTION_GPIO_ENABLED;
            gpio_write(state->zone, TRUE);

            g_log_structured(G_LOG_DOMAIN, G_LOG_LEVEL_MESSAGE, "MESSAGE", "Temperature threshold exceeded, reaction completed",
                "CONTROL_ZONE", state->zone->id);

        } else if (state->current_temp <= state->zone->temp_min && state->reaction == CONTROL_ZONE_REACTION_GPIO_ENABLED) {
            state->reaction = CONTROL_ZONE_REACTION_NONE;
            gpio_write(state->zone, FALSE);

            g_log_structured(G_LOG_DOMAIN, G_LOG_LEVEL_MESSAGE, "MESSAGE", "Temperature reduction to the required level detected, reaction completed",
                "CONTROL_ZONE", state->zone->id);
        }
    }

    return G_SOURCE_CONTINUE;
}