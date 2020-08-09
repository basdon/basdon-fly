
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "maps.h"
#include "timer.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#define MAX_MAPS (50)
#define MAP_MAX_FILENAME (34) /*see db col*/
/*define to print msg to console each time map streams in/out*/
#define MAPS_LOG_STREAMING
/*define to show total amount of maps/objects/removes/zones at boot*/
#define MAPS_PRINT_STATS
#define MAX_REMOVED_OBJECTS 1000

/*max amount of objects the mapsystem may use*/
/*these are also the ids, so mapsystem uses ids 0-899
(unless the code changed to not use objectid 0)*/
#define MAX_MAPSYSTEM_OBJECTS (900)
#define ROTATING_RADAR_OBJECT_ID (999)
/*Octavia's actor id*/
#define OCTA_ACTORID 999

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
EXPECT_SIZE(struct REMOVEDOBJECT, sizeof(struct RPCDATA_RemoveBuilding));
/*this has to be synced with the SetGangZone RPC data*/
/*the map file format gang zone is made to be in sync*/
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

/*the rotating radar object for each map, NULL for maps that don't have any*/
/*they are automatically made when a base object is found in a map (7981)*/
static struct OBJECT *map_radar_object_for_map[MAX_MAPS];
static struct OBJECT *map_radar_object_for_player[MAX_PLAYERS];

static struct MAP maps[MAX_MAPS];
static int num_maps;

/*doing size +1 to prevent an overrun when reading map files*/
static struct REMOVEDOBJECT removed_objects[MAX_REMOVED_OBJECTS + 1];
static int num_removed_objects = 0;

/*maps a player's object id to a map idx*/
static short player_objectid_to_mapidx[MAX_PLAYERS][MAX_MAPSYSTEM_OBJECTS];

/*maps a player's gang zone id to map idx*/
/*this is (at time of writing) the only system that uses gang zones,
so no checks are being done on a global scale if a gang zone is being used*/
static short player_gangzoneid_to_mapidx[MAX_PLAYERS][MAX_GANG_ZONES];

/*the mapidx of octavia island map where the actor should be made*/
static int octavia_island_actor_mapidx;

