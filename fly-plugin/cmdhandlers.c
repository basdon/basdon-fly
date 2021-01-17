static
int cmd_admin_streamdistance(CMDPARAMS)
{
	int newdistance;
	float *stream_distance;
	char msg144[144];

	stream_distance = samphost_GetPtrStreamDistance();
	if (cmd_get_int_param(cmdtext, &parseidx, &newdistance)) {
		*stream_distance = (float) newdistance;
	}
	sprintf(msg144, "stream_distance=%.0f", *stream_distance);
	SendClientMessage(playerid, -1, msg144);
	return 1;
}

static
int cmd_admin_goto(CMDPARAMS)
{
	int oldparseidx;
	int targetplayerid;
	int x, y, z;
	struct vec4 pos;

	pos.r = 0.0f;

	oldparseidx = parseidx;
	if (cmd_get_int_param(cmdtext, &parseidx, &x) &&
		cmd_get_int_param(cmdtext, &parseidx, &y) &&
		cmd_get_int_param(cmdtext, &parseidx, &z))
	{
		/*TODO: accept interior id as param*/
		pos.coords.x = (float) x;
		pos.coords.y = (float) y;
		pos.coords.z = (float) z;
	} else {
		parseidx = oldparseidx;

		if (cmd_get_player_param(cmdtext, &parseidx, &targetplayerid)) {
			if (targetplayerid == INVALID_PLAYER_ID) {
				SendClientMessage(playerid, COL_WARN, "Target player is offline");
				return 1;
			}
			common_GetPlayerPos(targetplayerid, &pos.coords);
			/*TODO: same interior*/
			pos.coords.x += 0.3f;
			pos.coords.y += 0.3f;
		} else {
			SendClientMessage(playerid, COL_WARN, WARN"Syntax: //goto (<x> <y> <z>|<playerid/name>)");
			return 1;
		}
	}

	if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER) {
		common_SetVehiclePos(GetPlayerVehicleID(playerid), &pos.coords);
	} else {
		common_tp_player(playerid, pos);
	}

	return 1;
}

static
int cmd_admin_tocar(CMDPARAMS)
{
	struct vec3 vehicle_pos, player_pos;
	int vehicleid, closest_vehicleid, modelid, seat, jack;
	float dx, dy;
	float min_distance_sq;

	seat = 0;
	if (cmd_get_int_param(cmdtext, &parseidx, &vehicleid)) {
		cmd_get_int_param(cmdtext, &parseidx, &seat);
		if (GetVehicleModel(vehicleid)) {
			natives_PutPlayerInVehicle(playerid, vehicleid, seat);
		} else {
			SendClientMessage(playerid, COL_WARN, WARN"Invalid vehicle id");
		}
	} else if (cmd_get_vehiclemodel_param(cmdtext, &parseidx, &modelid)) {
		jack = 0;
		if (cmd_get_int_param(cmdtext, &parseidx, &seat)) {
			cmd_get_int_param(cmdtext, &parseidx, &jack);
		}
		closest_vehicleid = -1;
		min_distance_sq = float_pinf;
		common_GetPlayerPos(playerid, &player_pos);
		for (vehicleid = NC_GetVehiclePoolSize(); vehicleid >= 0; vehicleid--) {
			if (GetVehicleModel(vehicleid) == modelid) {
				GetVehiclePosUnsafe(vehicleid, &vehicle_pos);
				dx = player_pos.x - vehicle_pos.x;
				dy = player_pos.y - vehicle_pos.y;
				if (dx * dx + dy * dy < min_distance_sq &&
					(jack || common_find_player_in_vehicle_seat(vehicleid, seat) == INVALID_PLAYER_ID))
				{
					min_distance_sq = dx * dx + dy * dy;
					closest_vehicleid = vehicleid;
				}
			}
		}
		if (closest_vehicleid != -1) {
			if (seat == 0 && common_find_player_in_vehicle_seat(closest_vehicleid, 0) != INVALID_PLAYER_ID) {
				SendClientMessage(playerid, COL_INFO, INFO"Trying to jack existing driver (won't work)");
			}
			natives_PutPlayerInVehicle(playerid, closest_vehicleid, seat);
		} else {
			SendClientMessage(playerid, COL_WARN, WARN"None spawned or available");
		}
	} else {
		SendClientMessage(playerid, COL_WARN, WARN"Syntax: //tocar (<vehicleid>|<modelname>) [seat] [jack:1/0]");
	}
	return 1;
}

