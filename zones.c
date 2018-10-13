
/* vim: set filetype=c ts=8 noexpandtab: */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include "common.h"
#include "zones.h"
#include <string.h>

extern char *zonenames[];
extern struct region regions[];
extern struct zone zones[];
extern int regioncount;
extern int zonecount;

int lastzoneid[MAX_PLAYERS];
int lastregionid[MAX_PLAYERS];
int lastzoneindex[MAX_PLAYERS];

void zones_init()
{
	struct region *r = regions, *rmax = regions + regioncount;
	struct zone *pz = zones;
	int z = 0, zmax = zonecount;

	while (z < zmax) {
nr:
		if (pz->x1 < r->zone.x1 || r->zone.x2 < pz->x2 ||
			pz->y1 < r->zone.y1 || r->zone.y2 < pz->y2)
		{
			r->maxzone = z;
			r++;
			if (r >= rmax) {
				logprintf("zones.c error: at zone %d but regions are depleted", z);
				return;
			}
			r->minzone = z;
			goto nr;
		}
		z++;
		pz++;
	}
	r->maxzone = z;

	for (z = 0; z < MAX_PLAYERS; z++) {
		lastzoneindex[z] = lastzoneid[z] = -1;
		lastregionid[z] = ZONE_INVALID;
	}
}

/* native Zones_InvalidateForPlayer(playerid) */
cell AMX_NATIVE_CALL Zones_InvalidateForPlayer(AMX *amx, cell *params)
{
	lastzoneindex[params[1]] = lastzoneid[params[1]] = -1;
	lastregionid[params[1]] = ZONE_INVALID;
	return 1;
}

static int isinzone(float x, float y, float z, struct zone *zone)
{
	return zone->x1 <= x && x <= zone->x2 &&
		zone->y1 <= y && y <= zone->y2 &&
		zone->z1 <= z && z <= zone->z2;
}

/* native Zones_UpdateForPlayer(playerid, Float:x, Float:y, Float:z) */
cell AMX_NATIVE_CALL Zones_UpdateForPlayer(AMX *amx, cell *params)
{
	int playerid = params[1];
	struct region *r = regions, *rmax = regions + regioncount;
	struct zone *pz;
	int i;
	int lrid = lastregionid[playerid];
	int lzid = lastzoneid[playerid];
	float x = amx_ctof(params[2]), y = amx_ctof(params[3]), z = amx_ctof(params[4]);

	if (lastzoneindex[playerid] >= 0 && isinzone(x, y, z, zones + lastzoneindex[playerid])) {
		return 0;
	}

	lastzoneindex[playerid] = -1;
	r = regions;
	rmax = regions + regioncount;

	while (r < rmax) {
		if (isinzone(x, y, z, &r->zone)) {
			lastregionid[playerid] = r->zone.id;
			for (i = r->minzone; i < r->maxzone; i++) {
				pz = zones + i;
				if (isinzone(x, y, z, pz)) {
					lastzoneindex[playerid] = i;
					lastzoneid[playerid] = pz->id;
					goto ret;
					
				}
			}
			lastzoneid[playerid] = -1;
			goto ret;
		}
		r++;
	}

	lastregionid[playerid] = ZONE_NONE_NW + ((y < 0.0f) << 1) + (x > 0.0f);
ret:
	return lrid != lastregionid[playerid] || lzid != lastzoneid[playerid];
}

/* native Zones_FormatForPlayer(playerid, buf[]) */
cell AMX_NATIVE_CALL Zones_FormatForPlayer(AMX *amx, cell *params)
{
	int lzid = lastzoneid[params[1]];
	int lrid = lastregionid[params[1]];
	cell *addr;
	char result[100];

	if (lzid != -1) {
		sprintf(result, "%s~n~%s", zonenames[lzid], zonenames[lrid]);
	} else {
		result[0] = result[2] = '~';
		result[1] = 'n';
		strcpy(result + 3, zonenames[lrid]);
	}
	amx_GetAddr(amx, params[2], &addr);
	amx_SetUString(addr, result, sizeof(result));

	return 1;
}
