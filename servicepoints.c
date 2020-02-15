
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "a_samp.h"
#include "anticheat.h"
#include "cmd.h"
#include "login.h"
#include "missions.h"
#include "money.h"
#include "playerdata.h"
#include "servicepoints.h"
#include "vehicles.h"
#include <string.h>

#define SERVICE_MAP_DISTANCE 350.0f
#define SERVICE_MAP_DISTANCE_SQ (SERVICE_MAP_DISTANCE * SERVICE_MAP_DISTANCE)
#define SERVICE_MAP_ICON_TYPE 38 /*S sweet icon*/

#define SERVICE_DISTANCE (50.0f)
#define SERVICE_DISTANCE_SQ (SERVICE_DISTANCE * SERVICE_DISTANCE)

#define FIX_BASE_COST 150
#define FIX_HP_COST 2

#define FUEL_BASE_COST 75
#define FUEL_PCT_COST 10

static const char *MSG_REFUEL_NEED_SVP =
	WARN"You need to be at a service point to do this!";

struct SERVICEPOINT {
	int id;
	struct vec3 pos;
};

struct PLAYERSERVICEPOINT {
	struct SERVICEPOINT *svp;
	int textid;
};

/**
Array holding which service points are shown per player.
*/
static struct PLAYERSERVICEPOINT
	player_servicepoints[MAX_PLAYERS][MAX_SERVICE_MAP_ICONS];
static int numservicepoints;
static struct SERVICEPOINT *servicepoints;

void svp_dispose()
{
	if (servicepoints != NULL) {
		free(servicepoints);
		servicepoints = NULL;
		numservicepoints = 0;
	}
}

/**
Finds a service point in range of given coordinates.

@return service point id
*/
static
int svp_find_point(struct vec3 pos)
{
	float dist_sq;
	int i = numservicepoints;

	while (i--) {
		dist_sq = common_dist_sq(pos, servicepoints[i].pos);
		if (dist_sq < SERVICE_DISTANCE_SQ) {
			return servicepoints[i].id;
		}
	}
	return -1;
}

void svp_init()
{
	struct SERVICEPOINT *svp;
	int cacheid, rowcount;

	servicepoints = NULL;
	numservicepoints = 0;

	atoc(buf4096,
		"SELECT s.id,s.x,s.y,s.z "
		"FROM svp s "
		"JOIN apt a ON s.apt=a.i "
		"WHERE ISNULL(a.e) OR a.e=1",
		4096);
	cacheid = NC_mysql_query(buf4096a);
	numservicepoints = rowcount = NC_cache_get_row_count();
	servicepoints = malloc(sizeof(struct SERVICEPOINT) * numservicepoints);
	NC_PARS(2);
	while (rowcount--) {
		nc_params[1] = rowcount;
		svp = servicepoints + rowcount;
		svp->id = (nc_params[2] = 0, NC(n_cache_get_field_i));
		svp->pos.x = (nc_params[2] = 1, NCF(n_cache_get_field_f));
		svp->pos.y = (nc_params[2] = 2, NCF(n_cache_get_field_f));
		svp->pos.z = (nc_params[2] = 3, NCF(n_cache_get_field_f));
	}
	NC_cache_delete(cacheid);
}