/**
//tomsp [id]

Teleport to a missionpoint.

Either closest missionpoint for matching vehicle (or any if not in vehicle), or to given id.
*/
static
int cmd_admin_tomsp(CMDPARAMS)
{
	struct vec3 *closest_pos, player_pos;
	int mspid;
	float dx, dy;
	float closest_distance_sq;
	int vehicleid;
	int msptype_mask;
	int mspindex;

	vehicleid = GetPlayerVehicleID(playerid);
	if (vehicleid) {
		msptype_mask = missions_get_vehicle_model_msptype_mask(GetVehicleModel(vehicleid));
	} else {
		msptype_mask = -1;
	}

	closest_pos = NULL;
	if (cmd_get_int_param(cmdtext, &parseidx, &mspid)) {
		for (mspindex = 0; mspindex < nummissionpoints; mspindex++) {
			if (missionpoints[mspindex].id == mspid) {
				closest_pos = &missionpoints[mspindex].pos;
				break;
			}
		}
	} else {
		closest_distance_sq = float_pinf;
		common_GetPlayerPos(playerid, &player_pos);
		for (mspindex = 0; mspindex < nummissionpoints; mspindex++) {
			if (missionpoints[mspindex].type & msptype_mask) {
				dx = player_pos.x - missionpoints[mspindex].pos.x;
				dy = player_pos.y - missionpoints[mspindex].pos.y;
				if (dx * dx + dy * dy < closest_distance_sq) {
					closest_distance_sq = dx * dx + dy * dy;
					closest_pos = &missionpoints[mspindex].pos;
				}
			}
		}
	}

	if (closest_pos) {
		if (vehicleid) {
			common_SetVehiclePos(vehicleid, closest_pos);
		} else {
			natives_SetPlayerPos(playerid, *closest_pos);
		}
	} else {
		SendClientMessage(playerid, COL_WARN, WARN"Can't find compatible mission point");
	}

	return 1;
}

static
int cmd_at400(CMDPARAMS)
{
	struct vec3 playerpos, vehiclepos;
	struct dbvehicle *veh;
	int vehicleid, found_vehicle;
	float dx, dy, dz, shortest_distance, tmpdistance;

	vehicleid = GetPlayerVehicleID(playerid);
	if (vehicleid) {
		return 1;
	}

	found_vehicle = 0;
	shortest_distance = float_pinf;
	common_GetPlayerPos(playerid, &playerpos);
	for (vehicleid = 0; vehicleid < MAX_VEHICLES; vehicleid++) {
		veh = gamevehicles[vehicleid].dbvehicle;
		if (veh != NULL &&
			veh->model == MODEL_AT400 &&
			veh_is_player_allowed_in_vehicle(playerid, veh))
		{
			GetVehiclePosUnsafe(vehicleid, &vehiclepos);
			dx = vehiclepos.x - playerpos.x;
			dy = vehiclepos.y - playerpos.y;
			dz = vehiclepos.z - playerpos.z;
			tmpdistance = dx * dx + dy * dy + dz * dz;
			if (tmpdistance < shortest_distance &&
				common_find_vehicle_driver(vehicleid)
					== INVALID_PLAYER_ID)
			{
				shortest_distance = tmpdistance;
				found_vehicle = vehicleid;
			}
		}
	}
	if (found_vehicle && shortest_distance < 25.0f * 25.0f) {
		natives_PutPlayerInVehicle(playerid, found_vehicle, 0);
	}

	return 1;
}

/**
The /camera command gives the player a camera.
*/
static
int cmd_camera(CMDPARAMS)
{
	NC_GivePlayerWeapon(playerid, WEAPON_CAMERA, 3036);
	return 1;
}

/**
The /chute command gives the player a parachute.
*/
static
int cmd_chute(CMDPARAMS)
{
	NC_GivePlayerWeapon(playerid, WEAPON_PARACHUTE, 1);
	return 1;
}

/**
The /engine command starts or stops the engine of the vehicle the player is currently driving.
*/
static
int cmd_engine(CMDPARAMS)
{
	if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER) {
		veh_start_or_stop_engine(playerid, GetPlayerVehicleID(playerid));
	}
	return 1;
}

/**
The /getspray command gets the colors of the vehicle the player is in.
*/
static
int cmd_getspray(CMDPARAMS)
{
	struct dbvehicle *veh;
	int vehicleid;
	char msg144[144];

	vehicleid = GetPlayerVehicleID(playerid);
	if (vehicleid && (veh = gamevehicles[vehicleid].dbvehicle)) {
		sprintf(msg144, "colors: %d, %d", veh->col1, veh->col2);
		SendClientMessage(playerid, -1, msg144);
	}
	return 1;
}

/**
The /helpkeys command shows gametext with important keys.
*/
static
int cmd_helpkeys(CMDPARAMS)
{
	B144("~w~start/stop engine: ~b~~k~~CONVERSATION_NO~~n~"
		"~w~landing gear: ~b~~k~~TOGGLE_SUBMISSIONS~");
	NC_GameTextForPlayer(playerid, buf144a, 5000, 3);
	return 1;
}

static
int cmd_me(CMDPARAMS)
{
	char *to, *from;
	int hascontent = 0;
	char msg[150 + MAX_PLAYER_NAME];

	from = (char*) cmdtext + 3;
	to = msg + sprintf(msg, "* %s", pdata[playerid]->name);
	while ((*to = *from)) {
		if (*from != ' ') {
			hascontent = 1;
		}
		from++;
		to++;
	}
	if (hascontent) {
		SendClientMessageToAll(-1, msg);
		echo_on_game_chat_or_action(1, playerid, (char*) cmdtext + 4);
	} else {
		SendClientMessage(playerid, COL_WARN, WARN"Syntax: /me <action>");
	}
	return 1;
}

