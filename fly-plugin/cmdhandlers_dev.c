#define CMD__M_SYNTAX "<amount>"
#define CMD__M_DESC "Gives or takes money"
static
int cmd_dev__m(struct COMMANDCONTEXT cmdctx)
{
	int i;

	if (cmd_get_int_param(&cmdctx, &i)) {
		money_give(cmdctx.playerid, i);
		return CMD_OK;
	}
	return CMD_SYNTAX_ERR;
}

#define CMD__W_SYNTAX "[weaponid]"
#define CMD__W_DESC "Gives a weapon"
static
int cmd_dev__w(struct COMMANDCONTEXT cmdctx)
{
	int i;

	if (cmd_get_int_param(&cmdctx, &i)) {
		GivePlayerWeapon(cmdctx.playerid, i, 40);
		return CMD_OK;
	}
	return CMD_SYNTAX_ERR;
}

#define CMD_CP_SYNTAX ""
#define CMD_CP_DESC "Spawns a checkpoint"
static
int cmd_dev_cp(struct COMMANDCONTEXT cmdctx)
{
	struct vec3 pos;
	int vehicleid;

	vehicleid = GetPlayerVehicleID(cmdctx.playerid);
	if (vehicleid) {
		GetVehiclePos(vehicleid, &pos);
	} else {
		GetPlayerPos(cmdctx.playerid, &pos);
	}
	SetPlayerRaceCheckpointNoDir(cmdctx.playerid, 2, &pos, 8.0f);
	return CMD_OK;
}

#define CMD_CRASHME_SYNTAX ""
#define CMD_CRASHME_DESC "Crashes your game"
static
int cmd_dev_crashme(struct COMMANDCONTEXT cmdctx)
{
	CrashPlayer(cmdctx.playerid);
	return CMD_OK;
}

#define CMD_DRVC_SYNTAX "<disable rvc>"
#define CMD_DRVC_DESC "calls DisableRemoteVehicleCollisions"
static
int cmd_dev_disableremotevehiclecollisions(struct COMMANDCONTEXT cmdctx)
{
	int disable;

	if (cmd_get_int_param(&cmdctx, &disable)) {
		DisableRemoteVehicleCollisions(cmdctx.playerid, disable);
		return CMD_OK;
	}
	return CMD_SYNTAX_ERR;
}

#define CMD_FWEATHER_SYNTAX "<weatherid>"
#define CMD_FWEATHER_DESC "Force weather now"
static
int cmd_dev_fweather(struct COMMANDCONTEXT cmdctx)
{
	int w;

	if (cmd_get_int_param(&cmdctx, &w)) {
		weather.current = weather.locked = weather.upcoming = w;
		timecyc_sync(cmdctx.playerid);
		SendClientMessageToAll(-1, "forced weather");
		return CMD_OK;
	}
	return CMD_SYNTAX_ERR;
}

#define CMD_GT_SYNTAX "<style> <text>"
#define CMD_GT_DESC "Shows game text"
static
int cmd_dev_gt(struct COMMANDCONTEXT cmdctx)
{
	int style;
	char textbuf[1024];

	if (cmd_get_int_param(&cmdctx, &style) && cmd_get_str_param(&cmdctx, textbuf)) {
		GameTextForPlayer(cmdctx.playerid, 4000, style, textbuf);
		return CMD_OK;
	}
	return CMD_SYNTAX_ERR;
}

#define CMD_JETPACK_SYNTAX ""
#define CMD_JETPACK_DESC "Gives a jetpack"
static
int cmd_dev_jetpack(struct COMMANDCONTEXT cmdctx)
{
	SetPlayerSpecialAction(cmdctx.playerid, SPECIAL_ACTION_USEJETPACK);
	return CMD_OK;
}

static
int cmd_dev_kickme_delayed(void *data)
{
	int playerid;

	playerid = (int) data;
	SendClientMessage(playerid, -1, "you're kicked, bye");
	natives_Kick(playerid, "requested \n''\n \0 ok", NULL, -1);
	return TIMER_NO_REPEAT;
}

#define CMD_KICKME_SYNTAX "[seconds_delay]"
#define CMD_KICKME_DESC "Kicks you"
static
int cmd_dev_kickme(struct COMMANDCONTEXT cmdctx)
{
	int delay;

	if (cmd_get_int_param(&cmdctx, &delay)) {
		timer_set(delay * 1000, cmd_dev_kickme_delayed, (void*) cmdctx.playerid);
	} else {
		cmd_dev_kickme_delayed((void*) cmdctx.playerid);
	}
	return CMD_OK;
}

#define CMD_KILL_SYNTAX ""
#define CMD_KILL_DESC "Kills you"
static
int cmd_dev_kill(struct COMMANDCONTEXT cmdctx)
{
	SetPlayerHealth(cmdctx.playerid, 0.0f);
	return CMD_OK;
}

