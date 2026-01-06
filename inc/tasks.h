#ifndef TASKS_H
#define TASKS_H

#include <stdbool.h>
#include "glib.h"
#include "config.h"

typedef struct task_control_block {
    gboolean (*core)(gpointer data);
    gpointer arg;
    uint16_t period_ms;
    bool is_initialized;
} task_control_block_t;

void add_tasks_to_loop(void);

#endif //TASKS_H