static struct RPCDATA_ShowActor rpcdata_show_actor;
static struct RPCDATA_HideActor rpcdata_hide_actor;
static struct BitStream bitstream_freeform;
static struct BitStream bs_maps_remove_building;
static struct RPCDATA_RemoveBuilding rpcdata_RemoveBuilding;
static struct BitStream bs_maps_create_object;
static struct RPCDATA_CreateObject rpcdata_CreateObject;
static struct BitStream bs_maps_destroy_object;
static struct RPCDATA_DestroyObject rpcdata_DestroyObject;
static struct BitStream bs_maps_show_gang_zone;
static struct RPCDATA_ShowGangZone rpcdata_ShowGangZone;
static struct BitStream bs_maps_move_object;
static struct RPCDATA_MoveObject rpcdata_MoveObject;

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
	struct GANG_ZONE *gang_zone;
	float middle_x, middle_y;
	int total_element_count_for_middle;
	float radar_angle, radar_offset_x, radar_offset_y;

	sprintf(filename, "scriptfiles/maps/%s.map", maps[mapidx].name);
	if (!(fs = fopen(filename, "rb"))) {
		logprintf("failed to open map %s", filename);
		return 0;
	}

	/*read header*/
	if (!fread(&header, sizeof(header), 1, fs)) {
		goto corrupted;
	}

	if (header.spec_version != 0x0250414D) {
		logprintf("unknown map spec %p in %s", header.spec_version, filename);
		goto exitzero;
	}

	maps[mapidx].stream_in_radius_sq = header.stream_in_radius * header.stream_in_radius;
	maps[mapidx].stream_out_radius_sq = header.stream_out_radius * header.stream_out_radius;

	/*TODO? could check if num_objects num_removes num_zones go over their limits*/

	/*read removed objects*/
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

	/*read objects*/
	map_radar_object_for_map[mapidx] = 0;
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

			/*automatically create object for the rotating radar if the base is found*/
			if (!map_radar_object_for_map[mapidx]) {
				if (object->map_file_object.model == 7981/*smallradar02_lvS*/) {
					radar_angle = object->map_file_object.rz * M_PI / 180.0f;
					radar_offset_x = 4.4148f * (float) cos(radar_angle);
					radar_offset_y = 4.4148f * (float) sin(radar_angle);
					goto addradar;
				} else if (object->map_file_object.model == 10810/*ap_smallradar1_SFSe*/) {
					radar_angle = object->map_file_object.rz * M_PI / 180.0f;
					radar_offset_x = -4.2038f * (float) cos(radar_angle) - 1.2057f * (float) sin(radar_angle);
					radar_offset_y = -4.2038f * (float) sin(radar_angle) + 1.2057f * (float) cos(radar_angle);
addradar:
					map_radar_object_for_map[mapidx] = malloc(sizeof(struct OBJECT));
					map_radar_object_for_map[mapidx]->map_file_object.model = 1682;
					map_radar_object_for_map[mapidx]->map_file_object.x = object->map_file_object.x + radar_offset_x;
					map_radar_object_for_map[mapidx]->map_file_object.y = object->map_file_object.y + radar_offset_y;
					map_radar_object_for_map[mapidx]->map_file_object.z = object->map_file_object.z + 11.4f;
					map_radar_object_for_map[mapidx]->map_file_object.rx = 0.0f;
					map_radar_object_for_map[mapidx]->map_file_object.ry = 0.0f;
					map_radar_object_for_map[mapidx]->map_file_object.rz = 0.0f;
					map_radar_object_for_map[mapidx]->drawdistance = header.draw_radius;
				}
			}

			object++;
		}
	}

	/*read gangzones*/
	maps[mapidx].num_gang_zones = header.num_gang_zones;
	if (header.num_gang_zones) {
		maps[mapidx].gang_zones = gang_zone = malloc(sizeof(struct GANG_ZONE) * header.num_gang_zones);
		for (i = header.num_gang_zones; i > 0; i--) {
			/*can not directly read into struct because the order of coordinates differ*/
			if (!fread(gang_zone, sizeof(struct GANG_ZONE), 1, fs)) {
				free(maps[mapidx].gang_zones);
				if (header.num_objects) {
					free(maps[mapidx].objects);
				}
				/*no need to null because the map will be discarded as a whole*/
				goto corrupted;
			}
			total_element_count_for_middle += 2;
			middle_x += gang_zone->min_x;
			middle_x += gang_zone->max_x;
			middle_y += gang_zone->min_y;
			middle_y += gang_zone->max_y;
			gang_zone++;
		}
	}

	/*determine center*/
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
				if (!strcmp("octa_lod", maps[num_maps].name)) {
					octavia_island_actor_mapidx = num_maps;
				}
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

static
int maps_timer_rotate_radar(void *data)
{
	static float obj_radar_z_rot = 0.0f;
	static int z_off_i = 0;

	struct OBJECT *object;
	int playerindex, playerid;
	float zoffset;

	obj_radar_z_rot += 179.0f;
	if (obj_radar_z_rot > 360.0f) {
		obj_radar_z_rot -= 360.0f;
	}
	z_off_i ^= 0x3bc49ba6;
	zoffset = *((float*) &z_off_i);

	for (playerindex = 0; playerindex < playercount; playerindex++) {
		playerid = players[playerindex];
		if ((object = map_radar_object_for_player[playerid])) {
			rpcdata_MoveObject.objectid = ROTATING_RADAR_OBJECT_ID;
			rpcdata_MoveObject.from_x = object->map_file_object.x;
			rpcdata_MoveObject.from_y = object->map_file_object.y;
			rpcdata_MoveObject.from_z = object->map_file_object.z;
			rpcdata_MoveObject.to_x = object->map_file_object.x;
			rpcdata_MoveObject.to_y = object->map_file_object.y;
			rpcdata_MoveObject.to_z = object->map_file_object.z + zoffset;
			rpcdata_MoveObject.speed = 0.0012f;
			rpcdata_MoveObject.to_rx = 0.0f;
			rpcdata_MoveObject.to_ry = 0.0f;
			rpcdata_MoveObject.to_rz = obj_radar_z_rot;
			SAMP_SendRPCToPlayer(RPC_MoveObject, &bs_maps_move_object, playerid, 2);
		}
	}

	return 5300;
	/*5000 is more accurate timing, but players with fluctuating ping might see
	the radar going back and forth instead of full circles*/
}

