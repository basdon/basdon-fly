
/* vim: set filetype=c ts=8 noexpandtab: */

#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <string.h>
#include "common.h"

#define MAP_FILENAMEFORMAT "scriptfiles/maps/%s.map"
#define MAP_MAX_FILENAME (24) /*see db col*/
/*define to print msg to console each time map streams in/out*/
/*#define MAPS_LOG_STREAMING*/
#define MAX_REMOVED_OBJECTS 1000

#pragma pack(push,1)
struct OBJECT {
	int model;
	cell x, y, z, rx, ry, rz, drawdistance;
};
struct REMOVEDOBJECT {
	cell model, x, y, z, radius;
};
#pragma pack(pop)

struct MAP {
	int id;
	float x, y;
	float radius_in_sq, radius_out_sq;
	char name[64];
	int numobjects;
	struct OBJECT *objects;
	/* 1 streamed in, 0 streamed out */
	char streamstatus[MAX_PLAYERS];
};

static int nummaps;
static struct MAP *maps;
/*maps object id to a map id*/
/*using MAX_OBJECTS+1 because object ideas start at 1*/
static int object_map_id[MAX_PLAYERS][MAX_OBJECTS+1];
static struct REMOVEDOBJECT removedobjects[MAX_REMOVED_OBJECTS];
static int numremovedobjects = 0;

int maps_load_from_file(struct MAP *map)
{
	const float forceddrawdistance = 1000.0f;
	FILE *filehandle;
	char filename[22 + MAP_MAX_FILENAME];
	int specversion;
	int numobjects, numrem;
	struct OBJECT *obj;
	struct REMOVEDOBJECT *remobj;

	sprintf_s(filename, sizeof(filename), MAP_FILENAMEFORMAT, map->name);
	if (!(filehandle = fopen(filename, "rb"))) {
		logprintf("failed to open map %s", filename);
		return 0;
	}

	if (fread(&specversion, sizeof(int), 1, filehandle) != 1) {
		goto corrupted;
	}

	if (specversion != 1) {
		logprintf("unknown map spec v%d for %s", specversion, filename);
		goto exitzero;
	}
	if (fread(&numrem, sizeof(int), 1, filehandle) != 1 ||
		fread(&numobjects, sizeof(int), 1, filehandle) != 1 ||
		numobjects < 0 || 900 < numobjects ||
		numrem < 0 || 100 < numrem)
	{
		goto corrupted;
	}
	if (numrem + numremovedobjects < MAX_REMOVED_OBJECTS) {
		remobj = removedobjects + numremovedobjects;
		while (numrem--) {
			if (fread(remobj, sizeof(struct REMOVEDOBJECT), 1,
				filehandle) != 1)
			{
				if (feof(filehandle)) {
					goto allread;
				} else {
					goto corrupted;
				}
			}
			/* as per spec */
			remobj->model = -remobj->model;
			/*coordinates are kept as cells to use as params as is*/
			remobj->x = amx_ftoc(remobj->x);
			remobj->y = amx_ftoc(remobj->y);
			remobj->z = amx_ftoc(remobj->z);
			remobj->radius = amx_ftoc(remobj->radius);
			remobj++;
			numremovedobjects++;
		}
	} else {
		logprintf("ERR: MAX_REMOVED_OBJECTS limit hit!");
	}
	if (numobjects == 0) {
		obj = map->objects = NULL;
		goto allread;
	}
	obj = map->objects = malloc(sizeof(struct OBJECT) * numobjects);
	do {
		if (fread(obj, sizeof(struct OBJECT), 1, filehandle) != 1) {
			if (feof(filehandle)) {
				goto allread;
			} else {
				free(map->objects);
				goto corrupted;
			}
		}
		/*coordinates are kept as cells to use as params as is*/
		obj->x = amx_ftoc(obj->x);
		obj->y = amx_ftoc(obj->y);
		obj->z = amx_ftoc(obj->z);
		obj->rx = amx_ftoc(obj->rx);
		obj->ry = amx_ftoc(obj->ry);
		obj->rz = amx_ftoc(obj->rz);
		obj->drawdistance = amx_ftoc(obj->drawdistance);
		obj->drawdistance = amx_ftoc(forceddrawdistance);
		obj++;
	} while (obj - map->objects < 900);
allread:
	map->numobjects = obj - map->objects;

	fclose(filehandle);
	return 1;
corrupted:
	logprintf("corrupted map %s", filename);
exitzero:
	fclose(filehandle);
	return 0;
}

