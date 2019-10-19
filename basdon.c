
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

/* native B_Validate(maxplayers, buf4096[], buf144[], buf64[], buf32[],
                     buf32_1[], emptystring) */
cell AMX_NATIVE_CALL B_Validate(AMX *amx, cell *params)
{
	int i;
	for (i = 0; i < MAX_PLAYERS; i++) {
		playeronlineflag[i] = 0;
	}
	playercount = 0;

	if (sizeof(cell) != sizeof(int)) {
		logprintf(
			"ERR: sizeof(cell)=%d sizeof(int)=%d",
			sizeof(cell),
			sizeof(int));
		return 0;
	}

	amx_GetAddr(amx, buf4096a = params[2], &buf4096);
	amx_GetAddr(amx, buf144a = params[3], &buf144);
	amx_GetAddr(amx, buf64a = params[4], &buf64);
	amx_GetAddr(amx, buf32a = params[5], &buf32);
	amx_GetAddr(amx, buf32_1a = params[6], &buf32_1);

	if (!natives_find(amx) || !publics_find(amx)) {
		return 0;
	}

	amx_SetUString(buf144, "sleep", 6);
	NC_GetConsoleVarAsInt(buf144a);
	if (nc_result != 5) {
		logprintf("ERR: sleep value %d should be 5", nc_result);
		return 0;
	}

	if (MAX_PLAYERS != params[1]) {
		logprintf(
			"ERR: MAX_PLAYERS mismatch: %d (plugin) vs %d (gm)",
			MAX_PLAYERS,
			params[1]);
		return 0;
	}

	gamemode_amx = amx;
	return MAX_PLAYERS;
}

/* native B_OnDialogResponse() */
cell AMX_NATIVE_CALL B_OnDialogResponse(AMX *amx, cell *params)
{
	int dialog_on_response(AMX*, int, int);
	void spawn_on_dialog_response(AMX*, int, int, int);

	const int playerid = params[1], dialogid = params[2];
	const int response = params[3], listitem = params[4];
	char inputtext[128];
	cell *addr;

	if (!dialog_on_response(amx, playerid, dialogid)) {
		return 0;
	}
	amx_GetAddr(amx, params[5], &addr);
	amx_GetUString(inputtext, addr, sizeof(inputtext));

	switch (dialogid) {
	case DIALOG_SPAWN_SELECTION:
		spawn_on_dialog_response(amx, playerid, response, listitem);
		return 1;
	}
	return 1;
}

/* native B_OnGameModeExit() */
cell AMX_NATIVE_CALL B_OnGameModeExit(AMX *amx, cell *params)
{
	void airports_destroy();
	void echo_dispose(AMX*);
	void missions_freepoints();
	void spawn_dispose();

	missions_freepoints(); /*call this before airports_destroy!*/
	airports_destroy();
	echo_dispose(amx);
	spawn_dispose();
	return 1;
}

/* native B_OnGameModeInit() */
cell AMX_NATIVE_CALL B_OnGameModeInit(AMX *amx, cell *params)
{
	void airports_init(AMX *amx);
	void class_init(AMX *amx);
	void echo_init(AMX *amx);
	void maps_load_from_db(AMX *amx);

	memset(spawned, 0, sizeof(spawned));

	airports_init(amx);
	class_init(amx);
	maps_load_from_db(amx);
	echo_init(amx);
	return 1;
}

/* native B_OnPlayerCommandText(playerid, cmdtext[]) */
cell AMX_NATIVE_CALL B_OnPlayerCommandText(AMX *amx, cell *params)
{
	int cmd_check(AMX*, const int, const int, const char*);
	int cmd_hash(char*);
	int spawn_on_player_command_text(AMX*, int);

	static const char *NO = WARN"You can't use commands when not spawned.";
	const int playerid = params[1];
	char cmdtext[145];
	cell *addr;
	int hash;

	if (!spawned[playerid]) {
		amx_SetUString(buf144, NO, 144);
		NC_SendClientMessage(playerid, COL_WARN, buf144a);
		return 1;
	}

	amx_GetAddr(amx, params[2], &addr);
	amx_GetUString(cmdtext, addr, sizeof(cmdtext));
	hash = cmd_hash(cmdtext);

	if (cmd_check(amx, playerid, hash, cmdtext)) {
		return 1;
	}

	PC_OnPlayerCommandTextHash(playerid, hash, params[2]);
	return pc_result;
}

