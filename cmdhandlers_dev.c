
/* vim: set filetype=c ts=8 noexpandtab: */

/*This file is directly included in cmd.c,
but also added as a source file in VC2005. Check
for the IN_CMD macro to only compile this
when we're inside cmd.c*/
#ifdef IN_CMD

/**
Toggles constantly showing the closest mission checkpoint for every player.
*/
static
int cmd_dev_closestmp(CMDPARAMS)
{
	dev_missions_toggle_closest_point();
	return 1;
}

/**
The /cp command creates a checkpoint on top of the player.
*/
static
int cmd_dev_cp(CMDPARAMS)
{
	struct vec3 pos;
	int vehicleid = NC_GetPlayerVehicleID(playerid);
	if (vehicleid) {
		common_GetVehiclePos(vehicleid, &pos);
	} else {
		common_GetPlayerPos(playerid, &pos);
	}
	NC_SetPlayerRaceCheckpoint(playerid, 2,
		pos.x, pos.y, pos.z,0.0f, 0.0f, 0.0f, 8.0f);
	return 1;
}

/**
The /gt command shows gametext for the player.
*/
static
int cmd_dev_gt(CMDPARAMS)
{
	int style;
	if (cmd_get_int_param(cmdtext, &parseidx, &style) &&
		cmd_get_str_param(cmdtext, &parseidx, cbuf4096))
	{
		B144(cbuf4096);
		NC_GameTextForPlayer(playerid, buf144a, 4000, style);
	} else {
		B144(WARN"Syntax: /gt <style> <text>");
		NC_SendClientMessage(playerid, COL_WARN, buf144a);
	}
	return 1;
}

/**
The /crashme command crashes the player.
*/
static
int cmd_dev_crashme(CMDPARAMS)
{
	common_crash_player(playerid);
	return 1;
}

/**
The /jetpack command gives player a jetpack.
*/
static
int cmd_dev_jetpack(CMDPARAMS)
{
	NC_SetPlayerSpecialAction(playerid, SPECIAL_ACTION_USEJETPACK);
	return 1;
}

/**
The /*m <amount> command to give or take money.
*/
static
int cmd_dev_STARm(CMDPARAMS)
{
	int i;
	if (cmd_get_int_param(cmdtext, &parseidx, &i)) {
		money_give(playerid, i);
	}
	return 1;
}

/**
Command to test parameter parsing in plugin code.
*/
static
int cmd_dev_testparpl(CMDPARAMS)
{
	int i;
	char buf[144];

	if (cmd_get_int_param(cmdtext, &parseidx, &i)) {
		printf("int %d\n", i);
	}
	if (cmd_get_player_param(cmdtext, &parseidx, &i)) {
		printf("player %d\n", i);
	}
	if (cmd_get_str_param(cmdtext, &parseidx, buf)) {
		printf("str -%s-\n", buf);
	}
	if (cmd_get_int_param(cmdtext, &parseidx, &i)) {
		printf("int %d\n", i);
	}
	return 1;
}

static int devplatformobj = -1;

/**
Creates a haystack object at the player's position.

Useful to make a platform to stand on for taking pictures.
*/
static
int cmd_dev_platform(CMDPARAMS)
{
	struct vec3 ppos;

	if (devplatformobj != -1) {
		NC_DestroyObject(devplatformobj);
	}
	common_GetPlayerPos(playerid, &ppos);
	NC_PARS(9);
	nc_params[0] = 3374;
	nc_paramf[1] = ppos.x;
	nc_paramf[2] = ppos.y;
	nc_paramf[3] = ppos.z;
	nc_params[4] = nc_params[5] = nc_params[6] = 0;
	nc_params[7] = 0x43960000;
	devplatformobj = NC(n_CreateObject);
	nc_paramf[2] += 4.0f;
	NC_PARS(4);
	nc_params[1] = playerid;
	nc_params[4] = *buf144;
	NC(n_SetPlayerPos_);
	return 1;
}

/**
The /kill command kills the player.
*/
static
int cmd_dev_kill(CMDPARAMS)
{
	NC_SetPlayerHealth(playerid, 0.0f);
	return 1;
}

/**
The /kickme commands kicks the player.
*/
static
int cmd_dev_kickme(CMDPARAMS)
{
	B144("you're kicked, bye");
	NC_SendClientMessage(playerid, -1, buf144a);
	natives_Kick(playerid, "requested \n''\n \0 ok", NULL, -1);
	return 1;
}

