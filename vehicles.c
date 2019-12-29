
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "a_samp.h"
#include "anticheat.h"
#include "game_sa.h"
#include "login.h"
#include "math.h"
#include "missions.h"
#include "playerdata.h"
#include "playerstats.h"
#include "score.h"
#include "servicepoints.h"
#include "vehicles.h"
#include "time/time.h"
#include <string.h>

#define FUEL_WARNING_SOUND 3200 /*air horn*/

#define MAX_ENGINE_CUTOFF_KPH (3.0f)
#define MAX_ENGINE_CUTOFF_VEL (MAX_ENGINE_CUTOFF_KPH / VEL_TO_KPH)
#define MAX_ENGINE_CUTOFF_VEL_SQ (MAX_ENGINE_CUTOFF_VEL * MAX_ENGINE_CUTOFF_VEL)

struct vehnode {
	struct dbvehicle *veh;
	struct vehnode *next;
};

/**
Linked list of vehicles that need an ODO update in db.
*/
static struct vehnode *vehstoupdate;
static struct dbvehicle *dbvehicles;
int numdbvehicles, dbvehiclealloc;
struct vehicle gamevehicles[MAX_VEHICLES];
short labelids[MAX_PLAYERS][MAX_VEHICLES]; /* 200KB+ of mapping, errrr */

/**
Holds global textdraw id of the vehicle panel background.
*/
static int txt_bg;
/**
Holds player textdraw id of the vehicle panel text, -1 if not created.
*/
static int ptxt_txt[MAX_PLAYERS];
/**
Holds player textdraw id of the fuel level bar text, -1 if not created.
*/
static int ptxt_fl[MAX_PLAYERS];
/**
Holds player textdraw id of the health bar text, -1 if not created.
*/
static int ptxt_hp[MAX_PLAYERS];
/**
Last fuel value that was shown to a player.
*/
static short ptxtcache_fl[MAX_PLAYERS];
/**
Last health value that was shown to a player.
*/
static short ptxtcache_hp[MAX_PLAYERS];
/**
Last vehicle id the player was in as driver.
*/
static int lastvehicle[MAX_PLAYERS];
/**
Last vehicle position for player (when driver), for ODO purposes.

To be inited in veh_on_player_now_driving and updated on ODO update.
*/
static struct vec3 lastvpos[MAX_PLAYERS];
/**
Last timestamp when a player had some sort of control activity.
*/
static unsigned long lastcontrolactivity[MAX_PLAYERS];

/**
Each player's total ODO value (m).

TODO: move this somewhere else.
*/
float playerodoKM[MAX_PLAYERS];


/**
Enlarges the vehicle database table pointed to by dbvehicles by 100.
*/
static
void veh_dbtable_grow()
{
	struct dbvehicle *nw;
	int i;

	dbvehiclealloc += 100;
	dbvehicles = realloc(dbvehicles,
		sizeof(struct dbvehicle) * dbvehiclealloc);
	nw = dbvehicles;
	for (i = 0; i < numdbvehicles; i++) {
		if (nw->spawnedvehicleid) {
			gamevehicles[nw->spawnedvehicleid].dbvehicle = nw;
		}
		nw++;
	}
}

void veh_on_player_connect(int playerid)
{
	short *labelid = &labelids[playerid][0];
	int i = MAX_VEHICLES;

	while (i--) {
		*labelid = INVALID_3DTEXT_ID;
		labelid++;
	}

	lastvehicle[playerid] = 0;
	playerodoKM[playerid] = 0.0f;
}

