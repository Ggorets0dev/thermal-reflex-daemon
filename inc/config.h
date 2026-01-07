#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include "glib.h"

/**
 * @brief Default path for app config
*/
#define DEFAULT_CONFIG_PATH     "/etc/thermal-reflex/config.json"

/**
 * @brief Max len of zone dir
*/
#define MAX_ZONE_PATH_LENGTH    (256)

/**
 * @brief Max len of config path
*/
#define MAX_CONFIG_PATH_LENGTH  (256)

/**
 * @brief Type of control zone
*/
typedef enum control_zone_type {
    CONTROL_ZONE_TYPE_NONE = 0,
    CONTROL_ZONE_TYPE_GPIO
} control_zone_type_e;

/**
 * @brief Reaction for control zone
*/
typedef enum control_zone_reaction {
    CONTROL_ZONE_REACTION_NONE = 0,
    CONTROL_ZONE_REACTION_GPIO_ENABLED
} control_zone_reaction_e;

/**
 * @brief Control zone
*/
typedef struct control_zone_t {
    uint8_t id;
    control_zone_type_e type;
    uint8_t pin;
    float temp_max;
    float temp_min;
    char path[MAX_ZONE_PATH_LENGTH];
} control_zone_t;

/**
 * @brief State of control zone
*/
typedef struct control_zone_state_t {
    const control_zone_t* zone;
    float current_temp;
    control_zone_reaction_e reaction;
} control_zone_state_t;

/**
 * @brief App config
*/
typedef struct threfd_config {
    GArray* control_zones;
} threfd_config_t;

/**
 * @brief App config control block
*/
typedef struct threfd_config_ctrl_block {
    threfd_config_t config;
    time_t modify_timestamp;
    char path[MAX_CONFIG_PATH_LENGTH];
} threfd_config_ctrl_block_t;

/**
 * @brief Parse App config
 * @param cfg_CB Config control block
 * @return State of read (0 - success)
*/
int parse_config(threfd_config_ctrl_block_t* cfg_CB);

/**
 * @brief Clear app config
 * @param config App config
*/
void config_clear(const threfd_config_t* config);

/**
 * @brief App config (var)
*/
extern threfd_config_ctrl_block_t app_config_CB;

#endif //CONFIG_H