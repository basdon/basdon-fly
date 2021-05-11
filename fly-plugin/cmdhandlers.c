#define CMD__GETCAR_SYNTAX "<vehicleid>"
#define CMD__GETCAR_DESC "Teleports a vehicle to you"
static
int cmd__getcar(struct COMMANDCONTEXT cmdctx)
{
	struct vec4 ppos;
	int vehicleid;

	if (cmd_get_int_param(&cmdctx, &vehicleid)) {
		GetPlayerPosRot(cmdctx.playerid, &ppos);
		if (common_SetVehiclePos(vehicleid, &ppos.coords)) {
			natives_PutPlayerInVehicle(cmdctx.playerid, vehicleid, 0);
			NC_SetVehicleZAngle(vehicleid, ppos.r);
		} else {
			SendClientMessage(cmdctx.playerid, COL_WARN, WARN" Invalid vehicleid");
		}
		return CMD_OK;
	}
	return CMD_SYNTAX_ERR;
}

#define CMD__GOTO_SYNTAX "(<x> <y> <z>|<airport code|playerid|(partial)name>)"
#define CMD__GOTO_DESC "Teleport to a position/airport/player"
static
int cmd__goto(struct COMMANDCONTEXT cmdctx)
{
	int oldparseidx;
	int targetplayerid;
	int x, y, z;
	int i;
	struct vec4 pos;
	char input_apcode[128];

	pos.r = 0.0f;

	oldparseidx = cmdctx.parseidx;
	if (cmd_get_int_param(&cmdctx, &x) &&
		cmd_get_int_param(&cmdctx, &y) &&
		cmd_get_int_param(&cmdctx, &z))
	{
		/*TODO: accept interior id as param*/
		pos.coords.x = (float) x;
		pos.coords.y = (float) y;
		pos.coords.z = (float) z;
	} else {
		cmdctx.parseidx = oldparseidx;

		if (cmd_get_str_param(&cmdctx, input_apcode) &&
			!input_apcode[4] && input_apcode[3] && input_apcode[2] && input_apcode[1] && input_apcode[0])
		{
			*((int*) &input_apcode) &= ~0x20202020;
			for (i = 0; i < numairports; i++) {
				if (!strcmp(airports[i].code, input_apcode)) {
					pos.coords = airports[i].pos;
					pos.coords.z += 1.0f;
					goto havecoords;
				}
			}
		}

		cmdctx.parseidx = oldparseidx;

		if (cmd_get_player_param(&cmdctx, &targetplayerid)) {
			if (targetplayerid == INVALID_PLAYER_ID) {
				SendClientMessage(cmdctx.playerid, COL_WARN, "Target player is offline");
				return CMD_OK;
			}
			GetPlayerPos(targetplayerid, &pos.coords);
			/*TODO: same interior*/
			pos.coords.x += 0.3f;
			pos.coords.y += 0.3f;
		} else {
			return CMD_SYNTAX_ERR;
		}
	}

havecoords:
	if (GetPlayerState(cmdctx.playerid) == PLAYER_STATE_DRIVER) {
		common_SetVehiclePos(GetPlayerVehicleID(cmdctx.playerid), &pos.coords);
	} else {
		common_tp_player(cmdctx.playerid, pos);
	}

	return CMD_OK;
}

#define CMD__GOTOREL_SYNTAX "[x] [y] [z]"
#define CMD__GOTOREL_DESC "Teleport relatively to your current position"
static
int cmd__gotorel(struct COMMANDCONTEXT cmdctx)
{
	int x, y, z;
	struct vec4 pos;

	GetPlayerPosRot(cmdctx.playerid, &pos);
	x = y = z = 0;
	if (cmd_get_int_param(&cmdctx, &x) && cmd_get_int_param(&cmdctx, &y)) {
		cmd_get_int_param(&cmdctx, &z);
	}
	pos.coords.x += (float) x;
	pos.coords.y += (float) y;
	pos.coords.z += (float) z;

	if (GetPlayerState(cmdctx.playerid) == PLAYER_STATE_DRIVER) {
		common_SetVehiclePos(GetPlayerVehicleID(cmdctx.playerid), &pos.coords);
	} else {
		common_tp_player(cmdctx.playerid, pos);
	}

	return CMD_OK;
}

#define CMD__MANPPVH_SYNTAX "<modelid|modelname>"
#define CMD__MANPPVH_DESC "Creates a new permanent public vehicle"
/**
It's that long because creating vehicles is no joke and there should be
sufficient self-reflection time while typing the command to be sure that
the user really wants to create a new permanent public vehicle.
*/
static
int cmd__makeanewpermanentpublicvehiclehere(struct COMMANDCONTEXT cmdctx)
{
	struct dbvehicle *veh;
	struct vec4 ppos;
	int modelid = -1, vehicleid;

	if (cmd_get_vehiclemodel_param(&cmdctx, &modelid)) {
		GetPlayerPosRot(cmdctx.playerid, &ppos);
		veh = veh_create_new_dbvehicle(modelid, &ppos);
		vehicleid = veh_create(veh);
		if (vehicleid != INVALID_VEHICLE_ID) {
			natives_PutPlayerInVehicle(cmdctx.playerid, veh->spawnedvehicleid, 0);
		}
		return CMD_OK;
	}
	return CMD_SYNTAX_ERR;
}