#define CMD_NWEATHER_SYNTAX ""
#define CMD_NWEATHER_DESC "Trigger transitioning to next weather (as if it's called by the timer)"
static
int cmd_dev_nweather(struct COMMANDCONTEXT cmdctx)
{
	SendClientMessageToAll(-1, "changing weather");
	timecyc_next_weather(NULL);
	return CMD_OK;
}

#define CMD_OWNER_SYNTAX ""
#define CMD_OWNER_DESC "Toggles owner group on your account"
static
int cmd_dev_owner(struct COMMANDCONTEXT cmdctx)
{
	pdata[cmdctx.playerid]->groups ^= GROUP_OWNER;
	pdata[cmdctx.playerid]->groups |= GROUP_MEMBER;
	return CMD_OK;
}

#define CMD_PLATFORM_SYNTAX ""
#define CMD_PLATFORM_DESC "Create a haystack object at player's position (useful to take pictures)"
static
int cmd_dev_platform(struct COMMANDCONTEXT cmdctx)
{
	struct RPCDATA_CreateObject rpcdata;
	struct BitStream bs;

	GetPlayerPos(cmdctx.playerid, (struct vec3*) &rpcdata.x);
	rpcdata.objectid = OBJECT_DEV_PLATFORM;
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
	return CMD_OK;
}

static
void dev_show_runway_labels(int playerid)
{
	struct RUNWAY *rnw;
	char text[32], text_bits[32];
	int bitlen;
	int id, ap;

	id = 0;
	for (ap = 0; ap < numairports; ap++) {
		rnw = airports[ap].runways;
		while (rnw != airports[ap].runwaysend) {
			sprintf(text, "%s%s\ntouchdown", airports[ap].code, rnw->id);
			bitlen = EncodeString(text_bits, text, sizeof(text_bits));
			Create3DTextLabel(
				playerid, DEBUG_RUNWAY_LABEL_ID_BASE + id, -1, &rnw->touchdown_pos, 1500.0f, 0,
				INVALID_PLAYER_ID, INVALID_VEHICLE_ID, text_bits, bitlen
			);
			id++;
			sprintf(text, "%s%s\nend", airports[ap].code, rnw->id);
			bitlen = EncodeString(text_bits, text, sizeof(text_bits));
			Create3DTextLabel(
				playerid, DEBUG_RUNWAY_LABEL_ID_BASE + id, -1, &rnw->pos, 1500.0f, 0,
				INVALID_PLAYER_ID, INVALID_VEHICLE_ID, text_bits, bitlen
			);
			id++;
			rnw++;
		}
	}
}

#define CMD_RNW_SYNTAX "show|hide|<runwaycode>"
#define CMD_RNW_DESC "Modify runway positions"
static
int cmd_dev_rnw(struct COMMANDCONTEXT cmdctx)
{
	struct RUNWAY *rnw;
	char arg1[144];
	int ap;
	int id;

	if (!cmd_get_str_param(&cmdctx, arg1)) {
		return CMD_SYNTAX_ERR;
	} else if (!strcmp(arg1, "show")) {
		dev_show_runway_labels(cmdctx.playerid);
		return CMD_OK;
	} else  if (!strcmp(arg1, "hide")) {
		id = 0;
		for (ap = 0; ap < numairports; ap++) {
			rnw = airports[ap].runways;
			while (rnw != airports[ap].runwaysend) {
				Delete3DTextLabel(cmdctx.playerid, DEBUG_RUNWAY_LABEL_ID_BASE + id);
				id++;
				Delete3DTextLabel(cmdctx.playerid, DEBUG_RUNWAY_LABEL_ID_BASE + id);
				id++;
				rnw++;
			}
		}
		return CMD_OK;
	} else {
		if (strlen(arg1) > 4) {
			id = 0;
			for (ap = 0; ap < numairports; ap++) {
				if (*(int*) &airports[ap].code == *(int*) arg1) {
					rnw = airports[ap].runways;
					while (rnw != airports[ap].runwaysend) {
						if (!strcmp(arg1 + 4, rnw->id)) {
							GetPlayerPos(cmdctx.playerid, &rnw->pos);
							printf("UPDATE rnw SET x=%.4f,y=%.4f,z=%.4f WHERE id=%d;\n", rnw->pos.x, rnw->pos.y, rnw->pos.z, rnw->dbid);
							airports_calculate_runway_length_and_touchdown_points();
							dev_show_runway_labels(cmdctx.playerid);
							return CMD_OK;
						}
						id += 2;
						rnw++;
					}
					SendClientMessage(cmdctx.playerid, COL_WARN, WARN"runway not found");
					return CMD_OK;
				} else {
					rnw = airports[ap].runways;
					while (rnw != airports[ap].runwaysend) {
						id += 2;
						rnw++;
					}
				}
			}
		}
		SendClientMessage(cmdctx.playerid, COL_WARN, WARN"airport not found");
		return CMD_SYNTAX_ERR;
	}
}

