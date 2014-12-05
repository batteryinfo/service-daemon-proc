#include <stdio.h>
#include <dlog.h>
#include <app.h>
#include <Ecore_X.h>
#include <vconf.h>
#include <unistd.h>
#include <privilege-control.h>
#include <app_manager.h>
#include <signal.h>
//#include <minicontrol-monitor.h>
//#include <feedback.h>


#define LOG_TAG		"SAMPLE" //DLOG Name
#define DLOG_ERR		DLOG_ERROR
#define PRT_ERR(fmt, arg...) \
	do { SLOG(LOG_ERR, LOG_TAG, fmt, ##arg); } while (0)

#define SAMPLE_SOCKET_PATH "/tmp/sample"


#define xxx memory/pm/current_brt

static void _notify_pm_state_cb(keynode_t * node, void *data)
{

	int val = -1;

/*
	if (data == NULL) {
		PRT_ERR("lockd is NULL");
		return;
	}
*/
	if (vconf_get_int(VCONFKEY_PM_STATE, &val) < 0) {
		PRT_ERR("Cannot get VCONFKEY_PM_STATE");
		return;
	}

	PRT_ERR("PM state Notification!!(%d)",val);

	switch(val)
	{
		case VCONFKEY_PM_STATE_LCDOFF:
		case VCONFKEY_PM_STATE_SLEEP:
			//indicator_util_set_update_flag(0);
			break;
		case VCONFKEY_PM_STATE_NORMAL:
			//indicator_util_set_update_flag(1);
			//indicator_wake_up_modules(data);
			break;
		case VCONFKEY_PM_STATE_LCDDIM:
		default:
			break;
	}

}

static void register_event_handler(void)
{
	int ret;
	
	//ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_BATTERY_PERCENTAGE_BOOL, _indicator_check_battery_percent_on_cb, (void *)ad);
	//memory/sysman/battery_capacity, value = 100 (int)
	//Can get battert capacity

	PRT_ERR("VCONFKEY_PM_STATE Init");

	if (vconf_notify_key_changed
	    (VCONFKEY_PM_STATE, _notify_pm_state_cb, NULL) != 0) {
		PRT_ERR("Fail vconf_notify_key_changed : VCONFKEY_PM_STATE");
	}

}


/*
return vconf_get_int(VCONFKEY_SYSMAN_BATTERY_CHARGE_NOW, val);

int get_setting_brightness(int *level)
{
	return vconf_get_int(VCONFKEY_SETAPPL_LCD_BRIGHTNESS, level);
}
*/


int sample_vconf_init(void){
 register_event_handler();
return 0;
}

