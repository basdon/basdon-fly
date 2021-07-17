/*define to print airport data when airports are loaded*/
#undef AIRPORT_PRINT_STATS
/*define to print map data when maps are loaded*/
#undef MAPS_PRINT_STATS
/*define to print msg to console each time map streams in/out*/
#undef MAPS_LOG_STREAMING
/*define to print msg when mission point indicator is made/destroyed/available*/
#undef MISSIONS_LOG_POINT_INDICATOR_ALLOC
/*define to test panel updates/cache busting*/
#undef PANEL_PRINT_UPDATES
/*define to test vor updates/cache busting*/
#undef VOR_PRINT_UPDATES
/*define to test ils updates/cache busting*/
#undef ILS_PRINT_UPDATES
/*define to test vehicle owner label allocation*/
#undef VEHICLE_PRINT_OWNER_LABEL_ALLOCATIONS



#include "vendor/SDK/amxplugin.c" /*includes plugincommon.h*/
/*Files starting with __ are generated during build by _preprocess.c,
just run the 'build' file with bash (or 'make plugin' in repo root).*/
#include "__settings.h"

#define STATIC_ASSERT(E) typedef char __static_assert_[(E)?1:-1]
#define EXPECT_SIZE(S,SIZE) STATIC_ASSERT(sizeof(S)==(SIZE))
/*May also use offsetof(), but that requires stddef.h*/
#define MEMBER_OFFSET(S,M) (__builtin_offsetof(S,M))
#define STATIC_ASSERT_MEMBER_OFFSET(S,M,O) STATIC_ASSERT(MEMBER_OFFSET(S,M)==(O))

typedef void (*cb_t)(void* data);

typedef void (*logprintf_t)(char* format, ...);
logprintf_t logprintf;
/*Run logprintf through printf_logprintf when DEV, so it'll also print to terminal.*/
#ifdef DEV
#define logprintf printf_logprintf
#endif

EXPECT_SIZE(char, 1);
EXPECT_SIZE(short, 2);
EXPECT_SIZE(int, 4);
EXPECT_SIZE(cell, 4);
EXPECT_SIZE(float, 4);

#define PW_HASH_LENGTH (65) /*Including zero term. This is for bcrypt.*/
#define SHA256BUFSIZE (65) /*Including zero term as well.*/

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
			SendClientMessageToAll(-1,cbuf644);
#else
#define DBG(...)
#endif

/*
These are moved to variables float_pinf and float_ninf because these are ints.
#define FLOAT_PINF (0x7F800000)
#define FLOAT_NINF (0xFF800000)
*/
static float float_pinf, float_ninf;
#define M_2PI 6.28318530717958647692f;
#define M_PI 3.14159265358979323846f
#define M_PI2 1.57079632679489661923f
#define DEG_TO_RAD (M_PI / 180.0f)

struct vec3 {
	float x, y, z;
};
EXPECT_SIZE(struct vec3, 3 * sizeof(cell));

static struct vec3 *vec3_zero = (void*) "\0\0\0\0\0\0\0\0\0\0\0\0";

struct vec4 {
	struct vec3 coords;
	float r;
};
EXPECT_SIZE(struct vec4, 4 * sizeof(cell));

struct quat {
	/*The w component comes first in SAMP, do not change this order.*/
	float qw, qx, qy, qz;
};
EXPECT_SIZE(struct quat, 4 * sizeof(cell));

#include "a_samp.h"
#include "raknet.h"
#include "raknet.c"
#include "samp.h"

/*anticheat*/

#define AC_IF_DISALLOWED_VEHICLE 0
#define INFRACTIONTYPES 1

#define AC_FLOOD_DECLINE_PER_100 3
#define AC_FLOOD_AMOUNT_DIALOG 10
#define AC_FLOOD_AMOUNT_CHAT 30
#define AC_FLOOD_AMOUNT_CMD 30
#define AC_FLOOD_LIMIT 100
#define AC_FLOOD_WARN_THRESHOLD \
	(AC_FLOOD_LIMIT - AC_FLOOD_AMOUNT_CHAT - AC_FLOOD_AMOUNT_CHAT / 2)

