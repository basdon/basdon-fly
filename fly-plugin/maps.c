/*
Object and gangzone streamer. Also deals with removed objects.

Map data is stored in .map files. Data is read into separate object and (gang)zone structs.
Objects and zones that are loaded from the same map file are grouped. The streamer works
by checking if that entire cluster of objects and zones should be streamed in or out, then
individual objects/zones are being sent to the client per map tick.
*/

#define MAX_MAPS (50)
#define MAP_MAX_FILENAME (34) /*see db col*/
#define MAX_REMOVED_OBJECTS 1000

#define MAPSTREAMING_UPDATE_INTERVAL 1000
#define MAX_OBJECTS_TO_CREATE_PER_TICK 150
#define MAX_ZONES_TO_CREATE_PER_TICK 75
#define IMM_STREAMING_RADIUS_SQ (100*100)

#pragma pack(1)
struct MAP_FILE_HEADER {
	int spec_version;
	int num_removes;
	int num_objects;
	int objdata_size;
	int num_gang_zones;
	float stream_in_radius;
	float stream_out_radius;
	float draw_radius;
};
/*Gangzones in the map file doesn't have zoneid, but we want to load it
immediately into a struct ready to send because it eliminates copying
stuff when actually having to send it. Therefor this union exists.*/
union GANG_ZONE {
	struct RPCDATA_ShowGangZone rpcdata;
	struct {
		short zoneid;
		struct {
			float min_x;
			float min_y;
			float max_x;
			float max_y;
			int colorABGR;
		} from_map_file;
	} data;
};
EXPECT_SIZE(union GANG_ZONE, sizeof(struct RPCDATA_ShowGangZone));
STATIC_ASSERT_MEMBER_OFFSET(union GANG_ZONE, data.zoneid,\
	MEMBER_OFFSET(struct RPCDATA_ShowGangZone, zoneid));
STATIC_ASSERT_MEMBER_OFFSET(union GANG_ZONE, data.from_map_file.min_x,\
	MEMBER_OFFSET(struct RPCDATA_ShowGangZone, min_x));
STATIC_ASSERT_MEMBER_OFFSET(union GANG_ZONE, data.from_map_file.min_y,\
	MEMBER_OFFSET(struct RPCDATA_ShowGangZone, min_y));
STATIC_ASSERT_MEMBER_OFFSET(union GANG_ZONE, data.from_map_file.max_x,\
	MEMBER_OFFSET(struct RPCDATA_ShowGangZone, max_x));
STATIC_ASSERT_MEMBER_OFFSET(union GANG_ZONE, data.from_map_file.max_y,\
	MEMBER_OFFSET(struct RPCDATA_ShowGangZone, max_y));
STATIC_ASSERT_MEMBER_OFFSET(union GANG_ZONE, data.from_map_file.colorABGR,\
	MEMBER_OFFSET(struct RPCDATA_ShowGangZone, colorABGR));
#pragma pack()

enum MAP_STREAM_STATUS {
	STREAMED_OUT,
	STREAMING_IN,
	STREAMED_IN,
};

static struct OBJECT_MAP {
	float middle_x, middle_y;
	float stream_in_radius_sq, stream_out_radius_sq;
	char name[MAP_MAX_FILENAME];
	struct RPCDATA_CreateObject *objects;
	int num_objects;
	enum MAP_STREAM_STATUS stream_status_for_player[MAX_PLAYERS];
	/**The rotating radar object for each map, NULL for maps that don't have any.*/
	struct RPCDATA_CreateObject *radar_object;
} obj_maps[MAX_MAPS];
static int num_obj_maps;

static struct ZONE_MAP {
	float middle_x, middle_y;
	float stream_in_radius_sq, stream_out_radius_sq;
	char name[MAP_MAX_FILENAME];
	union GANG_ZONE *gang_zones;
	int num_gang_zones;
	enum MAP_STREAM_STATUS stream_status_for_player[MAX_PLAYERS];
} zone_maps[MAX_MAPS];
static int num_zone_maps;

/**The rotating radar object created for player. This data is needed for the position information.*/
static struct RPCDATA_CreateObject *radar_object_for_player[MAX_PLAYERS];

/**All combined removed objects.*/
static struct RPCDATA_RemoveBuilding removed_objects[MAX_REMOVED_OBJECTS];
static int num_removed_objects;

/**Maps a player's object id to a map idx. -1 when object is not created.*/
static short player_objectid_to_objmapidx[MAX_PLAYERS][MAX_MAPSYSTEM_OBJECTS];

/**Per-player streaming status.*/
static struct STREAMING_DATA {
	/**The object map that is currently being streamed in.*/
	struct OBJECT_MAP *obj_map;
	/**Objects to be streamed in next map streaming tick.
	They will be streamed in from last to first.*/
	struct RPCDATA_CreateObject *queued_objects[MAX_MAPSYSTEM_OBJECTS];
	/**Number of objects left in {@link queued_objects} to stream in.*/
	short num_queued_objects;
	/**The gangzone map that is currently being streamed in.*/
	struct ZONE_MAP *zone_map;
	/**Index in the zone_map's gangzones to stream in next streaming tick.*/
	short next_zone_idx;
} streaming_data[MAX_PLAYERS];

/*maps a player's gang zone id to map idx*/
/*this is (at time of writing) the only system that uses gang zones,
so no checks are being done on a global scale if a gang zone is being used*/
/*-1 when unused, -2 when marked for deletion*/
static short player_gangzoneid_to_zonemapidx[MAX_PLAYERS][MAX_GANG_ZONES];

/*the mapidx of octavia island map where the actor should be made*/
static short octavia_island_actor_mapidx;

