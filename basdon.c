
/* vim: set filetype=c ts=8 noexpandtab: */

/*This file is directly included in basdonfly.c,
but also added as a source file in VC2005. Check
for the IN_BASDONFLY macro to only compile this
when we're inside basdonfly.c*/
#ifdef IN_BASDONFLY

char playeronlineflag[MAX_PLAYERS];
short players[MAX_PLAYERS];
int playercount;
int spawned[MAX_PLAYERS];

static const char *NOLOG = WARN"Log in first.";

/* native B_Validate(maxplayers, buf4096[], buf144[], buf64[], buf32[],
                     buf32_1[], emptystring, underscorestring) */
static
cell AMX_NATIVE_CALL B_Validate(AMX *local_amx, cell *params)
{
	int i, sleep;

	amx = local_amx;

	for (i = 0; i < MAX_PLAYERS; i++) {
		playeronlineflag[i] = 0;
	}
	playercount = 0;

	amx_GetAddr(amx, buf4096a = params[2], &buf4096);
	amx_GetAddr(amx, buf144a = params[3], &buf144);
	amx_GetAddr(amx, buf64a = params[4], &buf64);
	amx_GetAddr(amx, buf32a = params[5], &buf32);
	amx_GetAddr(amx, buf32_1a = params[6], &buf32_1);
	amx_GetAddr(amx, emptystringa = params[7], &emptystring);
	amx_GetAddr(amx, underscorestringa = params[8], &underscorestring);
	cbuf32_1 = (char*) buf32_1;
	cbuf32 = (char*) buf32;
	cbuf64 = (char*) buf64;
	cbuf144 = (char*) buf144;
	cbuf4096 = (char*) buf4096;
	cbuf4096_ = cbuf4096 + 4096 * 4 - 4000;
	cemptystring = (char*) emptystring;
	cunderscorestring = (char*) underscorestring;
	fbuf32_1 = (float*) buf32_1;
	fbuf32 = (float*) buf32;
	fbuf64 = (float*) buf64;
	fbuf144 = (float*) buf144;
	fbuf4096 = (float*) buf4096;

	if (!natives_find()) {
		goto fail;
	}

	amx_SetUString(buf144, "sleep", 6);
	sleep = NC_GetConsoleVarAsInt(buf144a);
	if (sleep != 5) {
		logprintf("ERR: sleep value %d should be 5", sleep);
		goto fail;
	}

	if (MAX_PLAYERS != params[1]) {
		logprintf(
			"ERR: MAX_PLAYERS mismatch: %d (plugin) vs %d (gm)",
			MAX_PLAYERS,
			params[1]);
		goto fail;
	}

	return MAX_PLAYERS;
fail:
	time_sleep(10000);
	return 0;
}

/* native B_OnCallbackHit(function, data) */
static
cell AMX_NATIVE_CALL B_OnCallbackHit(AMX *amx, cell *params)
{
	/*TODO: maybe I shouldn't be doing it like this*/
	((cb_t) params[1])((void*) params[2]);
	return 1;
}

/* native B_OnDialogResponse() */
static
cell AMX_NATIVE_CALL B_OnDialogResponse(AMX *amx, cell *params)
{
	const int playerid = params[1], dialogid = params[2];
	const int response = params[3], listitem = params[4];
	char inputtext[128];
	cell *addr;

	if (anticheat_flood(playerid, AC_FLOOD_AMOUNT_DIALOG) ||
		!dialog_on_response(playerid, dialogid))
	{
		return 0;
	}

	if (dialogid == DIALOG_DUMMY) {
		return 1;
	}

	amx_GetAddr(amx, params[5], &addr);
	amx_GetUString(inputtext, addr, sizeof(inputtext));

	switch (dialogid) {
	case DIALOG_SPAWN_SELECTION:
		spawn_on_dialog_response(playerid, response, listitem);
		return 1;
	case DIALOG_PREFERENCES:
		prefs_on_dialog_response(playerid, response, listitem);
		return 1;
	case DIALOG_AIRPORT_NEAREST:
		airport_list_dialog_response(playerid, response, listitem);
		return 1;
	case DIALOG_REGISTER_FIRSTPASS:
		login_dlg_register_firstpass(playerid, response, params[5]);
		return 1;
	case DIALOG_REGISTER_CONFIRMPASS:
		login_dlg_register_confirmpass(
			playerid, response, params[5], inputtext);
		return 1;
	case DIALOG_LOGIN_LOGIN_OR_NAMECHANGE:
		login_dlg_login_or_namechange(playerid, response, inputtext);
		return 1;
	case DIALOG_LOGIN_NAMECHANGE:
		login_dlg_namechange(playerid, response, inputtext);
		return 1;
	}
	return 1;
}

