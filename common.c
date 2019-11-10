
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include <string.h>

int loggedstatus[MAX_PLAYERS];
short kickdelay[MAX_PLAYERS];

void common_tp_player(AMX *amx, int playerid, struct vec4 pos)
{
	natives_NC_SetPlayerPos(amx, playerid, pos.coords);
	nc_params[0] = 2;
	nc_params[1] = playerid;
	nc_paramf[2] = pos.r;
	NC(n_SetPlayerFacingAngle);
	nc_params[0] = 1;
	NC(n_SetCameraBehindPlayer);
}

void common_hide_gametext_for_player(AMX *amx, int playerid)
{
	*buf32 = '_';
	*(buf32 + 1) = 0;
	NC_GameTextForPlayer(playerid, buf32a, 2, 3);
}

int common_find_player_in_vehicle_seat(AMX *amx, int vehicleid, int seat)
{
	int n = playercount;

	nc_params[0] = 1;
	while (n--) {
		nc_params[1] = players[n];
		NC(n_GetPlayerVehicleID);
		if (nc_result == vehicleid) {
			NC(n_GetPlayerVehicleSeat);
			if (nc_result == seat) {
				return players[n];
			}
		}
	}
	return INVALID_PLAYER_ID;
}

void common_crash_player(AMX *amx, int playerid)
{
	static const char
		*CRASHSTR = "Wasted~~k~SWITCH_DEBUG_CAM_ON~~k~~TOGGLE_DPAD~"
			"~k~~NETWORK_TALK~~k~~SHOW_MOUSE_POINTER_TOGGLE~";

	amx_SetUString(buf144, CRASHSTR, 144);
	NC_GameTextForPlayer(playerid, buf144a, 5, 5);
}

void common_set_vehicle_engine(AMX *amx, int vehicleid, int enginestatus)
{
	struct VEHICLEPARAMS params;
	common_GetVehicleParamsEx(amx,  vehicleid, &params);
	if (params.engine != enginestatus) {
		params.engine = enginestatus;
		common_SetVehicleParamsEx(amx, vehicleid, &params);
	}
}

#define _(N,V) (1 + (nc_params[N] = V))

void common_GetPlayerKeys(AMX *amx, int playerid, struct PLAYERKEYS *keys)
{
	nc_params[0] = 4;
	nc_params[1] = playerid;
	nc_params[4] = _(3,_(2,buf32a));
	NC(n_GetPlayerKeys);
	memcpy(keys, buf32, sizeof(struct PLAYERKEYS));
}

int common_GetPlayerPos(AMX *amx, int playerid, struct vec3 *pos)
{
	NC_GetPlayerPos(playerid, buf32a, buf64a, buf144a);
	pos->x = *fbuf32;
	pos->y = *fbuf64;
	pos->z = *fbuf144;
	return nc_result;
}

int common_GetVehicleParamsEx(AMX *amx, int vehicleid, struct VEHICLEPARAMS *p)
{
	nc_params[0] = 8;
	nc_params[1] = vehicleid;
	nc_params[8] = _(7,_(6,_(5,_(4,_(3,_(2,buf32a))))));
	NC(n_GetVehicleParamsEx);
	memcpy(p, buf32, sizeof(struct VEHICLEPARAMS));
	return nc_result;
}

int common_GetVehiclePos(AMX *amx, int vehicleid, struct vec3 *pos)
{
	NC_GetVehiclePos(vehicleid, buf32a, buf64a, buf144a);
	pos->x = *fbuf32;
	pos->y = *fbuf64;
	pos->z = *fbuf144;
	return nc_result;
}

int common_GetVehicleRotationQuat(AMX *amx, int vehicleid, struct quat *rot)
{
	/*in SA:MP qw comes first*/
	NC_GetVehicleRotationQuat(vehicleid, buf32_1a, buf32a, buf64a, buf144a);
	rot->qx = *fbuf32;
	rot->qy = *fbuf64;
	rot->qz = *fbuf144;
	rot->qw = *fbuf32_1;
	return nc_result;
}

int common_GetVehicleVelocity(AMX *amx, int vehicleid, struct vec3 *vel)
{
	NC_GetVehicleVelocity(vehicleid, buf32a, buf64a, buf144a);
	vel->x = *fbuf32;
	vel->y = *fbuf64;
	vel->z = *fbuf144;
	return nc_result;
}

int common_SetVehicleParamsEx(AMX *amx, int vehicleid, struct VEHICLEPARAMS *p)
{
	nc_params[0] = 8;
	nc_params[1] = vehicleid;
	memcpy(nc_params + 2, p, sizeof(struct VEHICLEPARAMS));
	NC(n_SetVehicleParamsEx);
	return nc_result;
}
#undef _