/*The mapidx of WMRE, to know when to create the windmill blades objects.*/
static short wmre_mapidx;
/*Objectids of the windmill blade objects for each player. -1 means it's not created.*/
static short wmre_blades_objectids[MAX_PLAYERS][12];
static struct RPCDATA_CreateObject *wmre_blade_template;
/*Positions of WMRE windmill blade objects.*/
static float wmre_blade_x[4] = {
	-7273.9067f, -7092.6699f, -7116.2734f, -7179.7515f
}, wmre_blade_y[4] = {
	3266.6731, 3122.7756f, 3194.5857f, 3279.6697f
}, wmre_blade_z[4] = {
	37.6830f, 35.7430f, 34.5030f, 37.2130f
};
/*Current Z-coordinate offset for WMRE windmill blade objects.*/
/*This is needed because you can't rotate an object without moving it,
so we're moving the Z back and forth every time.*/
/*Global variable so that it's synced for people that just got the blades streamed in.*/
static int wmre_blade_z_offset_i;
/*The angle to which the windmill blades are currently rotating.*/
/*Global variable so that it's synced for people that just got the blades streamed in.*/
static int wmre_blade_01_angle, wmre_blade_23_angle;

/**For each player, how much ms is left for next map streaming tick.*/
static short next_streaming_tick_delay[MAX_PLAYERS];

static struct RPCDATA_ShowActor rpcdata_show_actor;
static struct RPCDATA_HideActor rpcdata_hide_actor;