/* native B_OnGameModeExit() */
static
cell AMX_NATIVE_CALL B_OnGameModeExit(AMX *amx, cell *params)
{
	unsigned long starttime;

	missions_freepoints(); /*call this before airports_destroy!*/
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
	char mysqlcreds[255], *m;
	char usrlen, dblen, pwlen, mysqllen;
	int errno;

	t = time_timestamp();

	memset(spawned, 0, sizeof(spawned));

	if (!(mysqldat = fopen("scriptfiles/mysql.dat", "rb"))) {
		logprintf("cannot read mysql.dat");
		goto exit;
	}
	mysqllen = fread(mysqlcreds, 1, 255, mysqldat);
	fclose(mysqldat);
	usrlen = mysqlcreds[0];
	dblen = mysqlcreds[4];
	pwlen = mysqlcreds[8];
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
	amx_SetUString(buf64, "127.0.0.1", 64);

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
	errno = NC_mysql_errno();
	if (errno) {
		logprintf("db errno %d", errno);
exit:
		buf32[0] = 'e'; buf32[1] = 'x'; buf32[2] = 'i'; buf32[3] = 't';
		buf32[4] = 0;
		NC_SendRconCommand(buf32a);
		time_sleep(10000);
		return 1;

	}
	//mysql_set_charset "Windows-1252"

	airports_init();
	class_init();
	echo_init();
	heartbeat_create_session();
	maps_load_from_db();
	missions_create_tracker_socket();
	missions_init();
	objs_on_gamemode_init();
	panel_on_gamemode_init();
	playtime_init();
	protips_init();
	spawn_init(); /*MUST run after airports_init*/
	svp_init();
	timecyc_init();
	veh_create_global_textdraws();
	veh_init();

	printf("OnGameModeInit: %ldms\n", time_timestamp() - t);

	/*must be last*/
	timecyc_reset();
	timer_reset();
	return 1;
}

/* native B_OnPlayerCommandText(playerid, cmdtext[]) */
static
cell AMX_NATIVE_CALL B_OnPlayerCommandText(AMX *amx, cell *params)
{
	static const char *NO = WARN"You can't use commands when not spawned.";

	const int playerid = params[1];
	char cmdtext[145];
	cell *addr;

	if (!ISPLAYING(playerid)) {
		B144(NOLOG);
		NC_SendClientMessage(playerid, COL_WARN, buf144a);
		return 1;
	}

	if (!spawned[playerid]) {
		B144(NO);
		NC_SendClientMessage(playerid, COL_WARN, buf144a);
		return 1;
	}

	amx_GetAddr(amx, params[2], &addr);
	amx_GetUString(cmdtext, addr, sizeof(cmdtext));

	common_mysql_escape_string(cmdtext, cbuf144, 144 * sizeof(cell));
	sprintf(cbuf4096_,
		"INSERT INTO cmdlog(player,loggedstatus,stamp,cmd) "
		"VALUES(IF(%d<1,NULL,%d),%d,UNIX_TIMESTAMP(),'%s')",
		userid[playerid],
		userid[playerid],
		loggedstatus[playerid],
		cbuf144);
	amx_SetUString(buf4096, cbuf4096_, 4096);
	NC_mysql_tquery_nocb(buf4096a);

	return cmd_check(playerid, cmdtext);
}

