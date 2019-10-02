
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"

int n_ChangeVehicleColor;
int n_CreatePlayerObject;
int n_DestroyPlayerObject;
int n_DisablePlayerRaceCheckpoint;
int n_GetPlayerPos;
int n_GetPlayerVehicleID;
int n_SendClientMessage;
int n_SendClientMessageToAll;
int n_SetPlayerRaceCheckpoint;
int n_SetVehicleToRespawn;
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

int natives_find(AMX *amx)
{
	struct NATIVE {
		char *name;
		int *var;
	};
	struct NATIVE natives[] = {
		{ "ChangeVehicleColor", &n_ChangeVehicleColor },
		{ "CreatePlayerObject", &n_CreatePlayerObject },
		{ "DestroyPlayerObject", &n_DestroyPlayerObject },
		{ "DisablePlayerRaceCheckpoint",
			&n_DisablePlayerRaceCheckpoint },
		{ "GetPlayerPos", &n_GetPlayerPos },
		{ "GetPlayerVehicleID", &n_GetPlayerVehicleID },
		{ "SendClientMessage", &n_SendClientMessage },
		{ "SendClientMessageToAll", &n_SendClientMessageToAll },
		{ "SetPlayerRaceCheckpoint", &n_SetPlayerRaceCheckpoint },
		{ "SetVehicleToRespawn", &n_SetVehicleToRespawn },
		{ "cache_delete", &n_cache_delete },
		{ "cache_get_row", &n_cache_get_row },
		{ "cache_get_row_count", &n_cache_get_row_count },
		{ "cache_get_row_float", &n_cache_get_row_float },
		{ "cache_get_row_int", &n_cache_get_row_int },
		{ "printf", &n_printf },
		{ "mysql_query", &n_mysql_query },
		{ "random", &n_random },
	};
	struct NATIVE *n = natives + sizeof(natives)/sizeof(struct NATIVE);

	while (n-- != natives) {
		amx_FindNative(amx, n->name, n->var);
		if (*n->var == 0x7FFFFFFF) {
			logprintf("ERR: no %s native", n->name);
			return 0;
		}
	}

	return 1;
}
