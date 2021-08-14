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

	if (veh_GetPlayerVehicle(playerid, NULL, &veh) && veh) {
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
