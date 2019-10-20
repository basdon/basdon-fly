
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "vehicles.h"

/* Natives ending in _ means that they have
custom code and shouldn't be used directly.*/

int n_AddPlayerClass;
int n_ChangeVehicleColor;
int n_CreatePlayerObject;
int n_CreatePlayerTextDraw;
int n_CreateVehicle_;
int n_DestroyPlayerObject;
int n_DestroyVehicle_;
int n_DisablePlayerRaceCheckpoint;
int n_ForceClassSelection;
int n_GameTextForPlayer;
int n_GetConsoleVarAsInt;
int n_GetPlayerIp;
int n_GetPlayerName;
int n_GetPlayerPos;
int n_GetPlayerState;
int n_GetPlayerVehicleID;
int n_GetVehicleModel;
int n_GetVehiclePos;
int n_GetVehicleVelocity;
int n_GetVehicleZAngle;
int n_GivePlayerWeapon;
int n_PlayerPlaySound;
int n_PlayerTextDrawAlignment;
int n_PlayerTextDrawColor;
int n_PlayerTextDrawFont;
int n_PlayerTextDrawHide;
int n_PlayerTextDrawLetterSize;
int n_PlayerTextDrawSetOutline;
int n_PlayerTextDrawSetProportional;
int n_PlayerTextDrawSetShadow;
int n_PlayerTextDrawSetString;
int n_PlayerTextDrawShow;
int n_RemoveBuildingForPlayer;
int n_SendClientMessage;
int n_SendClientMessageToAll;
int n_SendRconCommand;
int n_SetCameraBehindPlayer;
int n_SetPlayerCameraPos;
int n_SetPlayerCameraLookAt;
int n_SetPlayerColor;
int n_SetPlayerFacingAngle;
int n_SetPlayerPos_;
int n_SetPlayerRaceCheckpoint;
int n_SetPlayerSpecialAction;
int n_SetVehicleToRespawn;
int n_ShowPlayerDialog_;
int n_SpawnPlayer_;
int n_TogglePlayerSpectating;
int n_cache_delete;
int n_cache_get_row;
int n_cache_get_row_count;
int n_cache_get_row_float;
int n_cache_get_row_int;
int n_gettime;
int n_printf;
int n_mysql_tquery;
int n_mysql_query;
int n_ssocket_connect;
int n_ssocket_create;
int n_ssocket_destroy;
int n_ssocket_listen;
int n_ssocket_send;
int n_random;
int n_tickcount;

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
		{ "AddPlayerClass", &n_AddPlayerClass },
		{ "ChangeVehicleColor", &n_ChangeVehicleColor },
		{ "CreatePlayerObject", &n_CreatePlayerObject },
		{ "CreatePlayerTextDraw", &n_CreatePlayerTextDraw },
		{ "CreateVehicle", &n_CreateVehicle_ },
		{ "DestroyPlayerObject", &n_DestroyPlayerObject },
		{ "DestroyVehicle", &n_DestroyVehicle_ },
		{ "DisablePlayerRaceCheckpoint",
			&n_DisablePlayerRaceCheckpoint },
		{ "ForceClassSelection", &n_ForceClassSelection },
		{ "GameTextForPlayer", &n_GameTextForPlayer },
		{ "GetConsoleVarAsInt", &n_GetConsoleVarAsInt },
		{ "GetPlayerIp", &n_GetPlayerIp },
		{ "GetPlayerName", &n_GetPlayerName },
		{ "GetPlayerState", &n_GetPlayerState },
		{ "GetPlayerPos", &n_GetPlayerPos },
		{ "GetPlayerVehicleID", &n_GetPlayerVehicleID },
		{ "GetVehicleModel", &n_GetVehicleModel },
		{ "GetVehiclePos", &n_GetVehiclePos },
		{ "GetVehicleVelocity", &n_GetVehicleVelocity },
		{ "GetVehicleZAngle", &n_GetVehicleZAngle },
		{ "GivePlayerWeapon", &n_GivePlayerWeapon },
		{ "PlayerPlaySound", &n_PlayerPlaySound },
		{ "PlayerTextDrawAlignment", &n_PlayerTextDrawAlignment },
		{ "PlayerTextDrawColor", &n_PlayerTextDrawColor },
		{ "PlayerTextDrawFont", &n_PlayerTextDrawFont },
		{ "PlayerTextDrawHide", &n_PlayerTextDrawHide },
		{ "PlayerTextDrawLetterSize", &n_PlayerTextDrawLetterSize },
		{ "PlayerTextDrawSetOutline", &n_PlayerTextDrawSetOutline },
		{ "PlayerTextDrawSetProportional",
			&n_PlayerTextDrawSetProportional },
		{ "PlayerTextDrawSetShadow", &n_PlayerTextDrawSetShadow },
		{ "PlayerTextDrawSetString", &n_PlayerTextDrawSetString },
		{ "PlayerTextDrawShow", &n_PlayerTextDrawShow },
		{ "RemoveBuildingForPlayer", &n_RemoveBuildingForPlayer },
		{ "SendClientMessage", &n_SendClientMessage },
		{ "SendClientMessageToAll", &n_SendClientMessageToAll },
		{ "SendRconCommand", &n_SendRconCommand },
		{ "SetCameraBehindPlayer", &n_SetCameraBehindPlayer },
		{ "SetPlayerCameraPos", &n_SetPlayerCameraPos },
		{ "SetPlayerCameraLookAt", &n_SetPlayerCameraLookAt },
		{ "SetPlayerColor", &n_SetPlayerColor },
		{ "SetPlayerPos", &n_SetPlayerPos_ },
		{ "SetPlayerFacingAngle", &n_SetPlayerFacingAngle },
		{ "SetPlayerRaceCheckpoint", &n_SetPlayerRaceCheckpoint },
		{ "SetPlayerSpecialAction", &n_SetPlayerSpecialAction },
		{ "SetVehicleToRespawn", &n_SetVehicleToRespawn },
		{ "ShowPlayerDialog", &n_ShowPlayerDialog_ },
		{ "SpawnPlayer", &n_SpawnPlayer_ },
		{ "TogglePlayerSpectating", &n_TogglePlayerSpectating },
		{ "cache_delete", &n_cache_delete },
		{ "cache_get_row", &n_cache_get_row },
		{ "cache_get_row_count", &n_cache_get_row_count },
		{ "cache_get_row_float", &n_cache_get_row_float },
		{ "cache_get_row_int", &n_cache_get_row_int },
		{ "gettime", &n_gettime },
		{ "printf", &n_printf },
		{ "mysql_tquery", &n_mysql_tquery },
		{ "mysql_query", &n_mysql_query },
		{ "random", &n_random },
		{ "ssocket_connect", &n_ssocket_connect },
		{ "ssocket_create", &n_ssocket_create },
		{ "ssocket_destroy", &n_ssocket_destroy },
		{ "ssocket_listen", &n_ssocket_listen },
		{ "ssocket_send", &n_ssocket_send },
		{ "tickcount", &n_tickcount },
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

