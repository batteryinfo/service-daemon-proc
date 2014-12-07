#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include <time.h>
#include <sys/time.h>
//#include <FBase.h>
//#include <FIo.h>
//#include <FApp.h>


#define LOG_TAG		"SAMPLE" //DLOG Name
#define DLOG_ERR		DLOG_ERROR
#define PRT_ERR(fmt, arg...) \
	do { SLOG(LOG_ERR, LOG_TAG, fmt, ##arg); } while (0)

#define SAMPLE_SOCKET_PATH "/tmp/sample"


#define xxx memory/pm/current_brt

#define DEVICE_ON	2	
#define DEVICE_OFF	0

struct tm *gpstTime;
//clock_t gstart, gend;
struct timeval gstart, gend;

typedef enum
{
     eWIFI,
     eLCD,
     eBATTERY, 
     eDEVICE_CNT
} DEVICE_TYPE;

unsigned char *gpaDeviceStateFile[eDEVICE_CNT] = {
    "/home/developer/WIFI_State.txt",
    "/home/developer/LCD_State.txt",
    "/home/developer/BATTERY_State.txt"
};

unsigned int _get_diff_second()
{
	unsigned int ret;
/*
	time_t nSec = time(NULL);
	struct tm *stTimeData = localtime(&nSec);

	PRT_ERR("_get_diff_second (%d)",ret);

	if (stTimeData->tm_year > gpstTime->tm_year)
	{
		ret = (stTimeData->tm_year - gpstTime->tm_year) * 60 * 60 * 60 * 60 * 60;
		PRT_ERR("year second (%d)",ret);
	}

	if (stTimeData->tm_mon > gpstTime->tm_mon)
	{
		ret += (stTimeData->tm_mon - gpstTime->tm_mon) * 60 * 60 * 60 * 60;
		PRT_ERR("mon second (%d)",ret);
	}

	if (stTimeData->tm_mday > gpstTime->tm_mday)
	{
		ret += (stTimeData->tm_mday - gpstTime->tm_mday) * 60 * 60 * 60;
		PRT_ERR("mday second (%d)",ret);
	}

	if (stTimeData->tm_hour > gpstTime->tm_hour)
	{
		ret += (stTimeData->tm_hour - gpstTime->tm_hour) * 60 * 60;
		PRT_ERR("hour second (%d)",ret);
	}

	if (stTimeData->tm_min > gpstTime->tm_min)
	{
		ret += (stTimeData->tm_min - gpstTime->tm_min) * 60;
		PRT_ERR("min second (%d)",ret);
	}

	if (stTimeData->tm_sec > gpstTime->tm_sec)
	{
		ret += (stTimeData->tm_sec - gpstTime->tm_sec);
		PRT_ERR("sec second (%d)",ret);
	}

	PRT_ERR("old second (%d)",gpstTime->tm_sec);
	PRT_ERR("new second (%d)",stTimeData->tm_sec);
*/
	gettimeofday(&gend, NULL);
	
	ret = (gend.tv_sec - gstart.tv_sec);

	return ret;
}

bool _store_info_to_file(DEVICE_TYPE eDeviceType, int val)
{
	FILE *fpDeviceState;
	time_t current_time;
	unsigned int nsecond;

	PRT_ERR("SAMPLE : _store_info_to_file\n");

	fpDeviceState = fopen(gpaDeviceStateFile[eDeviceType], "a+");

	if (fpDeviceState == NULL)
	{
		PRT_ERR("SAMPLE : can not open file\n");
		return false;
	}

	//time(&current_time);
	nsecond = _get_diff_second();

	fprintf(fpDeviceState, "%ld, %d\n", nsecond, val);

	fclose(fpDeviceState);

	PRT_ERR("SAMPLE : file closed\n");

	return true;

}

void _set_device_info(DEVICE_TYPE eDeviceType, int val)
{
	bool bstorefile = false;	

	PRT_ERR("_set_device_info\n");

	_store_info_to_file(eDeviceType, val);
}

static void _notify_wifi_state_cb(keynode_t * node, void *data)
{

	int val = -1;

	PRT_ERR("wifi call back function is called(%d)",val);

	if (vconf_get_int(VCONFKEY_WIFI_STATE, &val) < 0) {
		PRT_ERR("SAMPLE : Cannot get WIFI_STATE");
		return;
	}

	switch(val)
	{
		case VCONFKEY_WIFI_OFF:
			val = DEVICE_OFF;
			break;
		case VCONFKEY_WIFI_CONNECTED:
			val = DEVICE_ON;
			break;
		default:
			break;
	}

	PRT_ERR("WIFI state Notification!!(%d)",val);

	_set_device_info(eWIFI, val);
}

static void _notify_battery_state_cb(keynode_t * node, void *data)
{

	int val = -1;

	PRT_ERR("batter call back function is called(%d)",val);

	if (vconf_get_int(VCONFKEY_SYSMAN_BATTERY_CAPACITY, &val) < 0) {
		PRT_ERR("Cannot get VCONFKEY_SYSMAN_BATTERY_CAPACITY");
		return;
	}

	PRT_ERR("battery state Notification!!(%d)",val);

	_set_device_info(eBATTERY, val);
}

static void _notify_pm_state_cb(keynode_t * node, void *data)
{

	int val = -1;

	if (vconf_get_int(VCONFKEY_PM_STATE, &val) < 0) {
		PRT_ERR("Cannot get VCONFKEY_PM_STATE");
		return;
	}

	PRT_ERR("PM state Notification!!(%d)",val);

	switch(val)
	{
		case VCONFKEY_PM_STATE_LCDOFF:
		case VCONFKEY_PM_STATE_SLEEP:
			val = DEVICE_OFF;
			break;
		case VCONFKEY_PM_STATE_NORMAL:
		case VCONFKEY_PM_STATE_LCDDIM:
			val = DEVICE_ON;
			break;
		default:
			break;
	}

	_set_device_info(eLCD, val);
}

static void register_event_handler(void)
{
/*
	int ret;
	time_t nSec = time(NULL);
	
	PRT_ERR("SAMPLE : VCONFKEY_PM_STATE Init");
	
	gpstTime = localtime(&nSec);
*/

	gettimeofday(&gstart, NULL);

	if (vconf_notify_key_changed
	    (VCONFKEY_PM_STATE, _notify_pm_state_cb, NULL) != 0) {
		PRT_ERR("SAMPLE : Fail vconf_notify_key_changed : VCONFKEY_PM_STATE");
	}

	if (vconf_notify_key_changed
	    (VCONFKEY_WIFI_STATE, _notify_wifi_state_cb, NULL) != 0) {
		PRT_ERR("SAMPLE : Fail vconf_notify_key_changed : WIFI_STATE");
	}

	if (vconf_notify_key_changed
	    (VCONFKEY_SYSMAN_BATTERY_CAPACITY, _notify_battery_state_cb, NULL) != 0) {
		PRT_ERR("SAMPLE : Fail vconf_notify_key_changed : VCONFKEY_SYSMAN_BATTERY_CAPACITY");
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

