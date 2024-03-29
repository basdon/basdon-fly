/**
Whether to adjust player's world bounds on spawn.
*/
static char need_adjust_world_bounds[MAX_PLAYERS];

static char *NOLOG = WARN"Log in first.";

/*adjusted parameter addr, since FRM and return addres are on top of the stack*/
#define PARAM(X) (*(params + 2 + X))

/* native B_OnCallbackHit(function, data) */
static
cell AMX_NATIVE_CALL B_OnCallbackHit(AMX *amx, cell *params)
{
	/*TODO: maybe I shouldn't be doing it like this*/
	if (PARAM(1)) {
		((cb_t) PARAM(1))((void*) PARAM(2));
	}
	return 1;
}

/* native B_OnGameModeExit() */
static
cell AMX_NATIVE_CALL B_OnGameModeExit(AMX *amx, cell *params)
{
	missions_dispose();
	jobmap_dispose();
	airports_destroy();
	echo_dispose();
	heartbeat_end_session();
	help_dispose();
	missions_destroy_tracker_socket();
	spawn_dispose();
	svp_dispose();
	while (veh_commit_next_vehicle_odo_to_db());
	veh_dispose();

	NC_PARS(1);
	nc_params[1] = 1;
	NC(n_mysql_close);
	return 1;
}

/* native B_OnGameModeInit() */
static
cell AMX_NATIVE_CALL B_OnGameModeInit(AMX *amx, cell *params)
{
	unsigned long t;
	int mysql_errno;

	samp_init();

	t = time_timestamp();

	memset(spawned, 0, sizeof(spawned));

	assert(conf_mysql_user && conf_mysql_db && conf_mysql_pw);

	atoc(buf64, "127.0.0.1", 64);
	atoc(buf32, conf_mysql_user, 32);
	atoc(buf144, conf_mysql_db, 144);
	atoc(buf4096, conf_mysql_pw, 4096);

	/*See conf.c*/
	memset(conf_mysql_user, 0, 2048 * 3);
	free(conf_mysql_user);

	NC_PARS(2);
	nc_params[1] = 1 | 2; /*LOG_ERROR | LOG_WARNING*/
	nc_params[2] = 1; /*LOG_TYPE_TEXT*/
	NC(n_mysql_log);

	NC_PARS(7);
	nc_params[1] = buf64a; /*host*/
	nc_params[2] = buf32a; /*user*/
	nc_params[3] = buf144a; /*database*/
	nc_params[4] = buf4096a; /*password*/
	nc_params[5] = 3306;
	nc_params[6] = 1; /*autoreconnect*/
	nc_params[7] = 2; /*pool_size, 2 is default in a_mysql.inc*/
	if (!NC(n_mysql_connect)) {
		logprintf("no db connection error");
		goto exit;
	}
	if ((mysql_errno = NC_mysql_errno())) {
		logprintf("db errno %d", mysql_errno);
exit:
		SendRconCommand("exit");
		return 1;

	}
	/*mysql_set_charset "Windows-1252"*/
	memset(buf32, 0, sizeof(buf32));
	memset(buf144, 0, sizeof(buf144));
	memset(buf4096, 0, sizeof(buf4096));

	airports_init();
	changelog_init();
	class_init();
	dialog_init();
	discordflightlog_init();
	heartbeat_create_session();
	help_init();
	maps_init();
	missions_create_tracker_socket();
	jobmap_init();
	missions_init(); /*airports_init() must be called first, to initialize missionpoints*/
	playerstats_init();
	protips_init();
	radio_init();
	spawn_init(); /*MUST run after airports_init*/
	svp_init();
	timecyc_init();
	veh_init();

	SetGameModeText("Aviation/Piloting/Flying plane server");
	samp_pNetGame->usePlayerPedAnims = 1;
	samp_pNetGame->enableStuntBonus = 0;
	samp_pNetGame->showNametags = 0;

	logprintf("  Loaded gamemode basdon-fly in %ldms\n", time_timestamp() - t);

	/*must be last*/
	timecyc_reset();
	timer_reset();

	/*Delays here are semi random so they don't start ticking at the same time.. But they'll probably converge anyways*/
	timer_set(0, echo_init, NULL);
	timer_set(500, nametags_update, NULL);
	return 1;
}

static
int set_minconnectiontime(void *data)
{
	char buf[100];

	sprintf(buf, "minconnectiontime %d", (int) data);
	SendRconCommand(buf);
	return TIMER_NO_REPEAT;
}