/**
Toggle owner group on yourself.
*/
static
int cmd_dev_owner(CMDPARAMS)
{
	pdata[playerid]->groups ^= GROUP_OWNER;
	pdata[playerid]->groups |= GROUP_MEMBER;
	return 1;
}

/**
Dev vehicle id spawned by the /v command.
*/
static int devvehicle = INVALID_VEHICLE_ID;

/**
The /v cmd to spawn a dev vehicle.
*/
static
int cmd_dev_v(CMDPARAMS)
{
	int modelid;
	if (!cmd_get_int_param(cmdtext, &parseidx, &modelid)) {
		B144(WARN"Syntax: /v <modelid>");
		NC_SendClientMessage(playerid, COL_WARN, buf144a);
	} else {
		if (devvehicle != INVALID_VEHICLE_ID) {
			NC_PARS(1);
			nc_params[1] = devvehicle;
			NC(n_DestroyVehicle_);
		}
		NC_GetPlayerPos(playerid, buf32a, buf64a, buf144a);
		NC_GetPlayerFacingAngle(playerid, buf32_1a);
		NC_PARS(9);
		nc_params[1] = modelid;
		nc_params[2] = *buf32;
		nc_params[3] = *buf64;
		nc_params[4] = *buf144;
		nc_params[5] = *buf32_1;
		nc_params[6] = nc_params[7] = nc_params[8] = 126;
		nc_params[9] = 0;
		devvehicle = NC(n_CreateVehicle_);
		natives_PutPlayerInVehicle(playerid, devvehicle, 0);
	}
	return 1;
}

/**
The /sound <soundid> command plays a sound.
*/
static
int cmd_dev_sound(CMDPARAMS)
{
	int soundid;
	if (!cmd_get_int_param(cmdtext, &parseidx, &soundid)) {
		B144(WARN"Syntax: /sound <soundid>");
		NC_SendClientMessage(playerid, COL_WARN, buf144a);
	} else {
		NC_PlayerPlaySound0(playerid, soundid);
	}
	return 1;
}

/**
The /vdamage command prints vehicle damage status.
*/
static
int cmd_dev_vdamage(CMDPARAMS)
{
	struct VEHICLEDAMAGE vdmg;
	int vehicleid;

	vehicleid = NC_GetPlayerVehicleID(playerid);
	common_GetVehicleDamageStatus(vehicleid, &vdmg);
	csprintf(buf144,
		"panels %p doors %p lights %02x tires %02x",
		vdmg.panels,
		vdmg.doors,
		vdmg.lights,
		vdmg.tires);
	NC_SendClientMessageToAll(-1, buf144a);
	return 1;
}

/**
The /vehrespawn command respawns the player's vehicle.
*/
static
int cmd_dev_vehrespawn(CMDPARAMS)
{
	int vehicleid = NC_GetPlayerVehicleID(playerid);
	NC_SetVehicleToRespawn(vehicleid);
	return 1;
}

/**
The /vhp command prints the hp of the player's vehicle.
*/
static
int cmd_dev_vhp(CMDPARAMS)
{
	NC_PARS(2);
	nc_params[1] = NC_GetPlayerVehicleID(playerid);
	nc_params[2] = buf32a;
	NC(n_GetVehicleHealth_);
	csprintf(buf32, "hp %f", *fbuf32);
	NC_SendClientMessageToAll(-1, buf32a);
	return 1;
}

/**
The /vphnan command sets player's vehicle to NaN hp.
*/
static
int cmd_dev_vhpnan(CMDPARAMS)
{
	int vehicleid = NC_GetPlayerVehicleID(playerid);
	NC_SetVehicleHealth(vehicleid, 0x7F800100);
	return 1;
}

/**
The /vphninf command sets player's vehicle to negative infinite hp.
*/
static
int cmd_dev_vhpninf(CMDPARAMS)
{
	int vehicleid = NC_GetPlayerVehicleID(playerid);
	NC_SetVehicleHealth(vehicleid, FLOAT_NINF);
	return 1;
}

/**
The /vphpinf command sets player's vehicle to positive infinite hp.
*/
static
int cmd_dev_vhppinf(CMDPARAMS)
{
	int vehicleid = NC_GetPlayerVehicleID(playerid);
	NC_SetVehicleHealth(vehicleid, FLOAT_PINF);
	return 1;
}

#endif /*IN_CMD*/