/*ac log event type*/
#define AC_GENERIC 0
#define AC_VEH_HP_NAN 1
#define AC_VEH_HP_HIGH 2
#define AC_WRONG_DIALOGID 3
#define AC_UNAUTH_VEHICLE_ACCESS 4
#define AC_FLOOD 5
#define AC_VEH_HP_MISSION_INCREASE 6
#define AC_MISSION_TOOFAST 7
#define AC_DIALOG_SMELLY_INPUT 8
#define AC_DIALOG_LONG_INPUT 9

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
	int dbid;
	char id[AIRPORT_CODE_LEN];
	float heading, headingr;
	struct vec3 pos;
	struct vec3 touchdown_pos;
	unsigned char nav;
	/**
	Should be one of the RUNWAY_TYPE_ constants.
	*/
	int type;
	/**not applicable for helipads*/
	struct RUNWAY *other_end;
	/**not applicable for helipads*/
	unsigned short length;
	/*identifier of the runway, to link two ends together*/
	unsigned char identifier;
};

/**
See /settings.ini and init_mission_type_names
*/
static char *mission_type_names[SETTING__NUM_MISSION_TYPES];

/*excluding zero term*/
#define MAX_MSP_NAME (9)

#define MAX_MISSIONPOINTS_PER_AIRPORT 15

