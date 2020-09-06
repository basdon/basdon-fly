/*TODO put this is a more decently named file.. samp.c?*/

/**
Callback for timer kicking a player, backup when OnPlayerUpdate doesn't get called.
*/
static
int kick_timer_cb(void *data)
#ifdef NATIVES_IMPL
{
	int playerid;

	playerid = PLAYER_CC_GETID(data);
	if (PLAYER_CC_CHECK(data, playerid)) {
		NC_PARS(1);
		nc_params[1] = playerid;
		NC(n_Kick_);
	}
	return 0;
}
#endif
;

/**
Kicks (delayed), broadcasts message and logs.

@param reason NULL to not broadcast a message (will have empty reason in table)
@param issuer name of issuer (NULL for system kick) (does not get saved to db)
@param issuer_userid may be -1 for system kick
@return 1 if the player is kicked, 0 if they were already kicked.
*/
static
int natives_Kick(int playerid, char *reason, char *issuer, int issuer_userid)
#ifdef NATIVES_IMPL
{
	const static char *SYSTEM_ISSUER = "system", *REASONNULL = "(NULL)";

	int intv;
	char *escapedreason;

	if (!kick_update_delay[playerid]) {
		if (issuer == NULL) {
			issuer = (char*) SYSTEM_ISSUER;
		}

		if (reason != NULL) {
			csprintf(buf144,
				"%s[%d] was kicked by %s (%s)",
				pdata[playerid]->name,
				playerid,
				issuer,
				reason);
			NC_SendClientMessageToAll(COL_WARN, buf144a);
		}

		B144("~r~You've been kicked.");
		NC_GameTextForPlayer(playerid, buf144a, 0x800000, 3);
		kick_update_delay[playerid] = 2;

		if (reason != NULL) {
			escapedreason = malloc(1000);
			common_mysql_escape_string(reason, escapedreason, 1000);
			reason = escapedreason;
		} else {
			escapedreason = NULL;
			reason = (char*) REASONNULL;
		}

		csprintf(buf4096,
			"INSERT INTO kck(usr,ip,stamp,issuer,reason)"
			"VALUES ("
			"IF(%d<1,NULL,%d),'%s',UNIX_TIMESTAMP(),"
			"IF(%d<1,NULL,%d),'%s')",
			userid[playerid],
			userid[playerid],
			pdata[playerid]->ip,
			issuer_userid,
			issuer_userid,
			escapedreason);
		NC_mysql_tquery_nocb(buf4096a);
		free(escapedreason);

		/*Interval should be smaller than 1000, since if it's a kick
		caused by system, player should be gone before the next
		check (since most of them run at 1Hz).*/
		intv = NC_GetPlayerPing(playerid) * 2;
		if (intv > 970) {
			intv = 970;
		}
		if (intv < 20) {
			intv = 20;
		}
		timer_set(intv, kick_timer_cb, (void*) MK_PLAYER_CC(playerid));
		return 1;
	}
	return 0;
}
#endif
;

/**
Puts a player in vehicle, updating map, zone, vehiclestuff ...

Also resets the vehicle HP to 1000.0 when it's invalid.
*/
static
int natives_PutPlayerInVehicle(int playerid, int vehicleid, int seat)
#ifdef NATIVES_IMPL
{
	float hp;
	struct vec3 pos;

	if (seat == 0) {
		NC_PARS(2);
		nc_params[1] = vehicleid;
		nc_params[2] = buf32a;
		NC(n_GetVehicleHealth_);
		hp = *fbuf32;
		if (hp != hp || hp < 0.0f || 1000.0f < hp) {
			NC_SetVehicleHealth(vehicleid, 1000.0f);
		}
	}

	common_GetVehiclePos(vehicleid, &pos);
	maps_stream_for_player(playerid, pos);
	veh_on_player_now_driving(playerid, vehicleid, gamevehicles[vehicleid].dbvehicle);
	svp_update_mapicons(playerid, pos.x, pos.y);
	missions_available_msptype_mask[playerid] = missions_get_vehicle_model_msptype_mask(NC_GetVehicleModel(vehicleid));
	missions_update_missionpoint_indicators(playerid, pos.x, pos.y, pos.z);
	zones_update(playerid, pos);

	NC_PARS(3);
	nc_params[1] = playerid;
	nc_params[2] = vehicleid;
	nc_params[3] = seat;
	return NC(n_PutPlayerInVehicle_);
}
#endif
;

/**
To store player name and normalized name in plugin.
*/
static
int natives_SetPlayerName(int playerid, char *name)
#ifdef NATIVES_IMPL
{
	int res;

	atoc(buf32, name, 32);
	NC_PARS(2);
	nc_params[1] = playerid;
	nc_params[2] = buf32a;
	res = NC(n_SetPlayerName_);
	if (res) {
		if (pdata[playerid]->name != name) {
			strcpy(pdata[playerid]->name, name);
		}
		pdata_on_name_updated(playerid);
		csprintf(buf144,
			"Your name has been changed to '%s'",
			pdata[playerid]->name);
		NC_PARS(3);
		nc_params[2] = COL_SAMP_GREEN;
		nc_params[3] = buf144a;
		NC(n_SendClientMessage);
	}
	return res;
}
#endif
;

/**
Done here to do stuff, like streaming maps, anticheat?
*/
static
int natives_SetPlayerPos(int playerid, struct vec3 pos)
#ifdef NATIVES_IMPL
{
	/*TODO? Technically the amount of _models_ and not _objects_ determines how long
	it will take to load, because the models are loaded in a blocking way.
	Because of that, a map with 100 objects but all different models might block more
	than a 1000 object map with 1 model, but the check below is pure object count.*/
	if (maps_calculate_objects_to_create_for_player_at_position(playerid, pos) > 200) {
		B144("Loading objects...");
		NC_GameTextForPlayer(playerid, buf144a, 0x80000, 5);
	}
	maps_stream_for_player(playerid, pos);
	B144("_");
	NC_GameTextForPlayer(playerid, buf144a, 0, 5);
	svp_update_mapicons(playerid, pos.x, pos.y);
	missions_update_missionpoint_indicators(playerid, pos.x, pos.y, pos.z);
	zones_update(playerid, pos);

	NC_PARS(4);
	nc_params[1] = playerid;
	nc_paramf[2] = pos.x;
	nc_paramf[3] = pos.y;
	nc_paramf[4] = pos.z;
	return NC(n_SetPlayerPos_);
}
#endif
;

/**
SpawnPlayer kills players that are in vehicles, and spawns them with a bottle.

So this wrapper does SetPlayerPos (directly) if needed, because that will
eject a player.
*/
static
void natives_SpawnPlayer(int playerid)
#ifdef NATIVES_IMPL
{
	/*eject player first if they're in a vehicle*/
	if (NC_GetPlayerVehicleID(playerid)) {
		NC_GetPlayerPos(playerid, buf32a, buf64a, buf144a);
		NC(n_SetPlayerPos_);
	}
	NC_PARS(1);
	/*nc_params[1] = playerid;*/
	NC(n_SpawnPlayer_);

	spawn_prespawn(playerid);
}
#endif
;
