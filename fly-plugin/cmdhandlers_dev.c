/**
The /cp command creates a checkpoint on top of the player.
*/
static
int cmd_dev_cp(struct COMMANDCONTEXT cmdctx)
{
	struct vec3 pos;
	int vehicleid = GetPlayerVehicleID(cmdctx.playerid);
	if (vehicleid) {
		GetVehiclePos(vehicleid, &pos);
	} else {
		GetPlayerPos(cmdctx.playerid, &pos);
	}
	SetPlayerRaceCheckpointNoDir(cmdctx.playerid, 2, &pos, 8.0f);
	return 1;
}

/**
The /gt command shows gametext for the player.
*/
static
int cmd_dev_gt(struct COMMANDCONTEXT cmdctx)
{
	int style;
	if (cmd_get_int_param(&cmdctx, &style) &&
		cmd_get_str_param(&cmdctx, cbuf4096))
	{
		B144(cbuf4096);
		NC_GameTextForPlayer(cmdctx.playerid, buf144a, 4000, style);
	} else {
		SendClientMessage(cmdctx.playerid, COL_WARN, WARN"Syntax: /gt <style> <text>");
	}
	return 1;
}

/**
The /crashme command crashes the player.
*/
static
int cmd_dev_crashme(struct COMMANDCONTEXT cmdctx)
{
	CrashPlayer(cmdctx.playerid);
	return 1;
}

/**
The //drvc command calls DisableRemoteVehicleCollisions
*/
static
int cmd_dev_disableremotevehiclecollisions(struct COMMANDCONTEXT cmdctx)
{
	int disable;

	if (cmd_get_int_param(&cmdctx, &disable)) {
		DisableRemoteVehicleCollisions(cmdctx.playerid, disable);
	} else {
		SendClientMessage(cmdctx.playerid, COL_WARN, WARN"Syntax: //drvc <disable>");
	}
	return 1;
}

/**
The /jetpack command gives player a jetpack.
*/
static
int cmd_dev_jetpack(struct COMMANDCONTEXT cmdctx)
{
	NC_SetPlayerSpecialAction(cmdctx.playerid, SPECIAL_ACTION_USEJETPACK);
	return 1;
}

/**
The SLASH*m <amount> command to give or take money.
*/
static
int cmd_dev_STARm(struct COMMANDCONTEXT cmdctx)
{
	int i;
	if (cmd_get_int_param(&cmdctx, &i)) {
		money_give(cmdctx.playerid, i);
	}
	return 1;
}

/**
Command to test SendClientMessage message splitting.
*/
static
int cmd_dev_testmsgsplit(struct COMMANDCONTEXT cmdctx)
{
	/*72:	"------------------------------------------------------------------------"*/

	SendClientMessage(cmdctx.playerid, -1,
		"shorter than 144-----------------------------------------------------end");

	SendClientMessage(cmdctx.playerid, 0, "");
	SendClientMessage(cmdctx.playerid, -1,
		"exactly 144-------------------------------------------------------------"
		"---------------------------------------------------------------------end");

	SendClientMessage(cmdctx.playerid, 0, "");
	SendClientMessage(cmdctx.playerid, -1,
		"145---------------------------------------------------------------------"
		"----------------------------------------------------------------------en"

		"d");

	SendClientMessage(cmdctx.playerid, 0, "");
	SendClientMessage(cmdctx.playerid, -1,
		"512---------------------------------------------------------------------"
		"------------------------------------------------------------------------"

		"------------------------------------------------------------------------"
		"------------------------------------------------------------------------"

		"------------------------------------------------------------------------"
		"------------------------------------------------------------------------"

		"------------------------------------------end");

	SendClientMessage(cmdctx.playerid, 0, "");
	SendClientMessage(cmdctx.playerid, -1,
		"600 this should cut off-------------------------------------------------"
		"------------------------------------------------------------------------"

		"------------------------------------------------------------------------"
		"------------------------------------------------------------------------"

		"------------------------------------------------------------------------"
		"------------------------------------------------------------------------"

		"------------------------------------------------------------------------"
		"------------------------------------------------------------------------"

		"---------------------end");

	SendClientMessage(cmdctx.playerid, 0, "");
	SendClientMessage(cmdctx.playerid, -1,
		"512 one color change----------------------------------------------------"
		"------------------------------------------------------------------------"

		"------------------------------------------------------------------------"
		"------------------------------------------{00ff00}----------------------"

		"------------------------------------------------------------------------"
		"------------------------------------------------------------------------"

		"------------------------------------------end");

	SendClientMessage(cmdctx.playerid, 0, "");
	SendClientMessage(cmdctx.playerid, -1,
		"512 three color changes-------------------------------------------------"
		"-------------------------------{ff0000}---------------------------------"

		"------------------------------------------------------------------------"
		"------------------------------------------{00ff00}----------------------"

		"------------------------------------------------------------------------"
		"------------{0000ff}----------------------------------------------------"

		"------------------------------------------end");

	SendClientMessage(cmdctx.playerid, 0, "");
	SendClientMessage(cmdctx.playerid, -1,
		"200 skip bogus color code-------{ff0000}------{cdefgh}---{zzzzzz}------"
		"--{abc}--{00000ff}-----------------------------------------------------"

		"-----------------------------------------------end");

	SendClientMessage(cmdctx.playerid, 0, "");
	SendClientMessage(cmdctx.playerid, -1,
		"200}color brace out of bounds check------------------------------------"
		"-----------------------------------------------------------------------"

		"-----------------------------------------------end");

	SendClientMessage(cmdctx.playerid, 0, "");
	SendClientMessage(cmdctx.playerid, -1,
		"200 color change on boundary no space----------------------------------"
		"-------------------------------------------------------------------{ff0"

		"000}-------------------------------------------end");

	SendClientMessage(cmdctx.playerid, 0, "");
	SendClientMessage(cmdctx.playerid, -1,
		"200 split words---------------------------------------------------------"
		"------------------------------------------------------------------ quick"

		" brown -----------------------------------------end");

	SendClientMessage(cmdctx.playerid, 0, "");
	SendClientMessage(cmdctx.playerid, -1,
		"200 split words---------------------------------------------------------"
		"----------------------------------------------------------------- quick "

		"brown fox jumps over the lazy dog---------------end");

	SendClientMessage(cmdctx.playerid, 0, "");
	SendClientMessage(cmdctx.playerid, -1,
		"200 split words---------------------------------------------------------"
		"-------------------------------------------------------------------- qui"

		"ck brown----------------------------------------end");

	return 1;
}

