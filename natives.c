
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "vehicles.h"

/* Natives ending in _ means that they have
custom code and shouldn't be used directly.*/

int n_AddPlayerClass;
int n_ChangeVehicleColor;
int n_CreateObject;
int n_CreatePlayer3DTextLabel;
int n_CreatePlayerObject;
int n_CreatePlayerTextDraw;
int n_CreateVehicle_;
int n_DeletePlayer3DTextLabel;
int n_DestroyObject;
int n_DestroyPlayerObject;
int n_DestroyVehicle_;
int n_DisablePlayerRaceCheckpoint;
int n_ForceClassSelection;
int n_GameTextForPlayer;
int n_GetConsoleVarAsInt;
int n_GetPlayerFacingAngle;
int n_GetPlayerIp;
int n_GetPlayerName;
int n_GetPlayerPos;
int n_GetPlayerState;
int n_GetPlayerVehicleID;
int n_GetPlayerVehicleSeat;
int n_GetVehicleHealth;
int n_GetVehicleModel;
int n_GetVehicleParamsEx;
int n_GetVehiclePos;
int n_GetVehicleVelocity;
int n_GetVehicleZAngle;
int n_GivePlayerWeapon;
int n_Kick;
int n_PlayerPlaySound;
int n_PlayerTextDrawAlignment;
int n_PlayerTextDrawBackgroundColor;
int n_PlayerTextDrawColor;
int n_PlayerTextDrawDestroy;
int n_PlayerTextDrawFont;
int n_PlayerTextDrawHide;
int n_PlayerTextDrawLetterSize;
int n_PlayerTextDrawSetOutline;
int n_PlayerTextDrawSetProportional;
int n_PlayerTextDrawSetShadow;
int n_PlayerTextDrawSetString;
int n_PlayerTextDrawShow;
int n_PutPlayerInVehicle_;
int n_RemoveBuildingForPlayer;
int n_RemovePlayerMapIcon;
int n_SendClientMessage;
int n_SendClientMessageToAll;
int n_SendRconCommand;
int n_SetCameraBehindPlayer;
int n_SetPlayerCameraPos;
int n_SetPlayerCameraLookAt;
int n_SetPlayerColor;
int n_SetPlayerFacingAngle;
int n_SetPlayerMapIcon;
int n_SetPlayerPos_;
int n_SetPlayerRaceCheckpoint;
int n_SetPlayerSpecialAction;
int n_SetPlayerTime;
int n_SetVehicleHealth;
int n_SetVehicleParamsEx;
int n_SetVehicleToRespawn;
int n_ShowPlayerDialog_;
int n_SpawnPlayer_;
int n_TextDrawAlignment;
int n_TextDrawBoxColor;
int n_TextDrawColor;
int n_TextDrawCreate;
int n_TextDrawFont;
int n_TextDrawHideForPlayer;
int n_TextDrawLetterSize;
int n_TextDrawSetOutline;
int n_TextDrawSetProportional;
int n_TextDrawSetShadow;
int n_TextDrawShowForPlayer;
int n_TextDrawTextSize;
int n_TextDrawUseBox;
int n_TogglePlayerClock;
int n_TogglePlayerSpectating;
int n_cache_delete;
int n_cache_get_row;
int n_cache_get_row_count;
int n_cache_get_row_float;
int n_cache_get_row_int;
int n_cache_insert_id;
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
#define N(X) {#X,&n_##X}
#define N_(X) {#X,&n_##X##_}
	struct NATIVE {
		char *name;
		int *var;
	};
	struct NATIVE natives[] = {
		N(AddPlayerClass),
		N(AddPlayerClass),
		N(ChangeVehicleColor),
		N(CreateObject),
		N(CreatePlayer3DTextLabel),
		N(CreatePlayerObject),
		N(CreatePlayerTextDraw),
		N_(CreateVehicle),
		N(DeletePlayer3DTextLabel),
		N(DestroyObject),
		N(DestroyPlayerObject),
		N_(DestroyVehicle),
		N(DisablePlayerRaceCheckpoint),
		N(ForceClassSelection),
		N(GameTextForPlayer),
		N(GetConsoleVarAsInt),
		N(GetPlayerIp),
		N(GetPlayerName),
		N(GetPlayerState),
		N(GetVehicleParamsEx),
		N(GetPlayerFacingAngle),
		N(GetPlayerPos),
		N(GetPlayerVehicleID),
		N(GetPlayerVehicleSeat),
		N(GetVehicleHealth),
		N(GetVehicleModel),
		N(GetVehiclePos),
		N(GetVehicleVelocity),
		N(GetVehicleZAngle),
		N(GivePlayerWeapon),
		N(Kick),
		N(PlayerPlaySound),
		N(PlayerTextDrawAlignment),
		N(PlayerTextDrawBackgroundColor),
		N(PlayerTextDrawColor),
		N(PlayerTextDrawDestroy),
		N(PlayerTextDrawFont),
		N(PlayerTextDrawHide),
		N(PlayerTextDrawLetterSize),
		N(PlayerTextDrawSetOutline),
		N(PlayerTextDrawSetProportional),
		N(PlayerTextDrawSetShadow),
		N(PlayerTextDrawSetString),
		N(PlayerTextDrawShow),
		N_(PutPlayerInVehicle),
		N(RemoveBuildingForPlayer),
		N(RemovePlayerMapIcon),
		N(SendClientMessage),
		N(SendClientMessageToAll),
		N(SendRconCommand),
		N(SetCameraBehindPlayer),
		N(SetPlayerCameraPos),
		N(SetPlayerCameraLookAt),
		N(SetPlayerColor),
		N(SetPlayerMapIcon),
		N_(SetPlayerPos),
		N(SetPlayerFacingAngle),
		N(SetPlayerRaceCheckpoint),
		N(SetPlayerSpecialAction),
		N(SetPlayerTime),
		N(SetVehicleHealth),
		N(SetVehicleParamsEx),
		N(SetVehicleToRespawn),
		N_(ShowPlayerDialog),
		N_(SpawnPlayer),
		N(TogglePlayerClock),
		N(TogglePlayerSpectating),
		N(TextDrawAlignment),
		N(TextDrawBoxColor),
		N(TextDrawColor),
		N(TextDrawCreate),
		N(TextDrawFont),
		N(TextDrawHideForPlayer),
		N(TextDrawLetterSize),
		N(TextDrawSetOutline),
		N(TextDrawSetProportional),
		N(TextDrawSetShadow),
		N(TextDrawShowForPlayer),
		N(TextDrawTextSize),
		N(TextDrawUseBox),
		N(cache_delete),
		N(cache_get_row),
		N(cache_get_row_count),
		N(cache_get_row_float),
		N(cache_get_row_int),
		N(cache_insert_id),
		N(gettime),
		N(printf),
		N(mysql_tquery),
		N(mysql_query),
		N(random),
		N(ssocket_connect),
		N(ssocket_create),
		N(ssocket_destroy),
		N(ssocket_listen),
		N(ssocket_send),
		N(tickcount),
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

int natives_NC_PutPlayerInVehicle(
	AMX *amx, int playerid, int vehicleid, int seat)
{
	void maps_stream_for_player(AMX*, int, struct vec3);
	void veh_update_service_point_mapicons(AMX*, int, float, float);
	void zones_update(AMX*, int, struct vec3);

	struct vec3 pos;

	natives_NC_GetVehiclePos(amx, vehicleid, &pos);
	maps_stream_for_player(amx, playerid, pos);
	veh_on_player_now_driving(amx, playerid, vehicleid);
	veh_update_service_point_mapicons(amx, playerid, pos.x, pos.y);
	zones_update(amx, playerid, pos);

	nc_params[0] = 3;
	nc_params[1] = playerid;
	nc_params[2] = vehicleid;
	nc_params[3] = seat;
	NC(n_PutPlayerInVehicle_);
	return nc_result;
}

int natives_NC_SetPlayerPos(AMX *amx, int playerid, struct vec3 pos)
{
	void maps_stream_for_player(AMX*, int, struct vec3);
	void veh_update_service_point_mapicons(AMX*, int, float, float);
	void zones_update(AMX*, int, struct vec3);

	maps_stream_for_player(amx, playerid, pos);
	veh_update_service_point_mapicons(amx, playerid, pos.x, pos.y);
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

int natives_NC_GetVehiclePos(AMX *amx, int vehicleid, struct vec3 *pos)
{
	NC_GetVehiclePos(vehicleid, buf32a, buf64a, buf144a);
	pos->x = *((float*) buf32);
	pos->y = *((float*) buf64);
	pos->z = *((float*) buf144);
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
