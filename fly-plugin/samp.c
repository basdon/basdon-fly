#ifndef SAMP_NATIVES_IMPL

struct SampNetGame *samp_pNetGame;

static unsigned char vehicle_gear_state[MAX_VEHICLES];
static int vehicle_gear_change_time[MAX_VEHICLES];

static
void SendRPCToPlayer(int playerid, int rpc, void *rpcdata, int size_bytes, int unk)
{
	struct BitStream bs;

	bs.ptrData = rpcdata;
	bs.numberOfBitsUsed = size_bytes * 8;
	SAMP_SendRPCToPlayer(rpc, &bs, playerid, unk);
}

static
void SetPlayerRaceCheckpointNoDir(int playerid, int type, struct vec3 *pos, float radius)
{
	struct RPCDATA_SetRaceCheckpoint rpcdata;
	struct BitStream bs;

	rpcdata.type = type;
	rpcdata.pos = *pos;
	rpcdata.radius = radius;
	bs.ptrData = &rpcdata;
	bs.numberOfBitsUsed = sizeof(rpcdata) * 8;
	SAMP_SendRPCToPlayer(RPC_SetRaceCheckpoint, &bs, playerid, 2);
}

static
void DisablePlayerRaceCheckpoint(int playerid)
{
	struct BitStream bs;

	bs.numberOfBitsUsed = 0;
	SAMP_SendRPCToPlayer(RPC_DisableRaceCheckpoint, &bs, playerid, 2);
}

/**
When respawning, the player will always regain control.

Player's vehicle will not get damage when a player is not controllable.
IT EVEN GETS REPAIRED. Vehicle HP will be set to 300 when making player
uncontrollable and the vehicle hp is < 250 (maybe even when it's < 300?)
*/
static
void TogglePlayerControllable(int playerid, char controllable)
{
	struct BitStream bs;

	bs.ptrData = &controllable;
	bs.numberOfBitsUsed = 8;
	SAMP_SendRPCToPlayer(RPC_TogglePlayerControllable, &bs, playerid, 2);
}

static
void SendClientMessageToBatch(short *playerids, int numplayerids, int color, char *message)
{
	/*Using 4 packets should handle messages with a length up to 144*4.*/
	struct RPCDATA_SendClientMessage rpcdata[4];
	struct BitStream bs;
	int i, j, packetidx;
	int num_packets;
	char *msg_start;
	int msglen;
	int cut_pos, min_cut_pos;
	int pos_inc;
	char colorpart;
	int newcolor;

	msglen = strlen(message);

	/*(Probably unneeded) optimization to skip cutting and looping stuff.*/
	if (msglen < 145) {
		rpcdata[0].color = color;
		rpcdata[0].message_length = msglen;
		memcpy(rpcdata[0].message, message, msglen);
		bs.ptrData = &rpcdata[0];
		bs.numberOfBitsUsed = 32 + 32 + msglen * 8;
		for (i = 0; i < numplayerids; i++) {
			SAMP_SendRPCToPlayer(RPC_SendClientMessage, &bs, playerids[i], 3);
		}
		return;
	}

	num_packets = 0;
	msg_start = message;

	/*See /testmsgsplit command.*/
	while (msglen > 0 && num_packets < 4) {
		if (msglen > 144) {
			/*Try to split on whitespace or color boundaries.*/
			for (cut_pos = 144, min_cut_pos = 144 - 15; cut_pos > min_cut_pos; cut_pos--) {
				if (msg_start[cut_pos] == ' ') {
					pos_inc = cut_pos + 1;
					goto have_cut_pos;
				} else if (cut_pos <= msglen - 7 && msg_start[cut_pos] == '{' && msg_start[cut_pos + 7] == '}') {
					pos_inc = cut_pos;
					goto have_cut_pos;
				}
			}
			cut_pos = pos_inc = 144;
		} else {
			cut_pos = pos_inc = msglen;
		}
have_cut_pos:
		rpcdata[num_packets].color = color;
		rpcdata[num_packets].message_length = cut_pos;
		memcpy(rpcdata[num_packets].message, msg_start, cut_pos);
		num_packets++;
		/*Find the last embedded color to use for the next packet.*/
		for (i = cut_pos - 1; i >= 7; i--) {
			if (msg_start[i] == '}' && msg_start[i - 7] == '{') {
				newcolor = color & 0xFF;
				for (j = 0; j < 6; j++) {
					colorpart = msg_start[i - 6 + j];
					if ('0' <= colorpart && colorpart <= '9') {
						newcolor |= (colorpart - '0') << (28 - j * 4);
					} else if ('a' <= colorpart && colorpart <= 'f') {
						newcolor |= (colorpart - 'a' + 10) << (28 - j * 4);
					} else if ('A' <= colorpart && colorpart <= 'F') {
						newcolor |= (colorpart - 'A' + 10) << (28 - j * 4);
					} else {
						goto not_a_color;
					}
				}
				color = newcolor;
				break;
			}
not_a_color:
			;
		}
		msglen -= pos_inc;
		msg_start += pos_inc;
	}

	for (i = 0; i < numplayerids; i++) {
		for (packetidx = 0; packetidx < num_packets; packetidx++) {
			bs.ptrData = &rpcdata[packetidx];
			bs.numberOfBitsUsed = 32 + 32 + rpcdata[packetidx].message_length * 8;
			SAMP_SendRPCToPlayer(RPC_SendClientMessage, &bs, playerids[i], 3);
		}
	}
}

