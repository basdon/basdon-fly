/**
The //getcar cmd teleports a vehicle to the player's location.
*/
static
int cmd_admin_getcar(CMDPARAMS)
{
	struct vec4 ppos;
	int vehicleid;

	if (!cmd_get_int_param(cmdtext, &parseidx, &vehicleid)) {
		B144(WARN" Syntax: //getcar <vehicleid>");
		NC_SendClientMessage(playerid, COL_WARN, buf144a);
	} else {
		common_GetPlayerPosRot(playerid, &ppos);
		if (common_SetVehiclePos(vehicleid, &ppos.coords)) {
			natives_PutPlayerInVehicle(playerid, vehicleid, 0);
			NC_SetVehicleZAngle(vehicleid, ppos.r);
		} else {
			B144(WARN" Invalid vehicleid");
			NC_SendClientMessage(playerid, COL_WARN, buf144a);
		}
	}
	return 1;
}

/**
The //respawn cmd respawns the vehicle the player is in.
*/
static
int cmd_admin_respawn(CMDPARAMS)
{
	int vehicleid;

	vehicleid = NC_GetPlayerVehicleID(playerid);
	if (vehicleid) {
		NC_SetVehicleToRespawn(vehicleid);
	}
	return 1;
}

/**
The //makeanewpermanentpublicvehiclehere creates a new public vehicle.

It's that long because creating vehicles is no joke and there should be
sufficient self-reflection time while typing the command to be sure that
the user really wants to create a new permanent public vehicle.
*/
static
int cmd_admin_makeanewpermanentpublicvehiclehere(CMDPARAMS)
{
	struct dbvehicle *veh;
	struct vec4 ppos;
	int modelid = -1, vehicleid;

	if (!cmd_get_int_param(cmdtext, &parseidx, &modelid) ||
		modelid < 400 || 400 + VEHICLE_MODEL_TOTAL <= modelid)
	{
		if (modelid == -1 &&
			cmd_get_str_param(cmdtext, &parseidx, cbuf144))
		{
			for (modelid = 0;
				modelid < VEHICLE_MODEL_TOTAL;
				modelid++)
			{
				if (!strcmp(vehmodelnames[modelid], cbuf144)) {
					modelid += 400;
					goto gotid;
				}
			}
		}
		B144(WARN"gimme a modelid or modelname");
		NC_SendClientMessage(playerid, COL_WARN, buf144a);
		return 1;
	}
gotid:

	common_GetPlayerPosRot(playerid, &ppos);
	veh = veh_create_new_dbvehicle(modelid, &ppos);
	vehicleid = veh_create(veh);
	if (vehicleid != INVALID_VEHICLE_ID) {
		natives_PutPlayerInVehicle(playerid, veh->spawnedvehicleid, 0);
	}
	return 1;
}