#define MAP_LOAD_RESULT_HAS_OBJECTS 1
#define MAP_LOAD_RESULT_HAS_ZONES 2
/*jeanine:p:i:24;p:22;a:r;x:766;y:-654;*/
/**
Load a map from file as specified in given map structs.

@return bitfield with:
        - {@link MAP_LOAD_RESULT_HAS_OBJECTS} if obj_map has been filled in
        - {@link MAP_LOAD_RESULT_HAS_ZONES} if zone_map has been filled in
        If none set, it means the map only contained removed objects, or was empty, or error.
*/
static
int maps_load_from_file(char *name, struct OBJECT_MAP *obj_map, struct ZONE_MAP *zone_map)
{
	FILE *fs;
	char filename[22 + MAP_MAX_FILENAME];
	int i;
	struct MAP_FILE_HEADER header;
	struct RPCDATA_CreateObject *object_data;
	union GANG_ZONE *gang_zone;
	float middle_x, middle_y;
	float radar_angle, radar_offset_x, radar_offset_y;
	int result;

	memset(obj_map, 0, sizeof(struct OBJECT_MAP));
	memset(zone_map, 0, sizeof(struct ZONE_MAP));
	strcpy(obj_map->name, name);
	strcpy(zone_map->name, name);
	sprintf(filename, "../maps/%s.map", name);
	if (!(fs = fopen(filename, "rb"))) {
		logprintf("failed to open map %s", filename);
		return 0;
	}

	/*read header*/
	if (!fread(&header, sizeof(header), 1, fs)) {
		goto corrupted;
	}

	if (header.spec_version != 0x0350414D) {
		logprintf("unknown map spec %p in %s", header.spec_version, filename);
		goto corrupted;
	}

	if (header.stream_out_radius < header.stream_in_radius + 50.0f) {
		logprintf("map %s has streamout %f < streamin %f + 100", name, header.stream_out_radius, header.stream_in_radius);
		abort();
	}

#ifdef MAPS_PRINT_STATS
	printf("map %s streamin %.0f streamout %.0f draw %.0f\n",
		name, header.stream_in_radius, header.stream_out_radius, header.draw_radius);
#endif

	zone_map->stream_in_radius_sq = obj_map->stream_in_radius_sq = header.stream_in_radius * header.stream_in_radius;
	zone_map->stream_out_radius_sq = obj_map->stream_out_radius_sq = header.stream_out_radius * header.stream_out_radius;

	result = 0;

	/*TODO? could check if num_objects num_removes num_zones go over their limits*/

	/*read removed objects*/
	for (i = header.num_removes; i > 0; i--) {
		if (!fread(&removed_objects[num_removed_objects], sizeof(struct RPCDATA_RemoveBuilding), 1, fs)) {
			goto corrupted;
		}
		if (num_removed_objects == MAX_REMOVED_OBJECTS - 1) {
			logprintf("ERR: MAX_REMOVED_OBJECTS limit hit!");
		} else {
			num_removed_objects++;
		}
	}

	/*read gangzones*/
	if (header.num_gang_zones) {
		middle_x = middle_y = 0.0f;
		zone_map->num_gang_zones = header.num_gang_zones;
		zone_map->gang_zones = gang_zone = malloc(sizeof(union GANG_ZONE) * header.num_gang_zones);
		for (i = header.num_gang_zones; i > 0; i--) {
			if (!fread(&gang_zone->data.from_map_file, sizeof(gang_zone->data.from_map_file), 1, fs)) {
				free(zone_map->gang_zones);
				/*no need to null because the map will be discarded as a whole*/
				goto corrupted;
			}
			middle_x += gang_zone->data.from_map_file.min_x;
			middle_x += gang_zone->data.from_map_file.max_x;
			middle_y += gang_zone->data.from_map_file.min_y;
			middle_y += gang_zone->data.from_map_file.max_y;
			gang_zone++;
		}
		/*div by 2 since there's min and max coords for each zone*/
		zone_map->middle_x = middle_x / header.num_gang_zones / 2;
		zone_map->middle_y = middle_y / header.num_gang_zones / 2;
		result |= MAP_LOAD_RESULT_HAS_ZONES;
	}

	/*read objects*/
	if (header.num_objects) {
		middle_x = middle_y = 0.0f;
		obj_map->num_objects = header.num_objects;
		obj_map->objects = object_data = malloc(header.objdata_size);
		for (i = header.num_objects; i > 0; i--) {
			if (!fread(object_data, 2, 1, fs) ||
				!fread((char*) object_data + 2, object_data->objectid - 2, 1, fs))
			{
				free(obj_map->objects);
				if (zone_map->gang_zones) {
					free(zone_map->gang_zones);
				}
				/*no need to null because the map will be discarded as a whole*/
				goto corrupted;
			}
			middle_x += object_data->x;
			middle_y += object_data->y;
			object_data->drawdistance = header.draw_radius;

			/*automatically create object for the rotating radar if the base is found*/
			if (!obj_map->radar_object) {
				if (object_data->modelid == 7981/*smallradar02_lvS*/) {
					radar_angle = object_data->rz * M_PI / 180.0f;
					radar_offset_x = 4.4148f * (float) cos(radar_angle);
					radar_offset_y = 4.4148f * (float) sin(radar_angle);
					goto addradar;
				} else if (object_data->modelid == 10810/*ap_smallradar1_SFSe*/) {
					radar_angle = object_data->rz * M_PI / 180.0f;
					radar_offset_x = -4.2038f * (float) cos(radar_angle) - 1.2057f * (float) sin(radar_angle);
					radar_offset_y = -4.2038f * (float) sin(radar_angle) + 1.2057f * (float) cos(radar_angle);
addradar:
					obj_map->radar_object = malloc(sizeof(struct RPCDATA_CreateObject));
					obj_map->radar_object->objectid = OBJECT_ROTATING_RADAR;
					obj_map->radar_object->modelid = 1682;
					obj_map->radar_object->x = object_data->x + radar_offset_x;
					obj_map->radar_object->y = object_data->y + radar_offset_y;
					obj_map->radar_object->z = object_data->z + 11.4f;
					obj_map->radar_object->rx = 0.0f;
					obj_map->radar_object->ry = 0.0f;
					obj_map->radar_object->rz = 0.0f;
					obj_map->radar_object->drawdistance = header.draw_radius;
					obj_map->radar_object->no_camera_col = 0;
					obj_map->radar_object->attached_object_id = -1;
					obj_map->radar_object->attached_vehicle_id = -1;
					obj_map->radar_object->num_materials = 0;
				}
			}

			object_data = (void*) ((char*) object_data + object_data->objectid);
		}
		obj_map->middle_x = middle_x / header.num_objects;
		obj_map->middle_y = middle_y / header.num_objects;
		result |= MAP_LOAD_RESULT_HAS_OBJECTS;
	}

	fclose(fs);
	return result;
corrupted:
	logprintf("corrupted map %s", filename);
	return 0;
}
/*jeanine:p:i:25;p:22;a:r;x:30;y:0;*/
#ifdef MAPS_PRINT_STATS
static
void maps_print_stats()
{
	struct OBJECT_MAP *obj_map;
	struct ZONE_MAP *zone_map;
	int total_objects, total_gang_zones;

	total_objects = total_gang_zones = 0;
	for (obj_map = obj_maps; obj_map != obj_maps + num_obj_maps; obj_map++) {
		total_objects += obj_map->num_objects;
	}
	for (zone_map = zone_maps; zone_map != zone_maps + num_zone_maps; zone_map++) {
		total_gang_zones += zone_map->num_gang_zones;
	}
	printf("%d object maps, %d objects\n", num_obj_maps, total_objects);
	printf("%d zone maps, %d zones\n", num_zone_maps, total_gang_zones);
	printf("%d removes\n", num_removed_objects);
}
#endif
/*jeanine:p:i:22;p:20;a:r;x:400;y:-984;*/
static
void maps_load_from_db()
{
	char mapname[MAP_MAX_FILENAME];
	char q[] =
		"SELECT filename FROM map "
		"LEFT JOIN apt ON map.ap=apt.i "
		"WHERE apt.e=1 OR apt.e IS NULL";
	int cacheid, rowcount, load_result;
	int i;

	atoc(buf4096, q, sizeof(q));
	cacheid = NC_mysql_query(buf4096a);
	rowcount = NC_cache_get_row_count();
	if (rowcount > MAX_MAPS) {
		/*Actual number of needed slots can be less because maps that are only removed objects don't take slots.*/
		logprintf("can't load all maps! missing %d slots", rowcount - MAX_MAPS);
		rowcount = MAX_MAPS;
	}
	num_obj_maps = 0;
	num_zone_maps = 0;
	for (i = 0; i < rowcount; i++) {
		nc_params[3] = buf32a;
		while (rowcount--) {
			NC_PARS(3);
			nc_params[1] = rowcount;
			nc_params[2] = 0;
			NC(n_cache_get_field_s);
			ctoa(mapname, buf32, MAP_MAX_FILENAME);
			load_result = maps_load_from_file(mapname, &obj_maps[num_obj_maps], &zone_maps[num_zone_maps]);/*jeanine:l:24*/
			if (load_result & MAP_LOAD_RESULT_HAS_OBJECTS) {
				if (!strcmp("octa_lod", mapname)) {
					octavia_island_actor_mapidx = num_obj_maps;
				}
				if (!strcmp("wmre_lod", mapname)) {
					wmre_mapidx = num_obj_maps;
				}
				num_obj_maps++;
			}
			if (load_result & MAP_LOAD_RESULT_HAS_ZONES) {
				num_zone_maps++;
			}
		}
	}
	NC_cache_delete(cacheid);

#ifdef MAPS_PRINT_STATS
	maps_print_stats();/*jeanine:l:25*/
#endif
}
/*jeanine:p:i:23;p:20;a:r;x:1232;y:65;*/
static
int maps_timer_rotate_radar(void *data)
{
	static float obj_radar_z_rot = 0.0f;
	static int z_off_i = 0;

	struct BitStream bitstream;
	struct RPCDATA_MoveObject rpcdata;
	register struct RPCDATA_CreateObject *object;
	int playerindex, playerid;
	float zoffset;

	obj_radar_z_rot += 120.0f;
	if (obj_radar_z_rot > 360.0f) {
		obj_radar_z_rot -= 360.0f;
	}
	z_off_i ^= 0x3bc49ba6; /*0.06f*/
	zoffset = *((float*) &z_off_i);

	bitstream.numberOfBitsUsed = sizeof(rpcdata) * 8;
	bitstream.ptrData = &rpcdata;

	rpcdata.objectid = OBJECT_ROTATING_RADAR;
	rpcdata.speed = 0.0024f;
	rpcdata.to_rx = 0.0f;
	rpcdata.to_ry = 0.0f;
	rpcdata.to_rz = obj_radar_z_rot;

	for (playerindex = 0; playerindex < playercount; playerindex++) {
		playerid = players[playerindex];
		if ((object = radar_object_for_player[playerid])) {
			rpcdata.from_x = object->x;
			rpcdata.from_y = object->y;
			rpcdata.from_z = object->z;
			rpcdata.to_x = object->x;
			rpcdata.to_y = object->y;
			rpcdata.to_z = object->z + zoffset;
			SAMP_SendRPCToPlayer(RPC_MoveObject, &bitstream, playerid, 2);
		}
	}

	return 2500;
}
/*jeanine:p:i:31;p:20;a:r;x:397;y:87;*/
static
int maps_timer_rotate_wmre_blades(void *data)
{
	struct BitStream bitstream;
	struct RPCDATA_MoveObject rpcdata;
	int blade, bladeidx, playerindex, playerid;
	float prev_zoffset, next_zoffset;

	wmre_blade_01_angle += 120;
	if (wmre_blade_01_angle > 360) {
		wmre_blade_01_angle -= 360;
	}
	wmre_blade_23_angle += 60;
	if (wmre_blade_23_angle > 360) {
		wmre_blade_23_angle -= 360;
	}

	prev_zoffset = *((float*) &wmre_blade_z_offset_i);
	wmre_blade_z_offset_i ^= 0x3bc49ba6; /*0.06f*/
	next_zoffset = *((float*) &wmre_blade_z_offset_i);

	bitstream.numberOfBitsUsed = sizeof(rpcdata) * 8;
	bitstream.ptrData = &rpcdata;

	rpcdata.speed = 0.003f;
	rpcdata.to_ry = 180.0f;
	rpcdata.to_rz = -160.0f;

	for (blade = 0; blade < 12; blade++) {
		bladeidx = blade / 3;
		rpcdata.from_x = wmre_blade_x[bladeidx];
		rpcdata.from_y = wmre_blade_y[bladeidx];
		rpcdata.from_z = wmre_blade_z[bladeidx] + prev_zoffset;
		rpcdata.to_x = wmre_blade_x[bladeidx];
		rpcdata.to_y = wmre_blade_y[bladeidx];
		rpcdata.to_z = wmre_blade_z[bladeidx] + next_zoffset;
		if (blade < 6) {
			rpcdata.to_rx = (float) (wmre_blade_01_angle + 120 * (blade % 3));
		} else {
			rpcdata.to_rx = (float) (wmre_blade_23_angle + 120 * (blade % 3));
		}
		for (playerindex = 0; playerindex < playercount; playerindex++) {
			playerid = players[playerindex];
			rpcdata.objectid = wmre_blades_objectids[playerid][blade];
			if (rpcdata.objectid != -1) {
				SAMP_SendRPCToPlayer(RPC_MoveObject, &bitstream, playerid, 2);
			}
		}
	}

	return 2000;
}
/*jeanine:p:i:30;p:20;a:r;x:395;y:-110;*/
/**
 * Loads the "wmre_blades.map" mapfile to copy the first object as template for the windmill's blades.
 *
 * Uses the {@link obj_maps} and {@link zone_maps} variables as temporary storage,
 * so this function should be called before any other map is loaded.
 */
