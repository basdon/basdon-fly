
/* vim: set filetype=c ts=8 noexpandtab: */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include "common.h"
#include "airport.h"
#include <string.h>
#include <math.h>

struct airport *airports;
int airportscount;

static int *indexmap[MAX_PLAYERS];

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
	while (i < MAX_PLAYERS) {
		indexmap[i++] = NULL;
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
	ap->pos.x = amx_ctof(params[6]);
	ap->pos.y = amx_ctof(params[7]);
	ap->pos.z = amx_ctof(params[8]);
	ap->runways = NULL;
	return 1;
}

/* native APT_AddRunway(aptindex, specifier, Float:heading, Float:x, Float:y, Float:z, nav) */
cell AMX_NATIVE_CALL APT_AddRunway(AMX *amx, cell *params)
{
	struct airport *ap = airports + params[1];
	struct runway *rnw = malloc(sizeof(struct runway));

	sprintf(rnw->id, "%02.0f", amx_ctof(params[3]) / 10.0f);
	rnw->id[2] = params[2];
	rnw->id[3] = 0;
	rnw->heading = amx_ctof(params[3]);
	rnw->pos.x = amx_ctof(params[4]);
	rnw->pos.y = amx_ctof(params[5]);
	rnw->pos.z = amx_ctof(params[6]);
	rnw->nav = params[7];
	rnw->next = ap->runways;
	ap->runways = rnw;
	return 1;
}

struct apref {
	float distance;
	int index;
};

static int sortaprefs(const void *_a, const void *_b)
{
	struct apref *a = (struct apref*) _a;
	struct apref *b = (struct apref*) _b;
	if (a->distance < b->distance) return 1;
	if (a->distance > b->distance) return -1;
	return 0;
}

/* native APT_FormatNearestList(playerid, Float:x, Float:y, buf[]) */
cell AMX_NATIVE_CALL APT_FormatNearestList(AMX *amx, cell *params)
{
	cell *addr;
	int i = 0, idx = 0, pid = params[1];
	float dx, dy;
	char buf[4096];
	struct airport *ap;
	struct apref *aps = malloc(sizeof(struct apref) * airportscount);
	
	while (i < airportscount) {
		dx = airports[i].pos.x - amx_ctof(params[2]);
		dy = airports[i].pos.y - amx_ctof(params[3]);
		aps[i].distance = sqrt(dx * dx + dy * dy);
		aps[i].index = i;
		i++;
	}
	qsort(aps, airportscount, sizeof(*aps), sortaprefs);
	if (indexmap[pid] == NULL) {
		indexmap[pid] = malloc(sizeof(indexmap[0]) * airportscount);
	}
	while (i--) {
		*(indexmap[pid] + airportscount - i - 1) = aps[i].index;
		ap = airports + aps[i].index;
		if (aps[i].distance < 1000.0f) {
			idx += sprintf(buf + idx, "\n%.0f\t%s\t[%s]", aps[i].distance, ap->name, ap->code);
		} else {
			idx += sprintf(buf + idx, "\n%.1fK\t%s\t[%s]", aps[i].distance / 1000.0f, ap->name, ap->code);
		}
	}
	amx_GetAddr(amx, params[4], &addr);
	amx_SetUString(addr, buf + 1, idx + 1);

	free(aps);
	return 1;
}

/* native APT_FormatBeaconList(buf[]) */
cell AMX_NATIVE_CALL APT_FormatBeaconList(AMX *amx, cell *params)
{
	cell *addr;
	char buf[4096];
	struct airport *ap = airports;
	int count = airportscount, idx = 0;

	while (count-- > 0) {
		idx += sprintf(buf + idx, " %s", ap->beacon);
		ap++;
	}

	amx_GetAddr(amx, params[1], &addr);
	amx_SetUString(addr, buf + 1, idx + 1);
	return 1;
}

/* native API_MapIndexFromListDialog(playerid, listitem=0) */
cell AMX_NATIVE_CALL APT_MapIndexFromListDialog(AMX *amx, cell *params)
{
	int value, pid = params[1];

	if (indexmap[pid] == NULL) {
		return 0;
	}

	if (params[2] < 0 || airportscount <= params[2]) {
		value = 0;
	} else {
		value = *(indexmap[pid] + params[2]);
	}

	free(indexmap[pid]);
	indexmap[pid] = NULL;
	return value;
}

/* native APT_FormatInfo(aptidx, buf[]) */
cell AMX_NATIVE_CALL APT_FormatInfo(AMX *amx, cell *params)
{
	int idx = params[1];
	struct airport *ap;
	struct runway *rnw;
	cell *addr;
	char buf[4096];
	char szRunways[] = "Runways:";

	amx_GetAddr(amx, params[2], &addr);
	if (idx < 0 || airportscount <= idx) {
		buf[0] = 0;
		amx_SetUString(addr, buf, 2);
		return 0;
	}
	ap = airports + idx;
	idx = sprintf(buf, "\nElevation:\t%.0f FT", ap->pos.z);
	idx += sprintf(buf + idx, "\nBeacon:\t%s", ap->beacon);
	rnw = ap->runways;
	while (rnw != NULL) {
		idx += sprintf(buf + idx, "\n%s\t%s", szRunways, rnw->id);
		if (rnw->nav) {
			idx += sprintf(buf + idx, " (VOR,ILS)");
		}
		rnw = rnw->next;
		szRunways[0] = '\t';
		szRunways[1] = 0;
	}

	amx_SetUString(addr, buf + 1, idx + 1);
	return 1;
}

/* native APT_FormatCodeAndName(aptidx, buf[]) */
cell AMX_NATIVE_CALL APT_FormatCodeAndName(AMX *amx, cell *params)
{
	int idx = params[1];
	cell *addr;
	char res[64];

	amx_GetAddr(amx, params[2], &addr);
	if (idx < 0 || airportscount <= idx) {
		res[0] = 'u';
		res[1] = 'n';
		res[2] = 'k';
		res[3] = 0;
		amx_SetUString(addr, res, 4);
		return 0;
	}

	sprintf(res, "%s - %s", airports[idx].code, airports[idx].name);
	amx_SetUString(addr, res, 64);
	return 1;
}