int svp_cmd_refuel(CMDPARAMS)
{
	struct VEHICLEPARAMS vparams;
	struct dbvehicle *veh;
	struct vec3 vpos;
	int vehicleid, driverid, budget, svpid, msgcol, cost, refuelpct;
	int driveruserid;
	float capacity, refuelamount;
	char *msg, buf1[10];

	vehicleid = veh_GetPlayerVehicle(playerid, NULL, &veh);
	if (!vehicleid || veh == NULL) {
		return 1;
	}

	common_GetVehicleParamsEx(vehicleid, &vparams);
	if (vparams.engine) {
		msgcol = COL_WARN;
		msg = "The engine must be turned off first. "
			"Press n or check out /helpkeys";
		goto retmsg;
	}

	common_GetVehiclePos(vehicleid, &vpos);
	svpid = svp_find_point(vpos);
	if (svpid == -1) {
		msgcol = COL_WARN;
		msg = (char*) MSG_REFUEL_NEED_SVP;
		goto retmsg;
	}

	capacity = model_fuel_capacity(veh->model);
	refuelamount = capacity - veh->fuel;
	refuelpct = (int) (100.0f * refuelamount / capacity);
	if (refuelpct <= 0) {
		sprintf(cbuf4096,
		        WARN"Refueling is not needed yet! "
			"Capacity: %.0f/%.0f (%.0f'/.)",
		        veh->fuel,
		        capacity,
			100.0f * veh->fuel / capacity);
		msg = cbuf4096;
		msgcol = COL_WARN;
		goto retmsg;
	}

	cost = FUEL_BASE_COST + FUEL_PCT_COST * refuelpct;
	budget = money_get(playerid);
	if (cost > budget) {
		refuelpct = (budget - FUEL_BASE_COST) / FUEL_PCT_COST;
		if (refuelpct < 1.0f) {
			sprintf(cbuf4096,
			        WARN"You can't pay the refuel fee! "
				"Capacity: %.0f/%.0f (%.0f'/.)",
			        veh->fuel,
			        capacity,
				100.0f * veh->fuel / capacity);
			msg = cbuf4096;
			msgcol = COL_WARN;
			goto retmsg;
		}
		refuelamount = refuelpct * capacity / 100.0f;
		cost = FUEL_BASE_COST + FUEL_PCT_COST * refuelpct;
		strcpy(buf1, "partially");
	} else {
		strcpy(buf1, "fully");
	}

	money_take(playerid, cost);

	veh->fuel += refuelamount;

	csprintf(buf144,
		INFO"Your vehicle has been %s refueled for $%d (+%d'/.) "
		"capacity: %.0f/%.0f (%.0f'/.)",
	        buf1,
	        cost,
		refuelpct,
	        veh->fuel,
	        capacity,
		100.0f * veh->fuel / capacity);
	NC_SendClientMessage(playerid, COL_INFO, buf144a);

	if (NC_GetPlayerVehicleSeat(playerid) != 0) {
		driverid = common_find_player_in_vehicle_seat(vehicleid, 0);
		if (driverid == INVALID_PLAYER_ID) {
			driveruserid = -1;
		} else {
			driveruserid = userid[driverid];
			csprintf(buf144,
				INFO"Player %s[%d] refueled your vehicle!",
				pdata[playerid]->name,
				playerid);
			NC_SendClientMessage(driverid, COL_INFO, buf144a);
		}
	} else {
		driverid = playerid;
		driveruserid = userid[driverid];
	}

	if (veh && userid[playerid] > 0) {
		csprintf(buf4096,
			"INSERT INTO refuellog"
			"(stamp,vehicle,driver,invokr,svp,paid,fuel) "
			"VALUES "
			"(UNIX_TIMESTAMP(),%d,IF(%d<1,NULL,%d),%d,%d,%d,%.4f)",
			veh->id,
			driveruserid,
			driveruserid,
			userid[playerid],
			svpid,
			cost,
			refuelamount);
		NC_mysql_tquery_nocb(buf4096a);
	}

	missions_on_vehicle_refueled(vehicleid, refuelamount);
	return 1;
retmsg:
	B144(msg);
	NC_SendClientMessage(playerid, msgcol, buf144a);
	return 1;
}

int svp_cmd_repair(CMDPARAMS)
{
	struct dbvehicle *veh;
	struct vec3 vpos;
	int vehicleid, driverid, driveruserid, budget, svpid, msgcol, cost;
	float hp, fixamount;
	char *msg;

	vehicleid = veh_GetPlayerVehicle(playerid, NULL, &veh);
	if (!vehicleid) {
		return 1;
	}

	common_GetVehiclePos(vehicleid, &vpos);
	svpid = svp_find_point(vpos);
	if (svpid == -1) {
		msgcol = COL_WARN;
		msg = (char*) MSG_REFUEL_NEED_SVP;
		goto retmsg;
	}

	hp = anticheat_GetVehicleHealth(vehicleid);
	if (hp > 999.9f) {
		msgcol = COL_WARN;
		msg = WARN"Your vehicle doesn't need to be repaired!";
		goto retmsg;
	}

	fixamount = 1000.0f - hp;
	cost = FIX_BASE_COST + FIX_HP_COST * (int) fixamount;
	budget = money_get(playerid);
	if (cost > budget) {
		fixamount = (float) ((budget - FIX_BASE_COST) / FIX_HP_COST);
		if (fixamount <= 0.0f) {
			msg = WARN"You can't afford the repair fee!";
			msgcol = COL_WARN;
			goto retmsg;
		}
		cost = FIX_BASE_COST + FIX_HP_COST * (int) fixamount;
		csprintf(buf144,
			INFO"Your vehicle has been partially repaired for $%d",
			cost);
	} else {
		csprintf(buf144,
			INFO"Your vehicle has been fully repaired for $%d",
			cost);
	}
	NC_SendClientMessage(playerid, COL_INFO, buf144a);

	hp += fixamount;
	money_take(playerid, cost);

	NC_PARS(1);
	nc_params[1] = vehicleid;
	NC(n_RepairVehicle);
	NC_PARS(2);
	nc_paramf[2] = hp;
	NC(n_SetVehicleHealth);

	if (NC_GetPlayerVehicleSeat(playerid) != 0) {
		driverid = common_find_player_in_vehicle_seat(vehicleid, 0);
		if (driverid == INVALID_PLAYER_ID) {
			driveruserid = -1;
		} else {
			driveruserid = userid[driverid];
			csprintf(buf144,
				INFO"Player %s[%d] repaired your vehicle!",
				pdata[playerid]->name,
				playerid);
			NC_SendClientMessage(driverid, COL_INFO, buf144a);
		}
	} else {
		driverid = playerid;
		driveruserid = userid[driverid];
	}

	if (veh && userid[playerid] > 0) {
		csprintf(buf4096,
			"INSERT INTO "
			"repairlog"
			"(stamp,vehicle,driver,invokr,svp,paid,damage) "
			"VALUES "
			"(UNIX_TIMESTAMP(),%d,IF(%d<1,NULL,%d),%d,%d,%d,%d)",
			veh->id,
			driveruserid,
			driveruserid,
			userid[playerid],
			svpid,
			cost,
			(int) fixamount);
		NC_mysql_tquery_nocb(buf4096a);
	}

	missions_on_vehicle_repaired(vehicleid, fixamount, hp);
	return 1;
retmsg:
	B144(msg);
	NC_SendClientMessage(playerid, msgcol, buf144a);
	return 1;
}

