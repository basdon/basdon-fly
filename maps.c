
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "maps.h"
#include <stdio.h>
#include <string.h>

#define MAP_FILENAMEFORMAT "scriptfiles/maps/%s.map"
#define MAP_MAX_FILENAME (24) /*see db col*/
/*define to print msg to console each time map streams in/out*/
/*#define MAPS_LOG_STREAMING*/
#define MAX_REMOVED_OBJECTS 1000

#pragma pack(push,1)
struct OBJECT {
	cell model;
	float x, y, z, rx, ry, rz, drawdistance;
};
struct REMOVEDOBJECT {
	cell model;
	float x, y, z, radius;
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

/**
Load a map from file as specified in given map.

@return 1 on success
*/
static
int maps_load_from_file(struct MAP *map)
{
	const float forceddrawdistance = 1000.0f;
	FILE *filehandle;
	char filename[22 + MAP_MAX_FILENAME];
	int specversion;
	int numobjects, numrem;
	struct OBJECT *obj;
	struct REMOVEDOBJECT *remobj;

	sprintf(filename, MAP_FILENAMEFORMAT, map->name);
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
			if (remobj->model != -1) {
				remobj->model = -remobj->model;
			}
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
		/*TODO: this forceddrawdistance thing*/
		obj->drawdistance = forceddrawdistance;
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

void maps_load_from_db()
{
	char q[] =
		"SELECT filename,id,midx,midy,radi,rado FROM map "
		"LEFT JOIN apt ON map.ap=apt.i "
		"WHERE apt.e=1 OR apt.e IS NULL";
	int cacheid, rowcount, *f = nc_params + 2;
	struct MAP *map, *m;
	const struct MAP *end;

	if (sizeof(struct OBJECT) != sizeof(cell) * 8) {
		logprintf("ERR: struct OBJECT invalid size");
		nummaps = 0;
		return;
	}

	atoc(buf4096, q, sizeof(q));
	cacheid = NC_mysql_query(buf4096a);
	rowcount = NC_cache_get_row_count();
	nummaps = rowcount;
	if (rowcount) {
		maps = malloc(nummaps * sizeof(struct MAP));
		nc_params[3] = buf32a;
		while (rowcount--) {
			map = maps + rowcount;
			NC_PARS(3);
			nc_params[1] = rowcount;
			*f = 0; NC(n_cache_get_field_s);
			ctoa(map->name, buf32, sizeof(map->name));
			if (!maps_load_from_file(map)) {
				map->id = -1;
				continue;
			}
			NC_PARS(2);
			map->id = (*f = 1, NC(n_cache_get_field_i));
			map->x = (*f = 2, NCF(n_cache_get_field_f));
			map->y = (*f = 3, NCF(n_cache_get_field_f));
			map->radius_in_sq = (*f = 4, NCF(n_cache_get_field_f));
			map->radius_out_sq = (*f = 5, NCF(n_cache_get_field_f));
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

/**
Creates all objects in given map for the given player.
*/
static
void maps_stream_in_for_player(int playerid, struct MAP *map)
{
	const int mapid = map->id;
	int *player_object_map_id = object_map_id[playerid], objectid;
	struct OBJECT *obj = map->objects, *end = obj + map->numobjects;

	NC_PARS(9);
	nc_params[1] = playerid;
	while (obj != end) {
		memcpy(nc_params + 2, obj, sizeof(cell) * 8);
		objectid = NC(n_CreatePlayerObject);
		if (objectid == INVALID_OBJECT_ID) {
			logprintf(
				"obj limit hit while streaming map %s",
				map->name);
			break;
		}
		player_object_map_id[objectid] = mapid;
		obj++;
	}
}

/**
Deletes all the objects from given map for the given player.
*/
static
void maps_stream_out_for_player(int playerid, struct MAP *map)
{
	const int mapid = map->id;
	int *start = object_map_id[playerid], *player_object_map_id = start;
	const int *end = object_map_id[playerid] + MAX_OBJECTS + 1;

	NC_PARS(2);
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

void maps_stream_for_player(int playerid, struct vec3 pos)
{
	float dx, dy, dist;
	struct MAP *map = maps + nummaps;

	while (map-- != maps) {
		dx = map->x - pos.x;
		dy = map->y - pos.y;
		dist = dx * dx + dy * dy;
		if (map->streamstatus[playerid]) {
			if (dist > map->radius_out_sq) {
				map->streamstatus[playerid] = 0;
				maps_stream_out_for_player(playerid, map);
#ifdef MAPS_LOG_STREAMING
				logprintf("map %s streamed out for %d",
					map->name,
					playerid);
#endif
			}
		} else  {
			if (dist < map->radius_in_sq) {
				map->streamstatus[playerid] = 1;
				maps_stream_in_for_player(playerid, map);
#ifdef MAPS_LOG_STREAMING
				logprintf("map %s streamed in for %d",
					map->name,
					playerid);
#endif
			}
		}
	}
}

void maps_process_tick()
{
	static int currentplayeridx = 0;
	int increment = 1 + playercount / 10;
	int playerid;
	struct vec3 ppos;

	if (playercount > 0) {
		while (increment--) {
			if (++currentplayeridx >= playercount) {
				currentplayeridx = 0;
			}
			playerid = players[currentplayeridx];
			common_GetPlayerPos(playerid, &ppos);
			maps_stream_for_player(playerid, ppos);
		}
	}
}

void maps_on_player_connect(int playerid)
{
	struct REMOVEDOBJECT *r = removedobjects + numremovedobjects;

	while (r-- != removedobjects) {
		NC_PARS(6);
		nc_params[1] = playerid;
		memcpy(nc_params + 2, r, sizeof(cell) * 5);
		NC(n_RemoveBuildingForPlayer);
	}
}

void maps_on_player_disconnect(int playerid)
{
	struct MAP *map = maps + nummaps;
	while (map-- != maps) {
		map->streamstatus[playerid] = 0;
	}
}