void maps_load_from_db(AMX *amx)
{
	char q[] =
		"SELECT id,filename,midx,midy,radi,rado FROM map "
		"LEFT JOIN apt ON map.ap=apt.i "
		"WHERE apt.e=1 OR apt.e IS NULL";
	cell cacheid, rowcount;
	struct MAP *map, *m;
	const struct MAP *end;

	if (sizeof(struct OBJECT) != sizeof(cell) * 8) {
		logprintf("ERR: struct OBJECT invalid size");
		nummaps = 0;
		return;
	}
	if (sizeof(cell) != 4) {
		logprintf("ERR: sizeof cell is not 4");
		nummaps = 0;
		return;
	}

	amx_SetUString(buf4096, q, sizeof(q));
	NC_mysql_query_(buf4096a, &cacheid);
	NC_cache_get_row_count_(&rowcount);
	nummaps = rowcount;
	if (rowcount) {
		maps = malloc(nummaps * sizeof(struct MAP));
		while (rowcount--) {
			map = maps + rowcount;
			NC_cache_get_field_int(rowcount, 0, (cell*) &map->id);
			NC_cache_get_field_str(rowcount, 1, buf32a);
			amx_GetUString(map->name, buf32, sizeof(map->name));
			if (!maps_load_from_file(map)) {
				map->id = -1;
				continue;
			}
			NC_cache_get_field_flt(rowcount, 2, map->x);
			NC_cache_get_field_flt(rowcount, 3, map->y);
			NC_cache_get_field_flt(rowcount, 4, map->radius_in_sq);
			NC_cache_get_field_flt(rowcount, 5, map->radius_out_sq);
			map->radius_in_sq *= map->radius_in_sq;
			map->radius_out_sq *= map->radius_out_sq;
		}
	}
	NC_cache_delete(cacheid);

	/*remove gaps caused by maps that failed to load*/
	end = maps + nummaps;
	map = m = maps;
	while (map < end) {
		if (map->id == -1) {
			nummaps--;
		} else {
			if (map != m) {
				memcpy(m, map, sizeof(struct MAP));
			}
			m++;
		}
		map++;
	}
}

void maps_stream_in_for_player(AMX *amx, int playerid, struct MAP *map)
{
	const int mapid = map->id;
	int *player_object_map_id = object_map_id[playerid];
	struct OBJECT *obj = map->objects, *end = obj + map->numobjects;

	nc_params[0] = 9;
	nc_params[1] = playerid;
	while (obj != end) {
		memcpy(nc_params + 2, obj, sizeof(cell) * 8);
		NC(n_CreatePlayerObject);
		if (nc_result == INVALID_OBJECT_ID) {
			logprintf(
				"obj limit hit while streaming map %s",
				map->name);
			break;
		}
		player_object_map_id[nc_result] = mapid;
		obj++;
	}
}

void maps_stream_out_for_player(AMX *amx, int playerid, struct MAP *map)
{
	const int mapid = map->id;
	int *start = object_map_id[playerid], *player_object_map_id = start;
	const int *end = object_map_id[playerid] + MAX_OBJECTS + 1;

	nc_params[0] = 2;
	nc_params[1] = playerid;
	while (player_object_map_id != end) {
		if (*player_object_map_id == mapid) {
			nc_params[2] = player_object_map_id - start;
			NC(n_DestroyPlayerObject);
			*player_object_map_id = -1;
		}
		player_object_map_id++;
	}
}

void maps_stream_for_player(AMX *amx, int playerid)
{
	float dx, dy, dist;
	struct MAP *map = maps + nummaps;

	NC_GetPlayerPos(playerid, buf32a, buf64a, buf144a);
	while (map-- != maps) {
		dx = map->x - amx_ctof(*buf32);
		dy = map->y - amx_ctof(*buf64);
		dist = dx * dx + dy * dy;
		if (map->streamstatus[playerid]) {
			if (dist > map->radius_out_sq) {
				map->streamstatus[playerid] = 0;
				maps_stream_out_for_player(amx, playerid, map);
#ifdef MAPS_LOG_STREAMING
				logprintf("map %s streamed out for %d",
					map->name,
					playerid);
#endif
			}
		} else  {
			if (dist < map->radius_in_sq) {
				map->streamstatus[playerid] = 1;
				maps_stream_in_for_player(amx, playerid, map);
#ifdef MAPS_LOG_STREAMING
				logprintf("map %s streamed in for %d",
					map->name,
					playerid);
#endif
			}
		}
	}
}

/*
Go over players to see if maps need to be streamed for them
This function should be called from loop100
It checks 1+playercount/10 amount of players per call, so:

 playercount | checked players | ms between ticks
     1-9              1              100-900
    10-19             2              500-950
    20-29             3              700-966
   100-109           11	            1000-1000
 (not sure if this is correct; I'm too tired)
*/
void maps_process_tick(AMX *amx)
{
	static int currentplayeridx = 0;
	int increment = 1 + playercount / 10;

	if (playercount > 0) {
		while (increment--) {
			if (++currentplayeridx >= playercount) {
				currentplayeridx = 0;
			}
			maps_stream_for_player(amx, players[currentplayeridx]);
		}
	}
}

void maps_OnPlayerConnect(AMX *amx, int playerid)
{
	struct REMOVEDOBJECT *r = removedobjects + numremovedobjects;

	while (r-- != removedobjects) {
		nc_params[0] = 6;
		nc_params[1] = playerid;
		memcpy(nc_params + 2, r, sizeof(cell) * 5);
		NC(n_RemoveBuildingForPlayer);
	}
}

void maps_OnPlayerDisconnect(int playerid)
{
	struct MAP *map = maps + nummaps;
	while (map-- != maps) {
		map->streamstatus[playerid] = 0;
	}
}
