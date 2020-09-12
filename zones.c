static int lastzoneid[MAX_PLAYERS];
static int lastregionid[MAX_PLAYERS];
static int lastzoneindex[MAX_PLAYERS];

static struct TEXTDRAW td_gps = { "gps", 200, 0, NULL };

static char gps_text_should_show[MAX_PLAYERS];
static char gps_text_is_shown[MAX_PLAYERS];

static
void zones_init()
{
	struct REGION *r = regions, *rmax = regions + regioncount;
	struct ZONE *pz = zones;
	int z = 0, zmax = zonecount;
#ifdef ZONES_DEBUG
	int zonesinregion = 0;
#endif /*ZONES_DEBUG*/

	while (z < zmax) {
nr:
		if (pz->min_x < r->zone.min_x || r->zone.max_x < pz->max_x ||
			pz->min_y < r->zone.min_y || r->zone.max_y < pz->max_y)
		{
#ifdef ZONES_DEBUG
			printf("%d zones in region %d\n", zonesinregion,
				r - regions);
			zonesinregion = 0;
#endif /*ZONES_DEBUG*/
			r->maxzone = z;
			r++;
			if (r >= rmax) {
				logprintf("zones.c error: at zone %d but "
					"regions are depleted", z);
				return;
			}
			r->minzone = z;
			goto nr;
		}
		z++;
		pz++;
#ifdef ZONES_DEBUG
		zonesinregion++;
#endif /*ZONES_DEBUG*/
	}
	r->maxzone = z;

	for (z = 0; z < MAX_PLAYERS; z++) {
		lastzoneindex[z] = lastzoneid[z] = -1;
		lastregionid[z] = ZONE_INVALID;
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
	gps_text_should_show[playerid] = 0;
	gps_text_is_shown[playerid] = 0;
	lastzoneindex[playerid] = lastzoneid[playerid] = -1;
	lastregionid[playerid] = ZONE_INVALID;
}

/**
Check if a single player their zone has changed, and update it if needed.

Should also be called when OnPlayerSetPos is called.
*/
static
void zones_update(int playerid, struct vec3 pos)
{
	struct REGION *r = regions, *rmax = regions + regioncount;
	struct ZONE *pz;
	int i;
	int lrid = lastregionid[playerid];
	int lzid = lastzoneid[playerid];
	char *target_buffer;
	short td_text_length;

	if (lastzoneindex[playerid] >= 0 && zones_is_in_zone(pos, zones + lastzoneindex[playerid])) {
		if (gps_text_should_show[playerid] && !gps_text_is_shown[playerid]) {
			goto showtext;
		}
		return;
	}

	lastzoneindex[playerid] = -1;
	lastzoneid[playerid] = -1;
	r = regions;
	rmax = regions + regioncount;

	while (r < rmax) {
		if (zones_is_in_zone(pos, &r->zone)) {
			lastregionid[playerid] = r->zone.id;
			for (i = r->minzone; i < r->maxzone; i++) {
				pz = zones + i;
				if (zones_is_in_zone(pos, pz)) {
					lastzoneindex[playerid] = i;
					lastzoneid[playerid] = pz->id;
					goto gotcha;
				}
			}
			goto gotcha;
		}
		r++;
	}

	lastregionid[playerid] = ZONE_NONE_NW + ((pos.y < 0.0f) << 1) + (pos.x > 0.0f);
gotcha:
	if (!gps_text_should_show[playerid]) {
		return;
	}

	if (lrid != lastregionid[playerid] || lzid != lastzoneid[playerid]) {
showtext:
		if (gps_text_is_shown[playerid]) {
			target_buffer = rpcdata_freeform.byte + 2 + 2; /*see struct RPCDATA_TextDrawSetString*/
		} else {
			target_buffer = td_gps.rpcdata->text;
		}

		if (lastzoneid[playerid] != -1) {
			td_text_length = sprintf(target_buffer, "%s~n~%s", zonenames[lastzoneid[playerid]], zonenames[lastregionid[playerid]]);
		} else {
			/*regionid should _always_ be valid*/
			td_text_length = sprintf(target_buffer, "~n~%s", zonenames[lastregionid[playerid]]);
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

/**
Update player zone for all players, should be called periodically.
*/
static
void zones_update_for_all()
{
	int idx, playerid;
	struct vec3 pos;

	idx = playercount;
	while (idx--) {
		playerid = players[idx];
		if (!isafk[playerid]) {
			common_GetPlayerPos(playerid, &pos);
			zones_update(playerid, pos);
		}
	}
}

static
int zones_cmd_loc(CMDPARAMS)
{
	int target, vehicleid, model;
	struct vec3 pos;
	float vx, vy, vz;
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
	if (lastzoneid[target] >= 0) {
		b += sprintf(b, "%s, ", zonenames[lastzoneid[target]]);
	}
	b += sprintf(b, "%s ", zonenames[lastregionid[target]]);
	vehicleid = NC_GetPlayerVehicleID(target);
	if (vehicleid) {
		model = NC_GetVehicleModel(vehicleid);
		NC_GetVehicleVelocity(vehicleid, buf32a, buf64a, buf144a);
		vx = *fbuf32;
		vy = *fbuf64;
		vz = *fbuf144;
		sprintf(b, "travelling at %.0f KPH in a %s (%.0f FT)",
			VEL_TO_KPH * sqrt(vx * vx + vy * vy + vz * vz),
			vehnames[model - 400],
			pos.z);
	} else {
		sprintf(b, "on foot (%.0f FT)", pos.z);
	}

	SendClientMessage(playerid, COL_INFO_GENERIC, buf);
	return 1;
}

/**
Shows and updates zone textdraw for player, to be called from OnPlayerSpawn.
*/
static
void zones_on_player_spawn(int playerid, struct vec3 pos)
{
	gps_text_should_show[playerid] = 1;
	zones_update(playerid, pos);
}

/**
Hides the zone textdraw.

Call in OnPlayerDeath
*/
static
void zones_hide_text(int playerid)
{
	if (gps_text_should_show[playerid]) {
		gps_text_should_show[playerid] = 0;
		gps_text_is_shown[playerid] = 0;
		textdraws_hide_consecutive(playerid, 1, TEXTDRAW_GPS);
	}
}