/**
Command to test parameter parsing in plugin code.
*/
static
int cmd_dev_testparpl(struct COMMANDCONTEXT cmdctx)
{
	int i;
	char buf[144];

	if (cmd_get_int_param(&cmdctx, &i)) {
		printf("int %d\n", i);
	}
	if (cmd_get_player_param(&cmdctx, &i)) {
		printf("player %d\n", i);
	}
	if (cmd_get_str_param(&cmdctx, buf)) {
		printf("str -%s-\n", buf);
	}
	if (cmd_get_int_param(&cmdctx, &i)) {
		printf("int %d\n", i);
	}
	return 1;
}

/**
Creates a haystack object at the player's position.

Useful to make a platform to stand on for taking pictures.
*/
static
int cmd_dev_platform(struct COMMANDCONTEXT cmdctx)
{
	struct RPCDATA_CreateObject rpcdata;
	struct BitStream bs;

	GetPlayerPos(cmdctx.playerid, (struct vec3*) &rpcdata.x);
	rpcdata.objectid = 995; /*999 is rotating radar id, 998 and lower may be other reserved things.*/
	rpcdata.modelid = 3374;
	rpcdata.rx = rpcdata.ry = rpcdata.rz = 0.0f;
	rpcdata.drawdistance = 2000.0f;
	rpcdata.no_camera_col = 0;
	rpcdata.attached_object_id = rpcdata.attached_vehicle_id = -1;
	rpcdata.num_materials = 0;
	bs.ptrData = &rpcdata;
	bs.numberOfBitsUsed = sizeof(rpcdata) * 8;
	SAMP_SendRPCToPlayer(RPC_CreateObject, &bs, cmdctx.playerid, 2);

	rpcdata.z += 4.0f;
	SetPlayerPosRaw(cmdctx.playerid, (struct vec3*) &rpcdata.x);
	return 1;
}

/**
The /kill command kills the player.
*/
static
int cmd_dev_kill(struct COMMANDCONTEXT cmdctx)
{
	NC_SetPlayerHealth(cmdctx.playerid, 0.0f);
	return 1;
}

/**
The /kickme commands kicks the player.
*/
static
int cmd_dev_kickme(struct COMMANDCONTEXT cmdctx)
{
	SendClientMessage(cmdctx.playerid, -1, "you're kicked, bye");
	natives_Kick(cmdctx.playerid, "requested \n''\n \0 ok", NULL, -1);
	return 1;
}

