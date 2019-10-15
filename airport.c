
/* vim: set filetype=c ts=8 noexpandtab: */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include "common.h"
#include "airport.h"
#include <string.h>
#include <math.h>

struct AIRPORT *airports = NULL;
int numairports = 0;

static int *indexmap[MAX_PLAYERS];
static short indexmapsize[MAX_PLAYERS];

/**
Clears all data and frees all allocated memory.
*/
void airports_destroy()
{
	struct AIRPORT *ap = airports;
	struct RUNWAY *rnw;

	while (numairports) {
		numairports--;
		free(ap->runways);
		ap++;
	}
        free(airports);
	airports = NULL;
}

/**
Loads airports and runways (and init other things).
*/
void airports_init(AMX *amx)
{
	int cacheid, rowcount, lastap, i;
	struct AIRPORT *ap;
	struct RUNWAY *rnw;

	if (airports != NULL) {
		logprintf("airport_init(): table was not empty");
		airports_destroy();
	}

	/*init indexmap*/
	i = MAX_PLAYERS;
	while (i--) {
		indexmap[i] = NULL;
	}

	/*load airports*/
	amx_SetUString(buf144,
		"SELECT c,e,n,b,x,y,z,flags,spawnx,spawny,spawnz,spawnr "
		"FROM apt ORDER BY i ASC", 144);
	NC_mysql_query_(buf144a, &cacheid);
	NC_cache_get_row_count_(&rowcount);
	if (!rowcount) {
		goto noairports;
	}
	numairports = rowcount;
	airports = malloc(sizeof(struct AIRPORT) * numairports);
	while (rowcount--) {
		ap = airports + rowcount;
		ap->runways = ap->runwaysend = NULL;
		ap->missionpoints = NULL;
		ap->missiontypes = 0;
		ap->id = rowcount;
		NC_cache_get_field_str(rowcount, 0, buf144a);
		amx_GetUString(ap->code, buf144, sizeof(ap->code));
		NC_cache_get_field_int(rowcount, 1, &nc_result);
		ap->enabled = (char) nc_result;
		NC_cache_get_field_str(rowcount, 2, buf144a);
		amx_GetUString(ap->name, buf144, sizeof(ap->name));
		NC_cache_get_field_str(rowcount, 3, buf144a);
		amx_GetUString(ap->beacon, buf144, sizeof(ap->beacon));
		NC_cache_get_field_flt(rowcount, 4, ap->pos.x);
		NC_cache_get_field_flt(rowcount, 5, ap->pos.y);
		NC_cache_get_field_flt(rowcount, 6, ap->pos.z);
		NC_cache_get_field_int(rowcount, 7, &ap->flags);
		if (ap->flags & (APT_FLAG_SPAWNHERE | APT_FLAG_SPAWNHEREMIL)) {
			NC_cache_get_field_flt(rowcount, 8, ap->spawnz);
			NC_cache_get_field_flt(rowcount, 9, ap->spawny);
			NC_cache_get_field_flt(rowcount, 10, ap->spawnz);
			NC_cache_get_field_flt(rowcount, 11, ap->spawnr);
		}
	}
noairports:
	NC_cache_delete(cacheid);

	/*load runways*/
	amx_SetUString(buf144,
		"SELECT a,h,s,x,y,z,n "
		"FROM rnw "
		"WHERE type="EQ(RUNWAY_TYPE_RUNWAY)" "
		"ORDER BY a ASC", 144);
	NC_mysql_query_(buf144a, &cacheid);
	NC_cache_get_row_count_(&rowcount);
	if (!rowcount) {
		goto norunways;
	}
	rowcount--;
	while (rowcount >= 0) {
		NC_cache_get_field_int(rowcount, 0, &lastap);
		ap = airports + lastap;
		/*look 'ahead' to see how many runways there are*/
		i = rowcount - 1;
		while (i > 0) {
			i--;
			NC_cache_get_field_int(i, 0, &nc_result);
			if (nc_result != lastap) {
				break;
			}
		}
		/*gottem*/
		i = rowcount - i;
		ap->runways = rnw = malloc(sizeof(struct RUNWAY) * i);
		ap->runwaysend = ap->runways + i;
		while (i--) {
			NC_cache_get_field_flt(rowcount, 1, rnw->heading);
			rnw->headingr = rnw->heading * DEG_TO_RAD;
			sprintf(rnw->id, "%02.0f", rnw->heading / 10.0f);
			NC_cache_get_field_str(rowcount, 2, buf32a);
			rnw->id[2] = (char) buf32[0];
			rnw->id[3] = 0;
			NC_cache_get_field_flt(rowcount, 3, rnw->pos.x);
			NC_cache_get_field_flt(rowcount, 4, rnw->pos.y);
			NC_cache_get_field_flt(rowcount, 5, rnw->pos.z);
			NC_cache_get_field_int(rowcount, 6, &rnw->nav);
			rnw++;
			rowcount--;
		}
	}
norunways:
	NC_cache_delete(cacheid);

#ifdef TEST_AIRPORT_LOADING
	ap = airports;
	i = numairports;
	while (i--) {
		printf("airport %d: %s code %s beacon %s\n", ap->id, ap->name,
			ap->code, ap->beacon);
		printf(" runwaycount: %d\n", ap->runwaysend - ap->runways);
		rnw = ap->runways;
		while (rnw != ap->runwaysend) {
			printf("  runway %s nav %d\n", rnw->id, rnw->nav);
			rnw++;
		}
		ap++;
	}
#endif
}

