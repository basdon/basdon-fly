
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "maps.h"
#include <stdio.h>
#include <string.h>

#define MAX_MAPS (20)
#define MAP_MAX_FILENAME (24) /*see db col*/
/*define to print msg to console each time map streams in/out*/
#define MAPS_LOG_STREAMING
/*define to show total amount of maps/objects/removes/zones at boot*/
#define MAPS_PRINT_STATS
#define MAX_REMOVED_OBJECTS 1000

#pragma pack(1)
struct MAPFILEHEADER {
	int spec_version;
	int num_removes;
	int num_objects;
	int num_gang_zones;
	float stream_in_radius;
	float stream_out_radius;
	float draw_radius;
};
struct MAP_FILE_OBJECT {
	int model;
	float x, y, z, rx, ry, rz;
};
struct OBJECT {
	struct MAP_FILE_OBJECT map_file_object;
	float drawdistance;
};
EXPECT_SIZE(struct OBJECT, sizeof(cell) * 8);
struct REMOVEDOBJECT {
	int model;
	float x, y, z, radius;
};
EXPECT_SIZE(struct REMOVEDOBJECT, sizeof(cell) * 5);
struct MAP_FILE_GANG_ZONE {
	float min_x;
	float max_y;
	float max_x;
	float min_y;
	int colorABGR;
};
EXPECT_SIZE(struct MAP_FILE_GANG_ZONE, 5 * 4);
/*this has to be synced with the SetGangZone RPC data*/
struct GANG_ZONE {
	float min_x;
	float min_y;
	float max_x;
	float max_y;
	int colorABGR;
};
EXPECT_SIZE(struct GANG_ZONE, 5 * 4);
#pragma pack()

struct MAP {
	int id;
	float middle_x, middle_y;
	float stream_in_radius_sq, stream_out_radius_sq;
	/*TODO: for fun we can add a command to change the draw distance at runtime*/
	char name[MAP_MAX_FILENAME];
	/*unallocated when num_objects is 0*/
	struct OBJECT *objects;
	int num_objects;
	/*unallocated when num_gang_zones is 0*/
	struct GANG_ZONE *gang_zones;
	int num_gang_zones;
	/* 1 streamed in, 0 streamed out */
	char stream_status_for_player[MAX_PLAYERS];
};

static struct MAP maps[MAX_MAPS];
static int num_maps;

/*doing size +1 to prevent an overrun when reading map files*/
static struct REMOVEDOBJECT removed_objects[MAX_REMOVED_OBJECTS + 1];
static int num_removed_objects = 0;

/*maps a player's object id to a map id*/
/*using MAX_OBJECTS+1 because object ids start at 1 (not that we will ever hit 1000 objects but still)*/
static int player_objectid_to_mapid[MAX_PLAYERS][MAX_OBJECTS + 1];

/*maps a player's gang zone id to map idx*/
/*this is (at time of writing) the only system that uses gang zones,
so no checks are being done on a global scale if a gang zone is being used*/
static int player_gangzoneid_to_mapidx[MAX_PLAYERS][MAX_GANG_ZONES];

static struct BitStream bs_maps_show_gang_zone;
static struct RPCDATA_ShowGangZone rpcdata_ShowGangZone;

