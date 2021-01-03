static int zone_last_id[MAX_PLAYERS];
static int zone_last_region[MAX_PLAYERS];
static int zone_last_index[MAX_PLAYERS];
static char gps_text_is_shown[MAX_PLAYERS];

static struct TEXTDRAW td_gps = { "gps", 200, NULL };

#define GPS_SHOULD_SHOW(PLAYERID) (spawned[PLAYERID] && (prefs[playerid] & PREF_SHOW_GPS))

static
void zones_init()
{
	struct REGION *region, *region_end;
	struct ZONE *pz = zones;
	int z = 0, zmax = zonecount;
#ifdef ZONES_DEBUG
	int zonesinregion = 0;
#endif /*ZONES_DEBUG*/

	region = regions;
	region_end = regions + regioncount;

	while (z < zmax) {
nr:
		if (pz->min_x < region->zone.min_x || region->zone.max_x < pz->max_x ||
			pz->min_y < region->zone.min_y || region->zone.max_y < pz->max_y)
		{
#ifdef ZONES_DEBUG
			printf("%d zones in region %d\n", zonesinregion, region - regions);
			zonesinregion = 0;
#endif /*ZONES_DEBUG*/
			region->to_zone_idx_exclusive = z;
			region++;
			if (region >= region_end) {
				logprintf("zones.c error: at zone %d but " "regions are depleted", z);
				return;
			}
			region->from_zone_idx = z;
			goto nr;
		}
		z++;
		pz++;
#ifdef ZONES_DEBUG
		zonesinregion++;
#endif /*ZONES_DEBUG*/
	}
	region->to_zone_idx_exclusive = z;

	for (z = 0; z < MAX_PLAYERS; z++) {
		zone_last_index[z] = zone_last_id[z] = -1;
		zone_last_region[z] = ZONE_INVALID;
	}

	textdraws_load_from_file("gps", TEXTDRAW_GPS, 1, &td_gps);
}

/**
Check if given position is inside given zone
*/
static
int zones_is_in_zone(struct vec3 pos, struct ZONE *zone)
{
	return zone->min_x <= pos.x && pos.x <= zone->max_x &&
		zone->min_y <= pos.y && pos.y <= zone->max_y &&
		zone->min_z <= pos.z && pos.z <= zone->max_z;
}

static
void zones_on_player_connect(int playerid)
{
	gps_text_is_shown[playerid] = 0;
	zone_last_index[playerid] = zone_last_id[playerid] = -1;
	zone_last_region[playerid] = ZONE_INVALID;
}

/**
Check if a single player their zone has changed, and update it if needed.

Should also be called when OnPlayerSetPos is called.
*/
static
void zones_update(int playerid, struct vec3 pos)
{
	struct REGION *region, *region_end;
	int zoneindex;
	int previous_last_region_id, previous_last_zone_id;
	char *target_buffer;
	short td_text_length;

	if (zone_last_index[playerid] >= 0 && zones_is_in_zone(pos, zones + zone_last_index[playerid])) {
		if (GPS_SHOULD_SHOW(playerid)) {
			if (!gps_text_is_shown[playerid]) {
				goto showtext;
			}
		} else {
			goto hide_if_shown_and_return;
		}
		return;
	}

	previous_last_region_id = zone_last_region[playerid];
	previous_last_zone_id = zone_last_id[playerid];
	zone_last_index[playerid] = -1;
	zone_last_id[playerid] = -1;
	region = regions;
	region_end = regions + regioncount;

	while (region < region_end) {
		if (zones_is_in_zone(pos, &region->zone)) {
			zone_last_region[playerid] = region->zone.id;
			for (zoneindex = region->from_zone_idx; zoneindex < region->to_zone_idx_exclusive; zoneindex++) {
				if (zones_is_in_zone(pos, &zones[zoneindex])) {
					zone_last_index[playerid] = zoneindex;
					zone_last_id[playerid] = zones[zoneindex].id;
					goto gotcha;
				}
			}
			goto gotcha;
		}
		region++;
	}

	zone_last_region[playerid] = ZONE_NONE_NW + ((pos.y < 0.0f) << 1) + (pos.x > 0.0f);
gotcha:
	if (!GPS_SHOULD_SHOW(playerid)) {
hide_if_shown_and_return:
		if (gps_text_is_shown[playerid]) {
			gps_text_is_shown[playerid] = 0;
			textdraws_hide_consecutive(playerid, 1, TEXTDRAW_GPS);
		}
		return;
	} else if (!gps_text_is_shown[playerid]) {
		goto showtext;
	}

	if (previous_last_region_id != zone_last_region[playerid] || previous_last_zone_id != zone_last_id[playerid]) {
showtext:
		if (gps_text_is_shown[playerid]) {
			target_buffer = rpcdata_freeform.byte + 2 + 2; /*see struct RPCDATA_TextDrawSetString*/
		} else {
			target_buffer = td_gps.rpcdata->text;
		}

		if (zone_last_id[playerid] != -1) {
			td_text_length = sprintf(target_buffer, "%s~n~%s",
						zonenames[zone_last_id[playerid]],
						zonenames[zone_last_region[playerid]]);
		} else {
			/*regionid should _always_ be valid*/
			td_text_length = sprintf(target_buffer, "~n~%s", zonenames[zone_last_region[playerid]]);
		}

		if (gps_text_is_shown[playerid]) {
			rpcdata_freeform.word[0] = td_gps.rpcdata->textdrawid;
			rpcdata_freeform.word[1] = td_text_length;
			bitstream_freeform.ptrData = &rpcdata_freeform;
			bitstream_freeform.numberOfBitsUsed = (2 + 2 + td_text_length) * 8;
			SAMP_SendRPCToPlayer(RPC_TextDrawSetString, &bitstream_freeform, playerid, 2);
		} else {
			gps_text_is_shown[playerid] = 1;
			td_gps.rpcdata->text_length = td_text_length;
			textdraws_show(playerid, 1, &td_gps);
		}
	}
}

static
int zones_cmd_loc(CMDPARAMS)
{
	int target, vehicleid, model;
	struct vec3 pos;
	struct vec3 vvel;
	char buf[144], *b;

	if (!cmd_get_player_param(cmdtext, &parseidx, &target)) {
		SendClientMessage(playerid, COL_WARN, WARN"Syntax: /loc [id/part of name]");
		return 1;
	}
	if (target == INVALID_PLAYER_ID) {
		SendClientMessage(playerid, COL_WARN, WARN"That player is not online.");
		return 1;
	}
	common_GetPlayerPos(target, &pos);
	zones_update(target, pos);

	b = buf;
	b += sprintf(buf, "%s(%d) is located in ",
		pdata[target]->name, target);
	if (zone_last_id[target] >= 0) {
		b += sprintf(b, "%s, ", zonenames[zone_last_id[target]]);
	}
	b += sprintf(b, "%s ", zonenames[zone_last_region[target]]);
	vehicleid = NC_GetPlayerVehicleID(target);
	if (vehicleid) {
		model = NC_GetVehicleModel(vehicleid);
		GetVehicleVelocityUnsafe(vehicleid, &vvel);
		sprintf(b, "travelling at %.0f KPH in a %s (%.0f FT)",
			VEL_TO_KPH * sqrt(vvel.x * vvel.x + vvel.y * vvel.y + vvel.z * vvel.z),
			vehnames[model - 400],
			pos.z);
	} else {
		sprintf(b, "on foot (%.0f FT)", pos.z);
	}

	SendClientMessage(playerid, COL_INFO_GENERIC, buf);
	return 1;
}