void maps_init()
{
	octavia_island_actor_mapidx = -1;
	maps_load_from_db();

	bs_maps_remove_building.numberOfBitsUsed = sizeof(rpcdata_RemoveBuilding) * 8;
	bs_maps_remove_building.numberOfBitsAllocated = sizeof(rpcdata_RemoveBuilding) * 8;
	bs_maps_remove_building.readOffset = 0;
	bs_maps_remove_building.ptrData = &rpcdata_RemoveBuilding;
	bs_maps_remove_building.copyData = 1;

	rpcdata_CreateObject.no_camera_col = 0;
	rpcdata_CreateObject.attached_object = -1;
	rpcdata_CreateObject.attached_vehicle = -1;

	bs_maps_create_object.numberOfBitsUsed = sizeof(rpcdata_CreateObject) * 8;
	bs_maps_create_object.numberOfBitsAllocated = sizeof(rpcdata_CreateObject) * 8;
	bs_maps_create_object.readOffset = 0;
	bs_maps_create_object.ptrData = &rpcdata_CreateObject;
	bs_maps_create_object.copyData = 1;

	bs_maps_destroy_object.numberOfBitsUsed = sizeof(rpcdata_DestroyObject) * 8;
	bs_maps_destroy_object.numberOfBitsAllocated = sizeof(rpcdata_DestroyObject) * 8;
	bs_maps_destroy_object.readOffset = 0;
	bs_maps_destroy_object.ptrData = &rpcdata_DestroyObject;
	bs_maps_destroy_object.copyData = 1;

	bs_maps_show_gang_zone.numberOfBitsUsed = sizeof(rpcdata_ShowGangZone) * 8;
	bs_maps_show_gang_zone.numberOfBitsAllocated = sizeof(rpcdata_ShowGangZone) * 8;
	bs_maps_show_gang_zone.readOffset = 0;
	bs_maps_show_gang_zone.ptrData = &rpcdata_ShowGangZone;
	bs_maps_show_gang_zone.copyData = 1;

	bs_maps_move_object.numberOfBitsUsed = sizeof(rpcdata_MoveObject) * 8;
	bs_maps_move_object.numberOfBitsAllocated = sizeof(rpcdata_MoveObject) * 8;
	bs_maps_move_object.readOffset = 0;
	bs_maps_move_object.ptrData = &rpcdata_MoveObject;
	bs_maps_move_object.copyData = 1;

	bitstream_freeform.readOffset = 0;
	bitstream_freeform.copyData = 1;

	rpcdata_show_actor.actorid = OCTA_ACTORID;
	rpcdata_show_actor.modelid = 141;
	rpcdata_show_actor.x = 12332.3926f;
	rpcdata_show_actor.y = 6521.1636f;
	rpcdata_show_actor.z = 7.0225f;
	rpcdata_show_actor.angle = 0.0f;
	rpcdata_show_actor.health = 255.0f;
	rpcdata_show_actor.invulnerable = 1;

	rpcdata_hide_actor.actorid = OCTA_ACTORID;

	timer_set(2000, maps_timer_rotate_radar, NULL);
}

