
/* vim: set filetype=c ts=8 noexpandtab: */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include "common.h"
#include "a_samp.h"
#include <string.h>
#include <math.h>
#include "game_sa.h"
#include "playerdata.h"
#include "vehicles.h"

#define SERVICE_MAP_DISTANCE 350.0f

struct vehnode {
	struct dbvehicle *veh;
	struct vehnode *next;
};

struct servicepoint {
	int id;
	float x, y, z;
};

struct mapservicepoint {
	struct servicepoint *svp;
	int textid;
};

static struct mapservicepoint mapservicepoints[MAX_PLAYERS][MAX_SERVICE_MAP_ICONS];
static int servicepointc;
static struct servicepoint *servicepoints;
static struct vehnode *vehiclestoupdate;
static struct dbvehicle *dbvehicles;
int dbvehiclenextid, dbvehiclealloc;
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

void veh_on_player_connect(AMX *amx, int playerid)
{
	ptxt_hp[playerid] = -1;
	ptxt_fl[playerid] = -1;
	ptxt_txt[playerid] = -1;
}

void veh_init()
{
	int i = MAX_PLAYERS, j;
	while (i--) {
		j = MAX_SERVICE_MAP_ICONS;
		while (j--) {
			mapservicepoints[i][j].svp = NULL;
		}
	}
	for (i = 0; i < MAX_VEHICLES; i++) {
		gamevehicles[i].dbvehicle = NULL;
		gamevehicles[i].reincarnation = 0;
		gamevehicles[i].need_recreation = 0;
	}
	vehiclestoupdate = NULL;
	servicepoints = NULL;
	servicepointc = 0;
	dbvehicles = NULL;
}

int
veh_can_player_modify_veh(int playerid, struct dbvehicle *veh)
{
	return veh &&
		(pdata[playerid]->userid == veh->owneruserid ||
		GROUPS_ISADMIN(pdata[playerid]->groups));
}

static int findServicePoint(float x, float y, float z)
{
	float dx, dy, dz;
	int i = servicepointc;

	while (i--) {
		dx = x - servicepoints[i].x;
		dy = y - servicepoints[i].y;
		dz = z - servicepoints[i].z;
		if (dx * dx + dy * dy + dz * dz < 50.0f * 50.0f) {
			return servicepoints[i].id;
		}
	}
	return -1;
}

static void freeDbVehicleTable()
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

static void resizeDbVehicleTable()
{
	struct dbvehicle *nw;
	int i;
	dbvehiclealloc += 100;
	dbvehicles = realloc(dbvehicles, sizeof(struct dbvehicle) * dbvehiclealloc);
	nw = dbvehicles;
	for (i = 0; i < dbvehiclenextid; i++) {
		if (nw->spawnedvehicleid) {
			gamevehicles[nw->spawnedvehicleid].dbvehicle = nw;
		}
		nw++;
	}
}

float model_fuel_capacity(int modelid)
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

static float model_fuel_usage(int modelid)
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

/* native Veh_Add(dbid, model, owneruserid, Float:x, Float:y, Float:z, Float:r, col1, col2, odo, ownername[]) */
cell AMX_NATIVE_CALL Veh_Add(AMX *amx, cell *params)
{
	cell *ownernameaddr;
	char ownername[MAX_PLAYER_NAME + 1];
	int model;
	struct dbvehicle *veh;
	if (dbvehiclenextid == dbvehiclealloc) {
		resizeDbVehicleTable();
	}
	veh = dbvehicles + dbvehiclenextid;
	dbvehiclenextid++;
	veh->id = params[1];
	veh->model = model = params[2];
	veh->owneruserid = params[3];
	veh->x = amx_ctof(params[4]);
	veh->y = amx_ctof(params[5]);
	veh->z = amx_ctof(params[6]);
	veh->r = amx_ctof(params[7]);
	veh->col1 = params[8];
	veh->col2 = params[9];
	veh->odo = (float) params[10];
	veh->fuel = model_fuel_capacity(model);
	if (veh->owneruserid != 0 && 400 <= model && model <= 611) {
		amx_GetAddr(amx, params[11], &ownernameaddr);
		amx_GetUString(ownername, ownernameaddr, sizeof(ownername));
		veh->ownerstringowneroffset = 7 + strlen(vehnames[model - 400]);
		veh->ownerstring = malloc((veh->ownerstringowneroffset + strlen(ownername) + 1) * sizeof(char));
		sprintf(veh->ownerstring, "%s Owner\n%s", vehnames[model - 400], ownername);
	} else {
		veh->owneruserid = 0;
		veh->ownerstring = NULL;
	}
	veh->spawnedvehicleid = 0;
	return dbvehiclenextid - 1;
}