#define CMD__RESPAWN_SYNTAX ""
#define CMD__RESPAWN_DESC "Respawns the vehicle you're in"
static
int cmd__respawn(struct COMMANDCONTEXT cmdctx)
{
	int vehicleid;

	vehicleid = GetPlayerVehicleID(cmdctx.playerid);
	if (vehicleid) {
		NC_SetVehicleToRespawn(vehicleid);
	}
	return CMD_OK;
}

#define CMD__RR_SYNTAX ""
#define CMD__RR_DESC "Respawns all unoccupied vehicles in a close range"
static
int cmd__rr(struct COMMANDCONTEXT cmdctx)
{
	static const float RR_SQ = 150.0f * 150.0f;

	struct vec3 ppos, oppos;
	int i, vehicleid;
	/*MAX_PLAYERS because there can be as much occupied vehicles
	as there are players*/
	int occupied_vehicles[MAX_PLAYERS];
	int numov = 0;

	GetPlayerPos(cmdctx.playerid, &ppos);
	for (i = 0; i < playercount; i++) {
		vehicleid = GetPlayerVehicleID(players[i]);
		if (vehicleid) {
			GetPlayerPos(players[i], &oppos);
			if (common_xy_dist_sq(ppos, oppos) < RR_SQ) {
				occupied_vehicles[numov++] = vehicleid;
			}
		}
	}
	for (vehicleid = NC_GetVehiclePoolSize(); vehicleid >= 0; vehicleid--) {
		for (i = 0; i < numov; i++) {
			if (vehicleid == occupied_vehicles[i]) {
				goto skip_occupied;
			}
		}
		if (GetVehiclePos(vehicleid, &oppos) && common_xy_dist_sq(ppos, oppos) < RR_SQ) {
			NC_SetVehicleToRespawn(vehicleid);
		}
skip_occupied:
		;
	}
	return CMD_OK;
}

#define CMD__STREAMDISTANCE_SYNTAX "[distance]"
#define CMD__STREAMDISTANCE_DESC "Changes stream_distance setting"
static
int cmd__streamdistance(struct COMMANDCONTEXT cmdctx)
{
	int newdistance;
	float *stream_distance;
	char msg144[144];

	stream_distance = samphost_GetPtrStreamDistance();
	if (cmd_get_int_param(&cmdctx, &newdistance)) {
		*stream_distance = (float) newdistance;
	}
	sprintf(msg144, "stream_distance=%.0f", *stream_distance);
	SendClientMessage(cmdctx.playerid, -1, msg144);
	return CMD_OK;
}

#define CMD__TOCAR_SYNTAX "<vehicleid|modelname> [seat] [jack:1/0]"
#define CMD__TOCAR_DESC "Teleports into closest unoccupied car of given model/id"
static
int cmd__tocar(struct COMMANDCONTEXT cmdctx)
{
	struct vec3 vehicle_pos, player_pos;
	int vehicleid, closest_vehicleid, modelid, seat, jack;
	float dx, dy;
	float min_distance_sq;

	seat = 0;
	if (cmd_get_int_param(&cmdctx, &vehicleid)) {
		cmd_get_int_param(&cmdctx, &seat);
		if (GetVehicleModel(vehicleid)) {
			natives_PutPlayerInVehicle(cmdctx.playerid, vehicleid, seat);
		} else {
			SendClientMessage(cmdctx.playerid, COL_WARN, WARN"Invalid vehicle id");
		}
	} else if (cmd_get_vehiclemodel_param(&cmdctx, &modelid)) {
		jack = 0;
		if (cmd_get_int_param(&cmdctx, &seat)) {
			cmd_get_int_param(&cmdctx, &jack);
		}
		closest_vehicleid = -1;
		min_distance_sq = float_pinf;
		GetPlayerPos(cmdctx.playerid, &player_pos);
		for (vehicleid = NC_GetVehiclePoolSize(); vehicleid >= 0; vehicleid--) {
			if (GetVehicleModel(vehicleid) == modelid) {
				GetVehiclePosUnsafe(vehicleid, &vehicle_pos);
				dx = player_pos.x - vehicle_pos.x;
				dy = player_pos.y - vehicle_pos.y;
				if (dx * dx + dy * dy < min_distance_sq &&
					(jack || GetPlayerInVehicleSeat(vehicleid, seat) == INVALID_PLAYER_ID))
				{
					min_distance_sq = dx * dx + dy * dy;
					closest_vehicleid = vehicleid;
				}
			}
		}
		if (closest_vehicleid != -1) {
			if (seat == 0 && GetPlayerInVehicleSeat(closest_vehicleid, 0) != INVALID_PLAYER_ID) {
				SendClientMessage(cmdctx.playerid, COL_INFO, INFO"Trying to jack existing driver (won't work)");
			}
			natives_PutPlayerInVehicle(cmdctx.playerid, closest_vehicleid, seat);
		} else {
			SendClientMessage(cmdctx.playerid, COL_WARN, WARN"None spawned or available");
		}
	} else {
		return CMD_SYNTAX_ERR;
	}
	return CMD_OK;
}