void veh_init()
{
	struct dbvehicle *veh;
	int i, tmp, rowcount, dbcache, vehicleid, *fld = nc_params + 2;
	char ownername[MAX_PLAYER_NAME + 1];

	for (i = 0; i < MAX_VEHICLES; i++) {
		gamevehicles[i].dbvehicle = NULL;
		gamevehicles[i].reincarnation = 0;
		gamevehicles[i].need_recreation = 0;
	}
	vehstoupdate = NULL;
	dbvehicles = NULL;

	atoc(buf4096,
		"SELECT v.i,m,IFNULL(ownerplayer,0),v.x,v.y,v.z,v.r,"
		"v.col1,v.col2,v.odo,u.name "
		"FROM veh v "
		"LEFT OUTER JOIN usr u ON v.ownerplayer = u.i "
		"LEFT OUTER JOIN apt a ON v.ap = a.i "
		"WHERE v.e=1 AND (ISNULL(v.ap) OR a.e = 1)",
		4096);
	dbcache = NC_mysql_query(buf4096a);
	rowcount = dbvehiclealloc = NC_cache_get_row_count();
	if (dbvehiclealloc <= 0) {
		dbvehiclealloc = 100;
	}
	numdbvehicles = 0;
	dbvehicles = malloc(sizeof(struct dbvehicle) * dbvehiclealloc);
	veh = dbvehicles;
	while (rowcount--) {
		nc_params[1] = rowcount;
		NC_PARS(2);
		veh->id = (*fld = 0, NC(n_cache_get_field_i));
		veh->model = (short) (*fld = 1, NC(n_cache_get_field_i));
		veh->owneruserid = (*fld = 2, NC(n_cache_get_field_i));
		veh->pos.coords.x = (*fld = 3, NCF(n_cache_get_field_f));
		veh->pos.coords.y = (*fld = 4, NCF(n_cache_get_field_f));
		veh->pos.coords.z = (*fld = 5, NCF(n_cache_get_field_f));
		veh->pos.r = (*fld = 6, NCF(n_cache_get_field_f));
		veh->col1 = (unsigned char) (*fld = 7, NC(n_cache_get_field_i));
		veh->col2 = (unsigned char) (*fld = 8, NC(n_cache_get_field_i));
		veh->odoKM = (*fld = 9, NCF(n_cache_get_field_f));
		veh->spawnedvehicleid = 0;
		if (veh->owneruserid) {
			NC_PARS(3);
			nc_params[3] = buf32a;
			*fld = 10, NC(n_cache_get_field_s);
			ctoa(ownername, buf32, sizeof(ownername));
			tmp = 7 + strlen(vehnames[veh->model - 400]);
			veh->ownerstringowneroffset = tmp;
			tmp += strlen(ownername) + 1;
			veh->ownerstring = malloc(tmp * sizeof(char));
			sprintf(veh->ownerstring,
				"%s Owner\n%s",
				vehnames[veh->model - 400],
				ownername);
		} else {
			veh->ownerstring = NULL;

			NC_PARS(9);
			nc_params[1] = veh->model;
			nc_paramf[2] = veh->pos.coords.x;
			nc_paramf[3] = veh->pos.coords.y;
			nc_paramf[4] = veh->pos.coords.z;
			nc_paramf[5] = veh->pos.r;
			nc_params[6] = veh->col1;
			nc_params[7] = veh->col2;
			nc_params[8] = VEHICLE_RESPAWN_DELAY;
			nc_params[9] = 0; /*addsiren*/
			vehicleid = NC(n_AddStaticVehicleEx);
			if (vehicleid != INVALID_VEHICLE_ID) {
				veh->spawnedvehicleid = vehicleid;
				gamevehicles[vehicleid].dbvehicle = veh;
			}
		}
		veh->fuel = model_fuel_capacity(veh->model);
		veh++;
		numdbvehicles++;
	}
	NC_cache_delete(dbcache);
}

int veh_can_player_modify_veh(int playerid, struct dbvehicle *veh)
{
	return (veh && userid[playerid] == veh->owneruserid) ||
		GROUPS_ISADMIN(pdata[playerid]->groups);
}

float model_fuel_capacity(short modelid)
{
	switch (modelid)
	{
	case MODEL_ANDROM: return 95000.0f;
	case MODEL_AT400: return 23000.0f;
	case MODEL_BEAGLE: return 518.0f;
	case MODEL_DODO: return 285.0f;
	case MODEL_NEVADA: return 3224.0f;
	case MODEL_SHAMAL: return 4160.0f;
	case MODEL_SKIMMER: return 285.0f;
	case MODEL_CROPDUST: return 132.0f;
	case MODEL_STUNT: return 91.0f;
	case MODEL_LEVIATHN: return 2925.0f;
	case MODEL_MAVERICK: return 416.0f;
	case MODEL_POLMAV: return 416.0f;
	case MODEL_RAINDANC: return 1360.0f;
	case MODEL_VCNMAV: return 416.0f;
	case MODEL_SPARROW: return 106.0f;
	case MODEL_SEASPAR: return 106.0f;
	case MODEL_CARGOBOB: return 5510.0f;
	case MODEL_HUNTER: return 2400.0f;
	case MODEL_HYDRA: return 3754.0f;
	case MODEL_RUSTLER: return 1018.0f;
	default: return 1000.0f;
	}
}

static
float model_fuel_usage(int modelid)
{
	switch (modelid)
	{
	case MODEL_ANDROM: return 10.0f;
	case MODEL_AT400: return 10.0f;
	case MODEL_BEAGLE: return 1.1f;
	case MODEL_DODO: return 0.2f;
	case MODEL_NEVADA: return 2.3f;
	case MODEL_SHAMAL: return 3.6f;
	case MODEL_SKIMMER: return 0.1f;
	case MODEL_CROPDUST: return 0.1f;
	case MODEL_STUNT: return 0.1f;
	case MODEL_LEVIATHN: return 1.7f;
	case MODEL_MAVERICK: return 0.6f;
	case MODEL_POLMAV: return 0.6f;
	case MODEL_RAINDANC: return 1.7f;
	case MODEL_VCNMAV: return 0.6f;
	case MODEL_SPARROW: return 0.2f;
	case MODEL_SEASPAR: return 0.2f;
	case MODEL_CARGOBOB: return 3.2f;
	case MODEL_HUNTER: return 2.3f;
	case MODEL_HYDRA: return 4.5f;
	case MODEL_RUSTLER: return 0.9f;
	default: return 1;
	}
}