/* native Float:Veh_AddOdo(vehicleid, playerid, Float:x1, Float:y1, Float:z1,
                           Float:x2, Float:y2, Float:z2, Float:podo) */
cell AMX_NATIVE_CALL Veh_AddOdo(AMX *amx, cell *params)
{
	void missions_add_distance(int, float);
	struct dbvehicle *veh;
	struct vehnode *updatequeue;
	const int vehicleid = params[1], playerid = params[2];
	const float dx = amx_ctof(params[3]) - amx_ctof(params[6]);
	const float dy = amx_ctof(params[4]) - amx_ctof(params[7]);
	const float dz = amx_ctof(params[5]) - amx_ctof(params[8]);
	float vs = sqrt(dx * dx + dy * dy + dz * dz) / 1000.0f;

	if ((amx_ftoc(vs) & 0x7F80000) == 0x7F80000 && (amx_ftoc(vs) & 0x007FFFFF) == 0) {
		vs = 0.0f;
	}

	if ((veh = gamevehicles[vehicleid].dbvehicle) != NULL) {
		veh->odo += vs;
		if (!veh->needsodoupdate) {
			veh->needsodoupdate = 1;
			if (vehiclestoupdate == NULL) {
				vehiclestoupdate = malloc(sizeof(struct vehnode));
				vehiclestoupdate->veh = veh;
				vehiclestoupdate->next = NULL;
			} else {
				updatequeue = vehiclestoupdate;
				while (updatequeue->next != NULL) {
					updatequeue = updatequeue->next;
				}
				updatequeue->next = malloc(sizeof(struct vehnode));
				updatequeue->next->veh = veh;
				updatequeue->next->next = NULL;
			}
		}
	}

	missions_add_distance(playerid, vs * 1000.0);
	vs += amx_ctof(params[9]);
	return amx_ftoc(vs);
}

/* native Veh_AddServicePoint(index, id, Float:x, Float:y, Float:z) */
cell AMX_NATIVE_CALL Veh_AddServicePoint(AMX *amx, cell *params)
{
	struct servicepoint *svp = servicepoints + params[1];
	svp->id = params[2];
	svp->x = amx_ctof(params[3]);
	svp->y = amx_ctof(params[4]);
	svp->z = amx_ctof(params[5]);
	return 1;
}

/* native Veh_CollectPlayerVehicles(userid, buf[]) */
cell AMX_NATIVE_CALL Veh_CollectPlayerVehicles(AMX *amx, cell *params)
{
	const int userid = params[1];
	int amount = 0;
	struct dbvehicle *veh = dbvehicles;
	int ctr = dbvehiclenextid;
	cell *addr;
	amx_GetAddr(amx, params[2], &addr);
	while (ctr--) {
		if (veh->owneruserid == userid) {
			*(addr++) = veh->col2;
			*(addr++) = veh->col1;
			*(addr++) = amx_ftoc(veh->r);
			*(addr++) = amx_ftoc(veh->z);
			*(addr++) = amx_ftoc(veh->y);
			*(addr++) = amx_ftoc(veh->x);
			*(addr++) = veh->model;
			*(addr++) = veh->id;
			amount++;
		}
		veh++;
	}
	return amount;
}

/* native Veh_CollectSpawnedVehicles(userid, buf[]) */
cell AMX_NATIVE_CALL Veh_CollectSpawnedVehicles(AMX *amx, cell *params)
{
	const int userid = params[1];
	int amount = 0;
	cell *addr;
	int i = MAX_VEHICLES;
	amx_GetAddr(amx, params[2], &addr);
	while (i--) {
		if (gamevehicles[i].dbvehicle != NULL &&
			gamevehicles[i].dbvehicle->owneruserid == userid)
		{
			*(addr++) = i;
			amount++;
		}
	}
	return amount;
}

static const char *MSG_FUEL_0 = WARN"Your vehicle ran out of fuel!";
static const char *MSG_FUEL_5 = WARN"Your vehicle has 5%% fuel left!";
static const char *MSG_FUEL_10 = WARN"Your vehicle has 10%% fuel left!";
static const char *MSG_FUEL_20 = WARN"Your vehicle has 20%% fuel left!";