#define CMD_SIREN_SYNTAX "<1/0>"
#define CMD_SIREN_DESC "Sets the vehicle's siren flag"
static
int cmd_dev_siren(struct COMMANDCONTEXT cmdctx)
{
	int flag;

	if (cmd_get_int_param(&cmdctx, &flag)) {
		SetVehicleSiren(GetPlayerVehicleID(cmdctx.playerid), flag);
		return CMD_OK;
	}
	return CMD_SYNTAX_ERR;
}

#define CMD_SKINX_SYNTAX "<skin>"
#define CMD_SKINX_DESC "Changes your skin"
static
int cmd_dev_skinx(struct COMMANDCONTEXT cmdctx)
{
	int skin;

	if (cmd_get_int_param(&cmdctx, &skin)) {
		SetPlayerSkin(cmdctx.playerid, skin);
		return CMD_OK;
	}
	return CMD_SYNTAX_ERR;
}

#define CMD_SOUND_SYNTAX "<soundid>"
#define CMD_SOUND_DESC "Plays a sound"
static
int cmd_dev_sound(struct COMMANDCONTEXT cmdctx)
{
	int soundid;

	if (cmd_get_int_param(&cmdctx, &soundid)) {
		PlayerPlaySound(cmdctx.playerid, soundid);
		return CMD_OK;
	}
	return CMD_SYNTAX_ERR;
}

#define CMD_TESTPARPL_SYNTAX ""
#define CMD_TESTPARPL_DESC "Test parameter parsing"
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
	return CMD_OK;
}

#define CMD_TIMECYC_SYNTAX ""
#define CMD_TIMECYC_DESC "Prints current timecyc data"
static
int cmd_dev_timecyc(struct COMMANDCONTEXT cmdctx)
{
	char msg144[144];

	sprintf(msg144,
		"%02d:%02d current %d upcoming %d locked %d "
		"syncstate %d\n",
		time_h,
		time_m,
		weather.current,
		weather.upcoming,
		weather.locked,
		timecycstate[cmdctx.playerid]);
	SendClientMessage(cmdctx.playerid, -1, msg144);
	return CMD_OK;
}

#define CMD_TIMEX_SYNTAX "<h> <m>"
#define CMD_TIMEX_DESC "Changes your time"
static
int cmd_dev_timex(struct COMMANDCONTEXT cmdctx)
{
	int h, m;

	if (cmd_get_int_param(&cmdctx, &h)) {
		m = 0;
		cmd_get_int_param(&cmdctx, &m);
		time_h = h;
		time_m = m;
		SetPlayerTime(cmdctx.playerid, h, m);
		panel_day_night_changed();
		return CMD_OK;
	}
	return CMD_SYNTAX_ERR;
}


#define CMD_TWEATHER_SYNTAX "<weatherid>"
#define CMD_TWEATHER_DESC "Transition weather to given id"
static
int cmd_dev_tweather(struct COMMANDCONTEXT cmdctx)
{
	int w;

	if (cmd_get_int_param(&cmdctx, &w)) {
		timecyc_set_weather(w);
		SendClientMessageToAll(-1, "changing weather");
		return CMD_OK;
	}
	return CMD_SYNTAX_ERR;
}

#define CMD_V_SYNTAX "<modelid|modelname>"
#define CMD_V_DESC "Spawns a dev vehicle"
static
int cmd_dev_v(struct COMMANDCONTEXT cmdctx)
{
	struct vec4 pos;
	int modelid, vehicleid;
	char msg144[144];

	if (!cmd_get_vehiclemodel_param(&cmdctx, &modelid)) {
		return CMD_SYNTAX_ERR;
	}

	GetPlayerPosRot(cmdctx.playerid, &pos);
	vehicleid = CreateVehicle(modelid, &pos, 126, 126, -1);
	natives_PutPlayerInVehicle(cmdctx.playerid, vehicleid, 0);
	sprintf(msg144, "%d - %s - %s", modelid, vehmodelnames[modelid - 400], vehnames[modelid - 400]);
	SendClientMessage(cmdctx.playerid, -1, msg144);
	return CMD_OK;
}

