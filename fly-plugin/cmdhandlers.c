static
int cmd_admin_streamdistance(struct COMMANDCONTEXT cmdctx)
{
	int newdistance;
	float *stream_distance;
	char msg144[144];

	stream_distance = samphost_GetPtrStreamDistance();
	if (cmd_get_int_param(&cmdctx, &newdistance)) {
		*stream_distance = (float) newdistance;
	}
	sprintf(msg144, "stream_distance=%.0f", *stream_distance);
	SendClientMessage(cmdctx.playerid, -1, msg144);
	return 1;
}

static
int cmd_admin_goto(struct COMMANDCONTEXT cmdctx)
{
	int oldparseidx;
	int targetplayerid;
	int x, y, z;
	struct vec4 pos;

	pos.r = 0.0f;

	oldparseidx = cmdctx.parseidx;
	if (cmd_get_int_param(&cmdctx, &x) &&
		cmd_get_int_param(&cmdctx, &y) &&
		cmd_get_int_param(&cmdctx, &z))
	{
		/*TODO: accept interior id as param*/
		pos.coords.x = (float) x;
		pos.coords.y = (float) y;
		pos.coords.z = (float) z;
	} else {
		cmdctx.parseidx = oldparseidx;

		if (cmd_get_player_param(&cmdctx, &targetplayerid)) {
			if (targetplayerid == INVALID_PLAYER_ID) {
				SendClientMessage(cmdctx.playerid, COL_WARN, "Target player is offline");
				return 1;
			}
			GetPlayerPos(targetplayerid, &pos.coords);
			/*TODO: same interior*/
			pos.coords.x += 0.3f;
			pos.coords.y += 0.3f;
		} else {
			SendClientMessage(cmdctx.playerid, COL_WARN, WARN"Syntax: //goto (<x> <y> <z>|<playerid/name>)");
			return 1;
		}
	}

	if (GetPlayerState(cmdctx.playerid) == PLAYER_STATE_DRIVER) {
		common_SetVehiclePos(GetPlayerVehicleID(cmdctx.playerid), &pos.coords);
	} else {
		common_tp_player(cmdctx.playerid, pos);
	}

	return 1;
}

static
int cmd_admin_tocar(struct COMMANDCONTEXT cmdctx)
{
	struct vec3 vehicle_pos, player_pos;
	int vehicleid, closest_vehicleid, modelid, seat, jack;
	float dx, dy;
	float min_distance_sq;

	seat = 0;
	if (cmd_get_int_param(&cmdctx, &vehicleid)) {
		cmd_get_int_param(&cmdctx, &seat);
		if (GetVehicleModel(vehicleid)) {
			natives_PutPlayerInVehicle(cmdctx.playerid, vehicleid, seat);
		} else {
			SendClientMessage(cmdctx.playerid, COL_WARN, WARN"Invalid vehicle id");
		}
	} else if (cmd_get_vehiclemodel_param(&cmdctx, &modelid)) {
		jack = 0;
		if (cmd_get_int_param(&cmdctx, &seat)) {
			cmd_get_int_param(&cmdctx, &jack);
		}
		closest_vehicleid = -1;
		min_distance_sq = float_pinf;
		GetPlayerPos(cmdctx.playerid, &player_pos);
		for (vehicleid = NC_GetVehiclePoolSize(); vehicleid >= 0; vehicleid--) {
			if (GetVehicleModel(vehicleid) == modelid) {
				GetVehiclePosUnsafe(vehicleid, &vehicle_pos);
				dx = player_pos.x - vehicle_pos.x;
				dy = player_pos.y - vehicle_pos.y;
				if (dx * dx + dy * dy < min_distance_sq &&
					(jack || GetPlayerInVehicleSeat(vehicleid, seat) == INVALID_PLAYER_ID))
				{
					min_distance_sq = dx * dx + dy * dy;
					closest_vehicleid = vehicleid;
				}
			}
		}
		if (closest_vehicleid != -1) {
			if (seat == 0 && GetPlayerInVehicleSeat(closest_vehicleid, 0) != INVALID_PLAYER_ID) {
				SendClientMessage(cmdctx.playerid, COL_INFO, INFO"Trying to jack existing driver (won't work)");
			}
			natives_PutPlayerInVehicle(cmdctx.playerid, closest_vehicleid, seat);
		} else {
			SendClientMessage(cmdctx.playerid, COL_WARN, WARN"None spawned or available");
		}
	} else {
		SendClientMessage(cmdctx.playerid, COL_WARN, WARN"Syntax: //tocar (<vehicleid>|<modelname>) [seat] [jack:1/0]");
	}
	return 1;
}