/* native Veh_ConsumeFuel(vehicleid, throttle, &ranOutOfFuel, buf[]) */
cell AMX_NATIVE_CALL Veh_ConsumeFuel(AMX *amx, cell *params)
{
	const float consumptionmp = params[2] ? 1.0f : 0.2f;
	struct dbvehicle *veh;
	float fuelcapacity, lastpercentage, newpercentage;
	cell *addr;
	const char *buf;

	if ((veh = gamevehicles[params[1]].dbvehicle) == NULL) {
		return 0;
	}

	fuelcapacity = model_fuel_capacity(veh->model);
	lastpercentage = veh->fuel / fuelcapacity;
	veh->fuel -= model_fuel_usage(veh->model) * consumptionmp;
	if (veh->fuel < 0.0f) {
		veh->fuel = 0.0f;
	}
	newpercentage = veh->fuel / fuelcapacity;
	amx_GetAddr(amx, params[3], &addr);
	*addr = 0;
	if (lastpercentage > 0.0f && newpercentage == 0.0f) {
		buf = MSG_FUEL_0;
		*addr = 1;
	} else if (lastpercentage > 0.05f && newpercentage <= 0.05f) {
		buf = MSG_FUEL_5;
	} else if (lastpercentage > 0.1f && newpercentage <= 0.1f) {
		buf = MSG_FUEL_10;
	} else if (lastpercentage > 0.2f && newpercentage <= 0.2f) {
		buf = MSG_FUEL_20;
	} else {
		return 0;
	}
	amx_GetAddr(amx, params[4], &addr);
	amx_SetUString(addr, buf, 80);
	return 1;
}

/* native Veh_Destroy() */
cell AMX_NATIVE_CALL Veh_Destroy(AMX *amx, cell *params)
{
	if (dbvehicles != NULL) {
		freeDbVehicleTable();
	}
	if (servicepoints != NULL) {
		free(servicepoints);
		servicepoints = NULL;
		servicepointc = 0;
	}
	return 1;
}

/* native Veh_GetLabelToDelete(vehicleid, playerid, &PlayerText3D:labelid) */
cell AMX_NATIVE_CALL Veh_GetLabelToDelete(AMX *amx, cell *params)
{
	const int vehicleid = params[1], playerid = params[2];
	const int labelid = labelids[playerid][vehicleid];
	cell *addr;
	if (labelid == -1) {
		return 0;
	}
	labelids[playerid][vehicleid] = -1;
	amx_GetAddr(amx, params[3], &addr);
	*addr = labelid;
	return 1;
}

/* native Veh_GetNextUpdateQuery(buf[]) */
cell AMX_NATIVE_CALL Veh_GetNextUpdateQuery(AMX *amx, cell *params)
{
	cell *addr;
	struct dbvehicle *veh;
	struct vehnode *tofree;
	char buf[80];

	if (vehiclestoupdate == NULL) {
		return 0;
	}
	veh = vehiclestoupdate->veh;
	tofree = vehiclestoupdate;
	vehiclestoupdate = vehiclestoupdate->next;
	free(tofree);
	veh->needsodoupdate = 0;
	sprintf(buf, "UPDATE veh SET odo=%d WHERE i=%d", (int) veh->odo, veh->id);
	amx_GetAddr(amx, params[1], &addr);
	amx_SetUString(addr, buf, sizeof(buf));
	return 1;
}

/* native Veh_Init(dbvehiclecount) */
cell AMX_NATIVE_CALL Veh_Init(AMX *amx, cell *params)
{
	int i = dbvehiclealloc = params[1];
	int pid;
	if (dbvehicles != NULL) {
		logprintf("Veh_Init: warning: dbvehicles table was not empty");
		freeDbVehicleTable();
	}
	dbvehiclenextid = 0;
	if (dbvehiclealloc <= 0) {
		dbvehiclealloc = 100;
	}
	dbvehicles = malloc(sizeof(struct dbvehicle) * dbvehiclealloc);
	i = 0;
	while (i < MAX_VEHICLES) {
		gamevehicles[i].dbvehicle = NULL;
		i++;
	}
	pid = MAX_PLAYERS;
	while (pid--) {
		for (i = 0; i < MAX_VEHICLES; i++) {
			labelids[pid][i] = -1;
		}
	}
	return 1;
}

/* native Veh_InitServicePoints(servicepointscount) */
cell AMX_NATIVE_CALL Veh_InitServicePoints(AMX *amx, cell *params)
{
	if (servicepoints != NULL) {
		logprintf("Veh_InitServicePoints: servicepoints table was not empty");
		free(servicepoints);
	}
	servicepointc = params[1];
	servicepoints = malloc(sizeof(struct servicepoint) * servicepointc);
	return 1;
}

/* native Veh_IsFuelEmpty(vehicleid) */
cell AMX_NATIVE_CALL Veh_IsFuelEmpty(AMX *amx, cell *params)
{
	struct dbvehicle *veh;
	if ((veh = gamevehicles[params[1]].dbvehicle) == NULL) {
		return 0;
	}
	return veh->fuel == 0.0f;
}

