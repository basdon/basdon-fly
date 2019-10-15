
/* vim: set filetype=c ts=8 noexpandtab: */

#include <stdlib.h>
#include <stdio.h>

#include "vendor/SDK/amx/amx.h"
#include "vendor/SDK/plugincommon.h"
#include "sharedsymbols.h"
#include "natives.h"
#include "publics.h"

typedef void (*logprintf_t)(char* format, ...);

extern logprintf_t logprintf;

#define amx_GetUString(dest, source, size) amx_GetString(dest, source, 0, size)
#define amx_SetUString(dest, source, size) amx_SetString(dest, source, 0, 0, size)

#define M_PI 3.14159265358979323846f
#define M_PI2 1.57079632679489661923f
#define DEG_TO_RAD (M_PI / 180.0f)

#define CLAMP(X,L,U) ((X < L) ? L : ((X > U) ? U : X))
#define Q(X) #X
#define EQ(X) Q(X)

struct vec3 {
	float x, y, z;
};

/* amx addresses of buffers */
extern cell emptystringa, buf32a, buf32_1a, buf64a, buf144a, buf4096a;
/* physical addresses of buffers */
extern cell *emptystring, *buf32, *buf32_1, *buf64, *buf144, *buf4096;

/* element at index playerid is either 0 or 1 */
extern char playeronlineflag[MAX_PLAYERS];
/* contains 'playercount' elements, ids of connected players (not sorted) */
extern short players[MAX_PLAYERS];
extern int playercount;
#define IsPlayerConnected(PLAYERID) playeronlineflag[PLAYERID]
