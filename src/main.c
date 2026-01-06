#include <glib.h>
#include <glib-unix.h>
#include <systemd/sd-daemon.h>
#include "tasks.h"
#include "log.h"
#include "gpio.h"
#include "config.h"

static gboolean signal_handler(gpointer user_data) {
	GMainLoop *loop = (GMainLoop *)user_data;

	g_log_structured(G_LOG_DOMAIN, G_LOG_LEVEL_MESSAGE, "MESSAGE", "SIGTERM/SIGINT received, stopping main loop...");

	g_main_loop_quit(loop);

	return G_SOURCE_REMOVE;
}

int main()
{
	int status = 0;
	GMainLoop *loop = g_main_loop_new(NULL, FALSE);

	// ===== Init config CB
	strcpy(app_config_CB.path, DEFAULT_CONFIG_PATH);
	// =====

	g_unix_signal_add(SIGINT, signal_handler, loop);
	g_unix_signal_add(SIGTERM, signal_handler, loop);

	// Init GPIO lib (WiringOP)
	if (wiringPiSetupGpio() == -1) {
		g_log_structured(G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "MESSAGE", "Failed to init GPIO library");
		return 1;
	}

	status = parse_config(&app_config_CB);
	if (status != 0) {
		return 1;
	}

	set_pins_modes(app_config_CB.config.control_zones);
	add_tasks_to_loop();

	g_log_structured(G_LOG_DOMAIN, G_LOG_LEVEL_MESSAGE, "MESSAGE", "Launching main loop...");
	sd_notify(0, "READY=1");

	g_main_loop_run(loop);

	g_main_loop_unref(loop);

	// ===== Cleanup
	config_clear(&app_config_CB.config);
	// =====

	return 0;
}