/* native Veh_IsPlayerAllowedInVehicle(userid, vehicleid, buf[]) */
cell AMX_NATIVE_CALL Veh_IsPlayerAllowedInVehicle(AMX *amx, cell *params)
{
	const int userid = params[1], vehicleid = params[2];
	cell *addr;
	char buf[144];
	struct dbvehicle *veh;
	if (gamevehicles[vehicleid].dbvehicle == NULL) {
		logprintf("Veh_IsPlayerAllowedInVehicle: unknown vehicleid");
		return 1;
	}
	veh = gamevehicles[vehicleid].dbvehicle;
	if (veh->owneruserid == 0 || veh->owneruserid == userid) {
		return 1;
	}
	sprintf(buf, WARN"This vehicle belongs to %s!", &veh->ownerstring[veh->ownerstringowneroffset]);
	amx_GetAddr(amx, params[3], &addr);
	amx_SetUString(addr, buf, sizeof(buf));
	return 0;
}

/* native Veh_OnPlayerDisconnect(playerid) */
cell AMX_NATIVE_CALL Veh_OnPlayerDisconnect(AMX *amx, cell *params)
{
	short *labelid = &labelids[params[1]][0];
	int i = MAX_VEHICLES;
	while (i--) {
		*labelid = -1;
		labelid++;
	}
	return 1;
}

static const char *MSG_REFUEL_NEED_SVP = WARN"You need to be at a service point to do this!";

/* native Veh_Refuel(Float:x, Float:y, Float:z, vehicleid, playerid, Float:priceperlitre,
                     budget, &refuelamount, msg[], querybuf[]) */
cell AMX_NATIVE_CALL Veh_Refuel(AMX *amx, cell *params)
{
	struct dbvehicle *veh;
	const int vehicleid = params[4], playerid = params[5], budget = params[7];
	int cost, svpid, userid;
	const float priceperlitre = amx_ctof(params[6]);
	float capacity, refuelamount;
	cell *addr;
	char buf1[11], buf[144];

	amx_GetAddr(amx, params[9], &addr);
	if ((svpid = findServicePoint(amx_ctof(params[1]), amx_ctof(params[2]), amx_ctof(params[3]))) == -1) {
		amx_SetUString(addr, MSG_REFUEL_NEED_SVP, 144);
		return 0;
	}
	if ((veh = gamevehicles[vehicleid].dbvehicle) == NULL ||
		(refuelamount = (capacity = model_fuel_capacity(veh->model)) - veh->fuel) < 1.0f)
	{
		sprintf(buf,
		        WARN"This vehicle is already fueled up! capacity: %.0f/%.0f",
		        veh->fuel,
		        capacity);
		amx_SetUString(addr, buf, sizeof(buf));
		return 0;
	}

	cost = 50 + (int) (refuelamount * priceperlitre);
	if (cost > budget) {
		refuelamount = (budget - 50) / priceperlitre;
		if (refuelamount <= 0) {
			sprintf(buf,
			        WARN"You can't pay the refuel fee! capacity: %.0f/%.0f",
			        veh->fuel,
			        capacity);
			amx_SetUString(addr, buf, sizeof(buf));
			return 0;
		}
		cost = budget;
		strcpy(buf1, "partially");
	} else {
		strcpy(buf1, "fully");
	}

	veh->fuel += refuelamount;
	sprintf(buf,
		INFO"Your vehicle has been %s refueled for $%d (%.2f litres @ $%.2f/litre) capacity: %.0f/%.0f",
	        buf1,
	        cost,
	        refuelamount,
	        priceperlitre,
	        veh->fuel,
	        capacity);
	amx_SetUString(addr, buf, sizeof(buf));

	amx_GetAddr(amx, params[8], &addr);
	*addr = amx_ftoc(refuelamount);

	amx_GetAddr(amx, params[10], &addr);
	if (veh == NULL ||
		pdata[playerid] == NULL ||
		(userid = pdata[playerid]->userid) < 1)
	{
		logprintf("Veh_Repair: unknown vehicle or unknown playerid");
		*addr = 0;
		return cost;
	}
	sprintf(buf,
	        "INSERT INTO refuellog(stamp,vehicle,player,svp,paid,fuel) "
	        "VALUES (UNIX_TIMESTAMP(),%d,%d,%d,%d,%.4f)",
	        veh->id,
		userid,
		svpid,
		cost,
		refuelamount);
	amx_SetUString(addr, buf, sizeof(buf));
	return cost;
}

