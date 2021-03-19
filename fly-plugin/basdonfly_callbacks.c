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
	((cb_t) PARAM(1))((void*) PARAM(2));
	return 1;
}

/* native B_OnDialogResponse() */
static
cell AMX_NATIVE_CALL B_OnDialogResponse(AMX *amx, cell *params)
{
	const int pid = PARAM(1), dialogid = PARAM(2);
	const int res = PARAM(3), listitem = PARAM(4);
	char inputtext[128], dirtyinputtext[128], c;
	int is_inputtext_dirty, i, j;
	cell *ia;

	if (anticheat_flood(pid, AC_FLOOD_AMOUNT_DIALOG) ||
		!dialog_on_response(pid, dialogid))
	{
		dialog_pop_queue(pid);
		return 0;
	}

	if (dialogid == DIALOG_DUMMY) {
		goto ret;
	}

	amx_GetAddr(amx, PARAM(5), &ia);
	/*sanitize input if dialog is not one of the password dialogs*/
	if (*ia == 0) {
		inputtext[0] = 0;
	} else if (dialogid == DIALOG_LOGIN_LOGIN_OR_NAMECHANGE ||
		dialogid == DIALOG_CHANGEPASS_PREVPASS ||
		dialogid == DIALOG_CHANGEPASS_NEWPASS ||
		dialogid == DIALOG_CHANGEPASS_CONFIRMPASS ||
		dialogid == DIALOG_GUESTREGISTER_FIRSTPASS ||
		dialogid == DIALOG_GUESTREGISTER_CONFIRMPASS)
	{
		ctoa(inputtext, ia, sizeof(inputtext));
	} else {
		ctoa(dirtyinputtext, ia, sizeof(dirtyinputtext));
		is_inputtext_dirty = 0;
		for (i = 0, j = 0; i < sizeof(dirtyinputtext); i++) {
			c = dirtyinputtext[i];
			if (c == '%') {
				is_inputtext_dirty = 1;
				inputtext[j++] = '#';
			} else if (c < ' ' && c != 0) {
				is_inputtext_dirty = 1;
				dirtyinputtext[i] = '~';
			} else {
				inputtext[j++] = c;
				if (!c) {
					break;
				}
			}
		}
		if (is_inputtext_dirty) {
			anticheat_log(pid, AC_DIALOG_SMELLY_INPUT,
				dirtyinputtext);
		}
	}

	switch (dialogid) {
	case DIALOG_SPAWN_SELECTION:
		spawn_on_dialog_response(pid, res, listitem);
		goto ret;
	case DIALOG_PREFERENCES:
		prefs_on_dialog_response(pid, res, listitem);
		goto ret;
	case DIALOG_AIRPORT_NEAREST:
		airport_nearest_dialog_response(pid, res, listitem);
		goto ret;
	case DIALOG_REGISTER_FIRSTPASS:
		login_dlg_register_firstpass(pid, res, PARAM(5));
		goto ret;
	case DIALOG_REGISTER_CONFIRMPASS:
		login_dlg_register_confirmpass(pid, res, PARAM(5), inputtext);
		goto ret;
	case DIALOG_LOGIN_LOGIN_OR_NAMECHANGE:
		login_dlg_login_or_namechange(pid, res, inputtext);
		goto ret;
	case DIALOG_LOGIN_NAMECHANGE:
		login_dlg_namechange(pid, res, inputtext);
		goto ret;
	case DIALOG_CHANGEPASS_PREVPASS:
		chpw_dlg_previous_password(pid, res, PARAM(5), ia);
		goto ret;
	case DIALOG_CHANGEPASS_NEWPASS:
		chpw_dlg_new_password(pid, res, PARAM(5), ia);
		goto ret;
	case DIALOG_CHANGEPASS_CONFIRMPASS:
		chpw_dlg_confirm_password(pid, res, PARAM(5), ia);
		goto ret;
	case DIALOG_GUESTREGISTER_CHANGENAME:
		guestreg_dlg_change_name(pid, res, inputtext);
		goto ret;
	case DIALOG_GUESTREGISTER_FIRSTPASS:
		guestreg_dlg_register_firstpass(pid, res, PARAM(5), ia);
		goto ret;
	case DIALOG_GUESTREGISTER_CONFIRMPASS:
		guestreg_dlg_register_confirmpass(pid, res, PARAM(5), ia);
		goto ret;
	case DIALOG_VEHINFO_VEHINFO:
		admin_dlg_vehinfo_response(pid, res, listitem);
		goto ret;
	case DIALOG_VEHINFO_ASSIGNAP:
		admin_dlg_vehinfo_assign_response(pid, res, listitem);
		goto ret;
	case DIALOG_PREFERENCES_NAMETAGDISTANCE:
		prefs_on_dialog_response_nametagdist(pid, res, inputtext);
		goto ret;
	}

	if (dialogid >= DIALOG_CHANGELOG_PAGE_0 &&
		dialogid < DIALOG_CHANGELOG_PAGE_0 + DIALOG_CHANGELOG_NUM_PAGES)
	{
		changelog_on_dialog_response(pid, res, dialogid, listitem);
	}

ret:
	dialog_pop_queue(pid);
	return 1;
}