static const char *MSG_FUEL_0 = WARN"Your vehicle ran out of fuel!";
static const char *MSG_FUEL_5 = WARN"Your vehicle has 5%% fuel left!";
static const char *MSG_FUEL_10 = WARN"Your vehicle has 10%% fuel left!";
static const char *MSG_FUEL_20 = WARN"Your vehicle has 20%% fuel left!";

/**
Make given vehicle consumer fuel. Should be called every second.

@param throttle whether the player is holding the throttle down
*/
static
void veh_consume_fuel(int playerid, int vehicleid, int throttle,
	struct VEHICLEPARAMS *vparams, struct dbvehicle *veh)
{
	const float consumptionmp = throttle ? 1.0f : 0.2f;
	float fuelcapacity, lastpercentage, newpercentage;

	fuelcapacity = model_fuel_capacity(veh->model);
	lastpercentage = veh->fuel / fuelcapacity;
	veh->fuel -= model_fuel_usage(veh->model) * consumptionmp;
	if (veh->fuel < 0.0f) {
		veh->fuel = 0.0f;
	}
	newpercentage = veh->fuel / fuelcapacity;

	if (lastpercentage > 0.0f && newpercentage == 0.0f) {
		vparams->engine = 0;
		common_SetVehicleParamsEx(vehicleid, vparams);
		B144((char*) MSG_FUEL_0);
	} else if (lastpercentage > 0.05f && newpercentage <= 0.05f) {
		B144((char*) MSG_FUEL_5);
	} else if (lastpercentage > 0.1f && newpercentage <= 0.1f) {
		B144((char*) MSG_FUEL_10);
	} else if (lastpercentage > 0.2f && newpercentage <= 0.2f) {
		B144((char*) MSG_FUEL_20);
	} else {
		return;
	}
	NC_SendClientMessage(playerid, COL_WARN, buf144a);
	NC_PlayerPlaySound0(playerid, FUEL_WARNING_SOUND);
}

void veh_dispose()
{
	struct dbvehicle *veh = dbvehicles;

	while (dbvehiclealloc--) {
		if (veh->ownerstring != NULL) {
			free(veh->ownerstring);
			veh->ownerstring = NULL;
		}
		veh++;
	}
	free(dbvehicles);
        dbvehicles = NULL;
}

/**
Prevent player from entering or being in vehicle and send them a message.

If player is already inside, this will instantly eject them.
*/
static
void veh_disallow_player_in_vehicle(int playerid, struct dbvehicle *v)
{
	/*when player is entering, this stops them*/
	/*when player is already in, this should instantly eject the player*/
	NC_ClearAnimations(playerid, 1);

	csprintf(buf4096,
		WARN"This vehicle belongs to %s!",
		v->ownerstring + v->ownerstringowneroffset);
	NC_SendClientMessage(playerid, COL_WARN, buf144a);
}

int veh_is_player_allowed_in_vehicle(int playerid, struct dbvehicle *veh)
{
	return veh == NULL ||
		veh->owneruserid == 0 ||
		veh->owneruserid == userid[playerid];
}

void veh_on_player_disconnect(int playerid)
{
	struct dbvehicle *veh;
	int n, vehicleid, driver;

	if (userid[playerid] > 0) {
		veh = dbvehicles;
		n = numdbvehicles;
		while (n--) {
			if (veh->owneruserid == userid[playerid] &&
				veh->spawnedvehicleid)
			{
				veh_DestroyVehicle(veh->spawnedvehicleid);
			}
			veh++;
		}
	}

	vehicleid = lastvehicle[playerid];
	if (vehicleid && NC_IsValidVehicle(vehicleid)) {
		driver = common_find_vehicle_driver(vehicleid);
		/*driver actually still is playerid at this point*/
		if (driver == playerid || driver == INVALID_PLAYER_ID) {
			NC_SetVehicleToRespawn(vehicleid);
		}
	}
}

void veh_on_player_enter_vehicle(int playerid, int vehicleid, int ispassenger)
{
	struct dbvehicle *veh;

	veh = gamevehicles[vehicleid].dbvehicle;
	if (!ispassenger && !veh_is_player_allowed_in_vehicle(playerid, veh)) {
		veh_disallow_player_in_vehicle(playerid, veh);
	}
}