/* native B_OnPlayerConnect(playerid) */
static
cell AMX_NATIVE_CALL B_OnPlayerConnect(AMX *amx, cell *params)
{
	const int playerid = params[1];

#ifdef DEV
	amx_SetUString(buf144, "PL: DEVELOPMENT BUILD", 144);
	NC_SendClientMessage(playerid, COL_WARN, buf144a);
#endif /*DEV*/

	_cc[playerid]++;

	playeronlineflag[playerid] = 1;
	loggedstatus[playerid] = LOGGED_NO;
	kick_update_delay[playerid] = 0;
	temp_afk[playerid] = 0;

	/*keep this top*/
	pdata_on_player_connect(playerid);
	/*keep this second*/
	if (!login_on_player_connect(playerid)) {
		return 1;
	}

	class_on_player_connect(playerid);
	echo_on_player_connection(playerid, ECHO_CONN_REASON_GAME_CONNECTED);
	dialog_on_player_connect(playerid);
	maps_on_player_connect(playerid);
	missions_on_player_connect(playerid);
	money_set(playerid, 0);
	objs_on_player_connect(playerid);
	panel_on_player_connect(playerid);
	playtime_on_player_connect(playerid);
	pm_on_player_connect(playerid);
	prefs_on_player_connect(playerid);
	score_on_player_connect(playerid);
	svp_on_player_connect(playerid);
	timecyc_on_player_connect(playerid);
	veh_create_player_textdraws(playerid);
	veh_on_player_connect(playerid);
	zones_on_player_connect(playerid);

	return 1;
}

/* native B_OnPlayerDeath(playerid, killerid, reason */
static
cell AMX_NATIVE_CALL B_OnPlayerDeath(AMX *amx, cell *params)
{
	const int playerid = params[1]/*, killerdid = params[2]*/;
	/*const int reason = params[3];*/

	if (!ISPLAYING(playerid)) {
		return 0;
	}

	spawned[playerid] = 0;

	missions_on_player_death(playerid);
	spawn_prespawn(playerid);
	timecyc_on_player_death(playerid);
	zones_hide_text(playerid);
	return 1;
}

/* native B_OnPlayerDisconnect(playerid, reason)*/
static
cell AMX_NATIVE_CALL B_OnPlayerDisconnect(AMX *amx, cell *params)
{
	const int playerid = params[1], reason = params[2];
	int i;

	airport_on_player_disconnect(playerid);
	echo_on_player_connection(playerid, reason);
	dialog_on_player_disconnect(playerid);
	maps_on_player_disconnect(playerid);
	missions_on_player_disconnect(playerid);
	panel_remove_panel_player(playerid);
	playtime_on_player_disconnect(playerid);
	pm_on_player_disconnect(playerid);
	veh_on_player_disconnect(playerid);

	/*keep this here, clears logged in status*/
	login_on_player_disconnect(playerid, reason);
	/*keep this last*/
	pdata_on_player_disconnect(playerid);

	playeronlineflag[playerid] = 0;
	spawned[playerid] = 0;

	for (i = 0; i < playercount; i++) {
		if (players[i] == playerid) {
			players[i] = players[playercount - 1];
			playercount--;
			return 1;
		}
	}
	return 1;
}

/* native B_OnPlayerEnterRaceCP(playerid) */
static
cell AMX_NATIVE_CALL B_OnPlayerEnterRaceCP(AMX *amx, cell *params)
{
	const int playerid = params[1];

	return missions_on_player_enter_race_checkpoint(playerid);
}

/* native B_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger) */
static
cell AMX_NATIVE_CALL B_OnPlayerEnterVehicle(AMX *amx, cell *params)
{
	const int playerid = params[1], vehicleid = params[2];
	const int ispassenger = params[3];

	veh_on_player_enter_vehicle(playerid, vehicleid, ispassenger);

	return 1;
}

/* native B_OnPlayerKeyStateChange(playerid, oldkeys, newkeys) */
static
cell AMX_NATIVE_CALL B_OnPlayerKeyStateChange(AMX *amx, cell *params)
{
	const int playerid = params[1], oldkeys = params[2];
	const int newkeys = params[3];

	veh_on_player_key_state_change(playerid, oldkeys, newkeys);
	return 1;
}

