
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "maps.h"
#include "natives.h"
#include "servicepoints.h"
#include "spawn.h"
#include "vehicles.h"
#include "zones.h"

/* Natives ending in _ means that they have
custom code and shouldn't be used directly.*/

AMX_NATIVE n_AddPlayerClass;
AMX_NATIVE n_ChangeVehicleColor;
AMX_NATIVE n_ClearAnimations;
AMX_NATIVE n_CreateObject;
AMX_NATIVE n_CreatePlayer3DTextLabel;
AMX_NATIVE n_CreatePlayerObject;
AMX_NATIVE n_CreatePlayerTextDraw;
AMX_NATIVE n_CreateVehicle_;
AMX_NATIVE n_DeletePlayer3DTextLabel;
AMX_NATIVE n_DestroyObject;
AMX_NATIVE n_DestroyPlayerObject;
AMX_NATIVE n_DestroyVehicle_;
AMX_NATIVE n_DisablePlayerRaceCheckpoint;
AMX_NATIVE n_ForceClassSelection;
AMX_NATIVE n_GameTextForPlayer;
AMX_NATIVE n_GetConsoleVarAsInt;
AMX_NATIVE n_GetPlayerFacingAngle;
AMX_NATIVE n_GetPlayerIp;
AMX_NATIVE n_GetPlayerKeys;
AMX_NATIVE n_GetPlayerName;
AMX_NATIVE n_GetPlayerPos;
AMX_NATIVE n_GetPlayerState;
AMX_NATIVE n_GetPlayerVehicleID;
AMX_NATIVE n_GetPlayerVehicleSeat;
AMX_NATIVE n_GetServerTickRate;
AMX_NATIVE n_GetVehicleHealth_;
AMX_NATIVE n_GetVehicleModel;
AMX_NATIVE n_GetVehicleParamsEx;
AMX_NATIVE n_GetVehiclePos;
AMX_NATIVE n_GetVehicleRotationQuat;
AMX_NATIVE n_GetVehicleVelocity;
AMX_NATIVE n_GetVehicleZAngle;
AMX_NATIVE n_GivePlayerMoney_;
AMX_NATIVE n_GivePlayerWeapon;
AMX_NATIVE n_Kick_;
AMX_NATIVE n_PlayerPlaySound;
AMX_NATIVE n_PlayerTextDrawAlignment;
AMX_NATIVE n_PlayerTextDrawBackgroundColor;
AMX_NATIVE n_PlayerTextDrawColor;
AMX_NATIVE n_PlayerTextDrawDestroy;
AMX_NATIVE n_PlayerTextDrawFont;
AMX_NATIVE n_PlayerTextDrawHide;
AMX_NATIVE n_PlayerTextDrawLetterSize;
AMX_NATIVE n_PlayerTextDrawSetOutline;
AMX_NATIVE n_PlayerTextDrawSetProportional;
AMX_NATIVE n_PlayerTextDrawSetShadow;
AMX_NATIVE n_PlayerTextDrawSetString;
AMX_NATIVE n_PlayerTextDrawShow;
AMX_NATIVE n_PutPlayerInVehicle_;
AMX_NATIVE n_RemoveBuildingForPlayer;
AMX_NATIVE n_ResetPlayerMoney_;
AMX_NATIVE n_RepairVehicle;
AMX_NATIVE n_RemovePlayerMapIcon;
AMX_NATIVE n_SendClientMessage;
AMX_NATIVE n_SendClientMessageToAll;
AMX_NATIVE n_SendRconCommand;
AMX_NATIVE n_SetCameraBehindPlayer;
AMX_NATIVE n_SetPlayerCameraPos;
AMX_NATIVE n_SetPlayerCameraLookAt;
AMX_NATIVE n_SetPlayerColor;
AMX_NATIVE n_SetPlayerFacingAngle;
AMX_NATIVE n_SetPlayerHealth;
AMX_NATIVE n_SetPlayerMapIcon;
AMX_NATIVE n_SetPlayerPos_;
AMX_NATIVE n_SetPlayerRaceCheckpoint;
AMX_NATIVE n_SetPlayerSpecialAction;
AMX_NATIVE n_SetPlayerTime;
AMX_NATIVE n_SetPlayerWeather;
AMX_NATIVE n_SetSpawnInfo;
AMX_NATIVE n_SetVehicleHealth;
AMX_NATIVE n_SetVehicleParamsEx;
AMX_NATIVE n_SetVehicleToRespawn;
AMX_NATIVE n_ShowPlayerDialog_;
AMX_NATIVE n_SpawnPlayer_;
AMX_NATIVE n_TextDrawAlignment;
AMX_NATIVE n_TextDrawBoxColor;
AMX_NATIVE n_TextDrawColor;
AMX_NATIVE n_TextDrawCreate;
AMX_NATIVE n_TextDrawFont;
AMX_NATIVE n_TextDrawHideForPlayer;
AMX_NATIVE n_TextDrawLetterSize;
AMX_NATIVE n_TextDrawSetOutline;
AMX_NATIVE n_TextDrawSetProportional;
AMX_NATIVE n_TextDrawSetShadow;
AMX_NATIVE n_TextDrawShowForPlayer;
AMX_NATIVE n_TextDrawTextSize;
AMX_NATIVE n_TextDrawUseBox;
AMX_NATIVE n_TogglePlayerClock;
AMX_NATIVE n_TogglePlayerControllable;
AMX_NATIVE n_TogglePlayerSpectating;
AMX_NATIVE n_cache_delete;
AMX_NATIVE n_cache_get_row;
AMX_NATIVE n_cache_get_row_count;
AMX_NATIVE n_cache_get_row_float;
AMX_NATIVE n_cache_get_row_int;
AMX_NATIVE n_cache_insert_id;
AMX_NATIVE n_mysql_tquery;
AMX_NATIVE n_mysql_query;
AMX_NATIVE n_ssocket_connect;
AMX_NATIVE n_ssocket_create;
AMX_NATIVE n_ssocket_destroy;
AMX_NATIVE n_ssocket_listen;
AMX_NATIVE n_ssocket_send;
AMX_NATIVE n_random;
AMX_NATIVE n_tickcount;

