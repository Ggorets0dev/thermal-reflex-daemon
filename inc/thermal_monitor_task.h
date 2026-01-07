#ifndef THERMAL_MONITOR_TASK_H
#define THERMAL_MONITOR_TASK_H

#include <glib.h>

/**
 * @brief Task for temperature monitoring
 * @param data Pointer to arg
 * @return State of task (glib)
*/
gboolean monitor_temp_task(gpointer data);

#endif //THERMAL_MONITOR_TASK_H