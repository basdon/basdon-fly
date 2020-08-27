#define VERSION "0.1"

#include "vendor/SDK/amxplugin.c" /*includes plugincommon.h*/

#define STATIC_ASSERT(E) typedef char __static_assert_[(E)?1:-1]
#define EXPECT_SIZE(S,SIZE) STATIC_ASSERT(sizeof(S)==(SIZE))

typedef void (*cb_t)(void* data);
typedef void (*logprintf_t)(char* format, ...);
static logprintf_t logprintf;

EXPECT_SIZE(char, 1);
EXPECT_SIZE(short, 2);
EXPECT_SIZE(int, 4);
EXPECT_SIZE(cell, 4);
EXPECT_SIZE(float, 4);

#define SLEEP (5)
#define JOINPRESSURE_INC (SLEEP * 200)
#define JOINPRESSURE_DEC (SLEEP)
#define JOINPRESSURE_MAX (JOINPRESSURE_INC * 4)
#define JOINPRESSURE_SLOWMODE_LEN (25000)
#define JOINPRESSURE_SLOWMODE_MINCONNECTIONTIME (1200)

struct vec3 {
	float x, y, z;
};
EXPECT_SIZE(struct vec3, 3 * sizeof(cell));

struct vec4 {
	struct vec3 coords;
	float r;
};
EXPECT_SIZE(struct vec4, 4 * sizeof(cell));

struct quat {
	float qx, qy, qz, qw;
};
EXPECT_SIZE(struct quat, 4 * sizeof(cell));

#include "a_samp.h"
#include "samp.h"

/*airportstuff*/

#define MAX_AIRPORT_NAME (24)

#define APT_FLAG_TOWERED (0x1)

#define RUNWAY_SURFACE_ASPHALT (1)
#define RUNWAY_SURFACE_DIRT (2)
#define RUNWAY_SURFACE_CONCRETE (3)
#define RUNWAY_SURFACE_GRASS (4)
#define RUNWAY_SURFACE_WATER (5)

#define RUNWAY_TYPE_RUNWAY (1)
#define RUNWAY_TYPE_HELIPAD (2)

struct RUNWAY {
	char id[4];
	float heading, headingr;
	struct vec3 pos;
	int nav;
	/**
	Should be one of the RUNWAY_TYPE_ constants.
	*/
	int type;
};

struct AIRPORT {
	int id;
	struct vec3 pos;
	char code[4 + 1];
	char enabled;
	char name[MAX_AIRPORT_NAME + 1];
	int missiontypes;
	int flags;
	struct RUNWAY *runways, *runwaysend;
	struct MISSIONPOINT *missionpoints;
	int num_missionpts;
};

/*prefs*/

#define PREF_ENABLE_PM 1
#define PREF_SHOW_MISSION_MSGS 2
#define PREF_CONSTANT_WORK 4
#define PREF_WORK_AUTONAV 8

#define DEFAULTPREFS (PREF_ENABLE_PM | PREF_SHOW_MISSION_MSGS | PREF_WORK_AUTONAV)

/*randomstuff*/

/**
Connection count per player id. Incremented every time playerid connects.

Used to check if a player is still valid between long-running tasks.
*/
static int _cc[MAX_PLAYERS];
static int joinpressure = 0;
static int minconnectiontime = 0;

/**
amx
*/
static AMX *amx;
/**
amx data segment
*/
static struct FAKEAMX fakeamx;

#include "allocations.h"
#include "samphost.h"
#include "common.h"
#include "admin.h"
#include "anticheat.h"
#include "class.h"
#include "changepassword.h"
#include "echo.h"
#include "game_sa.h"
#include "guestregister.h"
#include "login.h"
#include "maps.h"
#include "missions.h"
#include "nav.h"
#include "panel.h"
#include "playerdata.h"
#include "playerstats.h"
#include "servicepoints.h"
#include "spawn.h"
#include "timecyc.h"
#include "timer.h"
#include "vehicles.h"
#include "kneeboard.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void zones_update(int playerid, struct vec3 pos);

#include "cmd_utils.c"
#include "natives.c"
#include "time.c"
#include "money.c"
#include "common.c"
#include "dialog.c"
#include "prefs.c"
#include "airport.c"
#include "protips.c"

#include "score.c"
#include "admin.c"
#include "zones_data.c"
#include "zones.c"
#include "anticheat.c"
#include "changepassword.c"
#include "class.c"
#include "cmdhandlers.c"
#ifdef DEV
#include "cmdhandlers_dev.c"
#endif /*DEV*/
#include "echo.c"
#include "game_sa.c"
#include "game_sa_data.c"
#include "login.c"
#include "guestregister.c"
#include "heartbeat.c"
#include "kneeboard.c"
#include "maps.c"
#include "missions.c"
#include "nav.c"
#include "panel.c"
#include "playerdata.c"
#include "playerstats.c"
#include "pm.c"
#include "samp.c"
#include "servicepoints.c"
#include "spawn.c"
#include "timecyc.c"
#include "timer.c"
#include "vehicles.c"

#include "cmd.c"
#include "basdonfly_callbacks.c"

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

	if (joinpressure > 0) {
		joinpressure -= JOINPRESSURE_DEC;
	}

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
		veh_timed_speedo_update();
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
	REGISTERNATIVE(B_OnIncomingConnection),
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
		if (tmp != SLEEP) {
			logprintf(
				"ERR: sleep value %d should be %d",
				tmp,
				SLEEP);
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