static
void SendClientMessageToAll(int color, char *message)
{
	SendClientMessageToBatch(players, playercount, color, message);
}

static
void SendClientMessage(short playerid, int color, char *message)
{
	SendClientMessageToBatch(&playerid, 1, color, message);
}

static
void PlayerPlaySound(int playerid, int soundid)
{
	struct RPCDATA_PlaySound rpcdata;
	struct BitStream bs;

	rpcdata.soundid = soundid;
	*(int*)&rpcdata.pos.x = 0;
	*(int*)&rpcdata.pos.y = 0;
	*(int*)&rpcdata.pos.z = 0;
	bs.ptrData = &rpcdata;
	bs.numberOfBitsUsed = sizeof(rpcdata) * 8;
	SAMP_SendRPCToPlayer(RPC_PlaySound, &bs, playerid, 2);
}

static
int IsVehicleStreamedIn(int vehicleid, int forplayerid)
{
	return player[forplayerid]->vehicleStreamedIn[vehicleid];
}

static
short GetPlayerVehicleID(int playerid)
{
	return player[playerid]->vehicleid;
}

static
char GetPlayerState(int playerid)
{
	return player[playerid]->currentState;
}

/*
TODO: make a GetVehicleParamsExForPlayer? Where objective is set in case they're on a mission but not in their vehicle.
*/
static
void GetVehicleParamsEx(int vehicleid, struct SampVehicleParams *params)
{
	struct SampVehicle *vehicle;

	vehicle = samp_pNetGame->vehiclePool->vehicles[vehicleid];
	if (vehicle) {
		*params = vehicle->params;
	}
}

static
void SetVehicleParamsExForPlayer(int vehicleid, int playerid, struct SampVehicleParams *params)
{
	struct RPCDATA_SetVehicleParamsEx rpcdata;
	struct BitStream bs;

	rpcdata.vehicleid = (short) vehicleid;
	rpcdata.params = *params;
	bs.ptrData = &rpcdata;
	bs.numberOfBitsUsed = sizeof(rpcdata) * 8;
	SAMP_SendRPCToPlayer(RPC_SetVehicleParamsEx, &bs, playerid, 2);
}