#define CMD__TOMSP_SYNTAX "[id]"
#define CMD__TOMSP_DESC "Teleports to mission point with given id or closest for matching vehicle"
static
int cmd__tomsp(struct COMMANDCONTEXT cmdctx)
{
	struct vec3 *closest_pos, player_pos;
	int mspid;
	float dx, dy;
	float closest_distance_sq;
	int vehicleid;
	int msptype_mask;
	int mspindex;

	vehicleid = GetPlayerVehicleID(cmdctx.playerid);
	if (vehicleid) {
		msptype_mask = missions_get_vehicle_model_msptype_mask(GetVehicleModel(vehicleid));
	} else {
		msptype_mask = -1;
	}

	closest_pos = NULL;
	if (cmd_get_int_param(&cmdctx, &mspid)) {
		for (mspindex = 0; mspindex < nummissionpoints; mspindex++) {
			if (missionpoints[mspindex].id == mspid) {
				closest_pos = &missionpoints[mspindex].pos;
				break;
			}
		}
	} else {
		closest_distance_sq = float_pinf;
		GetPlayerPos(cmdctx.playerid, &player_pos);
		for (mspindex = 0; mspindex < nummissionpoints; mspindex++) {
			if (missionpoints[mspindex].type & msptype_mask) {
				dx = player_pos.x - missionpoints[mspindex].pos.x;
				dy = player_pos.y - missionpoints[mspindex].pos.y;
				if (dx * dx + dy * dy < closest_distance_sq) {
					closest_distance_sq = dx * dx + dy * dy;
					closest_pos = &missionpoints[mspindex].pos;
				}
			}
		}
	}

	if (closest_pos) {
		if (vehicleid) {
			common_SetVehiclePos(vehicleid, closest_pos);
		} else {
			natives_SetPlayerPos(cmdctx.playerid, *closest_pos);
		}
	} else {
		SendClientMessage(cmdctx.playerid, COL_WARN, WARN"Can't find compatible mission point");
	}

	return CMD_OK;
}

#define CMD__VEHINFO_SYNTAX ""
#define CMD__VEHINFO_DESC "Shows vehicle info, allows editing some state (enabled/linked airport)"
static
int cmd__vehinfo(struct COMMANDCONTEXT cmdctx)
{
	admin_engage_vehinfo_dialog(cmdctx.playerid);
	return CMD_OK;
}

static
int handle_cmd_at400_androm(struct COMMANDCONTEXT cmdctx, int vehiclemodel)
{
	struct vec3 playerpos, vehiclepos;
	int vehicleid, found_vehicle;
	float dx, dy, dz, shortest_distance, tmpdistance;

	vehicleid = GetPlayerVehicleID(cmdctx.playerid);
	if (vehicleid) {
		return CMD_OK;
	}

	found_vehicle = 0;
	shortest_distance = float_pinf;
	GetPlayerPos(cmdctx.playerid, &playerpos);
	for (vehicleid = NC_GetVehiclePoolSize(); vehicleid >= 0; vehicleid--) {
		if (GetVehicleModel(vehicleid) == vehiclemodel &&
			IsVehicleStreamedIn(vehicleid, cmdctx.playerid) &&
			veh_is_player_allowed_in_vehicle(cmdctx.playerid, gamevehicles[vehicleid].dbvehicle))
		{
			GetVehiclePosUnsafe(vehicleid, &vehiclepos);
			dx = vehiclepos.x - playerpos.x;
			dy = vehiclepos.y - playerpos.y;
			dz = vehiclepos.z - playerpos.z;
			tmpdistance = dx * dx + dy * dy + dz * dz;
			if (tmpdistance < shortest_distance && GetVehicleDriver(vehicleid) == INVALID_PLAYER_ID) {
				shortest_distance = tmpdistance;
				found_vehicle = vehicleid;
			}
		}
	}
	if (found_vehicle && shortest_distance < 22.0f * 22.0f) {
		natives_PutPlayerInVehicle(cmdctx.playerid, found_vehicle, 0);
	}

	return CMD_OK;
}

#define CMD_ADF_SYNTAX "[beacon] - see /beacons or /nearest"
#define CMD_ADF_DESC "Engages ADF navigation to given beacon, or disables nav when no beacon given"
static
int cmd_adf(struct COMMANDCONTEXT cmdctx)
{
	struct AIRPORT *ap;
	int vehicleid, len;
	char beacon[144], *bp;

	if (!nav_check_can_do_cmd(cmdctx.playerid, NAV_ADF, &vehicleid)) {
		return CMD_OK;
	}

	if (!cmd_get_str_param(&cmdctx, beacon)) {
		if (nav[vehicleid] != NULL) {
			nav_disable(vehicleid);
			PlayerPlaySound(cmdctx.playerid, NAV_DEL_SOUND);
			return CMD_OK;
		}
		return CMD_SYNTAX_ERR;
	}

	bp = beacon;
	len = 0;
	while (1) {
		if (*bp == 0) {
			break;
		}
		*bp &= ~0x20;
		if (++len >= 5 || *bp < 'A' || 'Z' < *bp) {
			goto unkbeacon;
		}
		bp++;
	}

	ap = airports;
	len = numairports;
	while (len--) {
		if (strcmp(beacon, ap->code) == 0) {
			nav_enable(vehicleid, &ap->pos, NULL);
			PlayerPlaySound(cmdctx.playerid, NAV_SET_SOUND);
			return 1;
		}
		ap++;
	}
unkbeacon:
	SendClientMessage(cmdctx.playerid, COL_WARN, WARN"Unknown beacon - see /beacons or /nearest");
	return CMD_OK;
}

