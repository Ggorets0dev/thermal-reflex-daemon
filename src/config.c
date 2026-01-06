#include <json-c/json.h>
#include <sys/stat.h>
#include <unistd.h>
#include "log.h"
#include "config.h"

#include <ctype.h>

threfd_config_ctrl_block_t app_config_CB;

int parse_sunxi_gpio(const char *pin_str, uint8_t* pin_out) {
    if (!pin_str || (pin_str[0] != 'P' && pin_str[0] != 'p')) {
        return 1;
    }

    const char port_letter = toupper(pin_str[1]);
    if (port_letter < 'A' || port_letter > 'Z') {
        return 2;
    }

    const int pin_num = atoi(&pin_str[2]);

    *pin_out = (uint8_t)((port_letter - 'A') * 32 + pin_num);

    return 0;
}

static control_zone_type_e parse_control_zone_type(const char* type) {
    if (strcmp(type, "GPIO") == 0) {
        return CONTROL_ZONE_TYPE_GPIO;
    }

    return CONTROL_ZONE_TYPE_NONE;
}

int parse_config(threfd_config_ctrl_block_t* cfg_CB) {
    if (!cfg_CB) {
        g_log_structured(G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "MESSAGE", "Failed to open config file with specified path");
        return 1;
    }

    struct json_object *root = json_object_from_file(cfg_CB->path);
    struct json_object *control_zones = NULL;

    if (!root) {
        g_log_structured(G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "MESSAGE", "Failed to open config file with specified path");
        return 1;
    }

    cfg_CB->config.control_zones = g_array_new(FALSE, TRUE, sizeof (control_zone_t));

    if (json_object_object_get_ex(root, "control_zones", &control_zones)) {
        const size_t n_zones = json_object_array_length(control_zones);

        for (size_t i = 0; i < n_zones; i++) {
            const struct json_object *zone_obj = json_object_array_get_idx(control_zones, i);
            struct json_object *id, *type, *temp_max, *temp_min, *path, *pin;
            control_zone_t zone = {0};
            int status = 1;

            status &= json_object_object_get_ex(zone_obj, "id", &id);
            status &= json_object_object_get_ex(zone_obj, "type", &type);
            status &= json_object_object_get_ex(zone_obj, "temp_max", &temp_max);
            status &= json_object_object_get_ex(zone_obj, "temp_min", &temp_min);
            status &= json_object_object_get_ex(zone_obj, "path", &path);
            status &= json_object_object_get_ex(zone_obj, "pin", &pin);

            if (!status) {
                g_log_structured(G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "MESSAGE", "Failed parse one of control zones");
                return 3;
            }

            zone.id = json_object_get_int(id);
            zone.temp_max = (float)json_object_get_double(temp_max);
            zone.temp_min = (float)json_object_get_double(temp_min);

            // ===== Parse type
            const char* type_str = json_object_get_string(type);
            zone.type = parse_control_zone_type(type_str);

            if (!zone.type) {
                g_log_structured(G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "MESSAGE", "Specified control zone type does not exist");
                return 4;
            }
            // =====

            // ===== Parse path
            const char* path_str = json_object_get_string(path);
            strcpy(zone.path, path_str);

            if (access(zone.path, F_OK) != 0) {
                g_log_structured(G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "MESSAGE", "Specified control zone path does not exist");
                return 5;
            }
            // =====

            // ===== Parse pin
            const char* pin_str = json_object_get_string(pin);
            status = parse_sunxi_gpio(pin_str, &zone.pin);

            if (status != 0) {
                g_log_structured(G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "MESSAGE", "Specified control zone pin does not exist");
                return 6;
            }
            // =====

            g_array_append_val(cfg_CB->config.control_zones, zone);
        }
    } else {
        g_log_structured(G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "MESSAGE", "Failed find any control zone in config");
        return 2;
    }

    if (!cfg_CB->config.control_zones->len) {
        g_log_structured(G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "MESSAGE", "Failed find any control zone in config");
        return 2;
    }

    // ===== Update modify timestamp
    struct stat config_stat;

    if (stat(cfg_CB->path, &config_stat) == 0) {
        cfg_CB->modify_timestamp = config_stat.st_mtime;
    } else {
        return 7;
    }
    // =====

    g_log_structured(G_LOG_DOMAIN, G_LOG_LEVEL_MESSAGE, "MESSAGE", "Parsed config with specified path");

    return 0;
}

void config_clear(const threfd_config_t* config) {
    g_array_free(config->control_zones, TRUE);
}