/* native B_OnGameModeExit() */
static
cell AMX_NATIVE_CALL B_OnGameModeExit(AMX *amx, cell *params)
{
	unsigned long starttime;

	missions_dispose();
	airports_destroy();
	echo_dispose();
	heartbeat_end_session();
	missions_destroy_tracker_socket();
	spawn_dispose();
	svp_dispose();
	while (veh_commit_next_vehicle_odo_to_db());
	veh_dispose();

	/*mysql_close might actually already wait for queued queries so maybe
	this can be removed...*/
	NC_PARS(1);
	nc_params[1] = 1;
	if (NC(n_mysql_unprocessed_queries)) {
		logprintf("waiting on queries before exiting");
		starttime = time_timestamp();
		do {
			if (time_timestamp() - starttime > 10000) {
				logprintf("taking > 10s, fuckit");
				goto fuckit;
			}
			time_sleep(500);
		} while (NC(n_mysql_unprocessed_queries));
		logprintf("done");
	}
fuckit:
	NC(n_mysql_close);
	return 1;
}

/* native B_OnGameModeInit() */
static
cell AMX_NATIVE_CALL B_OnGameModeInit(AMX *amx, cell *params)
{
	unsigned long t;
	FILE *mysqldat;
	char mysqlcreds[1000], *m;
	int mysql_errno, usrlen, dblen, pwlen, mysqllen;

	samp_pNetGame = *(struct SampNetGame**) 0x81CA4BC;

	t = time_timestamp();

	memset(spawned, 0, sizeof(spawned));

	if (!(mysqldat = fopen("scriptfiles/mysql.dat", "rb"))) {
		logprintf("cannot read mysql.dat");
		goto exit;
	}
	mysqllen = fread(mysqlcreds, 1, 1000, mysqldat);
	fclose(mysqldat);
	usrlen = *((int*) &mysqlcreds);
	dblen = *(((int*) &mysqlcreds) + 1);
	pwlen = *(((int*) &mysqlcreds) + 2);
	if (mysqllen != 12 + usrlen + dblen + pwlen) {
		logprintf("invalid mysql.dat format");
		goto exit;
	}

	m = mysqlcreds + 12;
	memcpy(cbuf32, m, usrlen);
	*((int*) (cbuf32 + usrlen)) = 0;
	m += usrlen;
	memcpy(cbuf32_1, m, dblen);
	*((int*) (cbuf32_1 + dblen)) = 0;
	m += dblen;
	memcpy(cbuf144, m, pwlen);
	*((int*) (cbuf144 + pwlen)) = 0;
	atoc(buf64, "127.0.0.1", 64);

	NC_PARS(2);
	nc_params[1] = 1 | 2; /*LOG_ERROR | LOG_WARNING*/
	nc_params[2] = 1; /*LOG_TYPE_TEXT*/
	NC(n_mysql_log);

	NC_PARS(7);
	nc_params[1] = buf64a; /*host*/
	nc_params[2] = buf32a; /*user*/
	nc_params[3] = buf32_1a; /*database*/
	nc_params[4] = buf144a; /*password*/
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
		buf32[0] = 'e'; buf32[1] = 'x'; buf32[2] = 'i'; buf32[3] = 't';
		buf32[4] = 0;
		NC_SendRconCommand(buf32a);
		time_sleep(10000);
		return 1;

	}
	/*mysql_set_charset "Windows-1252"*/

	airports_init();
	changelog_init();
	class_init();
	heartbeat_create_session();
	maps_init();
	missions_create_tracker_socket();
	missions_init(); /*airports_init() must be called first, to initialize missionpoints*/
	playerstats_init();
	protips_init();
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
	timer_set(250, maps_process_tick, NULL);
	timer_set(500, nametags_update, NULL);
	return 1;
}