static
void maps_load_wmre_blade_template_object()
{
	int size, result;

	result = maps_load_from_file("wmre_blades", obj_maps, zone_maps);
	/*This map should only have the blade objects, no zones.*/
	assert(result & MAP_LOAD_RESULT_HAS_OBJECTS);
	assert(!(result & ~MAP_LOAD_RESULT_HAS_OBJECTS));
	assert(obj_maps->num_objects);
	/*Objectid is used for the size of the whole CreateObject RPC data.*/
	size = obj_maps->objects[0].objectid;
	wmre_blade_template = malloc(size);
	memcpy(wmre_blade_template, obj_maps->objects, size);
	free(obj_maps->objects);
}
/*jeanine:p:i:20;p:0;a:b;x:0;y:30;*/
/**
Initialize mapping system. Loads maps from db and reads their files.
*/
static
void maps_init()
{
	octavia_island_actor_mapidx = -1;
	wmre_mapidx = -1;

	/*Add a remove for the peculiar telegraph pole that spawns
	in the center.*/
	removed_objects[0].model = 1308;
	removed_objects[0].x = 9.0234f;
	removed_objects[0].y = 15.1563f;
	removed_objects[0].z = -5.7109f;
	removed_objects[0].radius = 0.25f;
	num_removed_objects = 1;

	maps_load_wmre_blade_template_object();/*jeanine:l:30*/
	maps_load_from_db();/*jeanine:l:22*/

	rpcdata_show_actor.actorid = OCTA_ACTORID;
	rpcdata_show_actor.modelid = 141;
	rpcdata_show_actor.x = 12332.3926f;
	rpcdata_show_actor.y = 6521.1636f;
	rpcdata_show_actor.z = 7.0225f;
	rpcdata_show_actor.angle = 0.0f;
	rpcdata_show_actor.health = 255.0f;
	rpcdata_show_actor.invulnerable = 1;

	rpcdata_hide_actor.actorid = OCTA_ACTORID;

	timer_set(2500, maps_timer_rotate_radar, NULL);/*jeanine:l:23*/
	timer_set(2000, maps_timer_rotate_wmre_blades, NULL);/*jeanine:l:31*/
}
/*jeanine:p:i:21;p:27;a:r;x:28;y:619;*/
/**
*
*/
static
void maps_continue_stream_in_queued_zones_for_player(int playerid, struct STREAMING_DATA *streamingdata)
{
	struct BitStream bitstream;
	struct ZONE_MAP *map;
	union GANG_ZONE *zone;
	short *gangzoneid_to_zonemapidx;
	short zoneid;
	short num_created_zones;

	map = streamingdata->zone_map;
	bitstream.numberOfBitsUsed = sizeof(struct RPCDATA_ShowGangZone) * 8;
	zoneid = 0;
	gangzoneid_to_zonemapidx = player_gangzoneid_to_zonemapidx[playerid];
	zone = map->gang_zones + streamingdata->next_zone_idx;
	num_created_zones = 0;
	while (zone < map->gang_zones + map->num_gang_zones) {
		while (gangzoneid_to_zonemapidx[zoneid] >= 0) {
			zoneid++;
			if (zoneid >= MAX_GANG_ZONES) {
				logprintf("gang zone limit hit while streaming map %s", map->name);
				goto stop_streaming;
			}
		}
		gangzoneid_to_zonemapidx[zoneid] = map - zone_maps;
		zone->rpcdata.zoneid = zoneid;
		bitstream.ptrData = &zone->rpcdata;
		SAMP_SendRPCToPlayer(RPC_ShowGangZone, &bitstream, playerid, 2);
		if (++num_created_zones >= MAX_ZONES_TO_CREATE_PER_TICK) {
			return;
		}
		zone++;
		streamingdata->next_zone_idx++;
	}
stop_streaming:

#ifdef MAPS_LOG_STREAMING
	logprintf("zone map %s done streaming in for %d", streamingdata->zone_map->name, playerid);
#endif

	streamingdata->zone_map->stream_status_for_player[playerid] = STREAMED_IN;
	streamingdata->zone_map = NULL;
}
/*jeanine:p:i:11;p:17;a:r;x:106;y:-297;*/
/**
Deletes all the objects from given map for the given player.
*/
static
void maps_stream_out_objects_for_player(int playerid, struct OBJECT_MAP *map)
{
	struct RPCDATA_DestroyObject rpcdata_DestroyObject;
	struct BitStream bitstream;
	short *objectid_to_objmapidx;
	int objectid, mapidx;

#ifdef MAPS_LOG_STREAMING
	logprintf("obj map %s streamed out for %d", map->name, playerid);
#endif

	mapidx = map - obj_maps;

	/*objects*/
	bitstream.ptrData = &rpcdata_DestroyObject;
	bitstream.numberOfBitsUsed = sizeof(rpcdata_DestroyObject) * 8;
	objectid_to_objmapidx = player_objectid_to_objmapidx[playerid];
	for (objectid = 0; objectid < MAX_OBJECTS; objectid++) {
		if (objectid_to_objmapidx[objectid] == mapidx) {
			objectid_to_objmapidx[objectid] = -1;
			rpcdata_DestroyObject.objectid = objectid;
			SAMP_SendRPCToPlayer(RPC_DestroyObject, &bitstream, playerid, 2);
		}
	}

	/*The special rotating radar object.*/
	if (map->radar_object == radar_object_for_player[playerid]) {
		/*No real need to destroy the object.*/
		radar_object_for_player[playerid] = NULL;
	}

	/*Octavia actor*/
	if (mapidx == octavia_island_actor_mapidx) {
		SendRPCToPlayer(playerid, RPC_HideActor, &rpcdata_hide_actor, sizeof(rpcdata_hide_actor), 2);
	}

	/*WMRE windmill blades*/
	if (mapidx == wmre_mapidx) {
		/*The objectids are linked to the mapidx, so they will be deleted automatically.*/
		/*Just need to cleanup the objectid links.*/
		memset(wmre_blades_objectids[playerid], -1, sizeof(wmre_blades_objectids[playerid]));
	}
}
/*jeanine:p:i:19;p:17;a:r;x:106;y:274;*/
/**
Deletes all the gangzones from given map for the given player.
*/
static
void maps_stream_out_zones_for_player(int playerid, struct ZONE_MAP *map)
{
	short *gangzoneid_to_zonemapidx;
	short zoneid;
	int mapidx;

#ifdef MAPS_LOG_STREAMING
	logprintf("zone map %s streamed out for %d", map->name, playerid);
#endif

	mapidx = map - zone_maps;

	gangzoneid_to_zonemapidx = player_gangzoneid_to_zonemapidx[playerid];
	for (zoneid = 0; zoneid < MAX_GANG_ZONES; zoneid++) {
		if (gangzoneid_to_zonemapidx[zoneid] == mapidx) {
			/*No real need to hide the gang zone, just mark it as available and be done.*/
			gangzoneid_to_zonemapidx[zoneid] = -1;
		}
	}
}
/*jeanine:p:i:9;p:28;a:r;x:50;y:242;*/
static
void maps_print_object_breakdown(int playerid)
{
	int map_loaded[MAX_MAPS];
	int objects_per_mapidx[MAX_MAPS];
	int i, mapidx;
	int num_maps_loaded;

	memset(map_loaded, 0, sizeof(map_loaded));
	memset(objects_per_mapidx, 0, sizeof(objects_per_mapidx));
	num_maps_loaded = 0;
	for (i = 0; i < MAX_MAPSYSTEM_OBJECTS; i++) {
		mapidx = player_objectid_to_objmapidx[playerid][i];
		if (mapidx != -1) {
			if (!map_loaded[mapidx]) {
				map_loaded[mapidx] = 1;
				num_maps_loaded++;
			}
			objects_per_mapidx[mapidx]++;
		}
	}
	logprintf("%d maps loaded for player %d", num_maps_loaded, playerid);
	for (i = 0; i < num_obj_maps; i++) {
		if (map_loaded[i]) {
			logprintf("  map %s: %d objs", obj_maps[i].name, objects_per_mapidx[i]);
		}
	}
}
/*jeanine:p:i:28;p:27;a:r;x:944;y:-2;*/
/**
 * @return amount of objects created
 */