AMX *amx;

static cell nc_params_d[20];

cell *nc_params = nc_params_d;
float *nc_paramf = (float*) nc_params_d;

cell tmpfloat;

cell emptystringa, buf32a, buf32_1a, buf64a, buf144a, buf4096a;
cell underscorestringa;
cell *emptystring, *buf32, *buf32_1, *buf64, *buf144, *buf4096;
cell *underscorestring;
char *cemptystring, *cbuf32, *cbuf32_1, *cbuf64, *cbuf144, *cbuf4096;
char *cunderscorestring;
float *fbuf32_1, *fbuf32, *fbuf64, *fbuf144, *fbuf4096;

int natives_find()
{
#define N(X) {#X,(int*)&n_##X}
#define N_(X) {#X,(int*)&n_##X##_}
	struct NATIVE {
		char *name;
		int *var;
	};
	struct NATIVE natives[] = {
		N(AddPlayerClass),
		N(AddPlayerClass),
		N(ChangeVehicleColor),
		N(ClearAnimations),
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
		N(GetPlayerFacingAngle),
		N(GetPlayerKeys),
		N(GetPlayerName),
		N(GetPlayerPos),
		N(GetPlayerState),
		N(GetPlayerVehicleID),
		N(GetPlayerVehicleSeat),
		N(GetServerTickRate),
		N_(GetVehicleHealth),
		N(GetVehicleModel),
		N(GetVehicleRotationQuat),
		N(GetVehicleParamsEx),
		N(GetVehiclePos),
		N(GetVehicleVelocity),
		N(GetVehicleZAngle),
		N_(GivePlayerMoney),
		N(GivePlayerWeapon),
		N_(Kick),
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
		N(RepairVehicle),
		N_(ResetPlayerMoney),
		N(SendClientMessage),
		N(SendClientMessageToAll),
		N(SendRconCommand),
		N(SetCameraBehindPlayer),
		N(SetPlayerCameraPos),
		N(SetPlayerCameraLookAt),
		N(SetPlayerColor),
		N(SetPlayerHealth),
		N(SetPlayerMapIcon),
		N_(SetPlayerPos),
		N(SetPlayerFacingAngle),
		N(SetPlayerRaceCheckpoint),
		N(SetPlayerSpecialAction),
		N(SetPlayerTime),
		N(SetPlayerWeather),
		N(SetSpawnInfo),
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
		N(TogglePlayerControllable),
		N(cache_delete),
		N(cache_get_row),
		N(cache_get_row_count),
		N(cache_get_row_float),
		N(cache_get_row_int),
		N(cache_insert_id),
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
	AMX_HEADER *header;
	AMX_FUNCSTUB *func;
	unsigned char *nativetable;
	int nativesize;
	int idx;

	header = (AMX_HEADER*) amx->base;
	nativetable = (unsigned char*) header + header->natives;
	nativesize = (int) header->defsize;

	while (n-- != natives) {
		if (amx_FindNative(amx, n->name, &idx) == AMX_ERR_NOTFOUND) {
			logprintf("ERR: no %s native", n->name);
			return 0;
		}
		func = (AMX_FUNCSTUB*) (nativetable + idx * nativesize);
		*n->var = func->address;
	}
	return 1;
}

void natives_Kick(int playerid)
{
	/*TODO: log?*/
	/*TODO: more delay when player is afk?
	gamemode kicked after x player updates, or x ms if player is afk*/
	kickdelay[playerid] = 2;
}

int natives_PutPlayerInVehicle(int playerid, int vehicleid, int seat)
{
	float hp;
	struct vec3 pos;

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

	common_GetVehiclePos(vehicleid, &pos);
	maps_stream_for_player(playerid, pos);
	veh_on_player_now_driving(playerid, vehicleid);
	svp_update_mapicons(playerid, pos.x, pos.y);
	zones_update(playerid, pos);

	NC_PARS(3);
	nc_params[1] = playerid;
	nc_params[2] = vehicleid;
	nc_params[3] = seat;
	return NC(n_PutPlayerInVehicle_);
}

int natives_SetPlayerPos(int playerid, struct vec3 pos)
{
	maps_stream_for_player(playerid, pos);
	svp_update_mapicons(playerid, pos.x, pos.y);
	zones_update(playerid, pos);

	NC_PARS(4);
	nc_params[1] = playerid;
	nc_paramf[2] = pos.x;
	nc_paramf[3] = pos.y;
	nc_paramf[4] = pos.z;
	return NC(n_SetPlayerPos_);
}

void natives_SpawnPlayer(int playerid)
{
	/*eject player first if they're in a vehicle*/
	if (NC_GetPlayerVehicleID(playerid)) {
		NC_GetPlayerPos(playerid, buf32a, buf64a, buf144a);
		NC(n_SetPlayerPos_);
	}
	NC_PARS(1);
	/*nc_params[1] = playerid;*/
	NC(n_SpawnPlayer_);

	spawn_prespawn(playerid);
}