void veh_spawn_player_vehicles(int playerid)
{
	int vehicleid, n;
	struct dbvehicle *veh;

	if (userid[playerid] < 1) {
		return;
	}

	veh = dbvehicles;
	n = numdbvehicles;
	while (n--) {
		if (veh->owneruserid == userid[playerid]) {
			NC_PARS(9);
			nc_params[1] = veh->model;
			nc_paramf[2] = veh->pos.coords.x;
			nc_paramf[3] = veh->pos.coords.y;
			nc_paramf[4] = veh->pos.coords.z;
			nc_paramf[5] = veh->pos.r;
			nc_params[6] = veh->col1;
			nc_params[7] = veh->col2;
			nc_params[8] = VEHICLE_RESPAWN_DELAY;
			nc_params[9] = 0; /*addsiren*/
			vehicleid = NC(n_CreateVehicle_);
			if (vehicleid != INVALID_VEHICLE_ID) {
				gamevehicles[vehicleid].dbvehicle = veh;
				gamevehicles[vehicleid].reincarnation++;
				gamevehicles[vehicleid].need_recreation = 0;
				veh->spawnedvehicleid = vehicleid;
			}
		}
		veh++;
	}
}

/**
Handle player engine key press.

Pre: playerid is the driver of vehicleid and pressed the engine key.
*/
static
void veh_start_or_stop_engine(int playerid, int vehicleid)
{
	static const char
		*NOFUEL = WARN"The engine cannot be started, there is no fuel!",
		*MOVING = WARN"The engine cannot be shut down while moving!",
		*STARTED = INFO"Engine started",
		*STOPPED = INFO"Engine stopped";

	struct VEHICLEPARAMS vpars;
	struct dbvehicle *veh;
	struct vec3 vvel;

	common_GetVehicleParamsEx(vehicleid, &vpars);
	if (vpars.engine) {
		common_GetVehicleVelocity(vehicleid, &vvel);
		if (common_vectorsize_sq(vvel) > MAX_ENGINE_CUTOFF_VEL_SQ)
		{
			B144((char*) MOVING);
			NC_SendClientMessage(playerid, COL_WARN, buf144a);
		} else {
			vpars.engine = 0;
			common_SetVehicleParamsEx(vehicleid, &vpars);
			B144((char*) STOPPED);
			NC_SendClientMessage(playerid, COL_INFO, buf144a);
		}
	} else {
		veh = gamevehicles[vehicleid].dbvehicle;
		if (veh != NULL && veh->fuel == 0.0f) {
			B144((char*) NOFUEL);
			NC_SendClientMessage(playerid, COL_WARN, buf144a);
		} else {
			vpars.engine = 1;
			common_SetVehicleParamsEx(vehicleid, &vpars);
			B144((char*) STARTED);
			NC_SendClientMessage(playerid, COL_INFO, buf144a);
		}
	}
}

void veh_on_player_key_state_change(int playerid, int oldkeys, int newkeys)
{

	int vehicleid;

	/*not checking vehicle... for now*/
	lastcontrolactivity[playerid] = time_timestamp();

	if (newkeys & KEY_NO && !(oldkeys & KEY_NO)) {
		vehicleid = NC_GetPlayerVehicleID(playerid);
		if (vehicleid && NC_GetPlayerVehicleSeat(playerid) == 0) {
			veh_start_or_stop_engine(playerid, vehicleid);
		}
	}
	return;
}

void veh_on_player_now_driving(int playerid, int vehicleid)
{
	struct dbvehicle *veh;
	int reqenginestate;

	lastvehicle[playerid] = vehicleid;
	common_GetVehiclePos(vehicleid, &lastvpos[playerid]);

	lastcontrolactivity[playerid] = time_timestamp();
	reqenginestate =
		(veh = gamevehicles[vehicleid].dbvehicle) == NULL ||
		veh->fuel > 0.0f;
	common_set_vehicle_engine(vehicleid, reqenginestate);
	if (!reqenginestate) {
		B144(WARN"This vehicle is out of fuel!");
		NC_SendClientMessage(playerid, COL_WARN, buf144a);
	}
}

int veh_commit_next_vehicle_odo_to_db()
{
	struct dbvehicle *veh;
	struct vehnode *tofree;

	if (vehstoupdate) {
		veh = vehstoupdate->veh;
		tofree = vehstoupdate;
		vehstoupdate = vehstoupdate->next;
		free(tofree);
		veh->needsodoupdate = 0;
		csprintf(buf144,
			"UPDATE veh SET odo=%.4f WHERE i=%d",
			veh->odoKM,
			veh->id);
		NC_mysql_tquery_nocb(buf144a);
		return 1;
	}
	return 0;
}