#define CMD_ANDROM_SYNTAX ""
#define CMD_ANDROM_DESC "Teleports into a nearby andromada as driver"
static
int cmd_androm(struct COMMANDCONTEXT cmdctx)
{
	return handle_cmd_at400_androm(cmdctx, MODEL_ANDROM);
}

#define CMD_AT400_SYNTAX ""
#define CMD_AT400_DESC "Teleports into a nearby andromada as driver"
static
int cmd_at400(struct COMMANDCONTEXT cmdctx)
{
	return handle_cmd_at400_androm(cmdctx, MODEL_AT400);
}

#define CMD_BEACONS_SYNTAX ""
#define CMD_BEACONS_DESC "Shows all airport beacons, to use in nav commands"
static
int cmd_beacons(struct COMMANDCONTEXT cmdctx)
{
	struct DIALOG_INFO dialog;
	char *info;
	struct AIRPORT *ap = airports;
	int count = numairports;

	dialog_init_info(&dialog);
	if (numairports) {
		info = dialog.info;
		while (count-- > 0) {
			if (ap->enabled) {
				info += sprintf(info, " %s", ap->code);
			}
			ap++;
		}
	} else {
		strcpy(dialog.info, " None!");
	}
	dialog.transactionid = DLG_TID_AIRPORT_BEACONS;
	dialog.caption = "Beacons";
	dialog.button1 = "Close";
	dialog_show(cmdctx.playerid, &dialog);
	return CMD_OK;
}

#define CMD_CAMERA_SYNTAX ""
#define CMD_CAMERA_DESC "Gives you a camera"
static
int cmd_camera(struct COMMANDCONTEXT cmdctx)
{
	NC_GivePlayerWeapon(cmdctx.playerid, WEAPON_CAMERA, 3036);
	return CMD_OK;
}

#define CMD_CHANGELOG_SYNTAX ""
#define CMD_CHANGELOG_DESC "Shows the changelog of the server"
static
int cmd_changelog(struct COMMANDCONTEXT cmdctx)
{
	changelog_show_dialog(cmdctx.playerid, 0);
	return CMD_OK;
}

#define CMD_CHANGEPASSWORD_SYNTAX ""
#define CMD_CHANGEPASSWORD_DESC "Allows you to change your account password"
static
int cmd_changepassword(struct COMMANDCONTEXT cmdctx)
{
	chpw_engage(cmdctx.playerid);
	return CMD_OK;
}

#define CMD_CHUTE_SYNTAX ""
#define CMD_CHUTE_DESC "Gives you a parachute"
static
int cmd_chute(struct COMMANDCONTEXT cmdctx)
{
	NC_GivePlayerWeapon(cmdctx.playerid, WEAPON_PARACHUTE, 1);
	return CMD_OK;
}

#define CMD_CLOSE_SYNTAX ""
#define CMD_CLOSE_DESC "Closes the current open help screen"
static
int cmd_close(struct COMMANDCONTEXT cmdctx)
{
	help_hide(cmdctx.playerid);
	return CMD_OK;
}

static void cmd_engage_cmdlist(int playerid);

#define CMD_CMDS_SYNTAX ""
#define CMD_CMDS_DESC "Shows all commands available to you"
static
int cmd_cmds(struct COMMANDCONTEXT cmdctx)
{
	cmd_engage_cmdlist(cmdctx.playerid);
	return CMD_OK;
}

#define CMD_ENGINE_SYNTAX ""
#define CMD_ENGINE_DESC "Toggles the engine of your vehicle"
static
int cmd_engine(struct COMMANDCONTEXT cmdctx)
{
	if (GetPlayerState(cmdctx.playerid) == PLAYER_STATE_DRIVER) {
		veh_start_or_stop_engine(cmdctx.playerid, GetPlayerVehicleID(cmdctx.playerid));
	}
	return CMD_OK;
}

#define CMD_GETSPRAY_SYNTAX ""
#define CMD_GETSPRAY_DESC "Gets the colors of the vehicle you're in"
static
int cmd_getspray(struct COMMANDCONTEXT cmdctx)
{
	int vehicleid, col1, col2;
	char msg144[144];

	vehicleid = GetPlayerVehicleID(cmdctx.playerid);
	if (vehicleid && GetVehicleColor(vehicleid, &col1, &col2)) {
		sprintf(msg144, "colors: %d, %d", col1, col2);
		SendClientMessage(cmdctx.playerid, -1, msg144);
	}
	return CMD_OK;
}

#define CMD_HELP_SYNTAX ""
#define CMD_HELP_DESC "Shows you help pages"
static
int cmd_help(struct COMMANDCONTEXT cmdctx)
{
	if (ui_try_show(cmdctx.playerid, ui_help)) {
		help_show(cmdctx.playerid, NUM_HELPMAIN_TDS, tds_helpmain);
	}
	return CMD_OK;
}