/**
Creates all objects/gangzones in given map for the given player.
*/
static
void maps_stream_in_for_player(int playerid, int mapidx)
{
	struct OBJECT *obj, *objects_end;
	short *objectid_to_mapidx;
	short *gangzoneid_to_mapidx;
	int gang_zone_idx;

#ifdef MAPS_LOG_STREAMING
	logprintf("map %s streamed in for %d", maps[mapidx].name, playerid);
#endif

	maps[mapidx].stream_status_for_player[playerid] = 1;

	/*objects*/
	if (maps[mapidx].num_objects) {
		obj = maps[mapidx].objects;
		objects_end = obj + maps[mapidx].num_objects;
		objectid_to_mapidx = player_objectid_to_mapidx[playerid];
		rpcdata_CreateObject.objectid = 0; /*TODO: samp starts at objectid 1, should we also do that?*/
		while (obj != objects_end) {
			while (objectid_to_mapidx[rpcdata_CreateObject.objectid] != -1) {
				rpcdata_CreateObject.objectid++;
				if (rpcdata_CreateObject.objectid >= MAX_MAPSYSTEM_OBJECTS) {
					logprintf("object limit hit while streaming map %s", maps[mapidx].name);
					goto skip_objects;
				}
			}
			memcpy(&rpcdata_CreateObject.modelid, obj, sizeof(struct OBJECT));
			objectid_to_mapidx[rpcdata_CreateObject.objectid] = mapidx;
			obj++;
			/*TODO: what's the 2 for? possibly priority*/
			SAMP_SendRPCToPlayer(RPC_CreateObject, &bs_maps_create_object, playerid, 2);
		}
	}
skip_objects:
	;

	/*the special rotating radar object*/
	/*It won't be made if another streamed in map already has a radar, meaning when that other
	map gets streamed out, this new map will not have a radar...
	No maps with radars should have intersecting stream radii anyways.*/
	if (map_radar_object_for_map[mapidx] && !map_radar_object_for_player[playerid]) {
		map_radar_object_for_player[playerid] = map_radar_object_for_map[mapidx];
		rpcdata_CreateObject.objectid = ROTATING_RADAR_OBJECT_ID;
		memcpy(&rpcdata_CreateObject.modelid, map_radar_object_for_map[mapidx], sizeof(struct OBJECT));
		SAMP_SendRPCToPlayer(RPC_CreateObject, &bs_maps_create_object, playerid, 2);
	}

	/*gang zones*/
	rpcdata_ShowGangZone.zoneid = 0;
	gangzoneid_to_mapidx = player_gangzoneid_to_mapidx[playerid];
	for (gang_zone_idx = 0; gang_zone_idx < maps[mapidx].num_gang_zones; gang_zone_idx++) {
		while (gangzoneid_to_mapidx[rpcdata_ShowGangZone.zoneid] != -1) {
			rpcdata_ShowGangZone.zoneid++;
			if (rpcdata_ShowGangZone.zoneid >= MAX_GANG_ZONES) {
				logprintf("gang zone limit hit while streaming map %s", maps[mapidx].name);
				goto skip_gang_zones;
			}
		}
		gangzoneid_to_mapidx[rpcdata_ShowGangZone.zoneid] = mapidx;
		memcpy(&rpcdata_ShowGangZone.min_x, &maps[mapidx].gang_zones[gang_zone_idx], sizeof(struct GANG_ZONE));
		/*TODO: what's the 2 for? possibly priority*/
		SAMP_SendRPCToPlayer(RPC_ShowGangZone, &bs_maps_show_gang_zone, playerid, 2);
	}
skip_gang_zones:
	;

	/*Octavia actor*/
	if (mapidx == octavia_island_actor_mapidx) {
		bitstream_freeform.numberOfBitsUsed = sizeof(rpcdata_show_actor) * 8;
		bitstream_freeform.numberOfBitsAllocated = sizeof(rpcdata_show_actor) * 8;
		bitstream_freeform.ptrData = &rpcdata_show_actor;
		SAMP_SendRPCToPlayer(RPC_ShowActor, &bitstream_freeform, playerid, 2);
	}
}