int veh_GetPlayerVehicle(int playerid, int *reinc, struct dbvehicle **veh)
{
	int vehicleid;

	vehicleid = NC_GetPlayerVehicleID(playerid);
	if (reinc != NULL) {
		*reinc = gamevehicles[vehicleid].reincarnation;
	}
	*veh = gamevehicles[vehicleid].dbvehicle;
	return vehicleid;
}

void veh_update_odo(int playerid, int vehicleid, struct vec3 pos)
{
	struct vehnode *vuq;
	struct dbvehicle *veh;
	float dx, dy, dz, distanceM, distanceKM;

	dx = lastvpos[playerid].x - pos.x;
	dy = lastvpos[playerid].y - pos.y;
	dz = lastvpos[playerid].z - pos.z;
	distanceM = sqrt(dx * dx + dy * dy + dz * dz);
	lastvpos[playerid] = pos;
	if (distanceM != distanceM || distanceM == 0.0f) {
		return;
	}
	distanceKM = distanceM / 1000.0f;

	/*TODO: should this only check mission vehicle?*/
	/*this also adds while not loaded yet, this is correct*/
	missions_add_distance(playerid, distanceM);
	playerodoKM[playerid] += distanceKM;

	if ((veh = gamevehicles[vehicleid].dbvehicle) != NULL) {
		veh->odoKM += distanceKM;
		if (!veh->needsodoupdate) {
			veh->needsodoupdate = 1;
			if (vehstoupdate == NULL) {
				vehstoupdate = malloc(sizeof(struct vehnode));
				vehstoupdate->veh = veh;
				vehstoupdate->next = NULL;
			} else {
				vuq = vehstoupdate;
				while (vuq->next != NULL) {
					vuq = vuq->next;
				}
				vuq->next = malloc(sizeof(struct vehnode));
				vuq = vuq->next;
				vuq->veh = veh;
				vuq->next = NULL;
			}
		}
	}
}

/**
Continuation of veh_timed_1s_update.

Called when player is still in same in air vehicle as last in update.
*/
static
void veh_timed_1s_update_a(
	int playerid, int vehicleid, struct vec3 *vpos,
	struct VEHICLEPARAMS *vparams)
{
	struct vec3 vvel;
	struct quat vrot;
	float hp;

	if (!isafk[playerid]) {
		common_GetVehicleRotationQuat(vehicleid, &vrot);
		missions_update_satisfaction(playerid, vehicleid, &vrot);
	}

	if (missions_get_stage(playerid) == MISSION_STAGE_FLIGHT) {
		hp = anticheat_GetVehicleHealth(vehicleid);
		common_GetVehicleVelocity(vehicleid, &vvel);
		missions_send_tracker_data(
			playerid, vehicleid, hp,
			vpos, &vvel, vparams->engine);
	}
}

void veh_timed_1s_update()
{
	struct dbvehicle *v;
	struct vec3 vpos, *ppos = &vpos;
	struct VEHICLEPARAMS vparams;
	struct PLAYERKEYS pkeys;
	int playerid, vehicleid, vehiclemodel, n = playercount;
	unsigned long timestamp = time_timestamp();
	unsigned long ctrla = timestamp - 30000;

	while (n--) {
		playerid = players[n];

		common_GetPlayerPos(playerid, ppos);
		svp_update_mapicons(playerid, ppos->x, ppos->y);

		NC_PARS(1);
		nc_params[1] = playerid;
		vehicleid = NC(n_GetPlayerVehicleID);
		if (!vehicleid) {
			continue;
		}
		if (NC(n_GetPlayerVehicleSeat) != 0) {
			continue;
		}

		v = gamevehicles[vehicleid].dbvehicle;
		if (!veh_is_player_allowed_in_vehicle(playerid, v)) {
			veh_disallow_player_in_vehicle(playerid, v);
			anticheat_disallowed_vehicle_1s(playerid);
			continue;
		}

		if (v != NULL) {
			vehiclemodel = v->model;
		} else {
			vehiclemodel = NC_GetVehicleModel(vehicleid);
		}


		if (vehicleid == lastvehicle[playerid]) {
			common_GetVehiclePos(vehicleid, &vpos);
			veh_update_odo(playerid, vehicleid, vpos);

			common_GetVehicleParamsEx(vehicleid, &vparams);

			if (vparams.engine && v != NULL) {
				common_GetPlayerKeys(playerid, &pkeys);
				veh_consume_fuel(playerid, vehicleid,
					pkeys.keys & KEY_SPRINT, &vparams, v);
			}

			if (game_is_air_vehicle(vehiclemodel)) {
				veh_timed_1s_update_a(
					playerid, vehicleid, &vpos, &vparams);

				if (vparams.engine && !isafk[playerid] &&
					lastcontrolactivity[playerid] > ctrla)
				{
					score_flight_time[playerid]++;
				}
			}
		}

	}
}