static
int maps_continue_stream_in_queued_objects_for_player(int playerid, struct STREAMING_DATA *streamingdata, int max_objs_to_create)
{
	struct BitStream bitstream;
	struct RPCDATA_CreateObject *obj;
	short tmp_saved_objdata_size;
	int num_created_objects;
	int objectid;
	int mapidx, i;
	short *objectid_to_objmapidx;

	mapidx = streamingdata->obj_map - obj_maps;
	objectid_to_objmapidx = player_objectid_to_objmapidx[playerid];
	objectid = 0; /*Note: samp starts at objectid 1, should we also do that? Seems like it's okay.*/
	num_created_objects = 0;
	while (streamingdata->num_queued_objects > 0) {
		while (objectid_to_objmapidx[objectid] >= 0) {
			objectid++;
			if (objectid >= MAX_MAPSYSTEM_OBJECTS) {
				logprintf("object limit hit while streaming map %s", streamingdata->obj_map->name);
				maps_print_object_breakdown(playerid);/*jeanine:l:9*/
				goto stop_streaming;
			}
		}
		obj = streamingdata->queued_objects[--streamingdata->num_queued_objects];
		objectid_to_objmapidx[objectid] = mapidx;
		/*The objectid field is used to store the size of the object data.*/
		/*Temporary save this size data while setting the correct objectid.*/
		tmp_saved_objdata_size = obj->objectid;
		obj->objectid = objectid;
		bitstream.ptrData = obj;
		bitstream.numberOfBitsUsed = tmp_saved_objdata_size * 8;
		SAMP_SendRPCToPlayer(RPC_CreateObject, &bitstream, playerid, 2);
		/*Reset the object data size field.*/
		obj->objectid = tmp_saved_objdata_size;
		if (++num_created_objects > max_objs_to_create) {
			return num_created_objects;
		}
	}
stop_streaming:

#ifdef MAPS_LOG_STREAMING
	logprintf("object map %s done streaming in for %d", streamingdata->obj_map->name, playerid);
#endif

	/*Map specific things.*/
	if (mapidx == octavia_island_actor_mapidx) {
		/*Octavia actor*/
		SendRPCToPlayer(playerid, RPC_ShowActor, &rpcdata_show_actor, sizeof(rpcdata_show_actor), 2);
	} else if (mapidx == wmre_mapidx) {
		/*WMRE windmill blades*/
		bitstream.ptrData = wmre_blade_template;
		tmp_saved_objdata_size = wmre_blade_template->objectid;
		bitstream.numberOfBitsUsed = tmp_saved_objdata_size * 8;
		for (i = 0; i < 12; i++) {
			while (objectid_to_objmapidx[objectid] >= 0) {
				objectid++;
				if (objectid >= MAX_MAPSYSTEM_OBJECTS) {
					logprintf("object limit hit while streaming wmre blades");
					maps_print_object_breakdown(playerid);
					goto skip_wmre_blades;
				}
			}
			/*Link to same mapidx, then we don't have to delete the blades manually on unload.*/
			objectid_to_objmapidx[objectid] = mapidx;
			wmre_blade_template->x = wmre_blade_x[i / 3];
			wmre_blade_template->y = wmre_blade_y[i / 3];
			/*Add zoffset and correct rx, so the blades are set up at the position for the next movement tick.*/
			wmre_blade_template->z = wmre_blade_z[i / 3] + *((float*) &wmre_blade_z_offset_i);
			if (i < 6) {
				wmre_blade_template->rx = (float) (wmre_blade_01_angle + 120 * (i % 3));
			} else {
				wmre_blade_template->rx = (float) (wmre_blade_23_angle + 120 * (i % 3));
			}
			wmre_blade_template->ry = 180.0f;
			wmre_blade_template->rz = -160.0f;
			wmre_blade_template->objectid = objectid;
			SAMP_SendRPCToPlayer(RPC_CreateObject, &bitstream, playerid, 2);
			wmre_blades_objectids[playerid][i] = objectid;
			num_created_objects++;
		}
skip_wmre_blades:
		wmre_blade_template->objectid = tmp_saved_objdata_size;
	}

	/*The special rotating radar object.*/
	/*This will override the existing object if it exists, but no maps with radars should have intersecting stream radii anyways.*/
	if (streamingdata->obj_map->radar_object) {
		radar_object_for_player[playerid] = streamingdata->obj_map->radar_object;
		bitstream.ptrData = streamingdata->obj_map->radar_object;
		bitstream.numberOfBitsUsed = sizeof(struct RPCDATA_CreateObject) * 8;
		SAMP_SendRPCToPlayer(RPC_CreateObject, &bitstream, playerid, 2);
	}

	streamingdata->obj_map->stream_status_for_player[playerid] = STREAMED_IN;
	streamingdata->obj_map = NULL;

	return num_created_objects;
}
/*jeanine:p:i:16;p:27;a:r;x:3802;y:13;*/
static
struct OBJECT_MAP *maps_find_next_object_map_to_stream_for_player(int playerid, float x, float y)
{
	float dx, dy;
	float candidate_distance_sq, selected_stream_in_radius_sq;
	struct OBJECT_MAP *selected, *candidate;