/**
Toggle owner group on yourself.
*/
static
int cmd_dev_owner(struct COMMANDCONTEXT cmdctx)
{
	pdata[cmdctx.playerid]->groups ^= GROUP_OWNER;
	pdata[cmdctx.playerid]->groups |= GROUP_MEMBER;
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
int cmd_dev_v(struct COMMANDCONTEXT cmdctx)
{
	struct vec4 pos;
	int modelid = -1;
	char msg144[144];

	if (!cmd_get_vehiclemodel_param(&cmdctx, &modelid)) {
		SendClientMessage(cmdctx.playerid, COL_WARN, WARN"Syntax: /v <modelid|modelname>");
		return 1;
	}

	if (devvehicle != INVALID_VEHICLE_ID) {
		NC_PARS(1);
		nc_params[1] = devvehicle;
		NC(n_DestroyVehicle_);
	}
	GetPlayerPosRot(cmdctx.playerid, &pos);
	NC_PARS(9);
	nc_params[1] = modelid;
	nc_paramf[2] = pos.coords.x;
	nc_paramf[3] = pos.coords.y;
	nc_paramf[4] = pos.coords.z;
	nc_paramf[5] = pos.r;
	nc_params[6] = nc_params[7] = nc_params[8] = 126;
	nc_params[9] = 0;
	devvehicle = NC(n_CreateVehicle_);
	natives_PutPlayerInVehicle(cmdctx.playerid, devvehicle, 0);
	sprintf(msg144, "%d - %s - %s", modelid, vehmodelnames[modelid - 400], vehnames[modelid - 400]);
	SendClientMessage(cmdctx.playerid, -1, msg144);
	return 1;
}

/**
The /sound <soundid> command plays a sound.
*/
static
int cmd_dev_sound(struct COMMANDCONTEXT cmdctx)
{
	int soundid;
	if (!cmd_get_int_param(&cmdctx, &soundid)) {
		SendClientMessage(cmdctx.playerid, COL_WARN, WARN"Syntax: /sound <soundid>");
	} else {
		PlayerPlaySound(cmdctx.playerid, soundid);
	}
	return 1;
}

/**
The /vdamage command prints vehicle damage status.
*/
static
int cmd_dev_vdamage(struct COMMANDCONTEXT cmdctx)
{
	struct VEHICLEDAMAGE vdmg;
	int vehicleid;
	char msg144[144];

	vehicleid = GetPlayerVehicleID(cmdctx.playerid);
	common_GetVehicleDamageStatus(vehicleid, &vdmg);
	sprintf(msg144, "panels %X doors %X lights %02X tires %02X", vdmg.panels, vdmg.doors, vdmg.lights, vdmg.tires);
	SendClientMessageToAll(-1, msg144);
	return 1;
}

/**
The /vehrespawn command respawns the player's vehicle.
*/
static
int cmd_dev_vehrespawn(struct COMMANDCONTEXT cmdctx)
{
	int vehicleid = GetPlayerVehicleID(cmdctx.playerid);
	NC_SetVehicleToRespawn(vehicleid);
	return 1;
}

/**
/vhp [set_hp]
When hp_percent present, set the hp.
Prints the hp of the player's vehicle.
*/
static
int cmd_dev_vhp(struct COMMANDCONTEXT cmdctx)
{
	int vehicleid;
	int set_hp;

	vehicleid = GetPlayerVehicleID(cmdctx.playerid);
	if (!vehicleid) {
		return 1;
	}

	if (cmd_get_int_param(&cmdctx, &set_hp)) {
		NC_SetVehicleHealth(vehicleid, set_hp);
	}

	NC_PARS(2);
	nc_params[1] = vehicleid;
	nc_params[2] = buf32a;
	NC(n_GetVehicleHealth_);
	sprintf(cbuf32, "hp %f", *fbuf32);
	SendClientMessage(cmdctx.playerid, -1, cbuf32);
	return 1;
}

/**
/vfl [fl_percent]
When fl_percent present, set the fuel leven (in percentage).
Prints the fl of the player's vehicle.
*/
static
int cmd_dev_vfl(struct COMMANDCONTEXT cmdctx)
{
	struct dbvehicle *veh;
	int vehicleid;
	int fl_pct;
	float capacity;

	vehicleid = GetPlayerVehicleID(cmdctx.playerid);
	if (!vehicleid) {
		return 1;
	}

	veh = gamevehicles[vehicleid].dbvehicle;
	if (!veh) {
		SendClientMessage(cmdctx.playerid, COL_WARN, WARN"unknown vehicle");
		return 1;
	}

	capacity = model_fuel_capacity(veh->model);

	if (cmd_get_int_param(&cmdctx, &fl_pct)) {
		veh->fuel = capacity * fl_pct / 100.0f;
	}

	sprintf(cbuf32, "fl %f/%f (%.1f%%)", veh->fuel, capacity, veh->fuel / capacity);
	SendClientMessage(cmdctx.playerid, -1, cbuf32);
	return 1;
}

/**
The /vphnan command sets player's vehicle to NaN hp.
*/
static
int cmd_dev_vhpnan(struct COMMANDCONTEXT cmdctx)
{
	int vehicleid = GetPlayerVehicleID(cmdctx.playerid);
	NC_SetVehicleHealth(vehicleid, 0x7F800100);
	return 1;
}

/**
The /vphninf command sets player's vehicle to negative infinite hp.
*/
static
int cmd_dev_vhpninf(struct COMMANDCONTEXT cmdctx)
{
	int vehicleid = GetPlayerVehicleID(cmdctx.playerid);
	NC_SetVehicleHealth(vehicleid, float_ninf);
	return 1;
}

/**
The /vphpinf command sets player's vehicle to positive infinite hp.
*/
static
int cmd_dev_vhppinf(struct COMMANDCONTEXT cmdctx)
{
	int vehicleid = GetPlayerVehicleID(cmdctx.playerid);
	NC_SetVehicleHealth(vehicleid, float_pinf);
	return 1;
}