int veh_CreateVehicle(int model, struct vec4 pos, unsigned char col1,
	unsigned char col2, int respawn_delay, int addsiren)
{
	int vehicleid;

	NC_PARS(9);
	nc_params[1] = model;
	nc_paramf[2] = pos.coords.x;
	nc_paramf[3] = pos.coords.y;
	nc_paramf[4] = pos.coords.z;
	nc_paramf[5] = pos.r;
	nc_params[6] = col1;
	nc_params[7] = col2;
	nc_params[8] = respawn_delay;
	nc_params[9] = addsiren;
	vehicleid = NC(n_CreateVehicle_);
	gamevehicles[vehicleid].dbvehicle = NULL;
	gamevehicles[vehicleid].reincarnation++;
	gamevehicles[vehicleid].need_recreation = 0;
	return vehicleid;
}

int veh_DestroyVehicle(int vehicleid)
{
	if (gamevehicles[vehicleid].dbvehicle) {
		gamevehicles[vehicleid].dbvehicle->spawnedvehicleid = 0;
		gamevehicles[vehicleid].dbvehicle = NULL;
	}
	NC_PARS(1);
	nc_params[1] = vehicleid;
	return NC(n_DestroyVehicle_);
	/*DestroyVehicle triggers OnVehicleStreamOut,
	so no need to destroy labels here*/
}

/**
Creates a vehicle from a dbvehicle struct.
Makes sure references from vehicleid to db vehicle are updated.
*/
static
int veh_create_from_dbvehicle(struct dbvehicle *veh)
{
	int vehicleid = veh_CreateVehicle(
		veh->model, veh->pos, veh->col1, veh->col2,
		VEHICLE_RESPAWN_DELAY, 0);
	veh->spawnedvehicleid = vehicleid;
	gamevehicles[vehicleid].dbvehicle = veh;
	return vehicleid;
}

int veh_on_vehicle_spawn(int vehicleid)
{
	struct dbvehicle *veh;
	float min_fuel;

	veh = gamevehicles[vehicleid].dbvehicle;

	if (veh) {
		if (gamevehicles[vehicleid].need_recreation) {
			veh_DestroyVehicle(vehicleid);
			vehicleid = veh_create_from_dbvehicle(veh);
			if (vehicleid == INVALID_VEHICLE_ID) {
				/*expect things to crash*/
				logprintf("ERR: veh_OnVehicleSpawn: failed "
					  "to recreate vehicle");
			}
		}
		min_fuel = model_fuel_capacity(veh->model) * .25f;
		if (veh->fuel < min_fuel) {
			veh->fuel = min_fuel;
		}
	}
	/*this might've been already increased (due to recreate)
	but it doesn't matter, it just needs to change*/
	gamevehicles[vehicleid].reincarnation++;
	return vehicleid;
}

/**
Checks if a vehicle needs an owner label.

@param veh may be NULL
*/
static
int veh_needs_owner_label(struct dbvehicle *veh)
{
	return veh != NULL && veh->owneruserid;
}

/**
Creates a vehicle owner 3D text label for given player.

Does not check if the vehicle needs an owner label, use veh_needs_owner_label)
*/
static
void veh_owner_label_create(struct dbvehicle *veh, int vid, int playerid)
{
	if (labelids[playerid][vid] == INVALID_3DTEXT_ID) {
		atoc(buf144, veh->ownerstring, 144);
		NC_PARS(10);
		nc_params[1] = playerid;
		nc_params[2] = buf144a;
		nc_params[3] = 0xFFFF00FF;
		nc_paramf[4] = nc_paramf[5] = nc_paramf[6] = 0.0f;
		nc_paramf[7] = 75.0f;
		nc_params[8] = INVALID_PLAYER_ID;
		nc_params[9] = vid;
		nc_params[10] = 1; /*testLOS*/
		labelids[playerid][vid] = (short) NC(n_CreatePlayer3DTextLabel);
	}
}

/**
Destroys a vehicle owner 3D text label for given player.
*/
static
void veh_owner_label_destroy(int vehicleid, int playerid)
{
	struct dbvehicle *veh;
	short *labelid;

	veh = gamevehicles[vehicleid].dbvehicle;
	if (veh != NULL && veh->owneruserid != 0)
	{
		labelid = &labelids[playerid][vehicleid];
		if (*labelid != INVALID_3DTEXT_ID) {
			NC_DeletePlayer3DTextLabel(playerid, *labelid);
			*labelid = INVALID_3DTEXT_ID;
		}
	}
}

void veh_on_vehicle_stream_in(int vehicleid, int forplayerid)
{
	struct dbvehicle *veh;

	if (common_find_vehicle_driver(vehicleid) == INVALID_PLAYER_ID) {
		veh = gamevehicles[vehicleid].dbvehicle;
		if (veh_needs_owner_label(veh)) {
			veh_owner_label_create(veh, vehicleid, forplayerid);
		}
	}
}