	selected = NULL;
	selected_stream_in_radius_sq = float_ninf;
	candidate = obj_maps;
	while (candidate != obj_maps + num_obj_maps) {
		if (candidate->stream_status_for_player[playerid] == STREAMED_OUT) {
			dx = candidate->middle_x - x;
			dy = candidate->middle_y - y;
			candidate_distance_sq = dx * dx + dy * dy;
			if (candidate_distance_sq < candidate->stream_in_radius_sq &&
				/*Prioritize maps that have longer stream distance.*/
				candidate->stream_in_radius_sq > selected_stream_in_radius_sq)
			{
				selected = candidate;
				selected_stream_in_radius_sq = candidate->stream_in_radius_sq;
			}
		}
		candidate++;
	}

#ifdef MAPS_LOG_STREAMING
	if (selected) {
		logprintf("picked obj map %s to stream in for %d", selected->name, playerid);
	}
#endif

	return selected;
}
/*jeanine:p:i:7;p:27;a:r;x:2860;y:-2;*/
/**
 * Does not check if a map is already being streamed in.
 * Does not check if objects in the map are already streamed in.
 *
 * If a map is already being streamed in, that streaming will be stopped and the map
 * will stay in an unfinished streaming state.
 */
static
void maps_prepare_streaming_object_map(int playerid, struct OBJECT_MAP *map, struct STREAMING_DATA *streamingdata)
{
	struct RPCDATA_CreateObject *obj;
	int i;

	map->stream_status_for_player[playerid] = STREAMING_IN;
	streamingdata->obj_map = map;
	streamingdata->num_queued_objects = 0;
	obj = map->objects;
	for (i = map->num_objects; i > 0; i--) {
		streamingdata->queued_objects[streamingdata->num_queued_objects++] = obj;
		obj = (void*) ((char*) obj + obj->objectid);
	}

#ifdef MAPS_LOG_STREAMING
	logprintf("queued %d objects for map %s", streamingdata->num_queued_objects, map->name);
#endif
}
/*jeanine:p:i:17;p:27;a:r;x:4712;y:3;*/
/**
 * Both objects and zones.
 */