#define CMD_HELPADF_SYNTAX ""
#define CMD_HELPADF_DESC "Shows you help about ADF"
static
int cmd_helpadf(struct COMMANDCONTEXT cmdctx)
{
	if (ui_try_show(cmdctx.playerid, ui_help)) {
		help_show(cmdctx.playerid, NUM_HELPADF_TDS, tds_helpadf);
	}
	return CMD_OK;
}

static void cmd_show_help_for(int,char*);

#define CMD_HELPCMD_SYNTAX "<cmdname>"
#define CMD_HELPCMD_DESC "Shows info about a command"
static
int cmd_helpcmd(struct COMMANDCONTEXT cmdctx)
{
	char cmdname_buf[144];

	if (!cmd_get_str_param(&cmdctx, cmdname_buf + 1)) {
		return CMD_SYNTAX_ERR;
	}

	if (cmdname_buf[1] == '/') {
		cmd_show_help_for(cmdctx.playerid, cmdname_buf + 1);
	} else {
		cmdname_buf[0] = '/';
		cmd_show_help_for(cmdctx.playerid, cmdname_buf);
	}
	return CMD_OK;
}

#define CMD_HELPKEYS_SYNTAX ""
#define CMD_HELPKEYS_DESC "Shows some important key bindings"
static
int cmd_helpkeys(struct COMMANDCONTEXT cmdctx)
{
	GameTextForPlayer(cmdctx.playerid, 5000, 3,
		"~w~start/stop engine: ~b~~k~~CONVERSATION_NO~~n~"
		"~w~landing gear: ~b~~k~~TOGGLE_SUBMISSIONS~~n~"
		"~w~enter copilot mode: ~b~~k~~PED_FIREWEAPON~~n~"
		"~w~exit copilot mode: ~b~~k~~VEHICLE_BRAKE~");
	return CMD_OK;
}

#define CMD_ILS_SYNTAX ""
#define CMD_ILS_DESC "Toggles ILS, only when VOR is already active"
static
int cmd_ils(struct COMMANDCONTEXT cmdctx)
{
	struct vec4 vpos;
	struct vec3 vvel;
	struct NAVDATA *np;
	int vehicleid;

	if (!nav_check_can_do_cmd(cmdctx.playerid, NAV_ILS, &vehicleid)) {
		return CMD_OK;
	}
	if ((np = nav[vehicleid]) == NULL || np->vor == NULL) {
		SendClientMessage(cmdctx.playerid, COL_WARN, WARN"ILS can only be activated when VOR is already active");
		return CMD_OK;
	}
	if ((np->vor->nav & NAV_ILS) != NAV_ILS || !np->vor->other_end) {
		SendClientMessage(cmdctx.playerid, COL_WARN, WARN"The selected runway does not have ILS capabilities");
		return CMD_OK;
	}
	np->ilsx = np->ilsz = INVALID_ILS_VALUE;
	np->ils = !np->ils;
	PlayerPlaySound(cmdctx.playerid, np->ils ? NAV_SET_SOUND : NAV_DEL_SOUND);

	GetVehiclePosRotUnsafe(vehicleid, &vpos);
	GetVehicleVelocityUnsafe(vehicleid, &vvel);
	nav_update(vehicleid, &vpos, &vvel);
	panel_nav_updated(vehicleid);
	return CMD_OK;
}

#define CMD_IRC_SYNTAX ""
#define CMD_IRC_DESC "Shows information about the IRC bridge"
static
int cmd_irc(struct COMMANDCONTEXT cmdctx)
{
	echo_request_status_for_player(cmdctx.playerid);
	return CMD_OK;
}

#define CMD_LOC_SYNTAX "<playerid|(partial)name>"
#define CMD_LOC_DESC "Locates a player"
static
int cmd_loc(struct COMMANDCONTEXT cmdctx)
{
	int target, vehicleid, model;
	struct vec3 pos;
	struct vec3 vvel;
	char buf[144], *b;

	if (!cmd_get_player_param(&cmdctx, &target)) {
		return CMD_SYNTAX_ERR;
	}
	if (target == INVALID_PLAYER_ID) {
		SendClientMessage(cmdctx.playerid, COL_WARN, WARN"That player is not online.");
		return CMD_OK;
	}
	GetPlayerPos(target, &pos);
	zones_update(target, pos);

	b = buf;
	b += sprintf(buf, "%s(%d) is located in ",
		pdata[target]->name, target);
	if (zone_last_id[target] >= 0) {
		b += sprintf(b, "%s, ", zonenames[zone_last_id[target]]);
	}
	b += sprintf(b, "%s ", zonenames[zone_last_region[target]]);
	vehicleid = GetPlayerVehicleID(target);
	if (vehicleid) {
		model = GetVehicleModel(vehicleid);
		GetVehicleVelocityUnsafe(vehicleid, &vvel);
		sprintf(b, "travelling at %.0f KPH in a %s (%.0f FT)",
			VEL_TO_KPH * sqrt(vvel.x * vvel.x + vvel.y * vvel.y + vvel.z * vvel.z),
			vehnames[model - 400],
			pos.z);
	} else {
		sprintf(b, "on foot (%.0f FT)", pos.z);
	}

	SendClientMessage(cmdctx.playerid, COL_INFO_GENERIC, buf);
	return CMD_OK;
}