/**
Load a map from file as specified in given map.

@return 1 on success
*/
static
int maps_load_from_file(int mapidx)
{
	FILE *fs;
	char filename[22 + MAP_MAX_FILENAME];
	int i;
	struct MAPFILEHEADER header;
	struct OBJECT *object;
	struct MAP_FILE_GANG_ZONE map_file_gang_zone;
	struct GANG_ZONE *gang_zone;
	float middle_x, middle_y;
	int total_element_count_for_middle;

	sprintf(filename, "scriptfiles/maps/%s.map", maps[mapidx].name);
	if (!(fs = fopen(filename, "rb"))) {
		logprintf("failed to open map %s", filename);
		return 0;
	}

	if (!fread(&header, sizeof(header), 1, fs)) {
		goto corrupted;
	}

	if (header.spec_version != 0x0250414D) {
		logprintf("unknown map spec %p in %s", header.spec_version, filename);
		goto exitzero;
	}

	maps[mapidx].stream_in_radius_sq = header.stream_in_radius * header.stream_in_radius;
	maps[mapidx].stream_out_radius_sq = header.stream_out_radius * header.stream_out_radius;

	/*could check num_objects num_removes num_zones*/

	for (i = header.num_removes; i > 0; i--) {
		if (!fread(&removed_objects[num_removed_objects], sizeof(struct REMOVEDOBJECT), 1, fs)) {
			goto corrupted;
		}
		if (num_removed_objects == MAX_REMOVED_OBJECTS) {
			logprintf("ERR: MAX_REMOVED_OBJECTS limit hit!");
		} else {
			num_removed_objects++;
		}
	}

	total_element_count_for_middle = 0;
	middle_x = middle_y = 0.0f;

	maps[mapidx].num_objects = header.num_objects;
	if (header.num_objects) {
		maps[mapidx].objects = object = malloc(sizeof(struct OBJECT) * header.num_objects);
		for (i = header.num_objects; i > 0; i--) {
			if (!fread(object, sizeof(struct MAP_FILE_OBJECT), 1, fs)) {
				free(maps[mapidx].objects);
				/*no need to null because the map will be discarded as a whole*/
				goto corrupted;
			}
			total_element_count_for_middle++;
			middle_x += object->map_file_object.x;
			middle_y += object->map_file_object.y;
			object->drawdistance = header.draw_radius;
			object++;
		}
	}

	maps[mapidx].num_gang_zones = header.num_gang_zones;
	if (header.num_gang_zones) {
		maps[mapidx].gang_zones = gang_zone = malloc(sizeof(struct GANG_ZONE) * header.num_gang_zones);
		for (i = header.num_gang_zones; i > 0; i--) {
			if (!fread(&map_file_gang_zone, sizeof(map_file_gang_zone), 1, fs)) {
				free(maps[mapidx].gang_zones);
				if (header.num_objects) {
					free(maps[mapidx].objects);
				}
				/*no need to null because the map will be discarded as a whole*/
				goto corrupted;
			}
			total_element_count_for_middle += 2;
			middle_x += map_file_gang_zone.min_x;
			middle_x += map_file_gang_zone.max_x;
			middle_y += map_file_gang_zone.min_y;
			middle_y += map_file_gang_zone.max_y;
			gang_zone->min_x = map_file_gang_zone.min_x;
			gang_zone->min_y = map_file_gang_zone.min_y;
			gang_zone->max_x = map_file_gang_zone.max_x;
			gang_zone->max_y = map_file_gang_zone.max_y;
			gang_zone->colorABGR = map_file_gang_zone.colorABGR;
			gang_zone++;
		}
	}

	if (total_element_count_for_middle > 0) {
		middle_x /= total_element_count_for_middle;
		middle_y /= total_element_count_for_middle;
	}
	maps[mapidx].middle_x = middle_x;
	maps[mapidx].middle_y = middle_y;

	fclose(fs);
	return 1;
corrupted:
	logprintf("corrupted map %s", filename);
exitzero:
	fclose(fs);
	return 0;
}

#ifdef MAPS_PRINT_STATS
static
void maps_print_stats()
{
	int mapidx;
	int total_objects, total_gang_zones;

	total_objects = total_gang_zones = 0;
	for (mapidx = 0; mapidx < num_maps; mapidx++) {
		total_objects += maps[mapidx].num_objects;
		total_gang_zones += maps[mapidx].num_gang_zones;
	}
	printf("%d maps\n", num_maps);
	printf("  %d objects\n", total_objects);
	printf("  %d removes\n", num_removed_objects);
	printf("  %d gang zones\n", total_gang_zones);
}
#endif

static
void maps_load_from_db()
{
	char q[] =
		"SELECT filename,id FROM map "
		"LEFT JOIN apt ON map.ap=apt.i "
		"WHERE apt.e=1 OR apt.e IS NULL";
	int cacheid, rowcount;
	int i;

	atoc(buf4096, q, sizeof(q));
	cacheid = NC_mysql_query(buf4096a);
	rowcount = NC_cache_get_row_count();
	if (rowcount > MAX_MAPS) {
		logprintf("can't load all maps! missing %d slots", rowcount - MAX_MAPS);
		rowcount = MAX_MAPS;
	}
	num_maps = 0;
	for (i = 0; i < rowcount; i++) {
		nc_params[3] = buf32a;
		while (rowcount--) {
			NC_PARS(3);
			nc_params[1] = rowcount;
			nc_params[2] = 0;
			NC(n_cache_get_field_s);
			ctoa(maps[num_maps].name, buf32, sizeof(maps[rowcount].name));
			if (maps_load_from_file(num_maps)) {
				NC_PARS(2);
				nc_params[2] = 1;
				maps[num_maps].id = NC(n_cache_get_field_i);
				num_maps++;
			}
		}
	}
	NC_cache_delete(cacheid);

#ifdef MAPS_PRINT_STATS
	maps_print_stats();
#endif
}

void maps_init()
{
	maps_load_from_db();

	bs_maps_show_gang_zone.numberOfBitsUsed = sizeof(rpcdata_ShowGangZone) * 8;
	bs_maps_show_gang_zone.numberOfBitsAllocated = sizeof(rpcdata_ShowGangZone) * 8;
	bs_maps_show_gang_zone.readOffset = 0;
	bs_maps_show_gang_zone.ptrData = &rpcdata_ShowGangZone;
	bs_maps_show_gang_zone.copyData = 1;
}