int natives_NC_SetPlayerPos(AMX *amx, int playerid, struct vec3 pos)
{
	void maps_stream_for_player(AMX*, int, struct vec3);
	void zones_update(AMX*, int, struct vec3);

	maps_stream_for_player(amx, playerid, pos);
	zones_update(amx, playerid, pos);

	nc_params[0] = 4;
	nc_params[1] = playerid;
	*((float*) (nc_params + 2)) = pos.x;
	*((float*) (nc_params + 3)) = pos.y;
	*((float*) (nc_params + 4)) = pos.z;
	NC(n_SetPlayerPos_);
	return nc_result;
}

int natives_NC_SpawnPlayer(AMX *amx, int playerid)
{
	NC_GetPlayerVehicleID(playerid);
	if (nc_result) {
		NC_GetPlayerPos(playerid, buf32a, buf64a, buf144a);
		NC(n_SetPlayerPos_);
	}
	nc_params[0] = 1;
	/*nc_params[1] = playerid;*/
	NC(n_SpawnPlayer_);
	return nc_result;
}

int natives_NC_GetPlayerPos(AMX *amx, int playerid, struct vec3 *pos)
{
	NC_GetPlayerPos(playerid, buf32a, buf64a, buf144a);
	pos->x = *((float*) buf32);
	pos->y = *((float*) buf64);
	pos->z = *((float*) buf144);
	return nc_result;
}