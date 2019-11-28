
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "airport.h"
#include "class.h"
#include "anticheat.h"
#include "dialog.h"
#include "echo.h"
#include "game_sa.h"
#include "heartbeat.h"
#include "login.h"
#include "maps.h"
#include "missions.h"
#include "money.h"
#include "nav.h"
#include "objects.h"
#include "panel.h"
#include "playerdata.h"
#include "playtime.h"
#include "pm.h"
#include "prefs.h"
#include "protips.h"
#include "servicepoints.h"
#include "score.h"
#include "spawn.h"
#include "timecyc.h"
#include "timer.h"
#include "vehicles.h"
#include "zones.h"
#include "time/time.h"
#include <stdio.h>
#include <string.h>

logprintf_t logprintf;
extern void *pAMXFunctions;

EXPECT_SIZE(char, 1);
EXPECT_SIZE(short, 2);
EXPECT_SIZE(int, 4);
EXPECT_SIZE(cell, 4);
EXPECT_SIZE(float, 4);

int _cc[MAX_PLAYERS];

#define IN_BASDONFLY
#include "basdon.c"

#ifndef PRECOMPILED_TIME
#include "time/time.c"
#endif /*PRECOMPILED_TIME*/

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}

PLUGIN_EXPORT int PLUGIN_CALL Load(void **ppData)
{
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t) ppData[PLUGIN_DATA_LOGPRINTF];

	cmd_init();
	game_sa_init();
	nav_init();
	pdata_init();
	time_init();
	zones_init();

	return 1;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
}

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick()
{
	static int count = 0;
#ifdef LOG_SLOW_TICKS
	static int tickcount = 0;
	int newtickcount;
#endif /*LOG_SLOW_TICKS*/

	/*occurs when doing gmx*/
	if (amx == NULL) {
		count = 0;
		return;
	}

	timer_tick();

	if (count++ >= 19) {
		count = 0;

#ifdef LOG_SLOW_TICKS
		newtickcount = NC_tickcount();
		if (tickcount && newtickcount - tickcount > 120) {
			logprintf("slow 20 ticks %d", newtickcount - tickcount);
		}
		tickcount = newtickcount;
#endif /*LOG_SLOW_TICKS*/

		/*timer100*/
		anticheat_decrease_flood();
		maps_process_tick();
#ifdef DEV
		dev_missions_update_closest_point();
#endif /*DEV*/
		panel_timed_update();
		playtime_check_for_afk();

		timecyc_tick();
	}
}
#undef amx

#define REGISTERNATIVE(X) {#X, X}
AMX_NATIVE_INFO PluginNatives[] =
{
	REGISTERNATIVE(B_OnCallbackHit),
	REGISTERNATIVE(B_OnDialogResponse),
	REGISTERNATIVE(B_OnGameModeExit),
	REGISTERNATIVE(B_OnGameModeInit),
	REGISTERNATIVE(B_OnPlayerCommandText),
	REGISTERNATIVE(B_OnPlayerConnect),
	REGISTERNATIVE(B_OnPlayerDeath),
	REGISTERNATIVE(B_OnPlayerDisconnect),
	REGISTERNATIVE(B_OnPlayerEnterRaceCP),
	REGISTERNATIVE(B_OnPlayerEnterVehicle),
	REGISTERNATIVE(B_OnPlayerKeyStateChange),
	REGISTERNATIVE(B_OnPlayerRequestClass),
	REGISTERNATIVE(B_OnPlayerRequestSpawn),
	REGISTERNATIVE(B_OnPlayerSpawn),
	REGISTERNATIVE(B_OnPlayerStateChange),
	REGISTERNATIVE(B_OnPlayerText),
	REGISTERNATIVE(B_OnPlayerUpdate),
	REGISTERNATIVE(B_OnRecv),
	REGISTERNATIVE(B_OnVehicleSpawn),
	REGISTERNATIVE(B_OnVehicleStreamIn),
	REGISTERNATIVE(B_OnVehicleStreamOut),
	REGISTERNATIVE(B_Validate),
	{0, 0}
};

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx)
{
	return amx_Register(amx, PluginNatives, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *a)
{
	if (amx == a) {
		amx = NULL;
		/*Most likely doing gmx for update, so the server needs
		to restart (exit and start) to copy and load the new plugin.*/
		buf32[0] = 'e'; buf32[1] = 'x'; buf32[2] = 'i'; buf32[3] = 't';
		buf32[4] = 0;
		NC_SendRconCommand(buf32a);
		/*TODO this actually crashes the server somewhere. Although the
		result is the same, it would be nice if it didn't crash :)*/
	}
	return AMX_ERR_NONE;
}

