
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include <string.h>

int loggedstatus[MAX_PLAYERS];
short kickdelay[MAX_PLAYERS];

void common_tp_player(int playerid, struct vec4 pos)
{
	natives_SetPlayerPos(playerid, pos.coords);
	NC_PARS(2);
	nc_params[1] = playerid;
	nc_paramf[2] = pos.r;
	NC(n_SetPlayerFacingAngle);
	NC_PARS(1);
	NC(n_SetCameraBehindPlayer);
}

void common_hide_gametext_for_player(int playerid)
{
	buf32[0] = '_';
	buf32[1] = 0;
	NC_GameTextForPlayer(playerid, buf32a, 2, 3);
}

int common_find_player_in_vehicle_seat(int vehicleid, int seat)
{
	int n = playercount;

	NC_PARS(1);
	while (n--) {
		nc_params[1] = players[n];
		if (NC(n_GetPlayerVehicleID) == vehicleid &&
			NC(n_GetPlayerVehicleSeat) == seat)
		{
			return players[n];
		}
	}
	return INVALID_PLAYER_ID;
}

void common_crash_player(int playerid)
{
	amx_SetUString(buf144,
		"Wasted"
		"~k~SWITCH_DEBUG_CAM_ON~"
		"~k~~TOGGLE_DPAD~"
		"~k~~NETWORK_TALK~"
		"~k~~SHOW_MOUSE_POINTER_TOGGLE~",
		144);
	NC_GameTextForPlayer(playerid, buf144a, 5, 5);
}

float common_dist_sq(struct vec3 a, struct vec3 b)
{
	float dx, dy, dz;
	dx = a.x - b.x;
	dy = a.y - b.y;
	dz = a.z - b.z;
	return dx * dx + dy * dy + dz * dz;
}

void common_set_vehicle_engine(int vehicleid, int enginestatus)
{
	struct VEHICLEPARAMS params;
	common_GetVehicleParamsEx(vehicleid, &params);
	if (params.engine != enginestatus) {
		params.engine = enginestatus;
		common_SetVehicleParamsEx(vehicleid, &params);
	}
}

void common_GetPlayerKeys(int playerid, struct PLAYERKEYS *keys)
{
	NC_PARS(4);
	nc_params[1] = playerid;
	nc_params[2] = buf32a;
	nc_params[3] = buf32a + 4;
	nc_params[4] = buf32a + 8;
	NC(n_GetPlayerKeys);
	memcpy(keys, buf32, sizeof(struct PLAYERKEYS));
}

int common_GetPlayerPos(int playerid, struct vec3 *pos)
{
	int res;
	res = NC_GetPlayerPos(playerid, buf32a, buf64a, buf144a);
	pos->x = *fbuf32;
	pos->y = *fbuf64;
	pos->z = *fbuf144;
	return res;
}

int common_GetVehicleDamageStatus(int vehicleid, struct VEHICLEDAMAGE *d)
{
	int res;
	NC_PARS(5);
	nc_params[1] = vehicleid;
	nc_params[2] = buf32a;
	nc_params[3] = buf32a + 0x4;
	nc_params[4] = buf32a + 0x8;
	nc_params[5] = buf32a + 0xC;
	res = NC(n_GetVehicleDamageStatus);
	memcpy(d, buf32, sizeof(struct VEHICLEDAMAGE));
	return res;
}

int common_GetVehicleParamsEx(int vehicleid, struct VEHICLEPARAMS *p)
{
	int res;
	NC_PARS(8);
	nc_params[1] = vehicleid;
	nc_params[2] = buf32a;
	nc_params[3] = buf32a + 0x4;
	nc_params[4] = buf32a + 0x8;
	nc_params[5] = buf32a + 0xC;
	nc_params[6] = buf32a + 0x10;
	nc_params[7] = buf32a + 0x14;
	nc_params[8] = buf32a + 0x18;
	res = NC(n_GetVehicleParamsEx);
	memcpy(p, buf32, sizeof(struct VEHICLEPARAMS));
	return res;
}

int common_GetVehiclePos(int vehicleid, struct vec3 *pos)
{
	int res;
	res = NC_GetVehiclePos(vehicleid, buf32a, buf64a, buf144a);
	pos->x = *fbuf32;
	pos->y = *fbuf64;
	pos->z = *fbuf144;
	return res;
}

int common_GetVehiclePosRot(int vehicleid, struct vec4 *pos)
{
	int res;
	NC_GetVehiclePos(vehicleid, buf32a, buf64a, buf144a);
	pos->coords.x = *fbuf32;
	pos->coords.y = *fbuf64;
	pos->coords.z = *fbuf144;
	NC_PARS(2);
	res = NC(n_GetVehiclePos);
	pos->r = *fbuf32;
	return res;
}

int common_GetVehicleRotationQuat(int vehicleid, struct quat *rot)
{
	int res;
	/*in SA:MP qw comes first*/
	NC_PARS(5);
	nc_params[1] = vehicleid;
	nc_params[2] = buf32_1a;
	nc_params[3] = buf32a;
	nc_params[4] = buf64a;
	nc_params[5] = buf144a;
	res = NC(n_GetVehicleRotationQuat);
	rot->qx = *fbuf32;
	rot->qy = *fbuf64;
	rot->qz = *fbuf144;
	rot->qw = *fbuf32_1;
	return res;
}

int common_GetVehicleVelocity(int vehicleid, struct vec3 *vel)
{
	int res;
	res = NC_GetVehicleVelocity(vehicleid, buf32a, buf64a, buf144a);
	vel->x = *fbuf32;
	vel->y = *fbuf64;
	vel->z = *fbuf144;
	return res;
}

int common_SetVehicleParamsEx(int vehicleid, struct VEHICLEPARAMS *p)
{
	NC_PARS(8);
	nc_params[1] = vehicleid;
	memcpy(nc_params + 2, p, sizeof(struct VEHICLEPARAMS));
	return NC(n_SetVehicleParamsEx);
}

int common_UpdateVehicleDamageStatus(int vehicleid, struct VEHICLEDAMAGE *d)
{
	NC_PARS(5);
	nc_params[1] = vehicleid;
	memcpy(nc_params + 2, d, sizeof(struct VEHICLEDAMAGE));
	return NC(n_UpdateVehicleDamageStatus);
}

void common_mysql_tquery(char *query, mysql_cb callback, void *data)
{
	amx_SetUString(buf4096, query, 2000);
	buf4096[4088] = 'M';
	buf4096[4089] = 'M';
	buf4096[4090] = 0;
	buf4096[4091] = 'i';
	buf4096[4092] = 'i';
	buf4096[4093] = 0;
	buf4096[4094] = (cell) callback;
	buf4096[4095] = (cell) data;
	nc_params[0] = 6 << 2; /*varargs, don't remove*/
	nc_params[1] = 1;
	nc_params[2] = buf4096a;
	nc_params[3] = buf4096a + 4088 * 4;
	nc_params[4] = buf4096a + 4091 * 4;
	nc_params[5] = buf4096a + 4094 * 4;
	nc_params[6] = buf4096a + 4095 * 4;
	NC(n_mysql_tquery);
}

float common_vectorsize(struct vec3 vec)
{
	return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}