void veh_on_vehicle_stream_out(int vehicleid, int forplayerid)
{
	veh_owner_label_destroy(vehicleid, forplayerid);
}

/**
Updates vehicle panel for a single player.
*/
static
void veh_update_panel_for_player(int playerid)
{
	struct dbvehicle *veh;
	int vehicleid;
	float odo, hp, fuel;
	short cache;

	vehicleid = NC_GetPlayerVehicleID(playerid);
	if (!vehicleid) {
		return;
	}

	veh = gamevehicles[vehicleid].dbvehicle;
	if (veh != NULL) {
		odo = veh->odoKM;
		fuel = veh->fuel;
		fuel /= model_fuel_capacity(veh->model);
	} else {
		fuel = odo = 0.0f;
	}

	hp = anticheat_GetVehicleHealth(vehicleid);
	hp -= 250.0f;
	if (hp < 0.0f) {
		hp = 0.0f;
	} else {
		hp /= 750.0f;
	}

	sprintf(cbuf64,
		"ODO %08.0f~n~_FL i-------i~n~_HP i-------i",
		odo);
	if (time_m % 2) {
		if (fuel < 0.2f) {
			cbuf64[16] = '_';
			cbuf64[17] = '_';
		}
		if (hp < 0.2f) {
			cbuf64[32] = '_';
			cbuf64[33] = '_';
		}
	}
	B144(cbuf64);
	NC_PARS(3);
	nc_params[1] = playerid;
	nc_params[2] = ptxt_txt[playerid];
	nc_params[3] = buf144a;
	NC(n_PlayerTextDrawSetString);

	cache = (short) (fuel * 100.0f);
	if (ptxtcache_fl[playerid] != cache) {
		ptxtcache_fl[playerid] = cache;
		NC_PARS(2);
		nc_params[1] = playerid;
		nc_params[2] = ptxt_fl[playerid];
		NC(n_PlayerTextDrawDestroy);

		buf144[0] = 'i';
		buf144[1] = 0;
		NC_PARS(4);
		nc_params[1] = playerid;
		nc_paramf[2] = 555.0f + (608.0f - 555.0f) * fuel;
		nc_paramf[3] = 413.0f;
		nc_params[4] = buf144a;
		ptxt_fl[playerid] = NC(n_CreatePlayerTextDraw);

		nc_params[2] = ptxt_fl[playerid];
		nc_paramf[3] = 0.25f;
		nc_paramf[4] = 1.0f;
		NC(n_PlayerTextDrawLetterSize);

		NC_PARS(3);
		nc_params[3] = 0xFF00FFFF;
		NC(n_PlayerTextDrawColor);
		NC(n_PlayerTextDrawBackgroundColor);
		nc_params[3] = 1,
		NC(n_PlayerTextDrawSetOutline);
		NC(n_PlayerTextDrawSetProportional);
		nc_params[3] = 2;
		NC(n_PlayerTextDrawFont);

		NC_PARS(2);
		NC(n_PlayerTextDrawShow);
	}

	cache = (short) (hp * 100.0f);
	if (ptxtcache_hp[playerid] != cache) {
		ptxtcache_hp[playerid] = cache;
		NC_PARS(2);
		nc_params[1] = playerid;
		nc_params[2] = ptxt_hp[playerid];
		NC(n_PlayerTextDrawDestroy);

		buf144[0] = 'i';
		buf144[1] = 0;
		NC_PARS(4);
		nc_params[1] = playerid;
		nc_paramf[2] = 555.0f + (608.0f - 555.0f) * hp;
		nc_paramf[3] = 422.0f;
		nc_params[4] = buf144a;
		ptxt_hp[playerid] = NC(n_CreatePlayerTextDraw);

		nc_params[2] = ptxt_hp[playerid];
		nc_paramf[3] = 0.25f;
		nc_paramf[4] = 1.0f;
		NC(n_PlayerTextDrawLetterSize);

		NC_PARS(3);
		nc_params[3] = 0xFF00FFFF;
		NC(n_PlayerTextDrawColor);
		NC(n_PlayerTextDrawBackgroundColor);
		nc_params[3] = 1,
		NC(n_PlayerTextDrawSetOutline);
		NC(n_PlayerTextDrawSetProportional);
		nc_params[3] = 2;
		NC(n_PlayerTextDrawFont);

		NC_PARS(2);
		NC(n_PlayerTextDrawShow);
	}
}

