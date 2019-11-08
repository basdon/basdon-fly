
/* vim: set filetype=c ts=8 noexpandtab: */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include "common.h"
#include "airport.h"
#include "cmd.h"
#include "dialog.h"
#include <string.h>
#include <math.h>

struct AIRPORT *airports = NULL;
int numairports = 0;

/**
Map of airports corresponding to nearest airports from /nearest cmd.
*/
static struct AIRPORT **nearest_airports_map[MAX_PLAYERS];
/**
Amount of airports in the nearest_airports_map array.
*/
static short nearest_airports_map_size[MAX_PLAYERS];

/**
Clears all data and frees all allocated memory.
*/
void airports_destroy()
{
	struct AIRPORT *ap = airports;

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
		nearest_airports_map[i] = NULL;
	}

	/*load airports*/
	amx_SetUString(buf144,
		"SELECT c,e,n,b,x,y,z,flags "
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
	}
noairports:
	NC_cache_delete(cacheid);

	/*load runways*/
	amx_SetUString(buf144,
		"SELECT a,h,s,x,y,z,n,type "
		"FROM rnw "
		/*"WHERE type="EQ(RUNWAY_TYPE_RUNWAY)" "*/
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
			NC_cache_get_field_int(rowcount, 7, &rnw->type);
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
			printf("  runway %s type %d nav %d\n",
				rnw->id, rnw->type, rnw->nav);
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

/**
The /nearest command, shows a list dialog with airports.

Airports are sorted by distance from player. Choosing an airport shows an
additional dialog with information about the selected airport.
*/
int airport_cmd_nearest(CMDPARAMS)
{
	static const char *NONE = WARN"No airports!";

	int i = 0;
	int num = 0;
	float playerx, playery, dx, dy;
	char buf[4096], *b;
	struct AIRPORT *ap;
	struct AIRPORT **map;
	struct APREF *aps = malloc(sizeof(struct APREF) * numairports);

	NC_GetPlayerPos(playerid, buf32a, buf64a, buf144a);
	playerx = *((float*) buf32);
	playery = *((float*) buf64);

	while (i < numairports) {
		if (airports[i].enabled) {
			dx = airports[i].pos.x - playerx;
			dy = airports[i].pos.y - playery;
			aps[num].distance = sqrtf(dx * dx + dy * dy);
			aps[num].index = i;
			num++;
		}
		i++;
	}
	if (num == 0) {
		amx_SetUString(buf144, NONE, 144);
		NC_SendClientMessage(playerid, COL_WARN, buf144a);
		return 1;
	}
	qsort(aps, num, sizeof(struct APREF), sortaprefs);
	map = nearest_airports_map[playerid];
	if (map == NULL) {
		map = malloc(sizeof(struct AIRPORT*) * num);
		nearest_airports_map[playerid] = map;
	}
	i = nearest_airports_map_size[playerid] = num;
	b = buf;
	while (i--) {
		*map = ap = airports + aps[i].index;
		map++;
		if (aps[i].distance < 1000.0f) {
			b += sprintf(b, "\n%.0f", aps[i].distance);
		} else {
			b += sprintf(b, "\n%.1fK", aps[i].distance / 1000.0f);
		}
		b += sprintf(b, "\t%s\t[%s]", ap->name, ap->code);
	}

	dialog_NC_ShowPlayerDialog(
		amx,
		playerid,
		DIALOG_AIRPORT_NEAREST,
		DIALOG_STYLE_TABLIST,
		"Nearest airports",
		buf + 1,
		"Info",
		"Close",
		-1);

	free(aps);
	return 1;
}

/**
The /beacons command, shows a dialog with list of all airport beacons.
*/
int airport_cmd_beacons(CMDPARAMS)
{
	char buf[4096], *b = buf;
	struct AIRPORT *ap = airports;
	int count = numairports;

	while (count-- > 0) {
		if (ap->enabled) {
			b += sprintf(b, " %s", ap->beacon);
		}
		ap++;
	}
	if (b == buf) {
		strcpy(buf, " None!");
	}
	dialog_NC_ShowPlayerDialog(
		amx,
		playerid,
		DIALOG_DUMMY,
		DIALOG_STYLE_MSGBOX,
		"Beacons",
		buf + 1,
		"Close",
		NULL,
		-1);
	return 1;
}

/**
Cleanup stored stuff for player when they disconnect.
*/
void airport_on_player_disconnect(int playerid)
{
	if (nearest_airports_map[playerid] != NULL) {
		free(nearest_airports_map[playerid]);
		nearest_airports_map[playerid] = NULL;
	}
}

/**
Call when getting a response from DIALOG_AIRPORT_NEAREST, to show info dialog.
*/
void airport_list_dialog_response(AMX *amx, int playerid, int response, int idx)
{
	struct AIRPORT *ap;
	struct RUNWAY *rnw;
	int helipads = 0;
	char title[64], info[4096], *b;
	char szRunways[] = "Runways:";

	if (nearest_airports_map[playerid] == NULL) {
		return;
	}
	if (!response ||
		idx < 0 ||
		nearest_airports_map_size[playerid] <= idx)
	{
		goto freereturn;
	}
	ap = nearest_airports_map[playerid][idx];

	sprintf(title, "%s - %s", ap->code, ap->name);

	b = info;
	b += sprintf(b, "\nElevation:\t%.0f FT", ap->pos.z);
	b += sprintf(b, "\nBeacon:\t%s", ap->beacon);
	rnw = ap->runways;
	while (rnw != ap->runwaysend) {
		if (rnw->type == RUNWAY_TYPE_HELIPAD) {
			helipads++;
		} else {
			b += sprintf(b, "\n%s\t%s", szRunways, rnw->id);
			if (rnw->nav == (NAV_VOR | NAV_ILS)) {
				b += sprintf(b, " (VOR+ILS)");
			} else if (rnw->nav) {
				b += sprintf(b, " (VOR)");
			}
			szRunways[0] = '\t';
			szRunways[1] = 0;
		}
		rnw++;
	}
	b += sprintf(b, "\nHelipads:\t%d", helipads);

	dialog_NC_ShowPlayerDialog(
		amx,
		playerid,
		DIALOG_DUMMY,
		DIALOG_STYLE_MSGBOX,
		title,
		info + 1,
		"Close",
		NULL,
		-1);

freereturn:
	free(nearest_airports_map[playerid]);
	nearest_airports_map[playerid] = NULL;
}
