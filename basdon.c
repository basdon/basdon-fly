
/* vim: set filetype=c ts=8 noexpandtab: */

/*This file is directly included in basdonfly.c,
but also added as a source file in VC2005. Check
for the IN_BASDONFLY macro to only compile this
when we're inside basdonfly.c*/
#ifdef IN_BASDONFLY

short playeronlineflag[MAX_PLAYERS];
short players[MAX_PLAYERS];
int playercount;

int n_cache_delete;
int n_cache_get_row;
int n_cache_get_row_count;
int n_cache_get_row_float;
int n_cache_get_row_int;
int n_printf;
int n_mysql_query;
int n_random;

static cell nc_params_d[20];
cell *nc_params = nc_params_d;
cell nc_result;

cell emptystringa, buf32a, buf32_1a, buf64a, buf144a, buf4096a;
cell *emptystring, *buf32, *buf32_1, *buf64, *buf144, *buf4096;

/* native B_Validate(maxplayers, buf4096[], buf144[], buf64[], buf32[],
                     buf32_1[], emptystring) */
cell AMX_NATIVE_CALL B_Validate(AMX *amx, cell *params)
{
	int i;
	for (i = 0; i < MAX_PLAYERS; i++) {
		playeronlineflag[i] = 0;
	}
	playercount = 0;

	amx_GetAddr(amx, buf4096a = params[2], &buf4096);
	amx_GetAddr(amx, buf144a = params[3], &buf144);
	amx_GetAddr(amx, buf64a = params[4], &buf64);
	amx_GetAddr(amx, buf32a = params[5], &buf32);
	amx_GetAddr(amx, buf32_1a = params[6], &buf32_1);

	amx_FindNative(amx, "cache_delete", &n_cache_delete);
	if (n_cache_delete == 0x7FFFFFFF) {
		logprintf("ERR: no cache_delete native");
		return 0;
	}
	amx_FindNative(amx, "cache_get_row", &n_cache_get_row);
	if (n_cache_get_row == 0x7FFFFFFF) {
		logprintf("ERR: no cache_get_row native");
		return 0;
	}
	amx_FindNative(amx, "cache_get_row_count", &n_cache_get_row_count);
	if (n_cache_get_row_count == 0x7FFFFFFF) {
		logprintf("ERR: no cache_get_row_count native");
		return 0;
	}
	amx_FindNative(amx, "cache_get_row_float", &n_cache_get_row_float);
	if (n_cache_get_row_float == 0x7FFFFFFF) {
		logprintf("ERR: no cache_get_row_float native");
		return 0;
	}
	amx_FindNative(amx, "cache_get_row_int", &n_cache_get_row_int);
	if (n_cache_get_row_int == 0x7FFFFFFF) {
		logprintf("ERR: no cache_get_row_int native");
		return 0;
	}
	amx_FindNative(amx, "printf", &n_printf);
	if (n_printf == 0x7FFFFFFF) {
		logprintf("ERR: no printf native");
		return 0;
	}
	amx_FindNative(amx, "mysql_query", &n_mysql_query);
	if (n_mysql_query == 0x7FFFFFFF) {
		logprintf("ERR: no mysql_query native");
		return 0;
	}
	amx_FindNative(amx, "random", &n_random);
	if (n_random == 0x7FFFFFFF) {
		logprintf("ERR: no random native");
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

/* native B_Loop25() */
cell AMX_NATIVE_CALL B_Loop25(AMX *amx, cell *params)
{
	void maps_process_tick();

	static int loop25invoccount = 0;

	if (loop25invoccount > 3) {
		loop25invoccount = 1;
		/*loop100*/
		maps_process_tick();
	} else {
		loop25invoccount++;
	}
	return 1;
}

/* native B_OnGameModeInit() */
cell AMX_NATIVE_CALL B_OnGameModeInit(AMX *amx, cell *params)
{
	void maps_load_from_db(AMX *amx);

	maps_load_from_db(amx);
	return 1;
}

/* native B_OnPlayerConnect(playerid) */
cell AMX_NATIVE_CALL B_OnPlayerConnect(AMX *amx, cell *params)
{
	const int playerid = params[1];
	int i;

	playeronlineflag[playerid] = 1;

	for (i = 0; i < playercount; ){
		if (players[i] == playerid) {
			return 1;
		}
	}
	players[playercount++] = playerid;
	return 1;
}

/* native B_OnPlayerDisconnect(playerid, reason)*/
cell AMX_NATIVE_CALL B_OnPlayerDisconnect(AMX *amx, cell *params)
{
	const int playerid = params[1];
	int i;

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

#endif /*IN_BASDONFLY*/
