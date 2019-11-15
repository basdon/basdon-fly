
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
	void dev_missions_toggle_closest_point(AMX*);
	dev_missions_toggle_closest_point(amx);
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
	if (devplatformobj != -1) {
		NC_DestroyObject(devplatformobj);
	}
	NC_GetPlayerPos(playerid, buf32a, buf64a, buf144a);
	NC_CreateObject_(3374, *buf32, *buf64, *buf144,
		0, 0, 0, 0x43960000, &devplatformobj);
	*fbuf144 += 4.0f;
	nc_params[0] = 4;
	nc_params[1] = playerid;
	nc_params[4] = *buf144;
	NC(n_SetPlayerPos_);
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
		amx_SetUString(buf144, WARN"Syntax: /v <modelid>", 144);
		NC_SendClientMessage(playerid, COL_WARN, buf144a);
	} else {
		if (devvehicle != INVALID_VEHICLE_ID) {
			nc_params[0] = 1;
			nc_params[1] = devvehicle;
			NC(n_DestroyVehicle_);
		}
		NC_GetPlayerPos(playerid, buf32a, buf64a, buf144a);
		NC_GetPlayerFacingAngle(playerid, buf32_1a);
		nc_params[0] = 9;
		nc_params[1] = modelid;
		nc_params[2] = *buf32;
		nc_params[3] = *buf64;
		nc_params[4] = *buf144;
		nc_params[5] = *buf32_1;
		nc_params[6] = nc_params[7] = nc_params[8] = 126;
		nc_params[9] = 0;
		NC_(n_CreateVehicle_, &devvehicle);
		natives_NC_PutPlayerInVehicle(amx, playerid, devvehicle, 0);
	}
	return 1;
}

#endif /*IN_CMD*/
