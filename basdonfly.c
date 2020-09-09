#define VERSION "0.1"



/*define to print airport data when airports are loaded*/
#undef AIRPORT_PRINT_STATS
/*define to print map data when maps are loaded*/
#undef MAPS_PRINT_STATS
/*define to print msg to console each time map streams in/out*/
#undef MAPS_LOG_STREAMING
/*define to print msg when mission point indicator is made/destroyed/available*/
#undef MISSIONS_LOG_POINT_INDICATOR_ALLOC



#include "vendor/SDK/amxplugin.c" /*includes plugincommon.h*/

#define STATIC_ASSERT(E) typedef char __static_assert_[(E)?1:-1]
#define EXPECT_SIZE(S,SIZE) STATIC_ASSERT(sizeof(S)==(SIZE))
#define MEMBER_OFFSET(S,M) ((int) (&(((S*) 0)->M)))

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

#define VEL_MAX (0.66742320819112627986348122866894f)
#define VEL_TO_KPH (VEL_MAX * 270.0f)
#define VEL_TO_KTS (VEL_MAX * 145.0f)
#define VEL_TO_MPS (VEL_TO_KPH / 3.6f) /*(KPH / 3.6)*/
#define VEL_TO_KFPM (VEL_TO_MPS * 3.28084f * 60.0f / 1000.0f) /* K feet/m*/

#define csprintf(DST,FMT,...) atoci(DST,sprintf((char*)DST,FMT,__VA_ARGS__))
#define B144(X) atoc(buf144,X,144)

#ifdef DEV
#define DBG(format,...) sprintf(cbuf64,format,__VA_ARGS__);\
			printf("%s\n", cbuf64);printf("\n");\
			atoc(buf144,cbuf64,144);\
			NC_SendClientMessageToAll(-1,buf144a);
#else
#define DBG(...)
#endif

/*
These are moved to variables float_pinf and float_ninf because these are ints.
#define FLOAT_PINF (0x7F800000)
#define FLOAT_NINF (0xFF800000)
*/
static float float_pinf, float_ninf;
#define M_PI 3.14159265358979323846f
#define M_PI2 1.57079632679489661923f
#define DEG_TO_RAD (M_PI / 180.0f)

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
#define AIRPORT_CODE_LEN (4)

#define APT_FLAG_TOWERED (0x1)

#define RUNWAY_SURFACE_ASPHALT (1)
#define RUNWAY_SURFACE_DIRT (2)
#define RUNWAY_SURFACE_CONCRETE (3)
#define RUNWAY_SURFACE_GRASS (4)
#define RUNWAY_SURFACE_WATER (5)

#define RUNWAY_TYPE_RUNWAY (1)
#define RUNWAY_TYPE_HELIPAD (2)

struct RUNWAY {
	char id[AIRPORT_CODE_LEN];
	float heading, headingr;
	struct vec3 pos;
	int nav;
	/**
	Should be one of the RUNWAY_TYPE_ constants.
	*/
	int type;
};

/**
See {@link missions_get_gethicle_model_msptype_mask} for vehicle assignments
*/
#define NUM_MISSION_TYPES 11
static char *mission_type_names[NUM_MISSION_TYPES] = {
#define MISSION_TYPE_PASSENGER_S 1
	"Passengers (S)",
#define MISSION_TYPE_PASSENGER_M 2
	"Passengers (M)",
#define MISSION_TYPE_PASSENGER_L 4
	"Passengers (L)",
#define MISSION_TYPE_CARGO_S 8
	"Cargo (S)",
#define MISSION_TYPE_CARGO_M 16
	"Cargo (M)",
#define MISSION_TYPE_CARGO_L 32
	"Cargo (L)",
#define MISSION_TYPE_HELI 64
	"Heli",
#define MISSION_TYPE_HELI_CARGO 128
	"Heli (cargo)",
#define MISSION_TYPE_MIL_HELI 256
	"Heli (military)",
#define MISSION_TYPE_MIL 512
	"Military",
#define MISSION_TYPE_PASSENGER_WATER 1024
	"Passengers (water)",
/*
TODO
#define MISSION_TYPE_STUNT 2048
#define MISSION_TYPE_CROPD 4096
#define MISSION_TYPE_AWACS 8192
*/
/*TODO cargo water*/
};

/*Amount of preset mission locations each mission point should have (so excluding the last one, which is 'random destination'.*/
#define NUM_PRESET_MISSION_LOCATIONS (8)

#define PASSENGER_MISSIONTYPES \
	(MISSION_TYPE_PASSENGER_S | MISSION_TYPE_PASSENGER_M | \
	MISSION_TYPE_PASSENGER_L | MISSION_TYPE_PASSENGER_WATER | \
	MISSION_TYPE_HELI)

#define CARGO_MISSIONTYPES \
	(MISSION_TYPE_CARGO_S | MISSION_TYPE_CARGO_M | MISSION_TYPE_CARGO_L | MISSION_TYPE_HELI_CARGO)

