char dev_print_updatevehicledamagestatus_enabled;
char dev_print_unoccupiedvehiclesync_enabled;

static void dev_menu_show(ushort playerid);
/*jeanine:p:i:4;p:0;a:b;y:1.88;n:dev_print_updatevehicledamagestatus;*/
static
void dev_print_updatevehicledamagestatus(
	int playerid,
	int vehicleid,
	struct SampVehicle *vehicle,
	struct INOUTRPCDATA_UpdateVehicleDamageStatus *rpcdata
) {
	TRACE;

	if (dev_print_updatevehicledamagestatus_enabled) {
		sprintf(
			cbuf144,
			"UpdateVehicleDamageStatus pid %d vid %d panels %08X->%08X doors %08X->%08X lights %02X->%02X tires %02X->%02X",
			playerid, vehicleid,
			vehicle->damageStatus.panels.raw, rpcdata->panels,
			vehicle->damageStatus.doors.raw, rpcdata->doors,
			vehicle->damageStatus.broken_lights.raw, rpcdata->broken_lights,
			vehicle->damageStatus.popped_tires.raw, rpcdata->popped_tires
		);
		SendClientMessageToAll(COL_SAMP_GREY, cbuf144);
	}
}
/*jeanine:p:i:5;p:4;a:b;y:1.88;n:dev_print_unoccupiedvehiclesync;*/
static
void dev_print_unoccupiedvehiclesync(ushort playerid, ushort vehicleid, struct SYNCDATA_UnoccupiedVehicle *syncdata)
{
	TRACE;

	if (dev_print_unoccupiedvehiclesync_enabled) {
		sprintf(
			cbuf144,
			"UnoccupiedVehicleSync pid %d vid %d seat %d health %.1f "
				"vel %.1f %.1f %.1f angvel %.1f %.1f %.1f",
			playerid,
			vehicleid,
			syncdata->seat,
			syncdata->health,
			syncdata->velocity.x,
			syncdata->velocity.y,
			syncdata->velocity.z,
			syncdata->angularVelocity.x,
			syncdata->angularVelocity.y,
			syncdata->angularVelocity.z
		);
		SendClientMessageToAll(COL_SAMP_GREY, cbuf144);
	}
}
/*jeanine:p:i:1;p:3;a:r;x:9.00;n:dev_menu_callback;*/
static
void dev_menu_callback(int playerid, struct DIALOG_RESPONSE response)
{
	TRACE;

	if (response.response) {
		if (response.listitem == 1) {
			dev_print_unoccupiedvehiclesync_enabled ^= 1;
		} else if (response.listitem == 2) {
			dev_print_updatevehicledamagestatus_enabled ^= 1;
		}
		dev_menu_show(playerid);/*jeanine:s:a:r;i:3;*/
	}
}
/*jeanine:p:i:2;p:3;a:r;x:9.00;y:-10.00;n:dev_menu_append_toggle_entry;*/
static
char* dev_menu_append_toggle_entry(char *into, char *entry_name, char state)
{
	TRACE;

	while ((*(into++) = *(entry_name++)));
	if (state) {
		memcpy(into - 1, ":\t"ECOL_SUCC"yes\n", 14);
		return into + 13;
	} else {
		memcpy(into - 1, ":\t"ECOL_WARN"no\n", 13);
		return into + 12;
	}
}
/*jeanine:p:i:3;p:5;a:b;y:1.88;n:dev_menu_show;*/
static
void dev_menu_show(ushort playerid)
{
	TRACE;
	struct DIALOG_INFO dialog;
	char *bp;

	dialog_init_info(&dialog);
	/*Dummy entry to prevent doing things when quicky/accidentally pressing enter after the dialog shows.*/
	bp = dialog.info + sprintf(dialog.info, ECOL_SAMP_GREY"dummy entry\t\n");
	bp = dev_menu_append_toggle_entry(bp, "print SYNC UnoccupiedVehicle", dev_print_unoccupiedvehiclesync_enabled);/*jeanine:s:a:r;i:2;*/
	bp = dev_menu_append_toggle_entry(bp, "print RPC UpdateVehicleDamageStatus", dev_print_updatevehicledamagestatus_enabled);/*jeanine:r:i:2;*/
	*bp = 0;
	dialog.transactionid = DLG_TID_DEV;
	dialog.style = DIALOG_STYLE_TABLIST;
	dialog.caption = "Dev menu";
	dialog.button1 = "Do";
	dialog.button2 = "Close";
	dialog.handler.callback = dev_menu_callback;/*jeanine:r:i:1;*/
	dialog_show(playerid, &dialog);
}
