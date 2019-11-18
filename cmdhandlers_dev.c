
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
		natives_NC_PutPlayerInVehicle(playerid, devvehicle, 0);
	}
	return 1;
}

#endif /*IN_CMD*/