/*
XXX: this resets any player-specific doors_locked state back to the global doors_locked state.
*/
static
void SetVehicleObjectiveForPlayer(int vehicleid, int playerid, char objective)
{
	struct RPCDATA_SetVehicleParams rpcdata;
	struct BitStream bs;
	struct SampVehicleParams params;

	GetVehicleParamsEx(vehicleid, &params);
	/*SetVehicleParams does not work to unset the object, SetVehicleParamsEx must be used.*/
	if (!objective) {
		params.objective = 0; /*TODO remove once we have GetVehicleParamsExForPlayer*/
		SetVehicleParamsExForPlayer(vehicleid, playerid, &params);
	} else {
		rpcdata.vehicleid = (short) vehicleid;
		rpcdata.objective = objective;
		rpcdata.doors_locked = params.doors_locked;
		bs.ptrData = &rpcdata;
		bs.numberOfBitsUsed = sizeof(rpcdata) * 8;
		SAMP_SendRPCToPlayer(RPC_SetVehicleParams, &bs, playerid, 2);
	}
}

/*
Crashes when vehicle is not created.
*/
static
char GetVehicleEngineState(int vehicleid)
{
	return samp_pNetGame->vehiclePool->vehicles[vehicleid]->params.engine;
}

/*
Crashes when vehicle is not created.

XXX: this resets any player-specific state back to the global state.
TODO: it should at least persist player specific objective state by checking the missions.
*/
static
void SetVehicleEngineState(int vehicleid, char engine)
{
	struct RPCDATA_SetVehicleParamsEx rpcdata;
	struct BitStream bs;
	struct SampVehicleParams *params;
	int playerid, n;

	params = &samp_pNetGame->vehiclePool->vehicles[vehicleid]->params;
	if (params->engine == engine) {
		return;
	}

	params->engine = engine;

	rpcdata.vehicleid = (short) vehicleid;
	rpcdata.params = *params;
	bs.ptrData = &rpcdata;
	bs.numberOfBitsUsed = sizeof(rpcdata) * 8;

	for (n = playercount; n; ) {
		playerid = players[--n];
		if (IsVehicleStreamedIn(vehicleid, playerid)) {
			SAMP_SendRPCToPlayer(RPC_SetVehicleParamsEx, &bs, playerid, 2);
		}
	}
}

/**
In the PAWN API, text is actually the 2nd parameter. Placing it at the end here, for code style reasons.
*/
static
void GameTextForPlayer(int playerid, int milliseconds, int style, char *text)
{
	struct RPCDATA_ShowGameText rpcdata;
	struct BitStream bs;
	int len;

	len = strlen(text);
	rpcdata.style = style;
	rpcdata.time = milliseconds;
	rpcdata.message_length = len;
	memcpy(&rpcdata.message, text, len);
	bs.ptrData = &rpcdata;
	bs.numberOfBitsUsed = (4 + 4 + 4 + len) * 8;
	SAMP_SendRPCToPlayer(RPC_ShowGameText, &bs, playerid, 3);
}

static
int GetVehiclePos(int vehicleid, struct vec3 *pos)
{
	struct SampVehicle *vehicle;

	vehicle = samp_pNetGame->vehiclePool->vehicles[vehicleid];
	if (vehicle) {
		*pos = vehicle->pos;
		return 1;
	}
	return 0;
}

/**
Crashes if vehicle does not exist.
*/
static
void GetVehiclePosUnsafe(int vehicleid, struct vec3 *pos)
{
	*pos = samp_pNetGame->vehiclePool->vehicles[vehicleid]->pos;
}

/**
Crashes if vehicle does not exist.
*/
static
void GetVehicleVelocityUnsafe(int vehicleid, struct vec3 *vel)
{
	*vel = samp_pNetGame->vehiclePool->vehicles[vehicleid]->vel;
}

static
void HideGameTextForPlayer(int playerid)
{
	GameTextForPlayer(playerid, 2, 3, "_");
}