struct MISSIONPOINT {
	unsigned short id;
	struct vec3 pos;
	unsigned int type;
	char name[MAX_MSP_NAME + 1];
	struct AIRPORT *ap;
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

/*login stuff*/

#define LOGGED_NO 0
#define LOGGED_IN 1
#define LOGGED_GUEST 2

#define ISPLAYING(PLAYERID) (loggedstatus[PLAYERID])

#define MONEY_DEFAULT_AMOUNT 15000

/**
User id of player, -1 if no valid session (should be extremely rare)

When user is going through the login process,
use unconfirmed_userid instead (= don't set userid while the user is not logged in).

Starts from 1
*/
static int userid[MAX_PLAYERS];
/**
Player's session id, -1 if no valid session (should be extremely rare)

Starts from 1
*/
static int sessionid[MAX_PLAYERS];
/**
Logged-in status of each player (one of the LOGGED_ definitions).
*/
static int loggedstatus[MAX_PLAYERS];
/**
Some data saved to use in the progress of login in, registering, or changing pw.
TODO: split this for login/register/changepw, let each file have their own
*/
static void *pwdata[MAX_PLAYERS];

/*navs*/

#define NAV_NONE 0
#define NAV_ADF 1
#define NAV_VOR 2
#define NAV_ILS 4

/*prefs*/

#define PREF_ENABLE_PM 1
#define PREF_SHOW_MISSION_MSGS 2
#define PREF_SHOW_GPS 4
#define PREF_SHOW_KNEEBOARD 8
#define PREF_WORK_AUTONAV 16
#define PREF_PANEL_NIGHTCOLORS 32

#define DEFAULTPREFS \
	(PREF_ENABLE_PM | PREF_SHOW_MISSION_MSGS | PREF_SHOW_GPS |\
	PREF_SHOW_KNEEBOARD | PREF_WORK_AUTONAV | PREF_PANEL_NIGHTCOLORS)

static int prefs[MAX_PLAYERS];

/*timecyc*/

static struct {
	unsigned char current;
	unsigned char upcoming;
	unsigned char locked;
	unsigned char _pad;
} weather;
EXPECT_SIZE(weather, 4);

#define SPAWN_WEAPON_1 WEAPON_CAMERA
#define SPAWN_AMMO_1 3036
#define SPAWN_WEAPON_2_3 0
#define SPAWN_AMMO_2_3 0

/*See /settings.ini for class stuff */
static const int CLASS_SKINS[SETTING__NUM_CLASSES] = {
	61,
	61,
	275,
	287,
	287,
};

static char *CLASS_NAMES[SETTING__NUM_CLASSES];

static const int CLASS_COLORS[SETTING__NUM_CLASSES] = {
	0xa86efcff,
	0xfca5ffff,
	0x7087ffff,
	0x519c42ff,
	0xff3740ff,
};

/**
 * Mission types per class, see init_class_msptypes
 */
static unsigned int CLASS_MSPTYPES[SETTING__NUM_CLASSES];
/**
Class id of players (CLASS_ constants).
*/
static int classid[MAX_PLAYERS];

/*randomstuff*/

/*New world boundaries +40k/-40k*/
#define WORLD_XY_MAX 0x471C4000
#define WORLD_XY_MIN 0xC71C4000

#define KEY_JUST_DOWN(X) (!(oldkeys & X) && (newkeys & X))

/**
Connection count per player id. Incremented every time playerid connects.

A db query can be going on while a player disconnects and a new player connects in the same slot,
so a simple IsPlayerConnected check will not be sufficient if the result of the query is still
applicable to the player that holds the playerid. This is the value that should be checked against.
*/
static unsigned short _cc[MAX_PLAYERS];
static int joinpressure = 0;
static int minconnectiontime = 0;

/**
Set as soon as a player is connected.
*/
static char playeronlineflag[MAX_PLAYERS];
#define IsPlayerConnected(PLAYERID) playeronlineflag[PLAYERID]
/**
Contains all playerids that are passed the login screen (not sorted),
so logged in as guest or member (or none or db error).

i.e.: players that are actually playing.

Contains 'playercount' elements
*/
static short players[MAX_PLAYERS];
/**
amount of playerids in players
*/
static int playercount;
static struct SampPlayer *player[MAX_PLAYERS];
static int spawned[MAX_PLAYERS];

/**
The vehicle the player was last in, as driver.
This should be set to 0 at the end of OnPlayerUpdate (so all OnPlayerStateChange functions can use it).
It will be set back to the correct vehicleid at the end of hook_OnDriverSync.
hook_OnDriverSync will notice if player changed vehicle as driver without state changes.
PutPlayerInVehicle will set this if the player is already driver.
When checking if a player is now on foot with OnPlayerStateChange,
one can use this to know what vehicle the player exited.
This is also used to check if a player was put as driver in a vehicle while
they were already driver of another vehicle (does not trigger OnPlayerStateChange).
*/
static int lastvehicle_asdriver[MAX_PLAYERS];

/**
amx
*/
static AMX *amx;
/**
amx data segment
*/
static struct FAKEAMX fakeamx;

/*game things*/
#include "game_sa_data.c"

#define PLANE 0x1
#define HELI 0x2
#define RETRACTABLE_GEAR 0x4

static unsigned int vehicleflags[VEHICLE_MODEL_TOTAL];
/**
Some mapping, every air vehicle has a unique value 0-22, -1 for others
*/
static char aircraftmodelindex[VEHICLE_MODEL_TOTAL];

#include <assert.h>
#include "idalloc.h"
#include "sounds.h"
/*Files starting with __ are generated during build by _preprocess.c,
just run the 'build' file with bash (or 'make plugin' in repo root).*/
#include "__sampasm.h"
#include "common.h"
#include "echo.h"
#include "missions.h"
#include "playerdata.h"
#include "playerstats.h"
#include "vehicles.h"
#include <alloca.h>
#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define __USE_POSIX /*for sigaction and others*/
#include <signal.h>
#undef __USE_POSIX
#include <sys/mman.h>
#include <sys/time.h> /*for gettimeofday() and struct timeval*/
#include <sys/types.h> /*for pid_t*/
#include <time.h>
#define __USE_MISC /*for getpagesize() (dev)*/
#define __USE_BSD /*for getpagesize() (server)*/
#include <unistd.h>
#undef __USE_BSD
#undef __USE_MISC
#include <wait.h>
extern char **environ; /*see 'man environ'*/

static unsigned int vehicle_msptypes[VEHICLE_MODEL_TOTAL];

static
void init_mission_type_names()
{
	mission_type_names[SETTING__MISSION_TYPEID_PASSENGER_S] = "Passengers (S)";
	mission_type_names[SETTING__MISSION_TYPEID_PASSENGER_M] = "Passengers (M)";
	mission_type_names[SETTING__MISSION_TYPEID_PASSENGER_L] = "Passengers (L)";
	mission_type_names[SETTING__MISSION_TYPEID_CARGO_S] = "Cargo (S)";
	mission_type_names[SETTING__MISSION_TYPEID_CARGO_M] = "Cargo (M)";
	mission_type_names[SETTING__MISSION_TYPEID_CARGO_L] = "Cargo (L)";
	mission_type_names[SETTING__MISSION_TYPEID_HELI] = "Heli";
	mission_type_names[SETTING__MISSION_TYPEID_HELI_CARGO] = "Heli (cargo)";
	mission_type_names[SETTING__MISSION_TYPEID_MIL_HELI] = "Heli (military)";
	mission_type_names[SETTING__MISSION_TYPEID_MIL] = "Military";
	mission_type_names[SETTING__MISSION_TYPEID_PASSENGER_WATER] = "Passengers (water)";
}

static
void init_vehicle_msptypes()
{
	memset(vehicle_msptypes, 0, sizeof(vehicle_msptypes));
	vehicle_msptypes[MODEL_ANDROM] = SETTING__ANDROM_MSPTYPES;
	vehicle_msptypes[MODEL_AT400] = SETTING__AT400_MSPTYPES;
	vehicle_msptypes[MODEL_BEAGLE] = SETTING__BEAGLE_MSPTYPES;
	vehicle_msptypes[MODEL_CARGOBOB] = SETTING__CARGOBOB_MSPTYPES;
	vehicle_msptypes[MODEL_CROPDUST] = SETTING__CROPDUST_MSPTYPES;
	vehicle_msptypes[MODEL_DODO] = SETTING__DODO_MSPTYPES;
	vehicle_msptypes[MODEL_HUNTER] = SETTING__HUNTER_MSPTYPES;
	vehicle_msptypes[MODEL_HYDRA] = SETTING__HYDRA_MSPTYPES;
	vehicle_msptypes[MODEL_LEVIATHN] = SETTING__LEVIATHN_MSPTYPES;
	vehicle_msptypes[MODEL_MAVERICK] = SETTING__MAVERICK_MSPTYPES;
	vehicle_msptypes[MODEL_NEVADA] = SETTING__NEVADA_MSPTYPES;
	vehicle_msptypes[MODEL_POLMAV] = SETTING__POLMAV_MSPTYPES;
	vehicle_msptypes[MODEL_RAINDANC] = SETTING__RAINDANC_MSPTYPES;
	vehicle_msptypes[MODEL_RUSTLER] = SETTING__RUSTLER_MSPTYPES;
	vehicle_msptypes[MODEL_SHAMAL] = SETTING__SHAMAL_MSPTYPES;
	vehicle_msptypes[MODEL_SKIMMER] = SETTING__SKIMMER_MSPTYPES;
	vehicle_msptypes[MODEL_SPARROW] = SETTING__SPARROW_MSPTYPES;
	vehicle_msptypes[MODEL_STUNT] = SETTING__STUNT_MSPTYPES;
	vehicle_msptypes[MODEL_VCNMAV] = SETTING__VCNMAV_MSPTYPES;
}

static
void init_class_msptypes()
{
	CLASS_MSPTYPES[SETTING__CLASSID_PASSENGER_PILOT] = SETTING__PASSENGER_PILOT_MSPTYPES;
	CLASS_MSPTYPES[SETTING__CLASSID_CARGO_PILOT] = SETTING__CARGO_PILOT_MSPTYPES;
	CLASS_MSPTYPES[SETTING__CLASSID_RESCUE] = SETTING__RESCUE_MSPTYPES;
	CLASS_MSPTYPES[SETTING__CLASSID_ARMY] = SETTING__ARMY_MSPTYPES;
	CLASS_MSPTYPES[SETTING__CLASSID_AID] = SETTING__AID_MSPTYPES;
}

static
void init_class_names()
{
	CLASS_NAMES[SETTING__CLASSID_PASSENGER_PILOT] = SETTING__PASSENGER_PILOT_COLOR SETTING__PASSENGER_PILOT_NAME;
	CLASS_NAMES[SETTING__CLASSID_CARGO_PILOT] = SETTING__CARGO_PILOT_COLOR SETTING__CARGO_PILOT_NAME;
	CLASS_NAMES[SETTING__CLASSID_RESCUE] = SETTING__RESCUE_COLOR SETTING__RESCUE_NAME;
	CLASS_NAMES[SETTING__CLASSID_ARMY] = SETTING__ARMY_COLOR SETTING__ARMY_NAME;
	CLASS_NAMES[SETTING__CLASSID_AID] = SETTING__AID_COLOR SETTING__AID_NAME;
}

static unsigned short nametags_max_distance[MAX_PLAYERS];

#include "conf.c"
#include "memstuff.c"
#include "natives.c"
#include "samp.c"
#include "textdraws.c"
#include "common.c"
#include "cmd_utils.c"
#include "time.c"
#include "timer.c"
#include "echo.c"
#include "anticheat.c"
#include "money.c"
#include "dialog.c"
#include "airport.c"
#include "protips.c"

#include "discordflightlog.c"
#include "ui.c"
#include "score.c"
#include "admin.c"
#include "changelog.c"
#include "zones.c"
#include "changepassword.c"
#include "class.c"
#include "game_sa.c"
#include "copilot.c"
#include "login.c"
#include "guestregister.c"
#include "heartbeat.c"
#include "kneeboard.c"
#include "maps.c"
#include "nav.c"
#include "panel.c"
#include "jobmap.c"
#include "missions.c"
#include "playerdata.c"
#include "playerstats.c"
#include "pm.c"
#include "radio.c"
#include "servicepoints.c"
#include "spawn.c"
#include "timecyc.c"
#include "nametags.c" /*requires timecyc*/
#include "vehicles.c"
#include "prefs.c"
#include "help.c"

#include "cmdhandlers.c"
#ifdef DEV
#include "cmdhandlers_dev.c"
#endif /*DEV*/

#include "cmd.c"
#define SAMP_NATIVES_IMPL
#include "samp.c"
#include "basdonfly_callbacks.c"

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}

