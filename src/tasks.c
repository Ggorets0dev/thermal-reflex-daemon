#include <sys/stat.h>
#include "tasks.h"

#include <systemd/sd-daemon.h>

#include "thermal_monitor_task.h"

#define TASKS_COUNT     (2)

// ==============
// Static variables
// ==============
static task_control_block_t app_tasks[TASKS_COUNT];
// ==============

void init_tasks(void) __attribute__((constructor));

static gboolean watch_dog_task(gpointer data __attribute__((unused))) {
    sd_notify(0, "WATCHDOG=1");
    return G_SOURCE_CONTINUE;
}

void init_tasks(void) {
    // ======= WatchDog task
    uint64_t usec = 0;
    const int enabled = sd_watchdog_enabled(0, &usec);

    if (enabled > 0) {
        app_tasks[0].core = watch_dog_task;
        app_tasks[0].period_ms = usec / (1000 * 2);
        app_tasks[0].arg = NULL;
        app_tasks[0].is_initialized = true;
    }
    // =======

    // ======= Thermal monitor task
    app_tasks[1].core = monitor_temp_task;
    app_tasks[1].period_ms = 1000;
    app_tasks[1].arg = &app_config_CB;
    app_tasks[1].is_initialized = true;
    // =======
}

void add_tasks_to_loop(void) {
    for (int i = 0; i < TASKS_COUNT; i++) {
        if (!app_tasks[i].is_initialized) {
            // Not initialized task control block
            continue;
        }

        g_timeout_add(app_tasks[i].period_ms, app_tasks[i].core, app_tasks[i].arg);
    }
}