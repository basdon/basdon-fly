
/* vim: set filetype=c ts=8 noexpandtab: */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include "common.h"
#include <string.h>

/*
value vs small vs large:
+---------+
|   large |
|   large |
|+--------+
||  small |
||+-------+
||| value |
||+-------+
||  small |
|+--------+
|   large |
|   large |
+---------+
*/

struct panelcache {
	short altitude;
	short altitudemetersmall;
	short altitudemeterlarge;
	short speed;
	short speedmetersmall;
	short speedmeterlarge;
	short heading;
} caches[MAX_PLAYERS];

cell AMX_NATIVE_CALL Panel_ResetCaches(AMX *amx, cell *params)
{
	int playerid = params[1];
	memset(&caches[playerid], 0xFF, sizeof(struct panelcache));
	return 1;
}

cell AMX_NATIVE_CALL Panel_UpdateAltitude(AMX *amx, cell *params)
{
	int playerid = params[1];
	int altitude = params[2];
	int altitude50 = altitude / 50;
	int i, tmp, tmp2;
	cell *addr4, *addr13, *addr44;
	char buf4[5];
	char buf13[14];
	char buf44[45];

	if (altitude == caches[playerid].altitude) {
		return 0;
	}
	caches[playerid].altitude = altitude;

	amx_GetAddr(amx, params[3], &addr4);
	amx_GetAddr(amx, params[4], &addr13);
	amx_GetAddr(amx, params[5], &addr44);

	/* alt */
	sprintf(buf4, "%03d", altitude);
	amx_SetUString(addr4, buf4, sizeof(buf4));

	/* alt meter large */
	altitude50 -= ((unsigned int) altitude & 0x80000000) >> 31;
	if (altitude50 != caches[playerid].altitudemeterlarge) {
		caches[playerid].altitudemeterlarge = altitude50;
		strcpy(buf44, "____-~n~____-~n~~n~~n~~n~~n~____-~n~____-~n~");
		for (i = 0; i < 4; i++) {
			tmp = altitude50 + 2 - i;
			if (tmp < -18 || 19 < tmp) {
				continue;
			}
			tmp2 = i * 8 + (i >> 1) * 12;
			sprintf(&buf44[tmp2], "%4d", (tmp * 50));
			buf44[tmp2 + 4] = '-';
		}
		amx_SetUString(addr44, buf44, sizeof(buf44));
	}  else {
		buf44[0] = 0;
		amx_SetUString(addr44, buf44, 1);
	}

	/* alt meter small */
	tmp = ((altitude + 10 * (((unsigned int) altitude & 0x8000000) >> 22)) / 10) % 10;
	if (tmp != caches[playerid].altitudemetersmall) {
		caches[playerid].altitudemetersmall = tmp;
		strcpy(buf13, "_00~n~~n~_00");
		if (altitude >= 0) {
			tmp = altitude + 10;
			tmp2 = altitude;
		} else {
			buf13[0] = buf13[9] = '-';
			tmp = -altitude;
			tmp2 = -altitude + 10;
		}
		buf13[1] = '0' + (tmp / 10) % 10;
		buf13[10] = '0' + (tmp2 / 10) % 10;
		amx_SetUString(addr13, buf13, sizeof(buf13));
	}  else {
		buf13[0] = 0;
		amx_SetUString(addr13, buf13, 1);
	}

	return 1;
}