/* native Veh_RegisterLabel(vehicleid, playerid, PlayerText3D:labelid) */
cell AMX_NATIVE_CALL Veh_RegisterLabel(AMX *amx, cell *params)
{
	const int vehicleid = params[1], playerid = params[2], labelid = params[3];
	labelids[playerid][vehicleid] = labelid;
	return 1;
}

static const char *MSG_REPAIR_NONEED = WARN"Your vehicle doesn't need to be repaired!";
static const char *MSG_REPAIR_NOAFFORD = WARN"You can't afford the repair fee!";

/* native Veh_Repair(Float:x, Float:y, Float:z, vehicleid, playerid, budget,
                     Float:hp, &Float:newhp, buf[], querybuf[]) */
cell AMX_NATIVE_CALL Veh_Repair(AMX *amx, cell *params)
{
	struct dbvehicle *veh;
	const int vehicleid = params[4], playerid = params[5], budget = params[6];
	cell *addr;
	const float hp = amx_ctof(params[7]);
	float fixamount, newhp;
	int cost, svpid, userid;
	char buf1[11], buf[144];

	amx_GetAddr(amx, params[9], &addr);
	if ((svpid = findServicePoint(amx_ctof(params[1]), amx_ctof(params[2]), amx_ctof(params[3]))) == -1) {
		amx_SetUString(addr, MSG_REFUEL_NEED_SVP, 144);
		return 0;
	}
	if (hp > 999.9f) {
		amx_SetUString(addr, MSG_REPAIR_NONEED, 144);
		return 0;
	}

	fixamount = 1000.0f - hp;
	cost = 150 + (int) (fixamount * 2.0f);
	if (cost > budget) {
		fixamount = (float) ((budget - 150) / 2);
		if (fixamount <= 0.0f) {
			amx_SetUString(addr, MSG_REPAIR_NOAFFORD, 144);
			return 0;
		}
		cost = budget;
		strcpy(buf1, "partially");
	} else {
		strcpy(buf1, "fully");
	}
	newhp = fixamount + hp;

	sprintf(buf, INFO"Your vehicle has been %s repaired for $%d", buf1, cost);
	amx_SetUString(addr, buf, sizeof(buf));

	amx_GetAddr(amx, params[8], &addr);
	*addr = amx_ftoc(newhp);

	amx_GetAddr(amx, params[10], &addr);
	if ((veh = gamevehicles[vehicleid].dbvehicle) == NULL ||
		pdata[playerid] == NULL ||
		(userid = pdata[playerid]->userid) < 1)
	{
		logprintf("Veh_Repair: unknown vehicle or unknown playerid");
		*addr = 0;
		return cost;
	}
	sprintf(buf,
	        "INSERT INTO repairlog(stamp,vehicle,player,svp,paid,damage) "
	        "VALUES (UNIX_TIMESTAMP(),%d,%d,%d,%d,%d)",
	        veh->id,
		userid,
		svpid,
		cost,
		(int) fixamount);
	amx_SetUString(addr, buf, sizeof(buf));
	return cost;
}

/* native Veh_ShouldCreateLabel(vehicleid, playerid, buf[]) */
cell AMX_NATIVE_CALL Veh_ShouldCreateLabel(AMX *amx, cell *params)
{
	const int vehicleid = params[1], playerid = params[2];
	const struct vehicle veh = gamevehicles[vehicleid];
	cell *addr;
	if (veh.dbvehicle == NULL) {
		logprintf("Veh_ShouldCreateLabel: unknown vehicle");
		return 0;
	}
	if (labelids[playerid][vehicleid] != -1) {
		return 0;
	}
	if (veh.dbvehicle->owneruserid == 0) {
		return 0;
	}
	amx_GetAddr(amx, params[3], &addr);
	amx_SetUString(addr, veh.dbvehicle->ownerstring, 144);
	return 1;
}

/* native Veh_UpdateServicePointTextId(playerid, index, PlayerText3D:textid) */
cell AMX_NATIVE_CALL Veh_UpdateServicePointTextId(AMX *amx, cell *params)
{
	const int playerid = params[1], index = params[2], textid = params[3];

	if (0 <= index && index < MAX_SERVICE_MAP_ICONS) {
		mapservicepoints[playerid][index].textid = textid;
	}

	return 1;
}

