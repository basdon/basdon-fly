
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "airport.h"
#include "dialog.h"
#include "math.h"
#include <string.h>

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

void airports_init()
{
	int cacheid, rowcount, lastap, i, *field = nc_params + 2;
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
		"SELECT c,n,b,e,x,y,z,flags "
		"FROM apt ORDER BY i ASC", 144);
	cacheid = NC_mysql_query(buf144a);
	rowcount = NC_cache_get_row_count();
	if (!rowcount) {
		goto noairports;
	}
	numairports = rowcount;
	airports = malloc(sizeof(struct AIRPORT) * numairports);
	nc_params[3] = buf144a;
	while (rowcount--) {
		ap = airports + rowcount;
		ap->runways = ap->runwaysend = NULL;
		ap->missionpoints = NULL;
		ap->num_missionpts = 0;
		ap->missiontypes = 0;
		ap->id = rowcount;
		NC_PARS(3);
		nc_params[1] = rowcount;
		*field = 0; NC(n_cache_get_field_s);
		amx_GetUString(ap->code, buf144, sizeof(ap->code));
		*field = 1; NC(n_cache_get_field_s);
		amx_GetUString(ap->name, buf144, sizeof(ap->name));
		*field = 2; NC(n_cache_get_field_s);
		amx_GetUString(ap->beacon, buf144, sizeof(ap->beacon));
		NC_PARS(2);
		ap->enabled = (char) (*field = 3, NC(n_cache_get_field_i));
		ap->pos.x = (*field = 4, NCF(n_cache_get_field_f));
		ap->pos.y = (*field = 5, NCF(n_cache_get_field_f));
		ap->pos.z = (*field = 6, NCF(n_cache_get_field_f));
		ap->flags = (*field = 7, NC(n_cache_get_field_i));
	}
noairports:
	NC_cache_delete(cacheid);

	/*load runways*/
	amx_SetUString(buf144,
		"SELECT a,x,y,z,n,type,h,s "
		"FROM rnw "
		/*"WHERE type="EQ(RUNWAY_TYPE_RUNWAY)" "*/
		"ORDER BY a ASC", 144);
	cacheid = NC_mysql_query(buf144a);
	rowcount = NC_cache_get_row_count();
	if (!rowcount) {
		goto norunways;
	}
	rowcount--;
	while (rowcount >= 0) {
		lastap = NC_cache_get_field_int(rowcount, 0);
		ap = airports + lastap;
		/*look 'ahead' to see how many runways there are*/
		i = rowcount - 1;
		nc_params[2] = 1;
		while (i > 0) {
			i--;
			nc_params[1] = i;
			if (NC(n_cache_get_field_i) != lastap) {
				break;
			}
		}

		/*gottem*/
		i = rowcount - i;
		ap->runways = rnw = malloc(sizeof(struct RUNWAY) * i);
		ap->runwaysend = ap->runways + i;
		nc_params[1] = rowcount;
		nc_params[3] = buf32a;
		while (i--) {
			NC_PARS(2);
			nc_params[1] = rowcount;
			rnw->pos.x = (*field = 1, NCF(n_cache_get_field_f));
			rnw->pos.y = (*field = 2, NCF(n_cache_get_field_f));
			rnw->pos.z = (*field = 3, NCF(n_cache_get_field_f));
			rnw->nav = (*field = 4, NC(n_cache_get_field_i));
			rnw->type = (*field = 5, NC(n_cache_get_field_i));
			rnw->heading = (*field = 6, NCF(n_cache_get_field_f));
			rnw->headingr = rnw->heading * DEG_TO_RAD;
			NC_PARS(3);
			*field = 7; NC(n_cache_get_field_s);
			sprintf(rnw->id, "%02.0f", rnw->heading / 10.0f);
			rnw->id[2] = (char) buf32[0];
			rnw->id[3] = 0;
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

/**
Structure used for sorting airports by distance for the /nearest list dialog.
*/
struct APREF {
	float distance;
	int index;
};

/**
Sort function used to sort airports by distance for the /nearest list dialog.
*/
static
int sortaprefs(const void *_a, const void *_b)
{
	struct APREF *a = (struct APREF*) _a, *b = (struct APREF*) _b;
	return (int) (10.0f * (b->distance - a->distance));
}

int airport_cmd_nearest(CMDPARAMS)
{
	static const char *NONE = WARN"No airports!";

	int i = 0;
	int num = 0;
	struct vec3 playerpos;
	float dx, dy;
	char buf[4096], *b;
	struct AIRPORT *ap;
	struct AIRPORT **map;
	struct APREF *aps = malloc(sizeof(struct APREF) * numairports);

	common_GetPlayerPos(playerid, &playerpos);

	while (i < numairports) {
		if (airports[i].enabled) {
			dx = airports[i].pos.x - playerpos.x;
			dy = airports[i].pos.y - playerpos.y;
			aps[num].distance = sqrt(dx * dx + dy * dy);
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

void airport_on_player_disconnect(int playerid)
{
	if (nearest_airports_map[playerid] != NULL) {
		free(nearest_airports_map[playerid]);
		nearest_airports_map[playerid] = NULL;
	}
}

void airport_list_dialog_response(int playerid, int response, int idx)
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