void svp_on_player_connect(int playerid)
{
	struct PLAYERSERVICEPOINT *psvp = player_servicepoints[playerid];
	int i;

	for (i = 0; i < MAX_SERVICE_MAP_ICONS; i++) {
		psvp[i].svp = NULL;
	}
}

void svp_update_mapicons(int playerid, float x, float y)
{
	static const char* SVP_TXT = "Service Point\n/repair - /refuel";

	struct SERVICEPOINT *svp;
	struct PLAYERSERVICEPOINT *psvps = player_servicepoints[playerid], *sp;
	float dx, dy;
	int i, selectedpsvp;

	/* remove old, now out of range ones */
	NC_PARS(2);
	for (i = 0; i < MAX_SERVICE_MAP_ICONS; i++) {
		if ((svp = psvps[i].svp) != NULL) {
			dx = svp->pos.x - x;
			dy = svp->pos.y - y;
			if (dx * dx + dy * dy > SERVICE_MAP_DISTANCE_SQ) {
				nc_params[1] = playerid;
				nc_params[2] = psvps[i].textid;
				NC(n_DeletePlayer3DTextLabel);
				nc_params[2] = i;
				NC(n_RemovePlayerMapIcon);
				psvps[i].svp = NULL;
			}
		}
	}

	/* add new ones */
	svp = servicepoints + numservicepoints;
	while (svp-- != servicepoints) {
		dx = x - svp->pos.x;
		dy = y - svp->pos.y;
		if (dx * dx + dy * dy < SERVICE_MAP_DISTANCE_SQ) {
			selectedpsvp = -1;
			for (i = 0; i < MAX_SERVICE_MAP_ICONS; i++) {
				if (psvps[i].svp == svp) {
					goto alreadyshown;
				}
				if (psvps[i].svp == NULL) {
					selectedpsvp = i;
				}
			}
			if (selectedpsvp == -1) {
				/*no more map icon slots*/
				break;
			}
			sp = psvps + selectedpsvp;
			NC_PARS(8);
			nc_params[1] = playerid;
			nc_params[2] = selectedpsvp;
			nc_paramf[3] = svp->pos.x;
			nc_paramf[4] = svp->pos.y;
			nc_paramf[5] = svp->pos.z;
			nc_params[6] = SERVICE_MAP_ICON_TYPE,
			nc_params[7] = 0;
			nc_params[8] = MAPICON_LOCAL;
			NC(n_SetPlayerMapIcon);
			B144((char*) SVP_TXT);
			NC_PARS(10);
			nc_params[2] = buf144a;
			nc_params[6] = nc_params[5];
			nc_params[5] = nc_params[4];
			nc_params[4] = nc_params[3];
			nc_params[3] = -1;
			nc_paramf[7] = 50.0f;
			nc_params[8] = nc_params[9] = INVALID_PLAYER_ID;
			nc_params[10] = 1;
			sp->textid = NC(n_CreatePlayer3DTextLabel);
			sp->svp = svp;
		}
alreadyshown:
		;
	}
}