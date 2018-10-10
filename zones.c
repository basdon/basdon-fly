
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