static
int set_minconnectiontime(void *data)
{
	minconnectiontime = (int) data;
	csprintf(buf144, "minconnectiontime %d", minconnectiontime);
	NC_SendRconCommand(buf144a);
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

/* native B_OnPlayerCommandText(playerid, cmdtext[]) */
static
cell AMX_NATIVE_CALL B_OnPlayerCommandText(AMX *amx, cell *params)
{
	const int playerid = PARAM(1);
	char cmdtext[145];
	cell *addr;

	if (!ISPLAYING(playerid)) {
		SendClientMessage(playerid, COL_WARN, NOLOG);
		return 1;
	}

	if (!spawned[playerid]) {
		SendClientMessage(playerid, COL_WARN, WARN"You can't use commands when not spawned.");
		return 1;
	}

	amx_GetAddr(amx, PARAM(2), &addr);
	ctoa(cmdtext, addr, sizeof(cmdtext));

	common_mysql_escape_string(cmdtext, cbuf144, 144 * sizeof(cell));
	csprintf(buf4096,
		"INSERT INTO cmdlog(player,loggedstatus,stamp,cmd) "
		"VALUES(IF(%d<1,NULL,%d),%d,UNIX_TIMESTAMP(),'%s')",
		userid[playerid],
		userid[playerid],
		loggedstatus[playerid],
		cbuf144);
	NC_mysql_tquery_nocb(buf4096a);

	return cmd_check(playerid, cmdtext);
}

/* native B_OnPlayerConnect(playerid) */
static
cell AMX_NATIVE_CALL B_OnPlayerConnect(AMX *amx, cell *params)
{
	const int playerid = PARAM(1);

	player[playerid] = samp_pNetGame->playerPool->players[playerid];

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

	changelog_on_player_connect(playerid);
	class_on_player_connect(playerid);
	chpw_on_player_connect(playerid);
	echo_on_player_connect(playerid);
	dialog_on_player_connect(playerid);
	kneeboard_on_player_connect(playerid);
	maps_on_player_connect(playerid);
	missions_on_player_connect(playerid);
	money_set(playerid, 0);
	nametags_on_player_connect(playerid);
	panel_on_player_connect(playerid);
	playerstats_on_player_connect(playerid);
	pm_on_player_connect(playerid);
	prefs_on_player_connect(playerid);
	score_on_player_connect(playerid);
	svp_on_player_connect(playerid);
	timecyc_on_player_connect(playerid);
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

	airport_on_player_disconnect(playerid);
	chpw_on_player_disconnect(playerid);
	echo_on_player_disconnect(playerid, reason);
	dialog_on_player_disconnect(playerid);
	maps_on_player_disconnect(playerid);
	missions_on_player_disconnect(playerid);
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
	return 1;
}

/* native B_OnPlayerEnterRaceCP(playerid) */
static
cell AMX_NATIVE_CALL B_OnPlayerEnterRaceCP(AMX *amx, cell *params)
{
	/*TODO: When actually using this, check if it's needed to call DisablePlayerRaceCheckpoint in OnPlayerConnect.*/
	return 1;
}

/* native B_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger) */
static
cell AMX_NATIVE_CALL B_OnPlayerEnterVehicle(AMX *amx, cell *params)
{
	const int playerid = PARAM(1), vehicleid = PARAM(2);
	const int ispassenger = PARAM(3);

	veh_on_player_enter_vehicle(playerid, vehicleid, ispassenger);

	return 1;
}

/* native B_OnPlayerKeyStateChange(playerid, oldkeys, newkeys) */
static
cell AMX_NATIVE_CALL B_OnPlayerKeyStateChange(AMX *amx, cell *params)
{
	/*const int playerid = PARAM(1), newkeys = PARAM(2), oldkeys = PARAM(3);*/

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
	maps_stream_for_player(playerid, pos);
	money_on_player_spawn(playerid);
	nametags_update_for_player(playerid);
	spawn_on_player_spawn(playerid);
	svp_update_mapicons(playerid, pos.x, pos.y);
	missions_update_missionpoint_indicators(playerid, pos.x, pos.y, pos.z);
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

	echo_on_game_chat_or_action(0, playerid, buf);
	return 1;
}

/* native B_OnPlayerUpdate(playerid) */
static
cell AMX_NATIVE_CALL B_OnPlayerUpdate(AMX *amx, cell *params)
{
	const int playerid = PARAM(1);

	if (kick_update_delay[playerid] > 0) {
		if (--kick_update_delay[playerid] == 0) {
			NC_PARS(1);
			nc_params[1] = playerid;
			NC(n_Kick_);
		}
		return 0;
	}
	playerstats_on_player_update(playerid);
	timecyc_on_player_update(playerid);

	lastvehicle_asdriver[playerid] = 0; /*This will be set again at the end of hook_OnDriverSync.*/
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

	logprintf("query err %d %s %s", errorid, cbuf144, cbuf4096);
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

	veh = gamevehicles[vehicleid].dbvehicle;
	if (veh == NULL) {
		/*can't put non-db vehicles back to a normal position*/
		veh_DestroyVehicle(vehicleid);
		return 0;
	}

	if (gamevehicles[vehicleid].need_recreation) {
		gamevehicles[vehicleid].need_recreation = 0;
		veh_DestroyVehicle(vehicleid);
		if (veh_create(veh) == INVALID_VEHICLE_ID) {
			logprintf(
				"ERR: couldn't recreate "
				"vehicle %d (public: %d)",
				veh->id,
				!veh->owneruserid);
		}
		/*always return because veh_create will already call
		OnVehicleSpawn for the recreated vehicle*/
		return 0;
	}

	/*this might've been already increased (due to recreate)
	but it doesn't matter, it just needs to change*/
	gamevehicles[vehicleid].reincarnation++;

	SetVehicleColor(vehicleid, veh->col1, veh->col2);
	NC_PARS(4);
	nc_params[1] = vehicleid;
	nc_paramf[2] = veh->pos.coords.x;
	nc_paramf[3] = veh->pos.coords.y;
	nc_paramf[4] = veh->pos.coords.z;
	NC(n_SetVehiclePos);

	/*     WARNING
	if you're looking for a place to clean up stuff,
	check common_on_vehicle_respawn_or_destroyed
	since OnVehicleSpawn might destroy and recreate a vehicle, in which
	case code will not reach this point for the vehicle you're expecting
	since the vehicle has been destroyed.*/

	common_on_vehicle_respawn_or_destroy(vehicleid, veh);
	veh_on_vehicle_spawn(veh);

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