/*all heli missions, including cargo*/
#define HELI_MISSIONTYPES \
	(MISSION_TYPE_HELI | MISSION_TYPE_HELI_CARGO | MISSION_TYPE_MIL_HELI)
#define MIL_MISSIONTYPES \
	(MISSION_TYPE_MIL | MISSION_TYPE_MIL_HELI)

/*excluding zero term*/
#define MAX_MSP_NAME (9)

/**
The point type, decides the enex color and related things.
*/
#define MISSION_POINT_PASSENGERS 1
#define MISSION_POINT_CARGO 2
#define MISSION_POINT_SPECIAL 4

#define MAX_MISSIONPOINTS_PER_AIRPORT 15

struct MISSIONPOINT {
	unsigned short id;
	struct vec3 pos;
	unsigned int type;
	/**
	See MISSION_POINT_*, decides the enex color and related things.
	*/
	char point_type;
	char name[MAX_MSP_NAME + 1];
	struct AIRPORT *ap;
	/**
	The last one is actually unused because it's the 'random' destination.
	*/
	struct MISSIONPOINT *missionlocations[NUM_PRESET_MISSION_LOCATIONS];
	/**
	Mission type is {@code 1 << this}
	*/
	int missiontypeindices[NUM_PRESET_MISSION_LOCATIONS];
};

static struct MISSIONPOINT *missionpoints;
static int nummissionpoints;

#define MAX_AIRPORTS 20

struct AIRPORT {
	int id;
	struct vec3 pos;
	char code[4 + 1];
	char enabled;
	char name[MAX_AIRPORT_NAME + 1];
	/**
	OR'd value of all the types of all the missionpoints
	*/
	int missiontypes;
	int flags;
	struct RUNWAY *runways, *runwaysend;
	struct MISSIONPOINT *missionpoints;
	int num_missionpts;
};

static struct AIRPORT *airports;
static int numairports;

/*prefs*/

#define PREF_ENABLE_PM 1
#define PREF_SHOW_MISSION_MSGS 2
#define PREF_CONSTANT_WORK 4
#define PREF_WORK_AUTONAV 8

#define DEFAULTPREFS (PREF_ENABLE_PM | PREF_SHOW_MISSION_MSGS | PREF_WORK_AUTONAV)

/*randomstuff*/

#define KEY_JUST_DOWN(X) (!(oldkeys & X) && (newkeys & X))

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

#include "idalloc.h"
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
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#define __USE_MISC /*for getpagesize()*/
#include <unistd.h>
#undef __USE_MISC

static void zones_update(int playerid, struct vec3 pos);
static void missions_update_missionpoint_indicators(int playerid, float player_x, float player_y, float player_z);
static int missions_get_vehicle_model_msptype_mask(int model);

static struct BitStream bitstream_create_object;
/**
Reset no_camera_col(0), attached_vehicle(-1), attached_object(-1) after use.
*/
static struct RPCDATA_CreateObject rpcdata_CreateObject;
static struct BitStream bitstream_destroy_object;
static struct RPCDATA_DestroyObject rpcdata_DestroyObject;

#include "memstuff.c"
#include "samp.c"
#include "textdraws.c"
#include "natives.c"
#include "natives_funcs.c"
#include "common.c"
#include "cmd_utils.c"
#include "time.c"
#include "money.c"
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
#include "servicepoints.c"
#include "spawn.c"
#include "timecyc.c"
#include "timer.c"
#include "vehicles.c"

#include "cmdhandlers.c"
#ifdef DEV
#include "cmdhandlers_dev.c"
#endif /*DEV*/

#include "cmd.c"
#include "basdonfly_callbacks.c"
#include "samphooks.c"

#define NATIVES_IMPL
#include "natives_funcs.c"

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}

PLUGIN_EXPORT int PLUGIN_CALL Load(void **ppData)
{
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t) ppData[PLUGIN_DATA_LOGPRINTF];

	*(int*) &float_pinf = 0x7F800000;
	*(int*) &float_ninf = 0xFF800000;

	bitstream_freeform.copyData = 1;

	rpcdata_CreateObject.no_camera_col = 0;
	rpcdata_CreateObject.attached_object = -1;
	rpcdata_CreateObject.attached_vehicle = -1;

	bitstream_create_object.numberOfBitsUsed = sizeof(rpcdata_CreateObject) * 8;
	bitstream_create_object.ptrData = &rpcdata_CreateObject;
	bitstream_create_object.copyData = 1;

	bitstream_destroy_object.numberOfBitsUsed = sizeof(rpcdata_DestroyObject) * 8;
	bitstream_destroy_object.ptrData = &rpcdata_DestroyObject;
	bitstream_destroy_object.copyData = 1;

	samphooks_init();
	textdraws_init();

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
#if DEV
	/*Getting here means the server is shutting down. In dev, it should be restarted immediately.
	But when the server exits, it kicks all the players, and I'd have to restart the game, nogood.
	So just crash the server here so it doesn't kick me :).
	TODO: find a better way, preferably one that is even more graceful than the 'lost connection' process.*/
	assert(((void) "crashing lololol", 0));
#endif
	return AMX_ERR_NONE;
}
