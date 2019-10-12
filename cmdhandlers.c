
/* vim: set filetype=c ts=8 noexpandtab: */

/*This file is directly included in cmd.c,
but also added as a source file in VC2005. Check
for the IN_CMD macro to only compile this
when we're inside cmd.c*/
#ifdef IN_CMD

/**
Respawns the vehicle the player is in.
*/
static int
cmd_admin_respawn(CMDPARAMS)
{
	NC_GetPlayerVehicleID(playerid);
	if (nc_result) {
		NC_SetVehicleToRespawn(nc_result);
	}
	return 1;
}

static const char
	*MSG_VEH_PARK_Y = SUCC"Vehicle parked!",
	*MSG_VEH_PARK_N = WARN"You are not allowed to park this vehicle",
	*MSG_VEH_SPRAY_N = WARN"You are not allowed to respray this vehicle";

static int
cmd_camera(CMDPARAMS)
{
	NC_GivePlayerWeapon(playerid, WEAPON_CAMERA, 3036);
	return 1;
}

static int
cmd_park(CMDPARAMS)
{
	char q[144];
	struct dbvehicle *veh;
	int vehicleid;
	float x, y, z, r;

	NC_GetPlayerVehicleID_(playerid, &vehicleid);
	if (vehicleid) {
		veh = gamevehicles[vehicleid].dbvehicle;
		if (!veh_can_player_modify_veh(playerid, veh))
		{
			amx_SetUString(buf144, MSG_VEH_PARK_N, 144);
			NC_SendClientMessage(playerid, COL_WARN, buf144a);
			return 1;
		}
		NC_GetVehiclePos(vehicleid, buf32a, buf64a, buf144a);
		NC_GetVehicleZAngle(vehicleid, buf32_1a);
		x = amx_ctof(*buf32);
		y = amx_ctof(*buf64);
		z = amx_ctof(*buf144);
		r = amx_ctof(*buf32_1);
		if (356.0f < r || r < 4.0f) {
			r = 0.0f;
		} else if (86.0f < r && r < 94.0f) {
			r = 90.0f;
		} else if (176.0f < r && r < 184.0f) {
			r = 180.0f;
		} else if (266.0f < r && r < 274.0f) {
			r = 270.0f;
		}
		veh->x = x;
		veh->y = y;
		veh->z = z;
		veh->r = r;
		gamevehicles[vehicleid].need_recreation = 1;
		sprintf(q, "UPDATE veh SET x=%f,y=%f,z=%f,r=%f WHERE i=%d",
			x, y, z, r, veh->id);
		amx_SetUString(buf144, q, sizeof(q));
		NC_mysql_tquery_nocb(buf144a);
		amx_SetUString(buf144, MSG_VEH_PARK_Y, 144);
		NC_SendClientMessage(playerid, COL_SUCC, buf144a);
	}
	return 1;
}

static int
cmd_spray(CMDPARAMS)
{
	char q[144];
	struct dbvehicle *veh;
	int vehicleid, col1, col2;

	NC_GetPlayerVehicleID_(playerid, &vehicleid);
	if (vehicleid) {
		veh = gamevehicles[vehicleid].dbvehicle;
		if (!veh_can_player_modify_veh(playerid, veh)) {
			amx_SetUString(buf144, MSG_VEH_SPRAY_N, 144);
			NC_SendClientMessage(playerid, COL_WARN, buf144a);
			return 1;
		}
		if (cmd_get_int_param(cmdtext, &parseidx, &col1)) {
			if (!cmd_get_int_param(cmdtext, &parseidx, &col2)) {
				goto rand2nd;
			}
		} else {
			NC_random_(256, &col1);
rand2nd:
			NC_random_(256, &col2);
		}
		NC_ChangeVehicleColor(vehicleid, col1, col2);
		if (veh != NULL) {
			veh->col1 = col1;
			veh->col2 = col2;
		}
		gamevehicles[vehicleid].need_recreation = 1;
		sprintf(q, "UPDATE veh SET col1=%d,col2=%d WHERE i=%d",
			col1, col2, veh->id);
		amx_SetUString(buf144, q, sizeof(q));
		NC_mysql_tquery_nocb(buf144a);
	}
	return 1;
}

#endif /*IN_CMD*/