static
void CrashPlayer(int playerid)
{
	struct RPCDATA_ShowGangZone rpcdata;
	struct BitStream bs;

	GameTextForPlayer(playerid, 5, 5, "Wasted~k~SWITCH_DEBUG_CAM_ON~~k~~TOGGLE_DPAD~~k~~NETWORK_TALK~~k~~SHOW_MOUSE_POINTER_TOGGLE~");

	bs.ptrData = &rpcdata;
	bs.numberOfBitsUsed = sizeof(rpcdata) * 8;
	rpcdata.zoneid = 65535;
	SAMP_SendRPCToPlayer(RPC_ShowGangZone, &bs, playerid, 2);
	rpcdata.zoneid = -2700;
	SAMP_SendRPCToPlayer(RPC_ShowGangZone, &bs, playerid, 2);
}
#endif


/*-----------------------------------------------------------------------------*/

/*TODO: convert these natives_ funcs to non-amx things*/

/**
Callback for timer kicking a player, backup when OnPlayerUpdate doesn't get called.
*/
static
int kick_timer_cb(void *data)
#ifdef SAMP_NATIVES_IMPL
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
#ifdef SAMP_NATIVES_IMPL
{
	const static char *SYSTEM_ISSUER = "system", *REASONNULL = "(NULL)";

	int intv;
	char *escapedreason;
	char msg[144];

	if (!kick_update_delay[playerid]) {
		if (issuer == NULL) {
			issuer = (char*) SYSTEM_ISSUER;
		}

		if (reason != NULL) {
			sprintf(msg,
				"%s[%d] was kicked by %s (%s)",
				pdata[playerid]->name,
				playerid,
				issuer,
				reason);
			SendClientMessageToAll(COL_WARN, msg);
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
#ifdef SAMP_NATIVES_IMPL
{
	float hp;
	struct vec3 pos;
	int oldvehicleid;

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

	GetVehiclePos(vehicleid, &pos);
	maps_stream_for_player(playerid, pos);
	if (NC_GetPlayerState(playerid) == PLAYER_STATE_DRIVER) {
		/*hook_OnDriverSync will invoke this already,
		but that's supposed to be as last-resort (for players that are vehicle warping)*/
		oldvehicleid = NC_GetPlayerVehicleID(playerid);
		veh_on_driver_changed_vehicle_without_state_change(playerid, oldvehicleid, vehicleid);
		lastvehicle_asdriver[playerid] = vehicleid; /*So hook_OnDriverSync doesn't detect warping.*/
	}
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
#ifdef SAMP_NATIVES_IMPL
{
	int res;
	char msg144[144];

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
		sprintf(msg144, "Your name has been changed to '%s'", pdata[playerid]->name);
		SendClientMessage(playerid, COL_SAMP_GREEN, msg144);
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
#ifdef SAMP_NATIVES_IMPL
{
	/*TODO? Technically the amount of _models_ and not _objects_ determines how long
	it will take to load, because the models are loaded in a blocking way.
	Because of that, a map with 100 objects but all different models might block more
	than a 1000 object map with 1 model, but the check below is pure object count.*/
	if (maps_calculate_objects_to_create_for_player_at_position(playerid, pos) > 200) {
		GameTextForPlayer(playerid, 0x80000, 3, "Loading objects...");
	}
	maps_stream_for_player(playerid, pos);
	GameTextForPlayer(playerid, 0, 5, "_");
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
#ifdef SAMP_NATIVES_IMPL
{
	/*eject player first if they're in a vehicle*/
	if (NC_GetPlayerVehicleID(playerid)) {
		NC_GetPlayerPos(playerid, buf32a, buf64a, buf144a);
		NC(n_SetPlayerPos_);
	}
	spawn_prespawn(playerid);

	NC_PARS(1);
	/*nc_params[1] = playerid;*/
	NC(n_SpawnPlayer_);
}
#endif
;

/*-----------------------------------------------------------------------------*/

#ifdef SAMP_NATIVES_IMPL
static int drive_keystates[MAX_PLAYERS];
static char drive_udkeystate[MAX_PLAYERS];

void hook_OnDriverSync(int playerid, struct SYNCDATA_Driver *data)
{
	/*TODO reset these keystate variables when player gets into the drive state?*/
	int oldkeys, newkeys;
	int storedlastvehicleid;

	newkeys = (data->partial_keys | (data->additional_keys << 16)) & 0x0003FFFF;

	vehicle_gear_state[data->vehicle_id] = data->landing_gear_state;

	/*keystate change*/
	oldkeys = drive_keystates[playerid];
	if (oldkeys != newkeys) {
		if (KEY_JUST_DOWN(KEY_SUBMISSION)) {
			vehicle_gear_change_time[data->vehicle_id] = time_timestamp();
		}
		missions_driversync_keystate_change(playerid, oldkeys, newkeys);
		drive_keystates[playerid] = newkeys;
	}

	/*up/down keystate change*/
	if (data->udkey == 0) {
		if (drive_udkeystate[playerid] != 0) {
			missions_driversync_udkeystate_change(playerid, data->udkey);
			drive_udkeystate[playerid] = 0;
		}
	} else if (data->udkey < 0) {
		if (drive_udkeystate[playerid] != -1) {
			missions_driversync_udkeystate_change(playerid, data->udkey);
			drive_udkeystate[playerid] = -1;
		}
	} else {
		if (drive_udkeystate[playerid] != 1) {
			missions_driversync_udkeystate_change(playerid, data->udkey);
			drive_udkeystate[playerid] = 1;
		}
	}

	/*printf("DRIVER SYNC ud %hd rl %hd lnd %hd\n", data->udkey, data->lrkey, data->landing_gear_state);*/

	storedlastvehicleid = lastvehicle_asdriver[playerid]; /*storing this because OnPlayerUpdate resets it to 0*/

	/*TODO remove this when all OnPlayerUpdates are replaced*/
	/*this is 3 because.. see PARAM definition*/
	nc_params[3] = playerid;
	B_OnPlayerUpdate(amx, nc_params);

	lastvehicle_asdriver[playerid] = data->vehicle_id; /*always write this because OnPlayerUpdate resets it to 0*/
	if (storedlastvehicleid != data->vehicle_id) {
		veh_on_driver_changed_vehicle_without_state_change(playerid, storedlastvehicleid, data->vehicle_id);
		/*TODO: This is vehicle warping when reaching this (but how trustworthy is it?
		Maybe a player is still sending driversync of the old vehicle
		after PutPlayerInVehicle was called?)*/
	}
}

static
void samp_init()
{
	/*See InitAfterPoolsCreated in asm for more init stuff.*/
	mem_mkjmp(0x80AB938, &InitAfterPoolsCreated);

	/*Remove filtering in chat messages coming from clients.*/
	/*OnPlayerText*/
	mem_protect(0x80B0760, 0x14, PROT_READ | PROT_WRITE | PROT_EXEC);
	*(int*) 0x80B0760 = 0x90909090;
	*(unsigned char*) 0x80B0764 = 0x90;
	/*OnPlayerCommandText*/
	mem_protect(0x80B16D6, 0x14, PROT_READ | PROT_WRITE | PROT_EXEC);
	*(int*) 0x80B16D6 = 0x90909090;
	*(unsigned char*) 0x80B16DA = 0x90;

	/*Sync data is dropped when coordinates exceed -20k/+20k, update those limits here.
	See 804B5D0 CheckSyncBounds.*/
	mem_protect(0x815070C, 0x10, PROT_READ | PROT_WRITE);
	*((int*) 0x815070C) = WORLD_XY_MAX; /*sync bounds x/y max, default 20000.0*/
	*((int*) 0x8150710) = WORLD_XY_MIN; /*sync bounds x/y min, default -20000.0*/
	/**(int*) 0x8150714 = ; *//*sync bounds z max, default 200000.0*/
	/**(int*) 0x8150718 = ; *//*sync bounds z min, default -1000.0*/

	mem_mkjmp(0x80AEC4F, &DriverSyncHook);
}
#endif
