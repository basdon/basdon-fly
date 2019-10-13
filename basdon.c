
/* vim: set filetype=c ts=8 noexpandtab: */

/*This file is directly included in basdonfly.c,
but also added as a source file in VC2005. Check
for the IN_BASDONFLY macro to only compile this
when we're inside basdonfly.c*/
#ifdef IN_BASDONFLY

char playeronlineflag[MAX_PLAYERS];
short players[MAX_PLAYERS];
int playercount;

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

	if (MAX_PLAYERS != params[1]) {
		logprintf(
			"ERR: MAX_PLAYERS mismatch: %d (plugin) vs %d (gm)",
			MAX_PLAYERS,
			params[1]);
		return 0;
	}
	return MAX_PLAYERS;
}

/* native B_Timer1000() */
cell AMX_NATIVE_CALL B_Timer1000(AMX *amx, cell *params)
{
	return 1;
}

void timer100(AMX *amx)
{
	void maps_process_tick(AMX *amx);
#ifdef DEV
	void dev_missions_update_closest_point(AMX*);

	dev_missions_update_closest_point(amx);
#endif /*DEV*/
	maps_process_tick(amx);
}

/* native B_Timer25() */
cell AMX_NATIVE_CALL B_Timer25(AMX *amx, cell *params)
{
	static int loop25invoccount = 0;

	if (loop25invoccount > 3) {
		loop25invoccount = 1;
		timer100(amx);
	} else {
		loop25invoccount++;
	}
	return 1;
}

/* native B_OnGameModeExit() */
cell AMX_NATIVE_CALL B_OnGameModeExit(AMX *amx, cell *params)
{
	void echo_dispose(AMX*);

	echo_dispose(amx);
	return 1;
}

/* native B_OnGameModeInit() */
cell AMX_NATIVE_CALL B_OnGameModeInit(AMX *amx, cell *params)
{
	void maps_load_from_db(AMX *amx);

	maps_load_from_db(amx);
	return 1;
}

/* native B_OnPlayerCommandText(playerid, cmdtext[]) */
cell AMX_NATIVE_CALL B_OnPlayerCommandText(AMX *amx, cell *params)
{
	int cmd_check(AMX*, const int, const int, const char*);
	int cmd_hash(char*);

	const int playerid = params[1];
	char cmdtext[145];
	cell *addr;
	int hash;

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
	void maps_OnPlayerConnect(AMX*, int);

	const int playerid = params[1];
	int i;

#ifdef DEV
	amx_SetUString(buf144, "PL: DEVELOPMENT BUILD", 144);
	NC_SendClientMessage(playerid, COL_WARN, buf144a);
#endif /*DEV*/

	pdata_init_player(amx, playerid);

	playeronlineflag[playerid] = 1;

	for (i = 0; i < playercount; ){
		if (players[i] == playerid) {
			return 1;
		}
	}
	players[playercount++] = playerid;

	maps_OnPlayerConnect(amx, playerid);
	echo_on_player_connection(amx, playerid, 3);
	return 1;
}

/* native B_OnPlayerDisconnect(playerid, reason)*/
cell AMX_NATIVE_CALL B_OnPlayerDisconnect(AMX *amx, cell *params)
{
	void echo_on_player_connection(AMX*, int, int);
	void maps_OnPlayerDisconnect(int playerid);

	const int playerid = params[1], reason = params[2];
	int i;

	maps_OnPlayerDisconnect(playerid);
	echo_on_player_connection(amx, playerid, reason);

	playeronlineflag[playerid] = 0;

	for (i = 0; i < playercount; i++) {
		if (players[i] == playerid) {
			players[i] = players[playercount - 1];
			playercount--;
			return 1;
		}
	}
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

/* native B_OnVehicleSpawn(&vehicleid) */
cell AMX_NATIVE_CALL B_OnVehicleSpawn(AMX *amx, cell *params)
{
	cell *addr;

	amx_GetAddr(amx, params[1], &addr);
	*addr = veh_OnVehicleSpawn(amx, *addr);
	return 1;
}

/* native B_onUDPReceiveData(Socket:id, data[], data_len,
                             remote_client_ip[], remote_client_port) */
cell AMX_NATIVE_CALL B_onUDPReceiveData(AMX *amx, cell *params)
{
	void echo_on_receive(AMX*, cell, cell, cell*, int);

	const int len = params[3];
	cell socket_handle = params[1];
	cell *addr;

	amx_GetAddr(amx, params[2], &addr);
	echo_on_receive(amx, socket_handle, params[2], addr, len);
	return 1;
}

#endif /*IN_BASDONFLY*/