/**
The //rr commands respawns all unoccupied vehicles in a close range.
*/
static
int cmd_admin_rr(CMDPARAMS)
{
	static const float RR_SQ = 150.0f * 150.0f;

	struct vec3 ppos, oppos;
	int i, vehicleid;
	/*MAX_PLAYERS because there can be as much occupied vehicles
	as there are players*/
	int occupied_vehicles[MAX_PLAYERS];
	int numov = 0;

	common_GetPlayerPos(playerid, &ppos);
	for (i = 0; i < playercount; i++) {
		vehicleid = NC_GetPlayerVehicleID(players[i]);
		if (vehicleid) {
			common_GetPlayerPos(players[i], &oppos);
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
		if (common_GetVehiclePos(vehicleid, &oppos) && common_xy_dist_sq(ppos, oppos) < RR_SQ) {
			NC_SetVehicleToRespawn(vehicleid);
		}
skip_occupied:
		;
	}
	return 1;
}

static
void admin_show_vehinfo(playerid, veh, enabled, airport)
	int playerid;
	struct dbvehicle *veh;
	char *enabled;
	char *airport;
{
	char data[1024];
	
	sprintf(data,
		"Model\t%s\n"
		"Game vehicleid\t%d\n"
		"Vehicle Db Id\t%d\n"
		"Owner\t%d (%s)\n"
		"Enabled\t%s\n"
		/*change listitem in admin_dlg_vehinfo_response when these
		lines change*/
		"Linked airport\t%s\n",
		vehmodelnames[veh->model - 400],
		veh->spawnedvehicleid,
		veh->id,
		veh->owneruserid,
		(veh->owneruserid
			? veh->ownerstring + veh->ownerstringowneroffset
			: "<PUBLIC>"),
		enabled,
		airport);
	dialog_ShowPlayerDialog(playerid, DIALOG_VEHINFO_VEHINFO,
		DIALOG_STYLE_TABLIST, "vehinfo", data, "Close", "",
		-1);
}

struct CB_VEHICLEINFO
{
	int player_cc;
	struct dbvehicle *veh;
};

static
void admin_cb_vehicleinfo(void *data)
{
	struct dbvehicle *veh;
	int cc, playerid;

	cc = ((struct CB_VEHICLEINFO*) data)->player_cc;
	veh = ((struct CB_VEHICLEINFO*) data)->veh;
	free(data);
	PLAYER_CC_GET_CHECK_RETURN(cc, playerid);

	if (NC_cache_get_row_count()) {
		NC_cache_get_field_str(0, 0, buf32a);
		NC_cache_get_field_str(0, 1, buf32_1a);
		ctoai(cbuf32);
		ctoai(cbuf32_1);
		admin_show_vehinfo(playerid, veh, cbuf32, cbuf32_1);
	} else {
		admin_show_vehinfo(playerid, veh, "<err>", "<err>");
	}
}

/**
Shows //vehinfo dialog and sends query to fetch non-cached data.
*/
static
void admin_vehinfo_show(int playerid)
{
	static char* FETCHING = "<fetching...>";

	struct dbvehicle *veh;
	struct CB_VEHICLEINFO *cbdata;
	int vehicleid;
	char query[100];

	vehicleid = NC_GetPlayerVehicleID(playerid);
	veh = gamevehicles[vehicleid].dbvehicle;
	if (veh == NULL) {
		dialog_ShowPlayerDialog(playerid, DIALOG_DUMMY,
			DIALOG_STYLE_MSGBOX, "vehinfo", "unknown vehicle",
			"Close", "", -1);
	} else {
		admin_show_vehinfo(playerid, veh, FETCHING, FETCHING);
		cbdata = malloc(sizeof(struct CB_VEHICLEINFO));
		cbdata->player_cc = MK_PLAYER_CC(playerid);
		cbdata->veh = veh;
		sprintf(query,
			"SELECT IF(veh.e=1,'yes','no'),apt.n "
			"FROM veh "
			"LEFT OUTER JOIN apt ON apt.i=veh.ap "
			"WHERE veh.i=%d",
			veh->id);
		common_mysql_tquery(query, admin_cb_vehicleinfo, cbdata);
	}
}

void admin_dlg_vehinfo_response(int pid, int res, int listitem)
{
	int i;
	char *b = cbuf4096_;

	if (res && listitem == 5) {
		for (i = 0; i < numairports; i++) {
			b += sprintf(b, "%s\n", airports[i].name);	
		}
		sprintf(b, "NULL\n");
		dialog_ShowPlayerDialog(pid, DIALOG_VEHINFO_ASSIGNAP,
			DIALOG_STYLE_LIST, "assign airport", cbuf4096_,
			"Ok", "Back",
			-1);
	}
}

void admin_dlg_vehinfo_assign_response(int pid, int res, int listitem)
{
	struct dbvehicle *veh;
	char *b = cbuf4096;

	if (res) {
		veh_GetPlayerVehicle(pid, NULL, &veh);
		if (veh != NULL) {
			b += sprintf(b, "UPDATE veh SET ap=");
			if (listitem < 0 || numairports <= listitem) {
				b += sprintf(b, "NULL");
			} else {
				b += sprintf(b, "%d", airports[listitem].id);
			}
			b += sprintf(b, " WHERE i=%d", veh->id);
			atoci(buf4096, b - cbuf4096);
			NC_mysql_tquery_nocb(buf4096a);
		} else {
			B144(WARN" Invalid vehicle");
			NC_SendClientMessage(pid, COL_WARN, buf144a);
		}
	}
	admin_vehinfo_show(pid);
}

/**
The //vehinfo cmd shows vehicle info

Also allows editing enabled state and linked airport.
*/
static
int cmd_admin_vehinfo(CMDPARAMS)
{
	admin_vehinfo_show(playerid);
	return 1;
}

/*-------------------------------------------*/
