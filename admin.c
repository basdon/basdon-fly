
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "admin.h"
#include "airport.h"
#include "cmd.h"
#include "dialog.h"
#include "game_sa.h"
#include "vehicles.h"

/*-------------------------------------------*/
/*//respawn*/

/**
Respawns the vehicle the player is in.
*/
int cmd_admin_respawn(CMDPARAMS)
{
	int vehicleid;

	vehicleid = NC_GetPlayerVehicleID(playerid);
	if (vehicleid) {
		NC_SetVehicleToRespawn(vehicleid);
	}
	return 1;
}

/*-------------------------------------------*/
/*//vehinfo*/

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
	char enabled[10], airport[MAX_AIRPORT_NAME + 1];

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

int cmd_admin_vehinfo(CMDPARAMS)
{
	admin_vehinfo_show(playerid);
	return 1;
}

/*-------------------------------------------*/
