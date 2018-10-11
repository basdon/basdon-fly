
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
int lastzoneindex[MAX_PLAYERS];

#define SPECIAL_ZONE_SA_NW = -1
#define SPECIAL_ZONE_SA_NE = -2
#define SPECIAL_ZONE_SA_SW = -3
#define SPECIAL_ZONE_SA_SE = -4

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
}

static int isinzone(float x, float y, float z, struct zone *zone)
{
	return zone->x1 <= x && x <= zone->x2 &&
		zone->y1 <= y && y <= zone->y2 &&
		zone->z1 <= z && z <= zone->z2;
}

/* native Zones_UpdateForPlayer(playerid, Float:x, Float:y, Float:z) */
cell AMX_NATIVE_CALL Zone_UpdateForPlayer(AMX *amx, cell *params)
{
	int playerid = params[1];
	struct region *r = regions, *rmax = regions + regioncount;
	struct zone *pz;
	int i, newid;
	float x = amx_ctof(params[2]), y = amx_ctof(params[3]), z = amx_ctof(params[4]);

	if (lastzoneindex[playerid] >= 0 && isinzone(x, y, z, zones + lastzoneindex[playerid])) {
		return 0;
	}

	lastzoneindex[playerid] = -1;
	r = regions;
	rmax = regions + regioncount;

	while (r < rmax) {
		if (isinzone(x, y, z, &r->zone)) {
			for (i = r->minzone; i < r->maxzone; i++) {
				pz = zones + i;
				if (isinzone(x, y, z, pz)) {
					lastzoneindex[playerid] = i;
					newid = pz->id;
					goto ret;
					
				}
			}
			newid = r->zone.id;
			goto ret;
		}
		r++;
	}

	newid = ZONE_NONE_NW + ((y < 0.0f) << 1) + (x > 0.0f);
ret:
	i = lastzoneid[playerid];
	return (lastzoneid[playerid] = newid) == i;
}