#define CMD_NEAREST_SYNTAX ""
#define CMD_NEAREST_DESC "Shows nearest airports"
static
int cmd_nearest(struct COMMANDCONTEXT cmdctx)
{
	airport_show_nearest_dialog(cmdctx.playerid);
	return CMD_OK;
}

#define CMD_ME_SYNTAX "<actiontext>"
#define CMD_ME_DESC "Sends an action message"
static
int cmd_me(struct COMMANDCONTEXT cmdctx)
{
	char *to, *from;
	int hascontent = 0;
	char msg[150 + MAX_PLAYER_NAME];

	from = cmdctx.cmdtext + 3;
	to = msg + sprintf(msg, "* %s", pdata[cmdctx.playerid]->name);
	while ((*to = *from)) {
		if (*from != ' ') {
			hascontent = 1;
		}
		from++;
		to++;
	}
	if (hascontent) {
		SendClientMessageToAll(COL_IRC_ACTION, msg);
		echo_on_game_chat_or_action(1, cmdctx.playerid, cmdctx.cmdtext + 4);
	} else {
		return CMD_SYNTAX_ERR;
	}
	return CMD_OK;
}

#define CMD_METAR_SYNTAX ""
#define CMD_METAR_DESC "Shows information about the current weather"
static
int cmd_metar(struct COMMANDCONTEXT cmdctx)
{
	char msg144[144];

	if (weather.upcoming != weather.current) {
		timecyc_fmt_metar_msg(msg144, "forecast", weather.upcoming);
	} else {
		timecyc_fmt_metar_msg(msg144, "report", weather.current);
	}
	SendClientMessage(cmdctx.playerid, COL_METAR, msg144);
	return CMD_OK;
}

#define CMD_PARK_SYNTAX ""
#define CMD_PARK_DESC "Changes the saved/parked/spawn position of the vehicle you're in"
static
int cmd_park(struct COMMANDCONTEXT cmdctx)
{
	struct dbvehicle *veh;
	int vehicleid;
	float lastrot;

	vehicleid = GetPlayerVehicleID(cmdctx.playerid);
	if (vehicleid) {
		veh = gamevehicles[vehicleid].dbvehicle;
		if (!veh_can_player_modify_veh(cmdctx.playerid, veh)) {
			SendClientMessage(cmdctx.playerid, COL_WARN, WARN"You are not allowed to park this vehicle");
			return CMD_OK;
		}
		lastrot = veh->pos.r;
		GetVehiclePosRotUnsafe(vehicleid, &veh->pos);
		if (356.0f < veh->pos.r || veh->pos.r < 4.0f) {
			veh->pos.r = 0.0f;
		} else if (86.0f < veh->pos.r && veh->pos.r < 94.0f) {
			veh->pos.r = 90.0f;
		} else if (176.0f < veh->pos.r && veh->pos.r < 184.0f) {
			veh->pos.r = 180.0f;
		} else if (266.0f < veh->pos.r && veh->pos.r < 274.0f) {
			veh->pos.r = 270.0f;
		}
		if (fabs(lastrot - veh->pos.r) > 3.0f) {
			gamevehicles[vehicleid].need_recreation = 1;
		}
		csprintf(buf144,
			"UPDATE veh SET x=%f,y=%f,z=%f,r=%f WHERE i=%d",
			veh->pos.coords.x,
			veh->pos.coords.y,
			veh->pos.coords.z,
			veh->pos.r,
			veh->id);
		NC_mysql_tquery_nocb(buf144a);
		SendClientMessage(cmdctx.playerid, COL_SUCC, SUCC"Vehicle parked!");
	}
	return CMD_OK;
}

#define CMD_PM_SYNTAX "<playerid|(partial)name> <message>"
#define CMD_PM_DESC "Send a private message to a player"
static
int cmd_pm(struct COMMANDCONTEXT cmdctx)
{
	int targetid;

	if (!cmd_get_player_param(&cmdctx, &targetid)) {
		return CMD_SYNTAX_ERR;
	}
	if (targetid == INVALID_PLAYER_ID) {
		SendClientMessage(cmdctx.playerid, COL_WARN, WARN"That player is not online.");
		return CMD_OK;
	}
	while (cmdctx.cmdtext[cmdctx.parseidx] == ' ') {
		cmdctx.parseidx++;
	}
	if (cmdctx.cmdtext[cmdctx.parseidx]) {
		pm_send(cmdctx.playerid, targetid, cmdctx.cmdtext + cmdctx.parseidx);
		return CMD_OK;
	}
	return CMD_SYNTAX_ERR;
}

#define CMD_PREFS_SYNTAX ""
#define CMD_PREFS_DESC "Allows you to change your preferences"
static
int cmd_prefs(struct COMMANDCONTEXT cmdctx)
{
	prefs_show_dialog(cmdctx.playerid);
	return CMD_OK;
}