/* native B_OnIncomingConnection(playerid, ip_address[], port) */
static
cell AMX_NATIVE_CALL B_OnIncomingConnection(AMX *amx, cell *params)
{
	int i, playerid;
	char msg144[144];
	short msg_playerids[MAX_PLAYERS];
	int num_msg_playerids;

	joinpressure += JOINPRESSURE_INC;
	if (joinpressure > JOINPRESSURE_MAX && !minconnectiontime) {
		set_minconnectiontime(
			(void*) JOINPRESSURE_SLOWMODE_MINCONNECTIONTIME);
		timer_set(JOINPRESSURE_SLOWMODE_LEN, set_minconnectiontime, 0);
		sprintf(msg144,
			WARN"too many joins; throttling incoming connections "
			"next %dms to one per %dms",
			JOINPRESSURE_SLOWMODE_LEN,
			JOINPRESSURE_SLOWMODE_MINCONNECTIONTIME);
		num_msg_playerids = 0;
		for (i = 0; i < playercount; i++) {
			playerid = players[i];
			if (GROUPS_ISADMIN(pdata[playerid]->groups)) {
				msg_playerids[num_msg_playerids] = playerid;
				num_msg_playerids++;
			}
		}
		SendClientMessageToBatch(msg_playerids, num_msg_playerids, COL_WARN, msg144);
	}
	return 1;
}

/* native B_OnPlayerConnect(playerid) */
static
cell AMX_NATIVE_CALL B_OnPlayerConnect(AMX *amx, cell *params)
{
	const int playerid = PARAM(1);

	player[playerid] = samp_pNetGame->playerPool->players[playerid];
	RakServer__GetPlayerIDFromIndex(&rakPlayerID[playerid], rakServer, playerid);

#ifdef DEV
	SendClientMessage(playerid, COL_WARN, "DEVELOPMENT BUILD");
#endif /*DEV*/

	/*do not remove so callbacks can validate*/
	/*needs to be present both on connect and disconnect*/
	_cc[playerid]++;

	playeronlineflag[playerid] = 1;
	loggedstatus[playerid] = LOGGED_NO;
	kick_update_delay[playerid] = 0;

	/*keep this top*/
	pdata_on_player_connect(playerid);
	/*keep this second*/
	if (!login_on_player_connect(playerid)) {
		return 1;
	}

	anticheat_on_player_connect(playerid);
	changelog_on_player_connect(playerid);
	class_on_player_connect(playerid);
	chpw_on_player_connect(playerid);
	echo_on_player_connect(playerid);
	dialog_on_player_connect(playerid);
	help_on_player_connect(playerid);
	kneeboard_on_player_connect(playerid);
	maps_on_player_connect(playerid);
	missions_on_player_connect(playerid);
	jobmap_on_player_connect(playerid);
	money_set(playerid, 0);
	nametags_on_player_connect(playerid);
	panel_on_player_connect(playerid);
	playerstats_on_player_connect(playerid);
	pm_on_player_connect(playerid);
	prefs_on_player_connect(playerid);
	score_on_player_connect(playerid);
	svp_on_player_connect(playerid);
	timecyc_on_player_connect(playerid);
	ui_on_player_connect(playerid);
	veh_on_player_connect(playerid);
	zones_on_player_connect(playerid);

	need_adjust_world_bounds[playerid] = 1;

	return 1;
}

/* native B_OnPlayerDeath(playerid, killerid, reason */
static
cell AMX_NATIVE_CALL B_OnPlayerDeath(AMX *amx, cell *params)
{
	struct vec3 pos;
	const int playerid = PARAM(1), killerid = PARAM(2);
	const int reason = PARAM(3);
	int killeruid;

	if (!ISPLAYING(playerid)) {
		return 0;
	}

	killeruid = killerid == INVALID_PLAYER_ID ? -1 : userid[killerid];
	csprintf(buf4096,
		"INSERT INTO deathlog(stamp,killee,killer,reason) "
		"VALUES(UNIX_TIMESTAMP(),IF(%d<1,NULL,%d),IF(%d<1,NULL,%d),%d)",
		userid[playerid],
		userid[playerid],
		killeruid,
		killeruid,
		reason);
	NC_mysql_tquery_nocb(buf4096a);

	spawned[playerid] = 0;

	spawn_prespawn(playerid);
	timecyc_on_player_death(playerid);
	GetPlayerPos(playerid, &pos);
	zones_update(playerid, pos);
	kneeboard_update_all(playerid, &pos);

	return 1;
}