static
void maps_stream_out_maps_for_player(int playerid, float posx, float posy)
{
	struct OBJECT_MAP *obj_map;
	struct ZONE_MAP *zone_map;
	float dx, dy, distance;

	for (obj_map = obj_maps; obj_map != obj_maps + num_obj_maps; obj_map++) {
		if (obj_map->stream_status_for_player[playerid] != STREAMED_OUT) {
			dx = obj_map->middle_x - posx;
			dy = obj_map->middle_y - posy;
			distance = dx * dx + dy * dy;
			if (distance > obj_map->stream_out_radius_sq) {
				if (streaming_data[playerid].obj_map == obj_map) {
					streaming_data[playerid].obj_map = NULL;
				}
				obj_map->stream_status_for_player[playerid] = STREAMED_OUT;
				maps_stream_out_objects_for_player(playerid, obj_map);/*jeanine:l:11*/
			}
		}
	}
	for (zone_map = zone_maps; zone_map != zone_maps + num_zone_maps; zone_map++) {
		if (zone_map->stream_status_for_player[playerid] != STREAMED_OUT) {
			dx = zone_map->middle_x - posx;
			dy = zone_map->middle_y - posy;
			distance = dx * dx + dy * dy;
			if (distance > zone_map->stream_out_radius_sq) {
				if (streaming_data[playerid].zone_map == zone_map) {
					streaming_data[playerid].zone_map = NULL;
				}
				zone_map->stream_status_for_player[playerid] = STREAMED_OUT;
				maps_stream_out_zones_for_player(playerid, zone_map);/*jeanine:l:19*/
			}
		}
	}
}
/*jeanine:p:i:18;p:27;a:r;x:124;y:-7;*/
static
struct ZONE_MAP *maps_find_next_zone_map_to_stream_for_player(int playerid, float x, float y)
{
	float dx, dy;
	float selected_distance_sq, candidate_distance_sq;
	struct ZONE_MAP *selected, *candidate;

	selected_distance_sq = float_pinf;
	selected = NULL;
	candidate = zone_maps;
	while (candidate != zone_maps + num_zone_maps) {
		if (candidate->stream_status_for_player[playerid] == STREAMED_OUT) {
			dx = candidate->middle_x - x;
			dy = candidate->middle_y - y;
			candidate_distance_sq = dx * dx + dy * dy;
			if (candidate_distance_sq < candidate->stream_in_radius_sq &&
				/*Prioritize zones that are closer.*/
				candidate_distance_sq < selected_distance_sq)
			{
				selected = candidate;
				selected_distance_sq = candidate_distance_sq;
			}
		}
		candidate++;
	}

#ifdef MAPS_LOG_STREAMING
	if (selected) {
		logprintf("picked zone %s to stream in for %d", selected->name, playerid);
	}
#endif

	return selected;
}
/*jeanine:p:i:15;p:27;a:r;x:2089;y:-9;*/
/**
 * Moves objects within {@link IMM_STREAMING_RADIUS_SQ} to the front of the queue (which is
 * actually at the end), and returns the amount of objects that is.
 */
static
int maps_prioritize_closest_objects(struct STREAMING_DATA *streamingdata, float x, float y)
{
	struct RPCDATA_CreateObject *obj;
	register float dx, dy;
	register int dist_sq;
	int i, j, count;

	i = j = streamingdata->num_queued_objects - 1;
	count = 0;
	for (; i >= 0; i--) {
		obj = streamingdata->queued_objects[i];
		dx = x - obj->x;
		dy = y - obj->y;
		dist_sq = (int) (dx * dx + dy * dy);
		if (dist_sq < IMM_STREAMING_RADIUS_SQ) {
			/*Swap current obj with obj on the current position where the
			prioritized objects end.*/
			streamingdata->queued_objects[i] = streamingdata->queued_objects[j];
			streamingdata->queued_objects[j] = obj;
			j--;
			count++;
		}
	}

#ifdef MAPS_LOG_STREAMING
	logprintf("prioritized %d objects", count);
#endif

	return count;
}
/*jeanine:p:i:5;p:27;a:r;x:38;y:506;*/
static
void maps_prepare_streaming_zone_map(int playerid, struct ZONE_MAP *map, struct STREAMING_DATA *streamingdata)
{
	map->stream_status_for_player[playerid] = STREAMING_IN;
	streamingdata->zone_map = map;
	streamingdata->next_zone_idx = 0;
}
/*jeanine:p:i:27;p:29;a:b;x:4;y:783;*/
enum OBJ_STREAM_MODE {
	OBJ_STREAM_MODE_ANY,
	OBJ_STREAM_MODE_CLOSEST_NOW,
};

