/**
The /cp command creates a checkpoint on top of the player.
*/
static
int cmd_dev_cp(CMDPARAMS)
{
	struct vec3 pos;
	int vehicleid = GetPlayerVehicleID(playerid);
	if (vehicleid) {
		GetVehiclePos(vehicleid, &pos);
	} else {
		common_GetPlayerPos(playerid, &pos);
	}
	SetPlayerRaceCheckpointNoDir(playerid, 2, &pos, 8.0f);
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
		SendClientMessage(playerid, COL_WARN, WARN"Syntax: /gt <style> <text>");
	}
	return 1;
}

/**
The /crashme command crashes the player.
*/
static
int cmd_dev_crashme(CMDPARAMS)
{
	CrashPlayer(playerid);
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
The SLASH*m <amount> command to give or take money.
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
Command to test SendClientMessage message splitting.
*/
static
int cmd_dev_testmsgsplit(CMDPARAMS)
{
	/*72:	"------------------------------------------------------------------------"*/

	SendClientMessage(playerid, -1,
		"shorter than 144-----------------------------------------------------end");

	SendClientMessage(playerid, 0, "");
	SendClientMessage(playerid, -1,
		"exactly 144-------------------------------------------------------------"
		"---------------------------------------------------------------------end");

	SendClientMessage(playerid, 0, "");
	SendClientMessage(playerid, -1,
		"145---------------------------------------------------------------------"
		"----------------------------------------------------------------------en"

		"d");

	SendClientMessage(playerid, 0, "");
	SendClientMessage(playerid, -1,
		"512---------------------------------------------------------------------"
		"------------------------------------------------------------------------"

		"------------------------------------------------------------------------"
		"------------------------------------------------------------------------"

		"------------------------------------------------------------------------"
		"------------------------------------------------------------------------"

		"------------------------------------------end");

	SendClientMessage(playerid, 0, "");
	SendClientMessage(playerid, -1,
		"600 this should cut off-------------------------------------------------"
		"------------------------------------------------------------------------"

		"------------------------------------------------------------------------"
		"------------------------------------------------------------------------"

		"------------------------------------------------------------------------"
		"------------------------------------------------------------------------"

		"------------------------------------------------------------------------"
		"------------------------------------------------------------------------"

		"---------------------end");

	SendClientMessage(playerid, 0, "");
	SendClientMessage(playerid, -1,
		"512 one color change----------------------------------------------------"
		"------------------------------------------------------------------------"

		"------------------------------------------------------------------------"
		"------------------------------------------{00ff00}----------------------"

		"------------------------------------------------------------------------"
		"------------------------------------------------------------------------"

		"------------------------------------------end");

	SendClientMessage(playerid, 0, "");
	SendClientMessage(playerid, -1,
		"512 three color changes-------------------------------------------------"
		"-------------------------------{ff0000}---------------------------------"

		"------------------------------------------------------------------------"
		"------------------------------------------{00ff00}----------------------"

		"------------------------------------------------------------------------"
		"------------{0000ff}----------------------------------------------------"

		"------------------------------------------end");

	SendClientMessage(playerid, 0, "");
	SendClientMessage(playerid, -1,
		"200 skip bogus color code-------{ff0000}------{cdefgh}---{zzzzzz}------"
		"--{abc}--{00000ff}-----------------------------------------------------"

		"-----------------------------------------------end");

	SendClientMessage(playerid, 0, "");
	SendClientMessage(playerid, -1,
		"200}color brace out of bounds check------------------------------------"
		"-----------------------------------------------------------------------"

		"-----------------------------------------------end");

	SendClientMessage(playerid, 0, "");
	SendClientMessage(playerid, -1,
		"200 color change on boundary no space----------------------------------"
		"-------------------------------------------------------------------{ff0"

		"000}-------------------------------------------end");

	SendClientMessage(playerid, 0, "");
	SendClientMessage(playerid, -1,
		"200 split words---------------------------------------------------------"
		"------------------------------------------------------------------ quick"

		" brown -----------------------------------------end");

	SendClientMessage(playerid, 0, "");
	SendClientMessage(playerid, -1,
		"200 split words---------------------------------------------------------"
		"----------------------------------------------------------------- quick "

		"brown fox jumps over the lazy dog---------------end");

	SendClientMessage(playerid, 0, "");
	SendClientMessage(playerid, -1,
		"200 split words---------------------------------------------------------"
		"-------------------------------------------------------------------- qui"

		"ck brown----------------------------------------end");

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

static int devplatformobj = INVALID_OBJECT_ID;

/**
Creates a haystack object at the player's position.

Useful to make a platform to stand on for taking pictures.
*/
static
int cmd_dev_platform(CMDPARAMS)
{
	struct vec3 ppos;

	if (devplatformobj != INVALID_OBJECT_ID) {
		NC_DestroyObject(devplatformobj);
	}
	common_GetPlayerPos(playerid, &ppos);
	NC_PARS(8);
	nc_params[1] = 3374;
	nc_paramf[2] = ppos.x;
	nc_paramf[3] = ppos.y;
	nc_paramf[4] = ppos.z;
	nc_params[5] = nc_params[6] = nc_params[7] = 0;
	nc_params[8] = 0x43960000;
	devplatformobj = NC(n_CreateObject);
	nc_paramf[4] += 4.0f;
	NC_PARS(4);
	nc_params[1] = playerid;
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
	SendClientMessage(playerid, -1, "you're kicked, bye");
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
	int modelid = -1;
	char msg144[144];

	if (!cmd_get_vehiclemodel_param(cmdtext, &parseidx, &modelid)) {
		SendClientMessage(playerid, COL_WARN, WARN"Syntax: /v <modelid|modelname>");
		return 1;
	}

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
	sprintf(msg144, "%d - %s - %s", modelid, vehmodelnames[modelid - 400], vehnames[modelid - 400]);
	SendClientMessage(playerid, -1, msg144);
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
		SendClientMessage(playerid, COL_WARN, WARN"Syntax: /sound <soundid>");
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
	char msg144[144];

	vehicleid = GetPlayerVehicleID(playerid);
	common_GetVehicleDamageStatus(vehicleid, &vdmg);
	sprintf(msg144, "panels %X doors %X lights %02X tires %02X", vdmg.panels, vdmg.doors, vdmg.lights, vdmg.tires);
	SendClientMessageToAll(-1, msg144);
	return 1;
}

/**
The /vehrespawn command respawns the player's vehicle.
*/
static
int cmd_dev_vehrespawn(CMDPARAMS)
{
	int vehicleid = GetPlayerVehicleID(playerid);
	NC_SetVehicleToRespawn(vehicleid);
	return 1;
}

/**
/vhp [set_hp]
When hp_percent present, set the hp.
Prints the hp of the player's vehicle.
*/
static
int cmd_dev_vhp(CMDPARAMS)
{
	int vehicleid;
	int set_hp;

	vehicleid = GetPlayerVehicleID(playerid);
	if (!vehicleid) {
		return 1;
	}

	if (cmd_get_int_param(cmdtext, &parseidx, &set_hp)) {
		NC_SetVehicleHealth(vehicleid, set_hp);
	}

	NC_PARS(2);
	nc_params[1] = vehicleid;
	nc_params[2] = buf32a;
	NC(n_GetVehicleHealth_);
	sprintf(cbuf32, "hp %f", *fbuf32);
	SendClientMessage(playerid, -1, cbuf32);
	return 1;
}

/**
/vfl [fl_percent]
When fl_percent present, set the fuel leven (in percentage).
Prints the fl of the player's vehicle.
*/
static
int cmd_dev_vfl(CMDPARAMS)
{
	struct dbvehicle *veh;
	int vehicleid;
	int fl_pct;
	float capacity;

	vehicleid = GetPlayerVehicleID(playerid);
	if (!vehicleid) {
		return 1;
	}

	veh = gamevehicles[vehicleid].dbvehicle;
	if (!veh) {
		SendClientMessage(playerid, COL_WARN, WARN"unknown vehicle");
		return 1;
	}

	capacity = model_fuel_capacity(veh->model);

	if (cmd_get_int_param(cmdtext, &parseidx, &fl_pct)) {
		veh->fuel = capacity * fl_pct / 100.0f;
	}

	sprintf(cbuf32, "fl %f/%f (%.1f%%)", veh->fuel, capacity, veh->fuel / capacity);
	SendClientMessage(playerid, -1, cbuf32);
	return 1;
}

/**
The /vphnan command sets player's vehicle to NaN hp.
*/
static
int cmd_dev_vhpnan(CMDPARAMS)
{
	int vehicleid = GetPlayerVehicleID(playerid);
	NC_SetVehicleHealth(vehicleid, 0x7F800100);
	return 1;
}

/**
The /vphninf command sets player's vehicle to negative infinite hp.
*/
static
int cmd_dev_vhpninf(CMDPARAMS)
{
	int vehicleid = GetPlayerVehicleID(playerid);
	NC_SetVehicleHealth(vehicleid, float_ninf);
	return 1;
}

/**
The /vphpinf command sets player's vehicle to positive infinite hp.
*/
static
int cmd_dev_vhppinf(CMDPARAMS)
{
	int vehicleid = GetPlayerVehicleID(playerid);
	NC_SetVehicleHealth(vehicleid, float_pinf);
	return 1;
}