/* native B_OnPlayerRequestClass(playerid, classid) */
static
cell AMX_NATIVE_CALL B_OnPlayerRequestClass(AMX *amx, cell *params)
{
	const int playerid = params[1], classid = params[2];

	class_on_player_request_class(playerid, classid);
	timecyc_on_player_request_class(playerid);
	return 1;
}

/* native B_OnPlayerRequestSpawn(playerid) */
static
cell AMX_NATIVE_CALL B_OnPlayerRequestSpawn(AMX *amx, cell *params)
{
	const int playerid = params[1];

	if (!ISPLAYING(playerid)) {
		B144(NOLOG);
		NC_SendClientMessage(playerid, COL_WARN, buf144a);
		return 1;
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
	struct vec3 pos;
	const int playerid = params[1];

	if (!ISPLAYING(playerid)) {
		return 0;
	}

	spawned[playerid] = 1;

	common_GetPlayerPos(playerid, &pos);
	maps_stream_for_player(playerid, pos);
	spawn_on_player_spawn(playerid);
	svp_update_mapicons(playerid, pos.x, pos.y);
	zones_on_player_spawn(playerid, pos);
	return 1;
}

/* native B_OnPlayerStateChange(playerid, newstate, oldstate) */
static
cell AMX_NATIVE_CALL B_OnPlayerStateChange(AMX *amx, cell *params)
{
	const int playerid = params[1];
	const int newstate = params[2], oldstate = params[3];

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
	const int playerid = params[1];

	if (!ISPLAYING(playerid)) {
		B144(NOLOG);
		NC_SendClientMessage(playerid, COL_WARN, buf144a);
		return 1;
	}

	if (!anticheat_on_player_text(playerid)) {
		return 0;
	}

	amx_GetAddr(amx, params[2], &addr);
	amx_GetUString(buf, addr, sizeof(buf));

	echo_on_game_chat_or_action(0, playerid, buf);
	return 1;
}

/* native B_OnPlayerUpdate(playerid) */
static
cell AMX_NATIVE_CALL B_OnPlayerUpdate(AMX *amx, cell *params)
{
	const int playerid = params[1];

	if (kick_update_delay[playerid] > 0) {
		if (--kick_update_delay[playerid] == 0) {
			NC_PARS(1);
			nc_params[1] = playerid;
			NC(n_Kick_);
		}
		return 0;
	}
	playtime_on_player_update(playerid);
	timecyc_on_player_update(playerid);
	return 1;
}

/* native B_OnRecv(ssocket:handle, data[], len) */
static
cell AMX_NATIVE_CALL B_OnRecv(AMX *amx, cell *params)
{
	const int len = params[3];
	cell socket_handle = params[1];
	cell *addr;

	amx_GetAddr(amx, params[2], &addr);
	echo_on_receive(socket_handle, params[2], (char*) addr, len);
	return 1;
}

/* native B_OnVehicleSpawn(&vehicleid) */
static
cell AMX_NATIVE_CALL B_OnVehicleSpawn(AMX *amx, cell *params)
{
	cell *addr;
	int vehicleid;

	/*keep this first*/
	amx_GetAddr(amx, params[1], &addr);
	*addr = vehicleid = veh_on_vehicle_spawn(*addr);

	nav_reset_for_vehicle(vehicleid);

	return 1;
}

/* native B_OnVehicleStreamIn(vehicleid, forplayerid) */
static
cell AMX_NATIVE_CALL B_OnVehicleStreamIn(AMX *amx, cell *params)
{
	const int vehicleid = params[1], forplayerid = params[2];

	veh_on_vehicle_stream_in(vehicleid, forplayerid);
	return 1;
}

/* native B_OnVehicleStreamOut(vehicleid, forplayerid) */
static
cell AMX_NATIVE_CALL B_OnVehicleStreamOut(AMX *amx, cell *params)
{
	const int vehicleid = params[1], forplayerid = params[2];

	veh_on_vehicle_stream_out(vehicleid, forplayerid);
	return 1;
}

void OnPlayerNowAfk(int playerid)
{
	panel_remove_panel_player(playerid);
}

void OnPlayerWasAfk(int playerid)
{
	panel_on_player_was_afk(playerid);
	timecyc_on_player_was_afk(playerid);
}

#endif /*IN_BASDONFLY*/
