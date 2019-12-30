
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "admin.h"
#include "airport.h"
#include "anticheat.h"
#include "class.h"
#include "changepassword.h"
#include "dialog.h"
#include "echo.h"
#include "game_sa.h"
#include "guestregister.h"
#include "heartbeat.h"
#include "login.h"
#include "maps.h"
#include "missions.h"
#include "money.h"
#include "nav.h"
#include "objects.h"
#include "panel.h"
#include "playerdata.h"
#include "playerstats.h"
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

AMX *amx;
struct FAKEAMX fakeamx;

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
#error replace this with time/time.h stuff instead of native call
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
		playerstats_check_for_afk();

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
	REGISTERNATIVE(B_OnQueryError),
	REGISTERNATIVE(B_OnRecv),
	REGISTERNATIVE(B_OnVehicleSpawn),
	REGISTERNATIVE(B_OnVehicleStreamIn),
	REGISTERNATIVE(B_OnVehicleStreamOut),
	{0, 0}
};

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *a)
{
	AMX_HEADER *hdr;
	int tmp;

	/*only the gamemode should reference B_OnGameModeInit*/
	if (amx_FindNative(a, "B_OnGameModeInit", &tmp) == AMX_ERR_NONE) {
		amx = a;
		fakeamx.emptystring = 0;
		fakeamx.underscorestring[0] = '_';
		fakeamx.underscorestring[1] = 0;

		/*relocate the data segment*/
		hdr = (AMX_HEADER*) amx->base;
		tmp = hdr->hea - hdr->dat;
		if (tmp) {
			/*data section will be cleared so whoever references it
			will probably overwrite our precious data*/
			logprintf("WARN: data section is not empty! (%d)", tmp);
		}
		tmp = hdr->stp - hdr->hea - STACK_HEAP_SIZE * sizeof(cell);
		if (tmp) {
			/*the value given to #pragma dynamic in the script
			should correspond to STACK_HEAP_SIZE*/
			logprintf("ERR: stack/heap size mismatch! "
				"(excess %d cells)",
				tmp / sizeof(cell));
			return 0;
		}
		/*copy content of stack/heap to our data segment (this is most
		likely empty, but better safe than sorry)*/
		memcpy(fakeamx._stackheap,
			amx->base + hdr->hea,
			STACK_HEAP_SIZE * sizeof(cell));
		/*finally do the relocation*/
		amx->data = (unsigned char*) &fakeamx._dat;
		/*adjust pointers since the data segment grew*/
		tmp = (char*) &fakeamx._stackheap - (char*) &fakeamx._dat;
		amx->frm += tmp;
		amx->hea += tmp;
		amx->hlw += tmp;
		amx->stk += tmp;
		amx->stp += tmp;
		/*samp core doesn't seem to use amx_SetString or amx_GetString
		so the data offset needs to be adjusted*/
		/*linux binary seems to have assertions enabled, so the code
		segment also needs to be relocated, yippie*/
		/*(it might be enough to just change the cod value to make the
		assertion `amx->cip >= 4 && amx->cip < (hdr->dat - hdr->cod)`
		pass, but just copy it as a whole for now)*/
		tmp = hdr->dat - hdr->cod;
		hdr->dat = (int) &fakeamx._dat - (int) hdr;
		if (tmp > sizeof(fakeamx._cod)) {
			logprintf("ERR: too small code segment! "
				"(missing %d bytes)",
				tmp - sizeof(fakeamx._cod));
			return 0;
		} else if (sizeof(fakeamx._cod) > tmp) {
			logprintf("INFO: large code segment "
				"(excess %d bytes)",
				sizeof(fakeamx._cod) - tmp);
		}
		memcpy(fakeamx._cod, amx->base + hdr->cod, tmp);
		hdr->cod = (int) &fakeamx._cod - (int) hdr;

		/*this will only work if this is the last plugin being loaded,
		if it's not there should be another one in OnGameModeInit!*/
		if (!natives_find()) {
			return 0;
		}

		for (tmp = 0; tmp < MAX_PLAYERS; tmp++) {
			playeronlineflag[tmp] = 0;
		}
		playercount = 0;

		B144("sleep");
		tmp = NC_GetConsoleVarAsInt(buf144a);
		if (tmp != 5) {
			logprintf("ERR: sleep value %d should be 5", tmp);
			return 0;
		}

		B144("maxplayers");
		tmp = NC_GetConsoleVarAsInt(buf144a);
		if (tmp > MAX_PLAYERS) {
			logprintf("ERR: slots (%d > %d)", tmp, MAX_PLAYERS);
			return 0;
		} else if (tmp < MAX_PLAYERS) {
			logprintf("INFO: slots (%d < %d)", tmp, MAX_PLAYERS);
		}
	}
	return amx_Register(a, PluginNatives, -1);
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