#define CMD_PROTIP_SYNTAX ""
#define CMD_PROTIP_DESC "Sends a random protip"
static
int cmd_protip(struct COMMANDCONTEXT cmdctx)
{
	SendClientMessage(cmdctx.playerid, COL_INFO_LIGHT, protips_get_random_protip());
	return CMD_OK;
}


#define CMD_R_SYNTAX "<message>"
#define CMD_R_DESC "Sends a private message reply to the player that last sent a pm to you"
static
int cmd_r(struct COMMANDCONTEXT cmdctx)
{
	while (cmdctx.cmdtext[cmdctx.parseidx] == ' ') {
		cmdctx.parseidx++;
	}

	if (cmdctx.cmdtext[cmdctx.parseidx]) {
		switch (lastpmtarget[cmdctx.playerid]) {
		case LAST_PMTARGET_NOBODY:
			SendClientMessage(cmdctx.playerid, COL_WARN, WARN"Nobody has sent you a PM yet!");
			return CMD_OK;
		case LAST_PMTARGET_INVALID:
			SendClientMessage(cmdctx.playerid, COL_WARN, WARN"The person who last sent you a PM left");
			return CMD_OK;
		default:
			pm_send(cmdctx.playerid, lastpmtarget[cmdctx.playerid], cmdctx.cmdtext + cmdctx.parseidx);
			return CMD_OK;
		}
	}
	return CMD_SYNTAX_ERR;
}

#define NO_RECLASSSPAWN WARN"You cannot reclass/respawn while on a mission. "\
			"Use /s to cancel your current mission for a fee "\
			"($"EQ(MISSION_CANCEL_FINE)")."

#define CMD_RECLASS_SYNTAX ""
#define CMD_RECLASS_DESC "Brings you to class selection to change your class (pilot/army etc)"
static
int cmd_reclass(struct COMMANDCONTEXT cmdctx)
{
	if (missions_is_player_on_mission(cmdctx.playerid)) {
		SendClientMessage(cmdctx.playerid, COL_WARN, NO_RECLASSSPAWN);
	} else {
		spawned[cmdctx.playerid] = 0;
		NC_ForceClassSelection(cmdctx.playerid);
		NC_TogglePlayerSpectating(cmdctx.playerid, 1);
		nc_params[2] = 0;
		NC(n_TogglePlayerSpectating);
	}
	return CMD_OK;
}

#define CMD_REGISTER_SYNTAX ""
#define CMD_REGISTER_DESC "Allows guests to register their account"
static
int cmd_register(struct COMMANDCONTEXT cmdctx)
{
	if (sessionid[cmdctx.playerid] == -1 || userid[cmdctx.playerid] == -1) {
		SendClientMessage(cmdctx.playerid, COL_WARN,
			WARN"Your guest session is not linked to a guest account. "
			"Please reconnect if you want to register.");
	} else {
		guestreg_show_dialog_namechange(cmdctx.playerid, 0, 0);
	}
	return CMD_OK;
}


#define CMD_REPAIR_SYNTAX ""
#define CMD_REPAIR_DESC "Fixes the vehicle when in range of a service point"
static
int cmd_repair(struct COMMANDCONTEXT cmdctx)
{
	svp_repair(cmdctx.playerid);
	return CMD_OK;
}

#define CMD_REFUEL_SYNTAX ""
#define CMD_REFUEL_DESC "Refuels the vehicle when in range of a service point"
static
int cmd_refuel(struct COMMANDCONTEXT cmdctx)
{
	svp_refuel(cmdctx.playerid);
	return CMD_OK;
}

#define CMD_RESPAWN_SYNTAX ""
#define CMD_RESPAWN_DESC "Respawns you"
static
int cmd_respawn(struct COMMANDCONTEXT cmdctx)
{
	if (missions_is_player_on_mission(cmdctx.playerid)) {
		SendClientMessage(cmdctx.playerid, COL_WARN, NO_RECLASSSPAWN);
	} else {
		natives_SpawnPlayer(cmdctx.playerid);
	}
	return CMD_OK;
}

#define CMD_S_SYNTAX ""
#define CMD_S_DESC "Cancels your current mission for $"EQ(MISSION_CANCEL_FINE)""
static
int cmd_s(struct COMMANDCONTEXT cmdctx)
{
	missions_process_cancel_request_by_player(cmdctx.playerid);
	return CMD_OK;
}

#define CMD_SPRAY_SYNTAX "[col1] [col2]"
#define CMD_SPRAY_DESC "Respray your vehicle. Colors may be omitted to use a fully random color, or use -1 for a normal random color fit for the vehicle."
static
int cmd_spray(struct COMMANDCONTEXT cmdctx)
{
	struct dbvehicle *veh;
	int vehicleid, col1, col2, *a, *b;

	vehicleid = GetPlayerVehicleID(cmdctx.playerid);
	if (vehicleid) {
		veh = gamevehicles[vehicleid].dbvehicle;
		if (!veh_can_player_modify_veh(cmdctx.playerid, veh)) {
			SendClientMessage(cmdctx.playerid, COL_WARN, WARN"You are not allowed to respray this vehicle");
			return CMD_OK;
		}
		a = b = NULL;
		if (cmd_get_int_param(&cmdctx, &col1)) {
			if (col1 == -1) {
				a = &col1;
			}
			if (!cmd_get_int_param(&cmdctx, &col2)) {
				goto rand2nd;
			}
			if (col2 == -1) {
				b = &col2;
			}
		} else {
			col1 = NC_random(256);
rand2nd:
			col2 = NC_random(256);
		}
		if (a != NULL || b != NULL) {
			game_random_carcol(GetVehicleModel(vehicleid), a, b);
		}
		SetVehicleColor(vehicleid, col1, col2);
		if (veh != NULL) {
			veh->col1 = (unsigned char) col1;
			veh->col2 = (unsigned char) col2;
			csprintf(buf144,
				"UPDATE veh SET col1=%d,col2=%d WHERE i=%d",
				veh->col1,
				veh->col2,
				veh->id);
			NC_mysql_tquery_nocb(buf144a);
		}
	}
	return CMD_OK;
}