/* native B_OnPlayerDisconnect(playerid, reason)*/
static
cell AMX_NATIVE_CALL B_OnPlayerDisconnect(AMX *amx, cell *params)
{
	const int playerid = PARAM(1), reason = PARAM(2);
	int i;

	chpw_on_player_disconnect(playerid);
	echo_on_player_disconnect(playerid, reason);
	dialog_on_player_disconnect(playerid);
	maps_on_player_disconnect(playerid);
	missions_on_player_disconnect(playerid, reason);
	jobmap_on_player_disconnect(playerid);
	panel_remove_panel_player(playerid);
	playerstats_on_player_disconnect(playerid);
	pm_on_player_disconnect(playerid);
	veh_on_player_disconnect(playerid);

	/*keep this here, clears logged in status*/
	login_on_player_disconnect(playerid, reason);
	/*keep this last*/
	pdata_on_player_disconnect(playerid);

	playeronlineflag[playerid] = 0;
	spawned[playerid] = 0;

	/*do not remove so callbacks can validate*/
	/*needs to be present both on connect and disconnect*/
	_cc[playerid]++;

	for (i = 0; i < playercount; i++) {
		if (players[i] == playerid) {
			players[i] = players[playercount - 1];
			playercount--;
			break;
		}
	}

	nametags_on_player_disconnect(playerid); /*Needs to be after player was removed from players array.*/
	player[playerid] = 0;

	return 1;
}

/* native B_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger) */
static
cell AMX_NATIVE_CALL B_OnPlayerEnterVehicle(AMX *amx, cell *params)
{
	const int playerid = PARAM(1), vehicleid = PARAM(2);
	const int ispassenger = PARAM(3);

	anticheat_on_player_enter_vehicle(playerid, vehicleid, ispassenger);
	veh_on_player_enter_vehicle(playerid, vehicleid, ispassenger);

	return 1;
}

/* native B_OnPlayerRequestClass(playerid, classid) */
static
cell AMX_NATIVE_CALL B_OnPlayerRequestClass(AMX *amx, cell *params)
{
	const int playerid = PARAM(1), classid = PARAM(2);

	class_on_player_request_class(playerid, classid);
	timecyc_on_player_request_class(playerid);
	return 1;
}

/* native B_OnPlayerRequestSpawn(playerid) */
/**
Called when trying to spawn from the 'class selection' state.
*/
static
cell AMX_NATIVE_CALL B_OnPlayerRequestSpawn(AMX *amx, cell *params)
{
	const int playerid = PARAM(1);

	if (!ISPLAYING(playerid)) {
		SendClientMessage(playerid, COL_WARN, NOLOG);
		return 0;
	}

	if (class_on_player_request_spawn(playerid)) {
		spawn_prespawn(playerid);
		return 1;
	}

	return 0;
}

/* native B_OnPlayerSpawn(playerid) */
static
cell AMX_NATIVE_CALL B_OnPlayerSpawn(AMX *amx, cell *params)
{
	struct RPCDATA_SetWorldBounds rpcdata;
	struct vec3 pos;
	const int playerid = PARAM(1);

	/*SetWorldBounds doesn't (always) work in OnPlayerConnect, so this is a good location I suppose.*/
	if (need_adjust_world_bounds[playerid]) {
		need_adjust_world_bounds[playerid] = 0;
		*(int*)&rpcdata.x_max = *(int*)&rpcdata.y_max = WORLD_XY_MAX;
		*(int*)&rpcdata.x_min = *(int*)&rpcdata.y_min = WORLD_XY_MIN;
		SendRPCToPlayer(playerid, RPC_SetWorldBounds, &rpcdata, sizeof(rpcdata), 2);
	}

	if (!ISPLAYING(playerid)) {
		return 0;
	}

	spawned[playerid] = 1;

	GetPlayerPos(playerid, &pos);
	kneeboard_update_all(playerid, &pos);
	maps_stream_for_player(playerid, pos.x, pos.y, OBJ_STREAM_MODE_CLOSEST_NOW);
	money_on_player_spawn(playerid);
	nametags_update_for_player(playerid);
	spawn_on_player_spawn(playerid);
	svp_update_mapicons(playerid, pos.x, pos.y);
	missions_on_player_spawn(playerid, pos);
	zones_update(playerid, pos);

	return 1;
}

