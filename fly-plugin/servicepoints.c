#define SERVICE_MAP_DISTANCE 350.0f
#define SERVICE_MAP_DISTANCE_SQ (SERVICE_MAP_DISTANCE * SERVICE_MAP_DISTANCE)
#define SERVICE_MAP_ICON_TYPE 38 /*S sweet icon*/

#define SERVICE_DISTANCE (50.0f)
#define SERVICE_DISTANCE_SQ (SERVICE_DISTANCE * SERVICE_DISTANCE)

#define FIX_BASE_COST 150
#define FIX_HP_COST 2

#define FUEL_BASE_COST 75
#define FUEL_PCT_COST 10

struct SERVICEPOINT {
	int id;
	struct vec3 pos;
};

/**
Array holding which service points are shown per player.
*/
static struct SERVICEPOINT *player_servicepoints[MAX_PLAYERS][MAX_SERVICE_MAP_ICONS];
static int numservicepoints;
static struct SERVICEPOINT *servicepoints;

#define SVP_TXT "Service Point\n/repair - /refuel"
static char svp_text_encoded_bits[24];
static int svp_text_encoded_bitlength;

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

	svp_text_encoded_bitlength = EncodeString(svp_text_encoded_bits, SVP_TXT, 32);

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

/**
Servicepoint /refuel command.
*/
static
int svp_cmd_refuel(CMDPARAMS)
{
	struct dbvehicle *veh;
	struct vec3 vpos;
	int vehicleid, driverid, budget, svpid, cost, refuelpct;
	int driveruserid;
	float capacity, refuelamount;
	char buf1[10];
	char msg144[144];

	vehicleid = veh_GetPlayerVehicle(playerid, NULL, &veh);
	if (!vehicleid || veh == NULL) {
		return 1;
	}

	if (GetVehicleEngineState(vehicleid)) {
		SendClientMessage(playerid, COL_WARN, WARN"The engine must be turned off first. Press n (/helpkeys) or use /engine");
		return 1;
	}

	GetVehiclePosUnsafe(vehicleid, &vpos);
	/*The created mapicons for the player could be used,
	but there can only exist 4 at a time.. Maybe that's too little?*/
	svpid = svp_find_point(vpos);
	if (svpid == -1) {
		SendClientMessage(playerid, COL_WARN, WARN"You need to be at a service point to do this!");
		return 1;
	}

	capacity = model_fuel_capacity(veh->model);
	refuelamount = capacity - veh->fuel;
	refuelpct = (int) (100.0f * refuelamount / capacity);
	if (refuelpct <= 0) {
		sprintf(msg144,
		        WARN"Refueling is not needed yet! "
			"Capacity: %.0f/%.0f (%.0f'/.)",
		        veh->fuel,
		        capacity,
			100.0f * veh->fuel / capacity);
		SendClientMessage(playerid, COL_WARN, msg144);
		return 1;
	}

	cost = FUEL_BASE_COST + FUEL_PCT_COST * refuelpct;
	budget = money_get(playerid);
	if (cost > budget) {
		refuelpct = (budget - FUEL_BASE_COST) / FUEL_PCT_COST;
		if (refuelpct < 1.0f) {
			sprintf(msg144,
			        WARN"You can't pay the refuel fee! "
				"Capacity: %.0f/%.0f (%.0f'/.)",
			        veh->fuel,
			        capacity,
				100.0f * veh->fuel / capacity);
			SendClientMessage(playerid, COL_WARN, msg144);
			return 1;
		}
		refuelamount = refuelpct * capacity / 100.0f;
		cost = FUEL_BASE_COST + FUEL_PCT_COST * refuelpct;
		strcpy(buf1, "partially");
	} else {
		strcpy(buf1, "fully");
	}

	money_take(playerid, cost);

	veh->fuel += refuelamount;

	sprintf(msg144,
		INFO"Your vehicle has been %s refueled for $%d (+%d'/.) "
		"capacity: %.0f/%.0f (%.0f'/.)",
	        buf1,
	        cost,
		refuelpct,
	        veh->fuel,
	        capacity,
		100.0f * veh->fuel / capacity);
	SendClientMessage(playerid, COL_INFO, msg144);

	if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER) {
		driverid = GetVehicleDriver(vehicleid);
		if (driverid == INVALID_PLAYER_ID) {
			driveruserid = -1;
		} else {
			driveruserid = userid[driverid];
			sprintf(msg144,
				INFO"Player %s[%d] refueled your vehicle!",
				pdata[playerid]->name,
				playerid);
			SendClientMessage(driverid, COL_INFO, msg144);
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
}

/**
Servicepoint /repair command.

Aliases: /fix
*/
static
int svp_cmd_repair(CMDPARAMS)
{
	struct dbvehicle *veh;
	struct vec3 vpos;
	int vehicleid, driverid, driveruserid, budget, svpid, cost;
	float hp, fixamount;
	char msg144[144];

	vehicleid = veh_GetPlayerVehicle(playerid, NULL, &veh);
	if (!vehicleid) {
		return 1;
	}

	GetVehiclePosUnsafe(vehicleid, &vpos);
	/*The created mapicons for the player could be used,
	but there can only exist 4 at a time.. Maybe that's too little?*/
	svpid = svp_find_point(vpos);
	if (svpid == -1) {
		SendClientMessage(playerid, COL_WARN, WARN"You need to be at a service point to do this!");
		return 1;
	}

	hp = anticheat_GetVehicleHealth(vehicleid);
	if (hp > 999.9f) {
		SendClientMessage(playerid, COL_WARN, WARN"Your vehicle doesn't need to be repaired!");
		return 1;
	}

	fixamount = 1000.0f - hp;
	cost = FIX_BASE_COST + FIX_HP_COST * (int) fixamount;
	budget = money_get(playerid);
	if (cost > budget) {
		fixamount = (float) ((budget - FIX_BASE_COST) / FIX_HP_COST);
		if (fixamount <= 0.0f) {
			SendClientMessage(playerid, COL_WARN, WARN"You can't afford the repair fee!");
			return 1;
		}
		cost = FIX_BASE_COST + FIX_HP_COST * (int) fixamount;
		sprintf(msg144, INFO"Your vehicle has been partially repaired for $%d", cost);
	} else {
		sprintf(msg144, INFO"Your vehicle has been fully repaired for $%d", cost);
	}
	SendClientMessage(playerid, COL_INFO, msg144);

	hp += fixamount;
	money_take(playerid, cost);

	NC_PARS(1);
	nc_params[1] = vehicleid;
	NC(n_RepairVehicle);
	NC_PARS(2);
	nc_paramf[2] = hp;
	NC(n_SetVehicleHealth);

	if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER) {
		driverid = GetVehicleDriver(vehicleid);
		if (driverid == INVALID_PLAYER_ID) {
			driveruserid = -1;
		} else {
			driveruserid = userid[driverid];
			sprintf(msg144, INFO"Player %s[%d] repaired your vehicle!", pdata[playerid]->name, playerid);
			SendClientMessage(driverid, COL_INFO, msg144);
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
}

void svp_on_player_connect(int playerid)
{
	memset(player_servicepoints[playerid], 0, sizeof(player_servicepoints[playerid]));
}

void svp_update_mapicons(int playerid, float x, float y)
{
	struct SERVICEPOINT **psvps;
	struct SERVICEPOINT *svp;
	float dx, dy;
	int i;

	psvps = player_servicepoints[playerid];

	/*Remove old, now out of range ones.*/
	for (i = 0; i < MAX_SERVICE_MAP_ICONS; i++) {
		svp = psvps[i];
		if (svp) {
			dx = svp->pos.x - x;
			dy = svp->pos.y - y;
			if (dx * dx + dy * dy > SERVICE_MAP_DISTANCE_SQ) {
				psvps[i] = NULL;
			}
		}
	}

	/*Add new ones.*/
	svp = servicepoints + numservicepoints;
	while (svp-- != servicepoints) {
		dx = x - svp->pos.x;
		dy = y - svp->pos.y;
		if (dx * dx + dy * dy < SERVICE_MAP_DISTANCE_SQ) {
			for (i = 0; ; i++) {
				if (psvps[i] == svp) {
					goto alreadyshown;
				}
				if (!psvps[i]) {
					psvps[i] = svp;
					break;
				}
				i++;
				if (i >= MAX_SERVICE_MAP_ICONS) {
					/*No more map icon slots.*/
					return;
				}
			}
			SetPlayerMapIcon(playerid, SERVICEPOINT_MAPICON_ID_BASE + i, &svp->pos, SERVICE_MAP_ICON_TYPE, 0, MAPICON_LOCAL);
			Create3DTextLabel(
				playerid, SERVICEPOINT_TEXTLABEL_ID_BASE + i, 0xFFFFFFFF, &svp->pos, 50.0f, 1,
				INVALID_PLAYER_ID, INVALID_VEHICLE_ID, svp_text_encoded_bits, svp_text_encoded_bitlength
			);
		}
alreadyshown:
		;
	}
}
