
/* vim: set filetype=c ts=8 noexpandtab: */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include "common.h"
#include <string.h>
#include <math.h>

#define ISINVALIDAIRPORT(X) ((X).code[0] == 0)
static int airportscount;

struct runway {
	char id[4];
	float heading, x, y, z;
	struct runway *next;
};

static struct airport {
	float x, y, z;
	char code[4 + 1];
	char enabled;
	char beacon[4 + 1];
	char name[MAX_AIRPORT_NAME + 1];
	struct runway *runways;
} *airports;

void freeAirportTable()
{
	struct airport *ap = airports;
	struct runway *rnw;
	while (airportscount) {
		airportscount--;
		while (ap->runways != NULL) {
			if (ISINVALIDAIRPORT(*ap)) {
				logprintf("warning: airport %d was not initialized", airportscount);
				break;
			}
			rnw = ap->runways->next;
			free(ap->runways);
			ap->runways = rnw;
		}
		ap++;
	}
        free(airports);
}

cell AMX_NATIVE_CALL APT_Init(AMX *amx, cell *params)
{
	int i;
	if (airports != NULL) {
		logprintf("APT_Init: warning: airports table was not empty");
		freeAirportTable();
	}
	if (params[1] <= 0) {
		airportscount = 0;
		airports = NULL;
		return 0;
	}
	i = airportscount = params[1];
	airports = malloc(sizeof(struct airport) * airportscount);
	while (i--) {
		airports[i].code[0] = 0;
	}
	return 1;
}

cell AMX_NATIVE_CALL APT_Destroy(AMX *amx, cell *params)
{
	if (airports != NULL) {
		freeAirportTable();
		airports = NULL;
	}
	return 1;
}

/* native APT_Add(index, code[], enabled, name[], beacon[], Float:x, Float:y, Float:z) */
cell AMX_NATIVE_CALL APT_Add(AMX *amx, cell *params)
{
	cell *addr;
	struct airport *ap = airports + params[1];

	amx_GetAddr(amx, params[2], &addr);
	amx_GetUString(ap->code, addr, sizeof(ap->code));
	ap->enabled = params[3];
	amx_GetAddr(amx, params[4], &addr);
	amx_GetUString(ap->name, addr, sizeof(ap->name));
	amx_GetAddr(amx, params[5], &addr);
	amx_GetUString(ap->beacon, addr, sizeof(ap->beacon));
	ap->x = amx_ctof(params[6]);
	ap->y = amx_ctof(params[7]);
	ap->z = amx_ctof(params[8]);
	ap->runways = NULL;
	return 1;
}

/* native APT_AddRunway(aptindex, specifier, Float:heading, Float:x, Float:y, Float:z) */
cell AMX_NATIVE_CALL APT_AddRunway(AMX *amx, cell *params)
{
	struct airport *ap = airports + params[1];
	struct runway *rnw = malloc(sizeof(struct runway));

	sprintf(rnw->id, "%02.0f", amx_ctof(params[3]) / 10.0f);
	rnw->id[2] = params[2];
	rnw->id[3] = 0;
	rnw->heading = amx_ctof(params[3]);
	rnw->x = amx_ctof(params[4]);
	rnw->y = amx_ctof(params[5]);
	rnw->z = amx_ctof(params[6]);
	rnw->next = ap->runways;
	ap->runways = rnw;
	return 1;
}

struct apref {
	float distance;
	int index;
};

int sortaprefs(const void *_a, const void *_b)
{
	struct apref *a = (struct apref*) _a;
	struct apref *b = (struct apref*) _b;
	if (a->distance < b->distance) return 1;
	if (a->distance > b->distance) return -1;
	return 0;
}

/* native APT_FormatNearestAirportList(Float:x, Float:y, buf[]) */
cell AMX_NATIVE_CALL APT_FormatNearestAirportsList(AMX *amx, cell *params)
{
	cell *addr;
	int i = 0, idx = 0;
	float dx, dy;
	char buf[4096];
	struct airport *ap;
	struct apref *aps = malloc(sizeof(struct apref) * airportscount);
	
	while (i < airportscount) {
		dx = airports[i].x - amx_ctof(params[1]);
		dy = airports[i].y - amx_ctof(params[2]);
		aps[i].distance = sqrt(dx * dx + dy * dy);
		aps[i].index = i;
		i++;
	}
	qsort(aps, airportscount, sizeof(*aps), sortaprefs);
	while (i--) {
		ap = airports + aps[i].index;
		if (aps[i].distance < 1000.0f) {
			idx += sprintf(buf + idx, "\n%.0f\t%s\t[%s]", aps[i].distance, ap->name, ap->code);
		} else {
			idx += sprintf(buf + idx, "\n%.1fK\t%s\t[%s]", aps[i].distance / 1000.0f, ap->name, ap->code);
		}
	}
	amx_GetAddr(amx, params[3], &addr);
	amx_SetUString(addr, buf + 1, sizeof(buf) - 1);
	
	free(aps);
	return 1;
}