PLUGIN_EXPORT int PLUGIN_CALL Load(void **ppData)
{
	char sha256buf[SHA256BUFSIZE + 1];

	init_mission_type_names();
	init_vehicle_msptypes();
	init_class_msptypes();
	init_class_names();

	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
#undef logprintf
	logprintf = (logprintf_t) ppData[PLUGIN_DATA_LOGPRINTF];

	*(int*) &float_pinf = 0x7F800000;
	*(int*) &float_ninf = 0xFF800000;

	sha256buf[SHA256BUFSIZE - 1] = 0x33;
	sha256buf[SHA256BUFSIZE] = 0x33;
	SAMP_SHA256(sha256buf, "input");
	assert(sha256buf[SHA256BUFSIZE] == 0x33); /*if fails, the func writes more than expected*/
	assert(sha256buf[SHA256BUFSIZE - 1] == 0); /*if fails, the func doesn't end writing with a zero term*/

#ifdef DEV
	sampasm_sanitycheck();
#endif
	textdraws_init();
	conf_load();

	cmd_init();
	game_sa_init();
	kneeboard_init();
	nav_init();
	panel_init();
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

	echo_tick();
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
	REGISTERNATIVE(B_OnGameModeExit),
	REGISTERNATIVE(B_OnGameModeInit),
	REGISTERNATIVE(B_OnIncomingConnection),
	REGISTERNATIVE(B_OnPlayerConnect),
	REGISTERNATIVE(B_OnPlayerDeath),
	REGISTERNATIVE(B_OnPlayerDisconnect),
	REGISTERNATIVE(B_OnPlayerEnterVehicle),
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
	return AMX_ERR_NONE;
}
