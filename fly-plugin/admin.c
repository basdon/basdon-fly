/**
The //getcar cmd teleports a vehicle to the player's location.
*/
static
int cmd_admin_getcar(struct COMMANDCONTEXT cmdctx)
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

/**
The //respawn cmd respawns the vehicle the player is in.
*/
static
int cmd_admin_respawn(struct COMMANDCONTEXT cmdctx)
{
	int vehicleid;

	vehicleid = GetPlayerVehicleID(cmdctx.playerid);
	if (vehicleid) {
		NC_SetVehicleToRespawn(vehicleid);
	}
	return CMD_OK;
}

/**
The //makeanewpermanentpublicvehiclehere creates a new public vehicle.

It's that long because creating vehicles is no joke and there should be
sufficient self-reflection time while typing the command to be sure that
the user really wants to create a new permanent public vehicle.
*/
static
int cmd_admin_makeanewpermanentpublicvehiclehere(struct COMMANDCONTEXT cmdctx)
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

/**
The //rr commands respawns all unoccupied vehicles in a close range.
*/
static
int cmd_admin_rr(struct COMMANDCONTEXT cmdctx)
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

static void admin_engage_vehinfo_dialog(int);

static
void admin_cb_dlg_vehinfo_assignap_response(int playerid, struct DIALOG_RESPONSE response)
{
	char *q;

	if (response.response) {
		q = cbuf4096;
		q += sprintf(q, "UPDATE veh SET ap=");
		if (response.listitem < 0 || numairports <= response.listitem) {
			q += sprintf(q, "NULL");
		} else {
			q += sprintf(q, "%d", airports[response.listitem].id);
		}
		q += sprintf(q, " WHERE i=%d", (int) response.data);
		atoci(buf4096, q - cbuf4096);
		NC_mysql_tquery_nocb(buf4096a);
	}
	admin_engage_vehinfo_dialog(playerid);
}

static
void admin_cb_dlg_vehinfo_response(int playerid, struct DIALOG_RESPONSE response)
{
	struct DIALOG_INFO dialog;
	int i;
	char *info;

	if (response.response && response.listitem == 5) {
		dialog_init_info(&dialog);
		info = dialog.info;
		for (i = 0; i < numairports; i++) {
			info += sprintf(info, "%s\n", airports[i].name);
		}
		sprintf(info, "NULL\n");
		dialog.transactionid = DLG_TID_ADMIN_VEHINFO_ASSIGNAP;
		dialog.style = DIALOG_STYLE_LIST;
		dialog.caption = "Assign airport";
		dialog.button1 = "Assign";
		dialog.button2 = "Cancel";
		dialog.handler.data = response.data; /*This should this be the veh (db) id.*/
		dialog.handler.callback = admin_cb_dlg_vehinfo_assignap_response;
		dialog_show(playerid, &dialog);
	}
}

static
void admin_show_vehinfo_dialog(playerid, veh, enabled, airport)
	int playerid;
	struct dbvehicle *veh;
	char *enabled;
	char *airport;
{
	struct DIALOG_INFO dialog;
	
	dialog_init_info(&dialog);
	sprintf(dialog.info,
		"Model\t%s\n"
		"Game vehicleid\t%d\n"
		"Vehicle Db Id\t%d\n"
		"Owner\t%d (%s)\n"
		"Enabled\t%s\n"
		/*change listitem in admin_cb_dlg_vehinfo_response when these lines change*/
		"Linked airport\t%s\n",
		vehmodelnames[veh->model - 400],
		veh->spawnedvehicleid,
		veh->id,
		veh->owneruserid,
		(veh->owneruserid ? veh->owner_name : "<PUBLIC>"),
		enabled,
		airport);
	dialog.transactionid = DLG_TID_ADMIN_VEHINFO;
	dialog.style = DIALOG_STYLE_TABLIST;
	dialog.caption = "vehinfo";
	dialog.button1 = "Close";
	dialog.handler.data = (void*) veh->id;
	dialog.handler.callback = admin_cb_dlg_vehinfo_response;
	dialog_show(playerid, &dialog);
}

struct CB_VEHICLEINFO {
	int player_cc;
	struct dbvehicle *veh;
};

static
void admin_cb_query_vehinfo(void *data)
{
	struct dbvehicle *veh;
	int cc, playerid;

	cc = ((struct CB_VEHICLEINFO*) data)->player_cc;
	veh = ((struct CB_VEHICLEINFO*) data)->veh;
	free(data);

	PLAYER_CC_GET_CHECK_RETURN(cc, playerid);

	if (dialog_get_transaction(playerid) != DLG_TID_ADMIN_VEHINFO) {
		/*They already closed the dialog.*/
		return;
	}

	if (NC_cache_get_row_count()) {
		NC_cache_get_field_str(0, 0, buf32a);
		NC_cache_get_field_str(0, 1, buf32_1a);
		ctoai(cbuf32);
		ctoai(cbuf32_1);
		admin_show_vehinfo_dialog(playerid, veh, cbuf32, cbuf32_1);
	} else {
		admin_show_vehinfo_dialog(playerid, veh, "<error>", "<error>");
	}
}

/**
Shows //vehinfo dialog for player (for the vehicle they're currently in).

Some non-cached data will show with placeholders in the dialog.
A db query is made for that data, and on query response it will
send the same dialog but with filled in data.
*/
static
void admin_engage_vehinfo_dialog(int playerid)
{
	struct DIALOG_INFO *dialog;
	struct dbvehicle *veh;
	struct CB_VEHICLEINFO *cbdata;
	char query[100];

	if (GetPlayerVehicle(playerid, &veh) && veh) {
		admin_show_vehinfo_dialog(playerid, veh, "<loading>", "<loading>");
		cbdata = malloc(sizeof(struct CB_VEHICLEINFO));
		cbdata->player_cc = MK_PLAYER_CC(playerid);
		cbdata->veh = veh;
		sprintf(query,
			"SELECT IF(veh.e=1,'yes','no'),apt.n "
			"FROM veh "
			"LEFT OUTER JOIN apt ON apt.i=veh.ap "
			"WHERE veh.i=%d",
			veh->id);
		common_mysql_tquery(query, admin_cb_query_vehinfo, cbdata);
	} else {
		dialog = alloca(sizeof(struct DIALOG_INFO));
		dialog_init_info(dialog);
		dialog->transactionid = DLG_TID_ADMIN_VEHINFO;
		dialog->info = "unknown vehicle";
		dialog->button1 = "Close";
		dialog->caption = "vehinfo";
		dialog_show(playerid, dialog);
	}
}

/**
The //vehinfo cmd shows vehicle info

Also allows editing enabled state and linked airport.
*/
static
int cmd_admin_vehinfo(struct COMMANDCONTEXT cmdctx)
{
	admin_engage_vehinfo_dialog(cmdctx.playerid);
	return CMD_OK;
}