/**
Deletes all the objects/gangzones from given map for the given player.
*/
static
void maps_stream_out_for_player(int playerid, int mapidx)
{
	short *objectid_to_mapidx;
	short *gangzoneid_to_mapidx;

#ifdef MAPS_LOG_STREAMING
	logprintf("map %s streamed out for %d", maps[mapidx].name, playerid);
#endif

	maps[mapidx].stream_status_for_player[playerid] = 0;

	/*objects*/
	objectid_to_mapidx = player_objectid_to_mapidx[playerid];
	for (rpcdata_DestroyObject.objectid = 0; rpcdata_DestroyObject.objectid < MAX_OBJECTS; rpcdata_DestroyObject.objectid++) {
		if (objectid_to_mapidx[rpcdata_DestroyObject.objectid] == mapidx) {
			objectid_to_mapidx[rpcdata_DestroyObject.objectid] = -1;
			/*TODO: what's the 2 for? possibly priority*/
			SAMP_SendRPCToPlayer(RPC_DestroyObject, &bs_maps_destroy_object, playerid, 2);
		}
	}

	/*the special rotating radar object*/
	if (map_radar_object_for_map[mapidx] && map_radar_object_for_map[mapidx] == map_radar_object_for_player[playerid]) {
		map_radar_object_for_player[playerid] = 0;
		rpcdata_DestroyObject.objectid = ROTATING_RADAR_OBJECT_ID;
		SAMP_SendRPCToPlayer(RPC_DestroyObject, &bs_maps_destroy_object, playerid, 2);
	}

	/*gang zones*/
	gangzoneid_to_mapidx = player_gangzoneid_to_mapidx[playerid];
	for (rpcdata_ShowGangZone.zoneid = 0; rpcdata_ShowGangZone.zoneid < MAX_GANG_ZONES; rpcdata_ShowGangZone.zoneid++) {
		if (gangzoneid_to_mapidx[rpcdata_ShowGangZone.zoneid] == mapidx) {
			gangzoneid_to_mapidx[rpcdata_ShowGangZone.zoneid] = -1;
			/*TODO: what's the 2 for? possibly priority*/
			/*there's no real need to hide the gang zone, just mark it as available and be done*/
			/*SAMP_SendRPCToPlayer(RPC_HideGangZone, &bs_maps_show_gang_zone, playerid, 2);*/
		}
	}

	/*Octavia actor*/
	if (mapidx == octavia_island_actor_mapidx) {
		bitstream_freeform.numberOfBitsUsed = sizeof(rpcdata_hide_actor) * 8;
		bitstream_freeform.numberOfBitsAllocated = sizeof(rpcdata_hide_actor) * 8;
		bitstream_freeform.ptrData = &rpcdata_hide_actor;
		SAMP_SendRPCToPlayer(RPC_HideActor, &bitstream_freeform, playerid, 2);
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

int maps_calculate_objects_to_create_for_player_at_position(int playerid, struct vec3 pos)
{
	float dx, dy, distance;
	int mapidx;
	int amount_of_objects;

	/*not checking amount of objects to destroy atm*/

	amount_of_objects = 0;
	for (mapidx = 0; mapidx < num_maps; mapidx++) {
		if (!maps[mapidx].stream_status_for_player[playerid]) {
			dx = maps[mapidx].middle_x - pos.x;
			dy = maps[mapidx].middle_y - pos.y;
			distance = dx * dx + dy * dy;
			if (distance < maps[mapidx].stream_in_radius_sq) {
				amount_of_objects += maps[mapidx].num_objects;
			}
		}
	}

	return amount_of_objects;
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

	for (i = num_removed_objects; i >= 0;) {
		i--;
		memcpy(&rpcdata_RemoveBuilding, &removed_objects[i], sizeof(struct REMOVEDOBJECT));
		SAMP_SendRPCToPlayer(RPC_RemoveBuilding, &bs_maps_remove_building, playerid, 2);
	}

	memset(player_objectid_to_mapidx[playerid], -1, sizeof(player_objectid_to_mapidx[playerid]));
	memset(player_gangzoneid_to_mapidx[playerid], -1, sizeof(player_gangzoneid_to_mapidx[playerid]));
	map_radar_object_for_player[playerid] = 0;
}

void maps_on_player_disconnect(int playerid)
{
	int mapidx;

	for (mapidx = num_maps; mapidx > 0; mapidx--) {
		maps[mapidx].stream_status_for_player[playerid] = 0;
	}
}