/* native Veh_UpdateServicePtsVisibility(playerid, Float:x, Float:y, data[]) */
cell AMX_NATIVE_CALL Veh_UpdateServicePtsVisibility(AMX *amx, cell *params)
{
	struct servicepoint *pt;
	const int playerid = params[1];
	struct mapservicepoint *msvp = mapservicepoints[playerid];
	const float x = amx_ctof(params[2]), y = amx_ctof(params[3]);
	int amount = 0, i, j, existing[MAX_SERVICE_MAP_ICONS], existingc = 0;
	float dx, dy;
	cell *addr;

	amx_GetAddr(amx, params[4], &addr);
	i = MAX_SERVICE_MAP_ICONS;
	while (i--) {
		*(addr + i * 5) = -2;
	}

	/* remove old, now out of range ones */
	i = MAX_SERVICE_MAP_ICONS;
	while (i--) {
		if ((pt = msvp[i].svp) != NULL) {
			dx = pt->x - x;
			dy = pt->y - y;
			if (dx * dx + dy * dy > SERVICE_MAP_DISTANCE * SERVICE_MAP_DISTANCE) {
				*(addr++) = msvp[i].textid;
				*addr = i;
				addr += 4;
				msvp[i].svp = NULL;
				amount++;
			} else {
				existing[existingc++] = pt->id;
			}
		}
	}

	/* add new ones */
	i = servicepointc;
	while (1) {
nextsvp:
		if (!i--) {
			return amount;
		}
		dx = x - servicepoints[i].x;
		dy = y - servicepoints[i].y;
		if (dx * dx + dy * dy < SERVICE_MAP_DISTANCE * SERVICE_MAP_DISTANCE) {
			j = existingc;
			while (j--) {
				if (existing[j] == servicepoints[i].id) {
					goto nextsvp;
				}
			}
			j = MAX_SERVICE_MAP_ICONS;
			while (j--) {
				if (msvp[j].svp == NULL) {
					msvp[j].svp = servicepoints + i;
					*(addr++) = -1;
					*(addr++) = j;
					*(addr++) = amx_ftoc(servicepoints[i].x);
					*(addr++) = amx_ftoc(servicepoints[i].y);
					*(addr++) = amx_ftoc(servicepoints[i].z);
					amount++;
					goto nextsvp;
				}
			}
			return amount;
		}
	}

	return amount;
}

/* native Veh_UpdateSlot(vehicleid, dbid) */
cell AMX_NATIVE_CALL Veh_UpdateSlot(AMX *amx, cell *params)
{
	const int vehicleid = params[1], dbid = params[2];
	int i = dbvehiclealloc;
	struct dbvehicle *veh = dbvehicles;

	if (dbid == -1) {
		if (gamevehicles[vehicleid].dbvehicle == NULL) {
			logprintf("Veh_UpdateSlot: slot to empty was empty");
			return 1;
		}
		gamevehicles[vehicleid].dbvehicle->spawnedvehicleid = 0;
		gamevehicles[vehicleid].dbvehicle = NULL;
		return 1;
	}
	while (i--) {
		if (veh->id == dbid) {
			if (gamevehicles[vehicleid].dbvehicle != NULL) {
				logprintf("Veh_UpdateSlot: slot to assign was not empty");
				gamevehicles[vehicleid].dbvehicle->spawnedvehicleid = 0;
			}
			gamevehicles[vehicleid].dbvehicle = veh;
			veh->spawnedvehicleid = vehicleid;
			return 1;
		}
		veh++;
	}
	gamevehicles[vehicleid].dbvehicle = NULL;
	logprintf("Veh_UpdateSlot: unknown dbid: %d", dbid);
	return 0;
}

/**
Creates a vehicle (see CreateVehicle doc).

@remarks Resets values of gamevehicles struct reflecting the created vehicle.
*/
int
veh_NC_CreateVehicle(AMX *amx, int model, float x, float y, float z,
		     float r, int col1, int col2, int respawn_delay,
		     int addsiren)
{
	int vehicleid;

	nc_params[0] = 9;
	nc_params[1] = model;
	nc_params[2] = amx_ftoc(x);
	nc_params[3] = amx_ftoc(y);
	nc_params[4] = amx_ftoc(z);
	nc_params[5] = amx_ftoc(r);
	nc_params[6] = col1;
	nc_params[7] = col2;
	nc_params[8] = respawn_delay;
	nc_params[9] = addsiren;
	amx_Callback(amx, n_CreateVehicle_, (cell*) &vehicleid, nc_params);
	gamevehicles[vehicleid].dbvehicle = NULL;
	gamevehicles[vehicleid].reincarnation++;
	gamevehicles[vehicleid].need_recreation = 0;
	return vehicleid;
}

