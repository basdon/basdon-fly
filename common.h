
/* vim: set filetype=c ts=8 noexpandtab: */

#define _CRT_SECURE_NO_DEPRECATE

#define VERSION "0.1"

#include <stdlib.h>
#include <stdio.h>

#include "vendor/SDK/amx/amx.h"
#include "vendor/SDK/plugincommon.h"
#include "sharedsymbols.h"

#define STATIC_ASSERT(E) typedef char __static_assert_[(E)?1:-1]
#define EXPECT_SIZE(S,SIZE) STATIC_ASSERT(sizeof(S)==(SIZE))

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

struct PLAYERKEYS {
	int keys;
	int updown;
	int leftright;
};
EXPECT_SIZE(struct PLAYERKEYS, 3 * sizeof(cell));

struct VEHICLEPARAMS {
	int engine;
	int lights;
	int alarm;
	int doors;
	int bonnet;
	int boot;
	int objectite;
};
EXPECT_SIZE(struct VEHICLEPARAMS, 7 * sizeof(cell));

struct VEHICLEDAMAGE {
	unsigned int panels;
	unsigned int doors;
	unsigned int lights;
	unsigned int tires;
};
EXPECT_SIZE(struct VEHICLEDAMAGE, 4 * sizeof(cell));

/*include cmd here because lots of headers files need it*/
#include "cmd.h"
#include "natives.h"

typedef void (*cb_t)(void* data);

typedef void (*logprintf_t)(char* format, ...);

extern logprintf_t logprintf;

/**
Convert char string to cell string.

@param max must be gt 0
*/
void atoc(cell *dst, char *src, int max);
/**
Convert cell string to char string.

@param max must be gt 0
*/
void ctoa(char *dst, cell *src, int max);
/**
Convert char string to cell string, in place.

@param len length of the string, excluding zero term
*/
void atoci(cell *dstsrc, int len);
/**
Convert a cell string to a char string, in place.

Relies on good faith that the cell string is zero terminated.
*/
void ctoai(char *dstsrc);
#define csprintf(DST,FMT,...) atoci(DST,sprintf((char*)DST,FMT,__VA_ARGS__))
#define B144(X) atoc(buf144,X,144)

#ifdef DEV
#define DBG(...) sprintf(cbuf64,__VA_ARGS__);\
			printf("%s\n", cbuf64);\
			atoc(buf144,cbuf64,144);\
			NC_SendClientMessageToAll(-1,buf144a);
#else
#define DBG(...)
#endif

#define FLOAT_PINF (0x7F800000)
#define FLOAT_NINF (0xFF800000)
#define M_PI 3.14159265358979323846f
#define M_PI2 1.57079632679489661923f
#define DEG_TO_RAD (M_PI / 180.0f)

#define CLAMP(X,L,U) ((X < L) ? L : ((X > U) ? U : X))
#define Q(X) #X
#define EQ(X) Q(X)

#define MK_PLAYER_CC(PLAYERID) \
	((_cc[PLAYERID] & 0x003FFFFF) | (PLAYERID << 22))
#define V_MK_PLAYER_CC(PLAYERID) ((void*) MK_PLAYER_CC(PLAYERID))
#define PLAYER_CC_GETID(VALUE) (((unsigned int) VALUE >> 22) & 0x3FF)
#define PLAYER_CC_CHECK(VALUE,PLAYERID) \
	(_cc[PLAYERID] == ((unsigned int) VALUE & 0x003FFFFF))

#define ECOL_INFO "{3498db}"
#define ECOL_WARN "{e84c3d}"
#define ECOL_SUCC "{2cc36b}"
#define ECOL_DIALOG_CAPTION "{d2d2d2}"
#define ECOL_DIALOG_TEXT "{a9c4e4}"
#define ECOL_DIALOG_BUTTON "{c8c8c8}"
#define ECOL_MISSION "{ff9900}"

#define WARN "! "
#define INFO "* "
#define SUCC "+ "

#define COL_SAMP_GREEN 0x88aa62ff // as in username error message
#define COL_SAMP_GREY 0xa9c4e4ff // as in 'Connected to..'
#define COL_SAMP_BLUE 0x2587ceff // as in '* Admin:'
#define COL_INFO 0x3498dbff
#define COL_WARN 0xe84c3dff
#define COL_SUCC 0x2cc36bff
#define COL_INFO_GENERIC 0xffffffff
#define COL_INFO_LIGHT 0x33ccffff
#define COL_PRIVMSG 0xffff00ff
#define COL_PRIVMSG_HINT 0xafafafff
#define COL_MISSION 0xff9900ff
#define COL_JOIN 0x1b8ae4ff
#define COL_QUIT 0x1674Bcff
#define COL_METAR COL_INFO_LIGHT