#define CMD_STOPLOCATE_SYNTAX ""
#define CMD_STOPLOCATE_DESC "Stops locating a mission point"
static
int cmd_stoplocate(struct COMMANDCONTEXT cmdctx)
{
	missions_stoplocate(cmdctx.playerid);
	return CMD_OK;
}

#define CMD_TICKRATE_SYNTAX ""
#define CMD_TICKRATE_DESC "Shows the server's tickrate"
static
int cmd_tickrate(struct COMMANDCONTEXT cmdctx)
{
	char msg144[144];

	sprintf(msg144, "%d", (int) NC_GetServerTickRate());
	SendClientMessage(cmdctx.playerid, -1, msg144);
	return CMD_OK;
}

#define CMD_VOR_SYNTAX "[beacon][runway] - see /nearest or /beacons"
#define CMD_VOR_DESC "Engages VOR navigation to given beacon+runway, or disables nav when none given"
static
int cmd_vor(struct COMMANDCONTEXT cmdctx)
{
	struct AIRPORT *ap;
	struct RUNWAY *rw;
	int vehicleid;
	char beaconpar[144], runwaypar[144], beacon[5], *b, *bp;
	int combinedparameter;
	int len;

	if (!nav_check_can_do_cmd(cmdctx.playerid, NAV_VOR, &vehicleid)) {
		return CMD_OK;
	}

	if (!cmd_get_str_param(&cmdctx, beaconpar)) {
		if (nav[vehicleid] != NULL) {
			nav_disable(vehicleid);
			PlayerPlaySound(cmdctx.playerid, NAV_DEL_SOUND);
			return CMD_OK;
		}
		return CMD_SYNTAX_ERR;
	}
	combinedparameter = !cmd_get_str_param(&cmdctx, runwaypar);

	b = beaconpar;
	bp = beacon;
	len = 0;
	while (1) {
		if ((*bp = (*b & ~0x20)) == 0) {
			if (!combinedparameter) {
				b = runwaypar;
			}
			goto havebeacon;
		}
		if (++len >= 5) {
			if (combinedparameter) {
				*bp = 0;
				goto havebeacon;
			}
			goto unkbeacon;
		}
		if (*bp < 'A' || 'Z' < *bp) {
			if (combinedparameter) {
				*bp = 0;
				goto havebeacon;
			}
			goto unkbeacon;
		}
		bp++;
		b++;
	}
havebeacon:
	ap = airports;
	len = numairports;
	while (len--) {
		if (strcmp(beacon, ap->code) == 0) {
			goto haveairport;
		}
		ap++;
	}
unkbeacon:
	SendClientMessage(cmdctx.playerid, COL_WARN, WARN"Unknown beacon - see /beacons or /nearest");
	return 1;
haveairport:
	if (b[0] < '0' || '9' < b[0] ||
		b[1] < '0' || '9' < b[1] ||
		(b[2] != 0 && (b[2] = b[2] & ~0x20) != 'L' &&
			b[2] != 'R' && b[2] != 'C' && b[3] != 0))
	{
		goto tellrws;
	}

	rw = ap->runways;
	while (rw != ap->runwaysend) {
		if (rw->type == RUNWAY_TYPE_RUNWAY && strcmp(rw->id, b) == 0) {
			nav_enable(vehicleid, NULL, rw);
			PlayerPlaySound(cmdctx.playerid, NAV_SET_SOUND);
			return 1;
		}
		rw++;
	}

tellrws:
	len = 0;
	b = beaconpar; /*reuse a buffer*/
	b += sprintf(b, WARN"Unknown runway, try one of:");
	rw = ap->runways;
	while (rw != ap->runwaysend) {
		if (rw->nav & NAV_VOR) {
			b += sprintf(b, " %s", rw->id);
			len++;
		}
		rw++;
	}
	if (len) {
		SendClientMessage(cmdctx.playerid, COL_WARN, beaconpar);
		return 1;
	}

	SendClientMessage(cmdctx.playerid, COL_WARN, WARN"There are no VOR capable runways at this beacon");
	return CMD_OK;
}

#define CMD_W_SYNTAX ""
#define CMD_W_DESC "Starts a mission or guides you to a mission point"
static
int cmd_w(struct COMMANDCONTEXT cmdctx)
{
	missions_engage_help_or_map(cmdctx.playerid);
	return CMD_OK;
}