/* native B_OnPlayerStateChange(playerid, newstate, oldstate) */
static
cell AMX_NATIVE_CALL B_OnPlayerStateChange(AMX *amx, cell *params)
{
	const int playerid = PARAM(1);
	const int newstate = PARAM(2), oldstate = PARAM(3);

	copilot_on_player_state_change(playerid, oldstate, newstate);
	missions_on_player_state_change(playerid, oldstate, newstate);
	panel_on_player_state_change(playerid, oldstate, newstate);
	veh_on_player_state_change(playerid, oldstate, newstate);
	return 1;
}

/* native B_OnPlayerText(playerid, text[]) */
static
cell AMX_NATIVE_CALL B_OnPlayerText(AMX *amx, cell *params)
{
	cell *addr;
	char buf[144];
	const int playerid = PARAM(1);

	if (!ISPLAYING(playerid)) {
		SendClientMessage(playerid, COL_WARN, NOLOG);
		return 0;
	}

	if (!anticheat_on_player_text(playerid)) {
		return 0;
	}

	amx_GetAddr(amx, PARAM(2), &addr);
	ctoa(buf, addr, sizeof(buf));

	if (buf[0] == '#') {
		radio_send_radio_msg(playerid, buf + 1);
		return 0;
	}

	echo_on_game_chat_or_action(0, playerid, buf);
	return 1;
}

/* native B_OnPlayerUpdate(playerid) */
static
cell AMX_NATIVE_CALL B_OnPlayerUpdate(AMX *amx, cell *params)
{
	samp_OnPlayerUpdate(PARAM(1));
	return 1;
}

/* native B_OnQueryError(errorid, error[], callback[], query[], handle) */
static
cell AMX_NATIVE_CALL B_OnQueryError(AMX *amx, cell *params)
{
	const int errorid = PARAM(1);
	cell *addr;

	amx_GetAddr(amx, PARAM(2), &addr);
	ctoa(cbuf144, addr, 144);
	amx_GetAddr(amx, PARAM(4), &addr);
	ctoa(cbuf4096, addr, 4096);

	logprintf("query err %d >>>\n%s\n===\n%s\n<<<", errorid, cbuf144, cbuf4096);
	return 1;
}

/* native B_OnRecv(ssocket:handle, data[], len) */
static
cell AMX_NATIVE_CALL B_OnRecv(AMX *amx, cell *params)
{
	const int len = PARAM(3);
	cell socket_handle = PARAM(1);
	cell *addr;

	amx_GetAddr(amx, PARAM(2), &addr);
	echo_on_receive(socket_handle, PARAM(2), (char*) addr, len);
	return 1;
}

/* native B_OnVehicleSpawn(&vehicleid) */
static
cell AMX_NATIVE_CALL B_OnVehicleSpawn(AMX *amx, cell *params)
{
	struct dbvehicle *veh;
	int vehicleid = PARAM(1);
	float min_fuel;

	veh = gamevehicles[vehicleid].dbvehicle;
	if (!veh) {
		/*can't put non-db vehicles back to a normal position*/
		veh_DestroyVehicle(vehicleid);
		return 1;
	}

	NC_PARS(4);
	nc_params[1] = vehicleid;
	nc_paramf[2] = veh->pos.coords.x;
	nc_paramf[3] = veh->pos.coords.y;
	nc_paramf[4] = veh->pos.coords.z;
	NC(n_SetVehiclePos);

	missions_on_vehicle_destroyed_or_respawned(vehicleid);
	nav_disable_for_respawned_or_destroyed_vehicle(vehicleid);

	min_fuel = vehicle_fuel_cap[vehicleid] * .25f;
	if (vehicle_fuel[vehicleid] < min_fuel) {
		vehicle_fuel[vehicleid] = min_fuel;
	}

	return 1;
}

/* native B_OnVehicleStreamIn(vehicleid, forplayerid) */
static
cell AMX_NATIVE_CALL B_OnVehicleStreamIn(AMX *amx, cell *params)
{
	const int vehicleid = PARAM(1), forplayerid = PARAM(2);

	missions_on_vehicle_stream_in(vehicleid, forplayerid);
	veh_on_vehicle_stream_in(vehicleid, forplayerid);
	return 1;
}

/* native B_OnVehicleStreamOut(vehicleid, forplayerid) */
static
cell AMX_NATIVE_CALL B_OnVehicleStreamOut(AMX *amx, cell *params)
{
	const int vehicleid = PARAM(1), forplayerid = PARAM(2);

	veh_on_vehicle_stream_out(vehicleid, forplayerid);
	return 1;
}
