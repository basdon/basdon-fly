
/* vim: set filetype=c ts=8 noexpandtab: */

#define _CRT_SECURE_NO_DEPRECATE

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

#include "cmd.h"
#include "natives.h"
#include "publics.h"

typedef void (*mysql_cb)(void* data);

typedef void (*logprintf_t)(char* format, ...);

extern logprintf_t logprintf;

#define amx_GetUString(dest, source, size) amx_GetString(dest, source, 0, size)
#define amx_SetUString(dest, source, size) amx_SetString(dest, source, 0, 0, size)
#define SETB144(X) amx_SetUString(buf144,X,144)

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
#define PLAYER_CC_GETID(VALUE) (((unsigned int) VALUE >> 22) & 0x3FF)
#define PLAYER_CC_CHECK(VALUE,PLAYERID) (_cc[PLAYERID] == (VALUE & 0x003FFFFF))

/* amx addresses of buffers */
extern cell emptystringa, buf32a, buf32_1a, buf64a, buf144a, buf4096a;
extern cell underscorestringa;
/* physical addresses of buffers */
extern cell *emptystring, *buf32, *buf32_1, *buf64, *buf144, *buf4096;
extern cell *underscorestring;
/* buffers as char pointers */
extern char *cemptystring, *cbuf32, *cbuf32_1, *cbuf64, *cbuf144, *cbuf4096;
extern char *cunderscorestring;
/* float pointers to buffers */
extern float *fbuf32_1, *fbuf32, *fbuf64, *fbuf144, *fbuf4096;
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
Kickdelay, when larger than 0, should be decremented in ProcessTick and
player kicked on 0.
*/
extern short kickdelay[MAX_PLAYERS];
/**
Holds spawned status of players.
*/
extern int spawned[MAX_PLAYERS];
/**
Holds class the players are playing as.
*/
extern int playerclass[MAX_PLAYERS];
/**
Player preferences, see sharedsymbols.h.
*/
extern int prefs[MAX_PLAYERS];
/**
Logged-in status of each player (one of the LOGGED_ definitions).
*/
extern int loggedstatus[MAX_PLAYERS];
/**
Variable holding afk state of players (temp until all is moved to plugin).
*/
extern int temp_afk[MAX_PLAYERS];
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
Calls mysql_tquery with a callback and data to pass.

Uses buf4096 for query. Also uses the last 8 components of buf4096.

@param callback cb to call when the query has been executed
@param data usually a pointer to allocated memory that should be freed in the cb
*/
void common_mysql_tquery(char *query, mysql_cb callback, void *data);
float common_vectorsize_sq(struct vec3 vec);