/**
//tomsp [id]

Teleport to a missionpoint.

Either closest missionpoint for matching vehicle (or any if not in vehicle), or to given id.
*/
static
int cmd_admin_tomsp(struct COMMANDCONTEXT cmdctx)
{
	struct vec3 *closest_pos, player_pos;
	int mspid;
	float dx, dy;
	float closest_distance_sq;
	int vehicleid;
	int msptype_mask;
	int mspindex;

	vehicleid = GetPlayerVehicleID(cmdctx.playerid);
	if (vehicleid) {
		msptype_mask = missions_get_vehicle_model_msptype_mask(GetVehicleModel(vehicleid));
	} else {
		msptype_mask = -1;
	}

	closest_pos = NULL;
	if (cmd_get_int_param(&cmdctx, &mspid)) {
		for (mspindex = 0; mspindex < nummissionpoints; mspindex++) {
			if (missionpoints[mspindex].id == mspid) {
				closest_pos = &missionpoints[mspindex].pos;
				break;
			}
		}
	} else {
		closest_distance_sq = float_pinf;
		GetPlayerPos(cmdctx.playerid, &player_pos);
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
			natives_SetPlayerPos(cmdctx.playerid, *closest_pos);
		}
	} else {
		SendClientMessage(cmdctx.playerid, COL_WARN, WARN"Can't find compatible mission point");
	}

	return 1;
}

static
int handle_cmd_at400_androm(struct COMMANDCONTEXT cmdctx, int vehiclemodel)
{
	struct vec3 playerpos, vehiclepos;
	int vehicleid, found_vehicle;
	float dx, dy, dz, shortest_distance, tmpdistance;

	vehicleid = GetPlayerVehicleID(cmdctx.playerid);
	if (vehicleid) {
		return 1;
	}

	found_vehicle = 0;
	shortest_distance = float_pinf;
	GetPlayerPos(cmdctx.playerid, &playerpos);
	for (vehicleid = NC_GetVehiclePoolSize(); vehicleid >= 0; vehicleid--) {
		if (GetVehicleModel(vehicleid) == vehiclemodel &&
			IsVehicleStreamedIn(vehicleid, cmdctx.playerid) &&
			veh_is_player_allowed_in_vehicle(cmdctx.playerid, gamevehicles[vehicleid].dbvehicle))
		{
			GetVehiclePosUnsafe(vehicleid, &vehiclepos);
			dx = vehiclepos.x - playerpos.x;
			dy = vehiclepos.y - playerpos.y;
			dz = vehiclepos.z - playerpos.z;
			tmpdistance = dx * dx + dy * dy + dz * dz;
			if (tmpdistance < shortest_distance && GetVehicleDriver(vehicleid) == INVALID_PLAYER_ID) {
				shortest_distance = tmpdistance;
				found_vehicle = vehicleid;
			}
		}
	}
	if (found_vehicle && shortest_distance < 22.0f * 22.0f) {
		natives_PutPlayerInVehicle(cmdctx.playerid, found_vehicle, 0);
	}

	return 1;
}

static
int cmd_androm(struct COMMANDCONTEXT cmdctx)
{
	return handle_cmd_at400_androm(cmdctx, MODEL_ANDROM);
}

static
int cmd_at400(struct COMMANDCONTEXT cmdctx)
{
	return handle_cmd_at400_androm(cmdctx, MODEL_AT400);
}

/**
The /camera command gives the player a camera.
*/
static
int cmd_camera(struct COMMANDCONTEXT cmdctx)
{
	NC_GivePlayerWeapon(cmdctx.playerid, WEAPON_CAMERA, 3036);
	return 1;
}

/**
The /chute command gives the player a parachute.
*/
static
int cmd_chute(struct COMMANDCONTEXT cmdctx)
{
	NC_GivePlayerWeapon(cmdctx.playerid, WEAPON_PARACHUTE, 1);
	return 1;
}

/**
The /engine command starts or stops the engine of the vehicle the player is currently driving.
*/
static
int cmd_engine(struct COMMANDCONTEXT cmdctx)
{
	if (GetPlayerState(cmdctx.playerid) == PLAYER_STATE_DRIVER) {
		veh_start_or_stop_engine(cmdctx.playerid, GetPlayerVehicleID(cmdctx.playerid));
	}
	return 1;
}

/**
The /getspray command gets the colors of the vehicle the player is in.
*/
static
int cmd_getspray(struct COMMANDCONTEXT cmdctx)
{
	struct dbvehicle *veh;
	int vehicleid;
	char msg144[144];

	vehicleid = GetPlayerVehicleID(cmdctx.playerid);
	if (vehicleid && (veh = gamevehicles[vehicleid].dbvehicle)) {
		sprintf(msg144, "colors: %d, %d", veh->col1, veh->col2);
		SendClientMessage(cmdctx.playerid, -1, msg144);
	}
	return 1;
}