/**
 * Streams out/in objects and zones.
 *
 * Limitations to {@code OBJ_STREAM_MODE_CLOSEST_NOW}:
 * - If a current map is still streaming in, that map will continue to stream in,
 *   even if it's not the closest.
 * - The next map picked to stream in might not be the closest map, as it will prefer maps
 *   with the greatest draw distance.
 *
 * @param mode - When {@link OBJ_STREAM_MODE_ANY}, will stream in up to
 *               {@link MAX_OBJECTS_TO_CREATE_PER_TICK} objects now.
 *             - When {@link OBJ_STREAM_MODE_CLOSEST_NOW}, will stream in all queued objects closer
 *               than {@link IMM_STREAMING_RADIUS_SQ} now, but only when a new map is being streamed in.
 *               Other objects may also be streamed in if the number of close objects is less than
 *               {@link MAX_OBJECTS_TO_CREATE_PER_TICK}.
 */
static
void maps_stream_for_player(int playerid, float posx, float posy, enum OBJ_STREAM_MODE mode)
{
	struct OBJECT_MAP *next_object_map;
	struct ZONE_MAP *next_zone_map;
	struct STREAMING_DATA *streamingdata;
	int max_objs_to_create;
	int num_objs_created;
	register int num_prioritized_objs;

	streamingdata = &streaming_data[playerid];

	maps_stream_out_maps_for_player(playerid, posx, posy);/*jeanine:l:17*/

	max_objs_to_create = MAX_OBJECTS_TO_CREATE_PER_TICK;
	num_objs_created = 0;
	do {
		if (!streamingdata->obj_map) {
			next_object_map = maps_find_next_object_map_to_stream_for_player(playerid, posx, posy);/*jeanine:l:16*/
			if (!next_object_map) {
				break;
			}
			maps_prepare_streaming_object_map(playerid, next_object_map, streamingdata);/*jeanine:l:7*/
		}
		if (mode == OBJ_STREAM_MODE_CLOSEST_NOW) {
			num_prioritized_objs = maps_prioritize_closest_objects(streamingdata, posx, posy);/*jeanine:l:15*/
			if (num_prioritized_objs > MAX_OBJECTS_TO_CREATE_PER_TICK) {
				max_objs_to_create = num_prioritized_objs;
			}
		}
		num_objs_created += maps_continue_stream_in_queued_objects_for_player(playerid, streamingdata, max_objs_to_create);/*jeanine:l:28*/
	} while (num_objs_created < MAX_OBJECTS_TO_CREATE_PER_TICK);

	if (!streamingdata->zone_map) {
		next_zone_map = maps_find_next_zone_map_to_stream_for_player(playerid, posx, posy);/*jeanine:l:18*/
		if (!next_zone_map) {
			return;
		}
		maps_prepare_streaming_zone_map(playerid, next_zone_map, streamingdata);/*jeanine:l:5*/
	}
	maps_continue_stream_in_queued_zones_for_player(playerid, streamingdata);/*jeanine:l:21*/
}
/*jeanine:p:i:26;p:20;a:b;x:0;y:30;*/
static
/**
 *Should be called from ProcessTick()
 */
void maps_process_tick(int elapsed_time)
{
	register struct vec3 *pos;
	register int playerid;

	int i;

	for (i = 0; i < playercount; i++) {
		playerid = players[i];
		next_streaming_tick_delay[playerid] -= elapsed_time;
		if (next_streaming_tick_delay[playerid] <= 0) {
			pos = &player[playerid]->pos;
			maps_stream_for_player(playerid, pos->x, pos->y, OBJ_STREAM_MODE_ANY);
			/*Do this in a loop to ensure we don't overload
			the streamer if some hang occurred.*/
			do {
				next_streaming_tick_delay[playerid] += MAPSTREAMING_UPDATE_INTERVAL;
			} while (next_streaming_tick_delay[playerid] <= 0);
		}
	}
}
/*jeanine:p:i:2;p:26;a:b;x:0;y:30;*/
static
void maps_on_player_connect(int playerid)
{
	struct BitStream bitstream;
	int i;

	bitstream.numberOfBitsUsed = sizeof(struct RPCDATA_RemoveBuilding) * 8;
	for (i = 0; i < num_removed_objects; i++) {
		bitstream.ptrData = &removed_objects[i];
		SAMP_SendRPCToPlayer(RPC_RemoveBuilding, &bitstream, playerid, 2);
	}

	memset(player_objectid_to_objmapidx[playerid], -1, sizeof(player_objectid_to_objmapidx[playerid]));
	memset(player_gangzoneid_to_zonemapidx[playerid], -1, sizeof(player_gangzoneid_to_zonemapidx[playerid]));
	memset(wmre_blades_objectids[playerid], -1, sizeof(wmre_blades_objectids[playerid]));
	for (i = num_obj_maps; i > 0; ) {
		obj_maps[--i].stream_status_for_player[playerid] = STREAMED_OUT;
	}
	for (i = num_zone_maps; i > 0; ) {
		zone_maps[--i].stream_status_for_player[playerid] = STREAMED_OUT;
	}
	streaming_data[playerid].obj_map = NULL;
	streaming_data[playerid].num_queued_objects = 0;
	streaming_data[playerid].zone_map = NULL;
	next_streaming_tick_delay[playerid] = 3000; /*Initial delay, different from repeating delay.*/
}
/*jeanine:p:i:29;p:2;a:b;x:0;y:30;*/
/**
 * To reset map stream states for player.
 */
static
void maps_on_player_disconnect(int playerid)
{
	int i;

	for (i = num_obj_maps; i > 0; ) {
		obj_maps[--i].stream_status_for_player[playerid] = STREAMED_OUT;
	}
	for (i = num_zone_maps; i > 0; ) {
		zone_maps[--i].stream_status_for_player[playerid] = STREAMED_OUT;
	}
}