#define VEL_MAX (0.66742320819112627986348122866894f)
#define VEL_TO_KPH (VEL_MAX * 270.0f)
#define VEL_TO_KTS (VEL_MAX * 145.0f)
#define VEL_TO_MPS (VEL_TO_KPH / 3.6f) /*(KPH / 3.6)*/
#define VEL_TO_KFPM (VEL_TO_MPS * 3.28084f * 60.0f / 1000.0f) /* K feet/m*/

#define GROUP_BANNED (1)
#define GROUP_GUEST (2)
#define GROUP_MEMBER (4)
#define GROUP_ADMIN (268435456)
#define GROUP_OWNER (1073741824)

#define GROUPS_ALL \
	(GROUP_BANNED|GROUP_GUEST|GROUP_MEMBER|\
	GROUP_ADMIN|GROUP_OWNER)
#define GROUPS_ADMIN (GROUP_ADMIN|GROUP_OWNER)
#define GROUPS_NOTBANNED (GROUPS_ALL&~GROUPS_BANNED)
#define GROUPS_REGISTERED (GROUPS_ALL&~GROUP_GUEST)

#define GROUPS_ISADMIN(X) ((X) >= GROUP_ADMIN)

/*in cells*/
#define STACK_HEAP_SIZE 1024

struct FAKEAMX_DATA {
	cell emptystring;
	cell underscorestring[2];
	union {
		cell ascell[32];
		char aschr[32 * sizeof(cell)];
		float asflt[32];
	} a32;
	union {
		cell ascell[32];
		char aschr[32 * sizeof(cell)];
		float asflt[32];
	} a32_1;
	union {
		cell ascell[64];
		char aschr[64 * sizeof(cell)];
		float asflt[64];
	} a64;
	union {
		cell ascell[144];
		char aschr[144 * sizeof(cell)];
		float asflt[144];
	} a144;
	union {
		cell ascell[4096];
		char aschr[4096 * sizeof(cell)];
		float asflt[4096];
		struct {
			char _pad[4096 * 3];
			char last4096[4096];
		} splitted;
	} a4096;
	cell _stackheap[STACK_HEAP_SIZE];
};
#define basea ((int) &fakeamx_data)
#define emptystringa ((int) &fakeamx_data.emptystring - basea)
#define underscorestringa ((int) &fakeamx_data.underscorestring - basea)
#define buf32a ((int) &fakeamx_data.a32 - basea)
#define buf32_1a ((int) &fakeamx_data.a32_1 - basea)
#define buf64a ((int) &fakeamx_data.a64 - basea)
#define buf144a ((int) &fakeamx_data.a144 - basea)
#define buf4096a ((int) &fakeamx_data.a4096 - basea)
#define buf32 (fakeamx_data.a32.ascell)
#define buf32_1 (fakeamx_data.a32_1.ascell)
#define buf64 (fakeamx_data.a64.ascell)
#define buf144 (fakeamx_data.a144.ascell)
#define buf4096 (fakeamx_data.a4096.ascell)
#define cbuf32 fakeamx_data.a32.aschr
#define cbuf32_1 fakeamx_data.a32_1.aschr
#define cbuf64 fakeamx_data.a64.aschr
#define cbuf144 fakeamx_data.a144.aschr
#define cbuf4096 fakeamx_data.a4096.aschr
#define cbuf4096_ fakeamx_data.a4096.splitted.last4096
#define fbuf32 fakeamx_data.a32.asflt
#define fbuf32_1 fakeamx_data.a32_1.asflt
#define fbuf64 fakeamx_data.a64.asflt
#define fbuf144 fakeamx_data.a144.asflt
#define fbuf4096 fakeamx_data.a4096.asflt

/*amx*/
extern AMX *amx;
/*amx data segment*/
extern struct FAKEAMX_DATA fakeamx_data;
/**
Connection count per player id. Incremented every time playerid connects.

Used to check if a player is still valid between long-running tasks.
*/
extern int _cc[MAX_PLAYERS];