int
veh_NC_DestroyVehicle(AMX *amx, int vehicleid)
{
	if (gamevehicles[vehicleid].dbvehicle) {
		gamevehicles[vehicleid].dbvehicle->spawnedvehicleid = 0;
		gamevehicles[vehicleid].dbvehicle = NULL;
	}
	nc_params[0] = 1;
	nc_params[1] = vehicleid;
	amx_Callback(amx, n_DestroyVehicle_, &nc_result, nc_params);
	return nc_result;
}

/**
Creates a vehicle from a dbvehicle struct.
Makes sure references from vehicleid to db vehicle are updated.
*/
int
veh_create_from_dbvehicle(AMX *amx, struct dbvehicle *veh)
{
	int vehicleid = veh_NC_CreateVehicle(
		amx, veh->model, veh->x, veh->y, veh->z,
		veh->r, veh->col1, veh->col2, VEHICLE_RESPAWN_DELAY, 0);
	veh->spawnedvehicleid = vehicleid;
	gamevehicles[vehicleid].dbvehicle = veh;
	return vehicleid;
}

int
veh_OnVehicleSpawn(AMX *amx, int vehicleid)
{
	struct dbvehicle *veh;
	float min_fuel;

	veh = gamevehicles[vehicleid].dbvehicle;

	if (veh) {
		if (gamevehicles[vehicleid].need_recreation) {
			veh_NC_DestroyVehicle(amx, vehicleid);
			vehicleid = veh_create_from_dbvehicle(amx, veh);
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

static void veh_update_panel_for_player(AMX *amx, int playerid)
{
	struct dbvehicle *veh;
	int vehicleid;
	float odo, hp, fuel;
	short cache;

	NC_GetPlayerVehicleID_(playerid, &vehicleid);
	if (!vehicleid) {
		return;
	}

	veh = gamevehicles[vehicleid].dbvehicle;

	if (veh != NULL) {
		odo = veh->odo;
		fuel = veh->fuel;
		fuel /= model_fuel_capacity(veh->model);
	} else {
		fuel = odo = 0.0f;
	}

	hp = anticheat_NC_GetVehicleHealth(amx, vehicleid);
	hp -= 250.0f;
	if (hp < 0.0f) {
		hp = 0.0f;
	} else {
		hp /= 750.0f;
	}

	sprintf((char*) buf64,
		"ODO %08.0f~n~_FL i-------i~n~_HP i-------i",
		veh->odo);
	if (time_m % 2) {
		if (fuel < 0.2f) {
			((char*) buf64)[16] = '_';
			((char*) buf64)[17] = '_';
		}
		if (hp < 0.2f) {
			((char*) buf64)[32] = '_';
			((char*) buf64)[33] = '_';
		}
	}
	amx_SetUString(buf144, (char*) buf64, 64);
	nc_params[0] = 3;
	nc_params[1] = playerid;
	nc_params[2] = ptxt_txt[playerid];
	nc_params[3] = buf144a;
	NC(n_PlayerTextDrawSetString);

	cache = (short) (fuel * 100.0f);
	if (ptxtcache_fl[playerid] != cache) {
		ptxtcache_fl[playerid] = cache;
		nc_params[0] = 2;
		nc_params[1] = playerid;
		nc_params[2] = ptxt_fl[playerid];
		NC(n_PlayerTextDrawDestroy);

		buf144[0] = 'i';
		buf144[1] = 0;
		nc_params[0] = 4;
		nc_params[1] = playerid;
		*((float*) (nc_params + 2)) =
			555.0f + (608.0f - 555.0f) * fuel;
		*((float*) (nc_params + 3)) = 413.0f;
		nc_params[4] = buf144a;
		NC_(n_CreatePlayerTextDraw, ptxt_fl + playerid);

		nc_params[2] = ptxt_fl[playerid];
		*((float*) (nc_params + 3)) = 0.25f;
		*((float*) (nc_params + 4)) = 1.0f;
		NC(n_PlayerTextDrawLetterSize);

		nc_params[0] = 3;
		nc_params[3] = 0xFF00FFFF;
		NC(n_PlayerTextDrawColor);
		NC(n_PlayerTextDrawBackgroundColor);
		nc_params[3] = 1,
		NC(n_PlayerTextDrawSetOutline);
		NC(n_PlayerTextDrawSetProportional);
		nc_params[3] = 2;
		NC(n_PlayerTextDrawFont);

		nc_params[0] = 2;
		NC(n_PlayerTextDrawShow);
	}

	cache = (short) (hp * 100.0f);
	if (ptxtcache_hp[playerid] != cache) {
		ptxtcache_hp[playerid] = cache;
		nc_params[0] = 2;
		nc_params[1] = playerid;
		nc_params[2] = ptxt_hp[playerid];
		NC(n_PlayerTextDrawDestroy);

		buf144[0] = 'i';
		buf144[1] = 0;
		nc_params[0] = 4;
		nc_params[1] = playerid;
		*((float*) (nc_params + 2)) =
			555.0f + (608.0f - 555.0f) * hp;
		*((float*) (nc_params + 3)) = 422.0f;
		nc_params[4] = buf144a;
		NC_(n_CreatePlayerTextDraw, ptxt_hp + playerid);

		nc_params[2] = ptxt_hp[playerid];
		*((float*) (nc_params + 3)) = 0.25f;
		*((float*) (nc_params + 4)) = 1.0f;
		NC(n_PlayerTextDrawLetterSize);

		nc_params[0] = 3;
		nc_params[3] = 0xFF00FFFF;
		NC(n_PlayerTextDrawColor);
		NC(n_PlayerTextDrawBackgroundColor);
		nc_params[3] = 1,
		NC(n_PlayerTextDrawSetOutline);
		NC(n_PlayerTextDrawSetProportional);
		nc_params[3] = 2;
		NC(n_PlayerTextDrawFont);

		nc_params[0] = 2;
		NC(n_PlayerTextDrawShow);
	}
}

void veh_on_player_state_change(AMX *amx, int playerid, int from, int to)
{
	if (to == PLAYER_STATE_DRIVER || to == PLAYER_STATE_PASSENGER) {
		buf144[0] = '_';
		buf144[1] = 0;
		nc_params[0] = 4;
		nc_params[1] = playerid;
		*((float*) (nc_params + 2)) = -10.0f;
		*((float*) (nc_params + 3)) = -10.0f;
		nc_params[4] = buf144a;
		NC_(n_CreatePlayerTextDraw, ptxt_hp + playerid);
		NC_(n_CreatePlayerTextDraw, ptxt_fl + playerid);

		nc_params[0] = 2;
		nc_params[2] = txt_bg;
		NC(n_TextDrawShowForPlayer);
		nc_params[2] = ptxt_txt[playerid];
		NC(n_PlayerTextDrawShow);

		ptxtcache_fl[playerid] = -1;
		ptxtcache_hp[playerid] = -1;

		veh_update_panel_for_player(amx, playerid);
	} else if (ptxt_fl[playerid] != -1 /*if panel active*/) {
		nc_params[0] = 2;
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
}

void veh_timed_panel_update(AMX *amx)
{
	int n = playercount;

	while (n--) {
		if (spawned[players[n]]) {
			veh_update_panel_for_player(amx, players[n]);
		}
	}
}

/**
Should be called in OnPlayerConnect.
*/
void veh_create_player_textdraws(AMX *amx, int playerid)
{
	float *f2, *f3, *f4;

	ptxt_fl[playerid] = ptxt_hp[playerid] = -1;

	f2 = (float*) (nc_params + 2);
	f3 = (float*) (nc_params + 3);
	f4 = (float*) (nc_params + 4);

	/*create em first*/
	nc_params[0] = 4;
	nc_params[1] = playerid;
	*f2 = 528.0f;
	*f3 = 404.0f;
	nc_params[4] = buf144a;
	SETB144("ODO 00000000~n~_FL i-------i~n~_HP i-------i");
	NC_(n_CreatePlayerTextDraw, ptxt_txt + playerid);

	/*letter sizes*/
	nc_params[2] = ptxt_txt[playerid];
	*f3 = 0.25f;
	*f4 = 1.0f;
	NC(n_PlayerTextDrawLetterSize);

	nc_params[0] = 3;
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

/**
Should be called in OnGameModeInit.
*/
void veh_create_global_textdraws(AMX *amx)
{
	float *f1, *f2, *f3;

	f1 = (float*) (nc_params + 1);
	f2 = (float*) (nc_params + 2);
	f3 = (float*) (nc_params + 3);

	nc_params[0] = 3;
	*f1 = 570.0f;
	*f2 = 405.0f;
	nc_params[3] = buf144a;
	SETB144("~n~~n~~n~");
	NC_(n_TextDrawCreate, &txt_bg);
	nc_params[1] = txt_bg;
	*f2 = 0.5f;
	*f3 = 1.0f;
	NC(n_TextDrawLetterSize);
	*f2 = 30.0f;
	*f3 = 90.0f;
	NC(n_TextDrawTextSize);
	nc_params[0] = 2;
	nc_params[2] = 0x00000077; /*should be same as PANEL_BG*/
	NC(n_TextDrawBoxColor);
	nc_params[2] = 1;
	NC(n_TextDrawFont);
	NC(n_TextDrawUseBox);
	nc_params[2] = 2;
	NC(n_TextDrawAlignment);
}