/* native B_OnPlayerConnect(playerid) */
cell AMX_NATIVE_CALL B_OnPlayerConnect(AMX *amx, cell *params)
{
	void echo_on_player_connection(AMX*, int, int);
	void class_on_player_connect(AMX*, int);
	void dialog_on_player_connect(AMX*, int);
	void maps_OnPlayerConnect(AMX*, int);
	void zones_OnPlayerConnect(AMX*, int);

	const int playerid = params[1];
	int i;

#ifdef DEV
	amx_SetUString(buf144, "PL: DEVELOPMENT BUILD", 144);
	NC_SendClientMessage(playerid, COL_WARN, buf144a);
#endif /*DEV*/

	pdata_init_player(amx, playerid);

	playeronlineflag[playerid] = 1;

	maps_OnPlayerConnect(amx, playerid);
	class_on_player_connect(amx, playerid);
	echo_on_player_connection(amx, playerid, 3);
	dialog_on_player_connect(amx, playerid);
	zones_on_player_connect(amx, playerid);

	for (i = 0; i < playercount; ){
		if (players[i] == playerid) {
			return 1;
		}
	}
	players[playercount++] = playerid;
	return 1;
}

/* native B_OnPlayerDeath(playerid, killerid, reason */
cell AMX_NATIVE_CALL B_OnPlayerDeath(AMX *amx, cell *params)
{
	void zones_on_player_death(AMX*, int);

	const int playerid = params[1], killerdid = params[2];
	const int reason = params[3];

	spawned[playerid] = 0;

	zones_on_player_death(amx, playerid);
	return 1;
}

/* native B_OnPlayerDisconnect(playerid, reason)*/
cell AMX_NATIVE_CALL B_OnPlayerDisconnect(AMX *amx, cell *params)
{
	void echo_on_player_connection(AMX*, int, int);
	void dialog_on_player_disconnect(AMX*, int);
	void maps_OnPlayerDisconnect(int playerid);

	const int playerid = params[1], reason = params[2];
	int i;

	maps_OnPlayerDisconnect(playerid);
	echo_on_player_connection(amx, playerid, reason);
	dialog_on_player_disconnect(amx, playerid);

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

/* native B_OnPlayerRequestClass(playerid, classid) */
cell AMX_NATIVE_CALL B_OnPlayerRequestClass(AMX *amx, cell *params)
{
	void class_on_player_request_class(AMX*, int, int);

	const int playerid = params[1], classid = params[2];

	class_on_player_request_class(amx, playerid, classid);
	return 1;
}

/* native B_OnPlayerRequestSpawn(playerid) */
cell AMX_NATIVE_CALL B_OnPlayerRequestSpawn(AMX *amx, cell *params)
{
	int class_on_player_request_spawn(AMX*, int);

	const int playerid = params[1];

	return class_on_player_request_spawn(amx, playerid);
}

/* native B_OnPlayerSpawn(playerid) */
cell AMX_NATIVE_CALL B_OnPlayerSpawn(AMX *amx, cell *params)
{
	void spawn_on_player_spawn(AMX*, int);
	void zones_on_player_spawn(AMX*, int);

	const int playerid = params[1];

	spawned[playerid] = 1;

	spawn_on_player_spawn(amx, playerid);
	zones_on_player_spawn(amx, playerid);
	return 1;
}

/* native B_OnPlayerText(playerid, text[]) */
cell AMX_NATIVE_CALL B_OnPlayerText(AMX *amx, cell *params)
{
	void echo_on_game_chat(AMX*, int, char*);

	cell *addr;
	char buf[144];
	const int playerid = params[1];

	amx_GetAddr(amx, params[2], &addr);
	amx_GetUString(buf, addr, sizeof(buf));

	echo_on_game_chat(amx, playerid, buf);
	return 1;
}

/* native B_OnRecv(ssocket:handle, data[], len) */
cell AMX_NATIVE_CALL B_OnRecv(AMX *amx, cell *params)
{
	void echo_on_receive(AMX*, cell, cell, char*, int);

	const int len = params[3];
	cell socket_handle = params[1];
	cell *addr;

	amx_GetAddr(amx, params[2], &addr);
	echo_on_receive(amx, socket_handle, params[2], (char*) addr, len);
	return 1;
}

/* native B_OnVehicleSpawn(&vehicleid) */
cell AMX_NATIVE_CALL B_OnVehicleSpawn(AMX *amx, cell *params)
{
	cell *addr;

	amx_GetAddr(amx, params[1], &addr);
	*addr = veh_OnVehicleSpawn(amx, *addr);
	return 1;
}

#endif /*IN_BASDONFLY*/