static
int cmd_park(CMDPARAMS)
{
	struct dbvehicle *veh;
	int vehicleid;
	float lastrot;

	vehicleid = GetPlayerVehicleID(playerid);
	if (vehicleid) {
		veh = gamevehicles[vehicleid].dbvehicle;
		if (!veh_can_player_modify_veh(playerid, veh)) {
			SendClientMessage(playerid, COL_WARN, WARN"You are not allowed to park this vehicle");
			return 1;
		}
		lastrot = veh->pos.r;
		GetVehiclePosRotUnsafe(vehicleid, &veh->pos);
		if (356.0f < veh->pos.r || veh->pos.r < 4.0f) {
			veh->pos.r = 0.0f;
		} else if (86.0f < veh->pos.r && veh->pos.r < 94.0f) {
			veh->pos.r = 90.0f;
		} else if (176.0f < veh->pos.r && veh->pos.r < 184.0f) {
			veh->pos.r = 180.0f;
		} else if (266.0f < veh->pos.r && veh->pos.r < 274.0f) {
			veh->pos.r = 270.0f;
		}
		if (fabs(lastrot - veh->pos.r) > 3.0f) {
			gamevehicles[vehicleid].need_recreation = 1;
		}
		csprintf(buf144,
			"UPDATE veh SET x=%f,y=%f,z=%f,r=%f WHERE i=%d",
			veh->pos.coords.x,
			veh->pos.coords.y,
			veh->pos.coords.z,
			veh->pos.r,
			veh->id);
		NC_mysql_tquery_nocb(buf144a);
		SendClientMessage(playerid, COL_SUCC, SUCC"Vehicle parked!");
	}
	return 1;
}

#define NO_RECLASSSPAWN WARN"You cannot reclass/respawn while on a mission. "\
			"Use /s to cancel your current mission for a fee "\
			"($"EQ(MISSION_CANCEL_FINE)")."

static
int cmd_reclass(CMDPARAMS)
{
	if (missions_is_player_on_mission(playerid)) {
		SendClientMessage(playerid, COL_WARN, NO_RECLASSSPAWN);
	} else {
		spawned[playerid] = 0;
		NC_ForceClassSelection(playerid);
		NC_TogglePlayerSpectating(playerid, 1);
		nc_params[2] = 0;
		NC(n_TogglePlayerSpectating);
	}
	return 1;
}

static
int cmd_respawn(CMDPARAMS)
{
	if (missions_is_player_on_mission(playerid)) {
		SendClientMessage(playerid, COL_WARN, NO_RECLASSSPAWN);
	} else {
		natives_SpawnPlayer(playerid);
	}
	return 1;
}

/**
The /spray [col1] [col2] command allows a player to spray their vehicle.

One or both colors may be omitted to get a fully random color. A 'normal'
random color combination is chosen when both colors are set to -1. These colors
are the colors as defined by the game itself.
*/
static
int cmd_spray(CMDPARAMS)
{
	struct dbvehicle *veh;
	int vehicleid, *col1, *col2, *a, *b;

	vehicleid = GetPlayerVehicleID(playerid);
	if (vehicleid) {
		veh = gamevehicles[vehicleid].dbvehicle;
		if (!veh_can_player_modify_veh(playerid, veh)) {
			SendClientMessage(playerid, COL_WARN, WARN"You are not allowed to respray this vehicle");
			return 1;
		}
		a = b = NULL;
		col1 = nc_params + 2;
		col2 = nc_params + 3;
		if (cmd_get_int_param(cmdtext, &parseidx, col1)) {
			if (*col1 == -1) {
				a = col1;
			}
			if (!cmd_get_int_param(cmdtext, &parseidx, col2)) {
				goto rand2nd;
			}
			if (*col2 == -1) {
				b = col2;
			}
		} else {
			*col1 = NC_random(256);
rand2nd:
			*col2 = NC_random(256);
		}
		if (a != NULL || b != NULL) {
			game_random_carcol(GetVehicleModel(vehicleid), a, b);
		}
		NC_PARS(3);
		nc_params[1] = vehicleid;
		NC(n_ChangeVehicleColor);
		if (veh != NULL) {
			veh->col1 = (unsigned char) *col1;
			veh->col2 = (unsigned char) *col2;
			csprintf(buf144,
				"UPDATE veh SET col1=%d,col2=%d WHERE i=%d",
				veh->col1,
				veh->col2,
				veh->id);
			NC_mysql_tquery_nocb(buf144a);
		}
	}
	return 1;
}

/**
The /tickrate command to print the server's current tick rate.
*/
static
int cmd_tickrate(CMDPARAMS)
{
	char msg144[144];

	sprintf(msg144, "%d", (int) NC_GetServerTickRate());
	SendClientMessage(playerid, -1, msg144);
	return 1;
}
