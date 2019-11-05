
/* vim: set filetype=c ts=8 noexpandtab: */

#include <stdlib.h>
#include <stdio.h>

#include "vendor/SDK/amx/amx.h"
#include "vendor/SDK/plugincommon.h"
#include "sharedsymbols.h"

struct vec3 {
	float x, y, z;
};

struct vec4 {
	struct vec3 coords;
	float r;
};

#include "natives.h"
#include "publics.h"

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

/* amx addresses of buffers */
extern cell emptystringa, buf32a, buf32_1a, buf64a, buf144a, buf4096a;
/* physical addresses of buffers */
extern cell *emptystring, *buf32, *buf32_1, *buf64, *buf144, *buf4096;
/* buffers as char pointers */
extern char *cemptystring, *cbuf32, *cbuf32_1, *cbuf64, *cbuf144, *cbuf4096;
/* float pointers to buffers */
extern float *fbuf32_1, *fbuf32, *fbuf64, *fbuf144, *fbuf4096;

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
void common_tp_player(AMX *amx, int playerid, struct vec4 pos);

/**
Hides game text for given player.
*/
void common_hide_gametext_for_player(AMX *amx, int playerid);

/**
Try to find the player that is in given seat of given vehicle.

@return player id or INVALID_PLAYER_ID
*/
int common_find_player_in_vehicle_seat(AMX *amx, int vehicleid, int seat);
#define common_find_vehicle_driver(X,Y) \
	common_find_player_in_vehicle_seat(X,Y,0)

/**
Attempt to crash the player.
*/
void common_crash_player(AMX *amx, int playerid);

/**
Kick a player next server tick.
*/
void common_NC_Kick(int playerid);

/**
Log some anticheat related thing.

@param eventtype one of AC_ definitions
@param info not escaped into db query
*/
void anticheat_log(AMX *amx, int playerid, int eventtype, char *info);

/**
Check if a float is any NaN

@return non-zero when given float is a NaN float
*/
int common_is_nan(float f);

/**
Sets the state of the engine for given vehicle id.
*/
void common_set_vehicle_engine(AMX *amx, int vehicleid, int enginestatus);

/**
Alternative for GetPlayerPos to get it directly into a vec3 struct.

Will use buf32, buf64, buf144.
*/
int common_NC_GetPlayerPos(AMX *amx, int playerid, struct vec3 *pos);

/**
Alternative for GetVehiclePos to get it directly into a vec3 struct.

Will use buf32, buf64, buf144.
*/
int common_NC_GetVehiclePos(AMX*, int vehicleid, struct vec3 *pos);

/**
Alternative for GetVehicleVelocity to get it directly into a vec3 struct.

Will use buf32, buf64, buf144.
*/
int common_NC_GetVehicleVelocity(AMX*, int vehicleid, struct vec3 *vel);