#define CMD_VDAMAGE_SYNTAX "[<pppppppp> <dddddddd> <ll> <tt>]"
#define CMD_VDAMAGE_DESC "Optionally updates, and prints vehicle damage status"
static
int cmd_dev_vdamage(struct COMMANDCONTEXT cmdctx)
{
	char msg144[144], param_panels[9], param_doors[9], param_lights[3], param_tires[3];
	struct SampVehicle *vehicle;
	int param_len;

	vehicle = vehiclepool->vehicles[GetPlayerVehicleID(cmdctx.playerid)];

	if (!vehicle) {
		SendClientMessage(cmdctx.playerid, COL_WARN, WARN"be in a vehicle");
		return CMD_OK;
	}

	if (cmdctx.cmdtext[cmdctx.parseidx]) {
		if (
			!cmd_get_str_param_n(&cmdctx, param_panels, 8, &param_len, NULL) || param_len != 8 ||
			!cmd_get_str_param_n(&cmdctx, param_doors, 8, &param_len, NULL) || param_len != 8 ||
			!cmd_get_str_param_n(&cmdctx, param_lights, 2, &param_len, NULL) || param_len != 2 ||
			!cmd_get_str_param_n(&cmdctx, param_tires, 2, &param_len, NULL) || param_len != 2
		) {
			return CMD_SYNTAX_ERR;
		}

		vehicle->damageStatus.panels.raw = hexnum(param_panels, 8);
		vehicle->damageStatus.doors.raw = hexnum(param_doors, 8);
		vehicle->damageStatus.broken_lights = (hexdigit(param_lights[0]) << 4) | hexdigit(param_lights[1]);
		vehicle->damageStatus.popped_tires.raw = (hexdigit(param_tires[0]) << 4) | hexdigit(param_tires[1]);
		SyncVehicleDamageStatus(vehicle);
	}

	sprintf(
		msg144,
		"panels %08X doors %08X lights %02X tires %02X",
		vehicle->damageStatus.panels.raw,
		vehicle->damageStatus.doors.raw,
		vehicle->damageStatus.broken_lights,
		vehicle->damageStatus.popped_tires.raw
	);
	SendClientMessageToAll(-1, msg144);
	return CMD_OK;
}

#define CMD_VFL_SYNTAX "[fl_percent]"
#define CMD_VFL_DESC "(Optionally sets then) gets vehicle fuel level"
static
int cmd_dev_vfl(struct COMMANDCONTEXT cmdctx)
{
	struct dbvehicle *veh;
	int vehicleid;
	int fl_pct;
	float capacity;

	vehicleid = GetPlayerVehicleID(cmdctx.playerid);
	if (!vehicleid) {
		return CMD_OK;
	}

	veh = gamevehicles[vehicleid].dbvehicle;
	if (!veh) {
		SendClientMessage(cmdctx.playerid, COL_WARN, WARN"unknown vehicle");
		return CMD_OK;
	}

	capacity = vehicle_fuel_cap[vehicleid];

	if (cmd_get_int_param(&cmdctx, &fl_pct)) {
		vehicle_fuel[vehicleid] = capacity * fl_pct / 100.0f;
	}

	sprintf(cbuf32, "fl %f/%f (%.1f%%)", vehicle_fuel[vehicleid], capacity, vehicle_fuel[vehicleid] / capacity);
	SendClientMessage(cmdctx.playerid, -1, cbuf32);
	return CMD_OK;
}

#define CMD_VHP_SYNTAX "[hp]"
#define CMD_VHP_DESC "(Optionally sets then) gets vehicle hp"
static
int cmd_dev_vhp(struct COMMANDCONTEXT cmdctx)
{
	int vehicleid;
	int set_hp;

	vehicleid = GetPlayerVehicleID(cmdctx.playerid);
	if (!vehicleid) {
		return CMD_OK;
	}

	if (cmd_get_int_param(&cmdctx, &set_hp)) {
		SetVehicleHealth(vehicleid, set_hp);
	}

	sprintf(cbuf32, "hp %f", GetVehicleHealthRaw(vehicleid));
	SendClientMessage(cmdctx.playerid, -1, cbuf32);
	return CMD_OK;
}

#define CMD_VHPNAN_SYNTAX ""
#define CMD_VHPNAN_DESC "Sets vehicle to NaN hp"
static
int cmd_dev_vhpnan(struct COMMANDCONTEXT cmdctx)
{
	float hp;

	hp = 0.0 / 0.0;
	assert(hp != hp);
	SetVehicleHealth(GetPlayerVehicleID(cmdctx.playerid), hp);
	return CMD_OK;
}

#define CMD_VHPNINF_SYNTAX ""
#define CMD_VHPNINF_DESC "Sets vehicle to negative infinite hp"
static
int cmd_dev_vhpninf(struct COMMANDCONTEXT cmdctx)
{
	SetVehicleHealth(GetPlayerVehicleID(cmdctx.playerid), float_ninf);
	return CMD_OK;
}

#define CMD_VHPPINF_SYNTAX ""
#define CMD_VHPPINF_DESC "Sets vehicle to positive infinite hp"
static
int cmd_dev_vhppinf(struct COMMANDCONTEXT cmdctx)
{
	SetVehicleHealth(GetPlayerVehicleID(cmdctx.playerid), float_pinf);
	return CMD_OK;
}