/**
element at index playerid is either 0 or 1
*/
extern char playeronlineflag[MAX_PLAYERS];
/**
contains 'playercount' elements, ids of connected players (not sorted)
*/
extern short players[MAX_PLAYERS];
extern int playercount;
/**
Should be decremented in OnPlayerUpdate and player kicked when it reaches 0.
*/
extern char kick_update_delay[MAX_PLAYERS];
/**
Holds spawned status of players.
*/
extern int spawned[MAX_PLAYERS];
/**
Holds class the players are playing as.
*/
extern int playerclass[MAX_PLAYERS];
/**
Current in-game hour.
*/
extern int time_h;
/**
Current in-game minute.
*/
extern int time_m;

#define IsPlayerConnected(PLAYERID) playeronlineflag[PLAYERID]

/**
Teleport the player to a coordinate, and set facing angle and reset camera.
*/
void common_tp_player(int playerid, struct vec4 pos);
/**
Hides game text for given player.
*/
void common_hide_gametext_for_player(int playerid);
/**
Try to find the player that is in given seat of given vehicle.

@return player id or INVALID_PLAYER_ID
*/
int common_find_player_in_vehicle_seat(int vehicleid, int seat);
#define common_find_vehicle_driver(VEHICLEID) \
	common_find_player_in_vehicle_seat(VEHICLEID,0)
/**
Attempt to crash the player.
*/
void common_crash_player(int playerid);
/**
@return squared distance between given points
*/
float common_dist_sq(struct vec3 a, struct vec3 b);
/**
Sets the state of the engine for given vehicle id.
*/
void common_set_vehicle_engine(int vehicleid, int enginestatus);
/**
Alternative for GetPlayerKeys to get it directly into a PLAYERKEYS struct.

Will use buf32.
*/
void common_GetPlayerKeys(int playerid, struct PLAYERKEYS *keys);
/**
Alternative for GetPlayerPos to get it directly into a vec3 struct.

Will use buf32, buf64, buf144.
*/
int common_GetPlayerPos(int playerid, struct vec3 *pos);
/**
Convenience method for GetVehicleDamageStatus.

Uses buf32.
*/
int common_GetVehicleDamageStatus(int vehicleid, struct VEHICLEDAMAGE *d);
/**
Gets vehicle params of given vehicle into given VEHICLEPARAMS struct.

Uses buf32.
*/
int common_GetVehicleParamsEx(int vehicleid, struct VEHICLEPARAMS *p);
/**
Alternative for GetVehiclePos to get it directly into a vec3 struct.

Will use buf32, buf64, buf144.
*/
int common_GetVehiclePos(int vehicleid, struct vec3 *pos);
/**
Gets a vehicle's position and Z angle into a vec4 struct.

Will use buf32, buf64, buf144.
*/
int common_GetVehiclePosRot(int vehicleid, struct vec4 *pos);
/**
Alternative for GetVehicleRotationQuat to get it directly into a quat struct.

Will use buf32, buf64, buf144, buf32_1.
*/
int common_GetVehicleRotationQuat(int vehicleid, struct quat *rot);
/**
Alternative for GetVehicleVelocity to get it directly into a vec3 struct.

Will use buf32, buf64, buf144.
*/
int common_GetVehicleVelocity(int vehicleid, struct vec3 *vel);
/**
Sets vehicle params of given vehicle into given VEHICLEPARAMS struct.

Uses buf32.
*/
int common_SetVehicleParamsEx(int vehicleid, struct VEHICLEPARAMS *p);
/**
Convenience method for UpdateVehicleDamageStatus.

Uses buf32.
*/
int common_UpdateVehicleDamageStatus(int vehicleid, struct VEHICLEDAMAGE *d);
/**
Check a string agains its hash.

Uses the last 8 components of buf4096 for callback stuff.

@param callback function to call when the checking is done
@param data usually a pointer to allocated memory that should be freed in the cb
*/
void common_bcrypt_check(cell pw, cell hash, cb_t callback, void *data);
/**
Hash a string.

Uses the last 8 components of buf4096 for callback stuff.

@param callback function to call when the hasing is done
@param data usually a pointer to allocated memory that should be freed in the cb
*/
void common_bcrypt_hash(cell pw, cb_t callback, void *data);
/**
Uses the mysql plugin escape string routine.

Uses buf4096
*/
void common_mysql_escape_string(char *data, char *dest, int maxlen);
/**
Calls mysql_tquery with a callback and data to pass.

Uses buf4096 for query. Also uses the last 8 components of buf4096.

@param callback function to call when the query has been executed
@param data usually a pointer to allocated memory that should be freed in the cb
*/
void common_mysql_tquery(char *query, cb_t callback, void *data);
float common_vectorsize_sq(struct vec3 vec);