void veh_on_player_state_change(int playerid, int from, int to)
{
	struct dbvehicle *veh;
	int vehicleid, n, p;

	if (to == PLAYER_STATE_DRIVER || to == PLAYER_STATE_PASSENGER) {
		buf144[0] = '_';
		buf144[1] = 0;
		NC_PARS(4);
		nc_params[1] = playerid;
		nc_paramf[2] = -10.0f;
		nc_paramf[3] = -10.0f;
		nc_params[4] = buf144a;
		ptxt_hp[playerid] = NC(n_CreatePlayerTextDraw);
		ptxt_fl[playerid] = NC(n_CreatePlayerTextDraw);

		NC_PARS(2);
		nc_params[2] = txt_bg;
		NC(n_TextDrawShowForPlayer);
		nc_params[2] = ptxt_txt[playerid];
		NC(n_PlayerTextDrawShow);

		ptxtcache_fl[playerid] = -1;
		ptxtcache_hp[playerid] = -1;

		veh_update_panel_for_player(playerid);
	} else if (ptxt_fl[playerid] != -1 /*if panel active*/) {
		NC_PARS(2);
		nc_params[1] = playerid;
		nc_params[2] = txt_bg;
		NC(n_TextDrawHideForPlayer);
		nc_params[2] = ptxt_txt[playerid];
		NC(n_PlayerTextDrawHide);
		nc_params[2] = ptxt_fl[playerid];
		NC(n_PlayerTextDrawDestroy);
		nc_params[2] = ptxt_hp[playerid];
		NC(n_PlayerTextDrawDestroy);

		ptxt_fl[playerid] = -1;
		ptxt_hp[playerid] = -1;

		ptxtcache_fl[playerid] = -1;
		ptxtcache_hp[playerid] = -1;
	}

	if (to == PLAYER_STATE_DRIVER) {
		vehicleid = NC_GetPlayerVehicleID(playerid);
		veh_on_player_now_driving(playerid, vehicleid);

		NC_PARS(2);
		nc_params[1] = vehicleid;
		n = playercount;
		while (n--) {
			nc_params[2] = players[n];
			if (NC(n_IsVehicleStreamedIn)) {
				veh_owner_label_destroy(vehicleid, players[n]);
			}
		}
	}

	if (from == PLAYER_STATE_DRIVER &&
		(vehicleid = lastvehicle[playerid]) &&
		common_find_vehicle_driver(vehicleid) == INVALID_PLAYER_ID &&
		veh_needs_owner_label(veh = gamevehicles[vehicleid].dbvehicle))
	{
		NC_PARS(2);
		nc_params[1] = vehicleid;
		n = playercount;
		while (n--) {
			nc_params[2] = p = players[n];
			if (NC(n_IsVehicleStreamedIn)) {
				veh_owner_label_create(veh, vehicleid, p);
			}
		}
	}
}

void veh_timed_panel_update()
{
	int n = playercount;

	while (n--) {
		if (spawned[players[n]]) {
			veh_update_panel_for_player(players[n]);
		}
	}
}

void veh_create_player_textdraws(int playerid)
{
	ptxt_fl[playerid] = ptxt_hp[playerid] = -1;

	/*create em first*/
	NC_PARS(4);
	nc_params[1] = playerid;
	nc_paramf[2] = 528.0f;
	nc_paramf[3] = 404.0f;
	nc_params[4] = buf144a;
	B144("ODO 00000000~n~_FL i-------i~n~_HP i-------i");
	ptxt_txt[playerid] = NC(n_CreatePlayerTextDraw);

	/*letter sizes*/
	nc_params[2] = ptxt_txt[playerid];
	nc_paramf[3] = 0.25f;
	nc_paramf[4] = 1.0f;
	NC(n_PlayerTextDrawLetterSize);

	NC_PARS(3);
	/*rest*/
	nc_params[2] = ptxt_txt[playerid];
	nc_params[3] = -1;
	NC(n_PlayerTextDrawColor);
	nc_params[3] = 0,
	NC(n_PlayerTextDrawSetOutline);
	NC(n_PlayerTextDrawSetProportional);
	NC(n_PlayerTextDrawSetShadow);
	nc_params[3] = 2;
	NC(n_PlayerTextDrawFont);
}

void veh_create_global_textdraws()
{
	NC_PARS(3);
	nc_paramf[1] = 570.0f;
	nc_paramf[2] = 405.0f;
	nc_params[3] = buf144a;
	B144("~n~~n~~n~");
	txt_bg = NC(n_TextDrawCreate);
	nc_params[1] = txt_bg;
	nc_paramf[2] = 0.5f;
	nc_paramf[3] = 1.0f;
	NC(n_TextDrawLetterSize);
	nc_paramf[2] = 30.0f;
	nc_paramf[3] = 90.0f;
	NC(n_TextDrawTextSize);
	NC_PARS(2);
	nc_params[2] = 0x00000077; /*should be same as PANEL_BG*/
	NC(n_TextDrawBoxColor);
	nc_params[2] = 1;
	NC(n_TextDrawFont);
	NC(n_TextDrawUseBox);
	nc_params[2] = 2;
	NC(n_TextDrawAlignment);
}