struct APREF {
	float distance;
	int index;
};

static int sortaprefs(const void *_a, const void *_b)
{
	struct APREF *a = (struct APREF*) _a, *b = (struct APREF*) _b;
	return (int) (10.0f * (b->distance - a->distance));
}

/* native APT_FormatNearestList(playerid, Float:x, Float:y, buf[]) */
cell AMX_NATIVE_CALL APT_FormatNearestList(AMX *amx, cell *params)
{
	cell *addr;
	int i = 0, idx = 0, pid = params[1];
	int enabledairportscount = 0;
	float dx, dy;
	char buf[4096];
	struct AIRPORT *ap;
	struct APREF *aps = malloc(sizeof(struct APREF) * numairports);
	
	while (i < numairports) {
		if (airports[i].enabled) {
			dx = airports[i].pos.x - amx_ctof(params[2]);
			dy = airports[i].pos.y - amx_ctof(params[3]);
			aps[enabledairportscount].distance = sqrt(dx * dx + dy * dy);
			aps[enabledairportscount].index = i;
			enabledairportscount++;
		}
		i++;
	}
	qsort(aps, enabledairportscount, sizeof(struct APREF), sortaprefs);
	if (indexmap[pid] == NULL) {
		indexmap[pid] = malloc(sizeof(indexmap[0]) * enabledairportscount);
	}
	i = indexmapsize[pid] = enabledairportscount;
	while (i--) {
		*(indexmap[pid] + enabledairportscount - i - 1) = aps[i].index;
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
	struct AIRPORT *ap = airports;
	int count = numairports, idx = 0;

	while (count-- > 0) {
		if (ap->enabled) {
			idx += sprintf(buf + idx, " %s", ap->beacon);
		}
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

	if (params[2] < 0 || indexmapsize[pid] <= params[2]) {
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
	struct AIRPORT *ap;
	struct RUNWAY *rnw;
	cell *addr;
	char buf[4096];
	char szRunways[] = "Runways:";

	amx_GetAddr(amx, params[2], &addr);
	if (idx < 0 || numairports <= idx) {
		buf[0] = 0;
		amx_SetUString(addr, buf, 2);
		return 0;
	}
	ap = airports + idx;
	idx = sprintf(buf, "\nElevation:\t%.0f FT", ap->pos.z);
	idx += sprintf(buf + idx, "\nBeacon:\t%s", ap->beacon);
	rnw = ap->runways;
	while (rnw != ap->runwaysend) {
		idx += sprintf(buf + idx, "\n%s\t%s", szRunways, rnw->id);
		if (rnw->nav == (NAV_VOR | NAV_ILS)) {
			idx += sprintf(buf + idx, " (VOR+ILS)");
		} else if (rnw->nav) {
			idx += sprintf(buf + idx, " (VOR)");
		}
		rnw++;
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
	if (idx < 0 || numairports <= idx) {
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