/**
Creates all objects/gangzones in given map for the given player.
*/
static
void maps_stream_in_for_player(int playerid, int mapidx)
{
	struct OBJECT *obj, *objects_end;
	int *objectid_to_mapid;
	int *gangzoneid_to_mapidx;
	int mapid;
	int objectid;
	int gang_zone_idx;
	int zoneid;

#ifdef MAPS_LOG_STREAMING
	logprintf("map %s streamed in for %d", maps[mapidx].name, playerid);
#endif

	maps[mapidx].stream_status_for_player[playerid] = 1;
	mapid = maps[mapidx].id;

	if (maps[mapidx].num_objects) {
		obj = maps[mapidx].objects;
		objects_end = obj + maps[mapidx].num_objects;
		objectid_to_mapid = player_objectid_to_mapid[playerid];
		NC_PARS(9);
		nc_params[1] = playerid;
		while (obj != objects_end) {
			memcpy(nc_params + 2, obj, sizeof(cell) * 8);
			objectid = NC(n_CreatePlayerObject);
			if (objectid == INVALID_OBJECT_ID) {
				logprintf("obj limit hit while streaming map %s", maps[mapidx].name);
				break;
			}
			objectid_to_mapid[objectid] = mapid;
			obj++;
		}
	}

	zoneid = 0;
	gangzoneid_to_mapidx = player_gangzoneid_to_mapidx[playerid];
	for (gang_zone_idx = maps[mapidx].num_gang_zones; gang_zone_idx > 0;) {
		while (gangzoneid_to_mapidx[zoneid] != -1) {
			zoneid++;
			if (zoneid >= MAX_GANG_ZONES) {
				logprintf("gang zone limit hit while streaming map %s", maps[mapidx].name);
				goto skip_gang_zones;
			}
		}
		gang_zone_idx--;
		gangzoneid_to_mapidx[zoneid] = mapidx;
		rpcdata_ShowGangZone.zoneid = zoneid;
		memcpy(&rpcdata_ShowGangZone.min_x, &maps[mapidx].gang_zones[gang_zone_idx], sizeof(struct GANG_ZONE));
		/*TODO: what's the 2 for? possibly priority*/
		SAMP_SendRPCToPlayer(RPC_ShowGangZone, &bs_maps_show_gang_zone, playerid, 2);
	}
skip_gang_zones:
	;
}

/**
Deletes all the objects/gangzones from given map for the given player.
*/
static
void maps_stream_out_for_player(int playerid, int mapidx)
{
	int *objectid_to_mapid;
	int *gangzoneid_to_mapidx;
	int mapid;
	int objectid;
	int zoneid;

#ifdef MAPS_LOG_STREAMING
	logprintf("map %s streamed out for %d", maps[mapidx].name, playerid);
#endif

	maps[mapidx].stream_status_for_player[playerid] = 0;
	mapid = maps[mapidx].id;

	if (maps[mapidx].num_objects) {
		nc_params[1] = playerid;
		objectid_to_mapid = player_objectid_to_mapid[playerid];
		for (objectid = 0; objectid < MAX_OBJECTS; objectid++) {
			if (objectid_to_mapid[objectid] == mapid) {
				nc_params[2] = objectid; NC(n_DestroyPlayerObject);
			}
		}
	}

	gangzoneid_to_mapidx = player_gangzoneid_to_mapidx[playerid];
	for (zoneid = 0; zoneid < MAX_GANG_ZONES; zoneid++) {
		if (gangzoneid_to_mapidx[zoneid] == mapidx) {
			rpcdata_ShowGangZone.zoneid = zoneid;
			/*TODO: what's the 2 for? possibly priority*/
			SAMP_SendRPCToPlayer(RPC_HideGangZone, &bs_maps_show_gang_zone, playerid, 2);
			gangzoneid_to_mapidx[zoneid] = -1;
		}
	}
}

void maps_stream_for_player(int playerid, struct vec3 pos)
{
	float dx, dy, distance;
	int mapidx;

	/*check everything to stream out first, only then stream in*/

	for (mapidx = 0; mapidx < num_maps; mapidx++) {
		if (maps[mapidx].stream_status_for_player[playerid]) {
			dx = maps[mapidx].middle_x - pos.x;
			dy = maps[mapidx].middle_y - pos.y;
			distance = dx * dx + dy * dy;
			if (distance > maps[mapidx].stream_out_radius_sq) {
				maps_stream_out_for_player(playerid, mapidx);
			}
		}
	}

	for (mapidx = 0; mapidx < num_maps; mapidx++) {
		if (!maps[mapidx].stream_status_for_player[playerid]) {
			dx = maps[mapidx].middle_x - pos.x;
			dy = maps[mapidx].middle_y - pos.y;
			distance = dx * dx + dy * dy;
			if (distance < maps[mapidx].stream_in_radius_sq) {
				maps_stream_in_for_player(playerid, mapidx);
			}
		}
	}
}

/*TODO recheck this*/
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
	int i;

	NC_PARS(6);
	nc_params[1] = playerid;
	for (i = num_removed_objects; i >= 0;) {
		i--;
		memcpy(nc_params + 2, &removed_objects[i], sizeof(struct REMOVEDOBJECT));
		NC(n_RemoveBuildingForPlayer);
	}

	memset(player_gangzoneid_to_mapidx[playerid], -1, sizeof(player_gangzoneid_to_mapidx[playerid]));
}

void maps_on_player_disconnect(int playerid)
{
	int mapidx;

	for (mapidx = num_maps; mapidx > 0; mapidx--) {
		maps[mapidx].stream_status_for_player[playerid] = 0;
	}
}
