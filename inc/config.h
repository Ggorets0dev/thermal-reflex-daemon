#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include "glib.h"

#define DEFAULT_CONFIG_PATH     "/etc/thermal-reflex/config.json"

#define MAX_ZONE_PATH_LENGTH    (256)
#define MAX_CONFIG_PATH_LENGTH  (256)

typedef enum control_zone_type {
    CONTROL_ZONE_TYPE_NONE = 0,
    CONTROL_ZONE_TYPE_GPIO
} control_zone_type_e;

typedef enum control_zone_reaction {
    CONTROL_ZONE_REACTION_NONE = 0,
    CONTROL_ZONE_REACTION_GPIO_ENABLED
} control_zone_reaction_e;

typedef struct control_zone_t {
    uint8_t id;
    control_zone_type_e type;
    uint8_t pin;
    float temp_max;
    float temp_min;
    char path[MAX_ZONE_PATH_LENGTH];
} control_zone_t;

typedef struct control_zone_state_t {
    const control_zone_t* zone;
    float current_temp;
    control_zone_reaction_e reaction;
} control_zone_state_t;

typedef struct threfd_config {
    GArray* control_zones;
} threfd_config_t;

typedef struct threfd_config_ctrl_block {
    threfd_config_t config;
    time_t modify_timestamp;
    char path[MAX_CONFIG_PATH_LENGTH];
} threfd_config_ctrl_block_t;

int parse_config(threfd_config_ctrl_block_t* cfg_CB);

void config_clear(const threfd_config_t* config);

extern threfd_config_ctrl_block_t app_config_CB;

#endif //CONFIG_H