/**
The /helpkeys command shows gametext with important keys.
*/
static
int cmd_helpkeys(struct COMMANDCONTEXT cmdctx)
{
	GameTextForPlayer(cmdctx.playerid, 5000, 3,
		"~w~start/stop engine: ~b~~k~~CONVERSATION_NO~~n~"
		"~w~landing gear: ~b~~k~~TOGGLE_SUBMISSIONS~~n~"
		"~w~enter copilot mode: ~b~~k~~PED_FIREWEAPON~~n~"
		"~w~exit copilot mode: ~b~~k~~VEHICLE_BRAKE~");
	return 1;
}

static
int cmd_me(struct COMMANDCONTEXT cmdctx)
{
	char *to, *from;
	int hascontent = 0;
	char msg[150 + MAX_PLAYER_NAME];

	from = cmdctx.cmdtext + 3;
	to = msg + sprintf(msg, "* %s", pdata[cmdctx.playerid]->name);
	while ((*to = *from)) {
		if (*from != ' ') {
			hascontent = 1;
		}
		from++;
		to++;
	}
	if (hascontent) {
		SendClientMessageToAll(COL_IRC_ACTION, msg);
		echo_on_game_chat_or_action(1, cmdctx.playerid, cmdctx.cmdtext + 4);
	} else {
		SendClientMessage(cmdctx.playerid, COL_WARN, WARN"Syntax: /me <action>");
	}
	return 1;
}

static
int cmd_park(struct COMMANDCONTEXT cmdctx)
{
	struct dbvehicle *veh;
	int vehicleid;
	float lastrot;

	vehicleid = GetPlayerVehicleID(cmdctx.playerid);
	if (vehicleid) {
		veh = gamevehicles[vehicleid].dbvehicle;
		if (!veh_can_player_modify_veh(cmdctx.playerid, veh)) {
			SendClientMessage(cmdctx.playerid, COL_WARN, WARN"You are not allowed to park this vehicle");
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
		SendClientMessage(cmdctx.playerid, COL_SUCC, SUCC"Vehicle parked!");
	}
	return 1;
}

/**
The /protip command sends a random protip.
*/
static
int cmd_protip(struct COMMANDCONTEXT cmdctx)
{
	SendClientMessage(cmdctx.playerid, COL_INFO_LIGHT, protips_get_random_protip());
	return 1;
}

#define NO_RECLASSSPAWN WARN"You cannot reclass/respawn while on a mission. "\
			"Use /s to cancel your current mission for a fee "\
			"($"EQ(MISSION_CANCEL_FINE)")."

static
int cmd_reclass(struct COMMANDCONTEXT cmdctx)
{
	if (missions_is_player_on_mission(cmdctx.playerid)) {
		SendClientMessage(cmdctx.playerid, COL_WARN, NO_RECLASSSPAWN);
	} else {
		spawned[cmdctx.playerid] = 0;
		NC_ForceClassSelection(cmdctx.playerid);
		NC_TogglePlayerSpectating(cmdctx.playerid, 1);
		nc_params[2] = 0;
		NC(n_TogglePlayerSpectating);
	}
	return 1;
}

static
int cmd_respawn(struct COMMANDCONTEXT cmdctx)
{
	if (missions_is_player_on_mission(cmdctx.playerid)) {
		SendClientMessage(cmdctx.playerid, COL_WARN, NO_RECLASSSPAWN);
	} else {
		natives_SpawnPlayer(cmdctx.playerid);
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
int cmd_spray(struct COMMANDCONTEXT cmdctx)
{
	struct dbvehicle *veh;
	int vehicleid, *col1, *col2, *a, *b;

	vehicleid = GetPlayerVehicleID(cmdctx.playerid);
	if (vehicleid) {
		veh = gamevehicles[vehicleid].dbvehicle;
		if (!veh_can_player_modify_veh(cmdctx.playerid, veh)) {
			SendClientMessage(cmdctx.playerid, COL_WARN, WARN"You are not allowed to respray this vehicle");
			return 1;
		}
		a = b = NULL;
		col1 = nc_params + 2;
		col2 = nc_params + 3;
		if (cmd_get_int_param(&cmdctx, col1)) {
			if (*col1 == -1) {
				a = col1;
			}
			if (!cmd_get_int_param(&cmdctx, col2)) {
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
int cmd_tickrate(struct COMMANDCONTEXT cmdctx)
{
	char msg144[144];

	sprintf(msg144, "%d", (int) NC_GetServerTickRate());
	SendClientMessage(cmdctx.playerid, -1, msg144);
	return 1;
}
