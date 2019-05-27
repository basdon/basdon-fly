
/* vim: set filetype=c ts=8 noexpandtab: */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include "common.h"
#include <string.h>
#include <math.h>
#include "game_sa.h"
#include "vehicles.h"

struct vehnode {
	struct dbvehicle *veh;
	struct vehnode *next;
};

static struct vehnode *vehiclestoupdate;
static struct dbvehicle *dbvehicles;
int dbvehiclenextid, dbvehiclealloc;
struct vehicle gamevehicles[MAX_VEHICLES];
short labelids[MAX_PLAYERS][MAX_VEHICLES]; /* 200KB+ of mapping errrr */
int panel_hpflcache[MAX_PLAYERS];
int panel_odocache[MAX_PLAYERS];

#define INVALID_HPFL_CACHE 2147483647
#define INVALID_ODO_CACHE INVALID_HPFL_CACHE

void veh_init()
{
	vehiclestoupdate = NULL;
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
		if (nw->spawnedvehicleid != -1) {
			gamevehicles[nw->spawnedvehicleid].dbvehicle = nw;
		}
		nw++;
	}
}

static float model_fuel_capacity(int modelid)
{
	switch (modelid)
	{
	default: return 1000.0f;
	}
}

static float model_fuel_usage(int modelid)
{
	switch (modelid)
	{
	default: return 1.0f;
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
	veh->spawnedvehicleid = -1;
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

/* native Veh_ConsumeFuel(vehicleid, throttle, &ranOutOfFuel, buf[]) */
cell AMX_NATIVE_CALL Veh_ConsumeFuel(AMX *amx, cell *params)
{
	const float consumptionmp = params[2] ? 1.0f : 0.2f;
	struct dbvehicle *veh;
	float fuelcapacity, lastpercentage, newpercentage;
	cell *addr;
	char buf[80];

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
		*addr = 1;
		strcpy(buf, WARN"Your vehicle ran out of fuel!");
		amx_GetAddr(amx, params[4], &addr);
		amx_SetUString(addr, buf, sizeof(buf));
		return 1;
	}
	if (lastpercentage > 0.05f && newpercentage <= 0.05f) {
		strcpy(buf, WARN"Your vehicle has 5%% fuel left!");
		amx_GetAddr(amx, params[4], &addr);
		amx_SetUString(addr, buf, sizeof(buf));
		return 1;
	}
	if (lastpercentage > 0.1f && newpercentage <= 0.1f) {
		strcpy(buf, WARN"Your vehicle has 10%% fuel left!");
		amx_GetAddr(amx, params[4], &addr);
		amx_SetUString(addr, buf, sizeof(buf));
		return 1;
	}
	if (lastpercentage > 0.2f && newpercentage <= 0.2f) {
		strcpy(buf, WARN"Your vehicle has 20%% fuel left!");
		amx_GetAddr(amx, params[4], &addr);
		amx_SetUString(addr, buf, sizeof(buf));
		return 1;
	}
	return 0;
}

/* native Veh_Destroy() */
cell AMX_NATIVE_CALL Veh_Destroy(AMX *amx, cell *params)
{
	if (dbvehicles != NULL) {
		freeDbVehicleTable();
	}
	return 1;
}

/* native Veh_EnsureHasFuel(vehicleid) */
cell AMX_NATIVE_CALL Veh_EnsureHasFuel(AMX *amx, cell *params)
{
	struct dbvehicle *veh;
	float minamount;
	if ((veh = gamevehicles[params[1]].dbvehicle) != NULL) {
		if (veh->fuel < (minamount = model_fuel_capacity(veh->model) * .1f)) {
			veh->fuel = minamount;
		}
	}
	return 1;
}

/* native Veh_FormatPanelText(int playerid, int vehicleid, Float:vehiclehp, buf[]) */
cell AMX_NATIVE_CALL Veh_FormatPanelText(AMX *amx, cell *params)
{
	struct dbvehicle *veh;
	const int pid = params[1], vid = params[2];
	const int vehiclehp = amx_ctof(params[3]);
	cell *addr;
	char buf[85];
	int fuel, fuelcapacity, panel_hpflcachevalue, panel_odocachevalue;
	float odo = 0.0f;

	fuel = fuelcapacity = 0;
	if ((veh = gamevehicles[vid].dbvehicle) != NULL) {
		fuel = veh->fuel;
		fuelcapacity = model_fuel_capacity(veh->model);
		odo = veh->odo;
	}

	;
	if ((panel_hpflcachevalue = fuel * 10000 + vehiclehp) == panel_hpflcache[pid] &&
	    (panel_odocachevalue = odo * 10.0f) == panel_odocache[pid])
	{
		return 0;
	}
	panel_hpflcache[pid] = panel_hpflcachevalue;
	panel_odocache[pid] = panel_odocachevalue;

	sprintf(buf,
	        "HP: ~g~%4d/1000  ~w~FL: ~g~%d/%d~n~~w~ODO: %08.1fkm",
	        vehiclehp,
	        fuel,
	        fuelcapacity,
	        odo);
	if (vehiclehp < 350) {
		buf[5] = 'r';
	}
	if (fuel <= (int) ((float) fuelcapacity * 0.2f)) {
		buf[26] = 'r';
	}
	amx_GetAddr(amx, params[4], &addr);
	amx_SetUString(addr, buf, sizeof(buf));
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

/* native Veh_Refuel(vehicleid, Float:priceperlitre, budget, &refuelamount, msg[]) */
cell AMX_NATIVE_CALL Veh_Refuel(AMX *amx, cell *params)
{
	struct dbvehicle *veh;
	const int budget = params[3];
	int cost;
	const float priceperlitre = amx_ctof(params[2]);
	float capacity, refuelamount;
	cell *addr;
	char buf1[11], buf[144];

	amx_GetAddr(amx, params[5], &addr);
	if ((veh = gamevehicles[params[1]].dbvehicle) == NULL ||
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

	amx_GetAddr(amx, params[4], &addr);
	*addr = amx_ftoc(refuelamount);
	return cost;
}

/* native Veh_RegisterLabel(vehicleid, playerid, PlayerText3D:labelid) */
cell AMX_NATIVE_CALL Veh_RegisterLabel(AMX *amx, cell *params)
{
	const int vehicleid = params[1], playerid = params[2], labelid = params[3];
	labelids[playerid][vehicleid] = labelid;
	return 1;
}

/* native Veh_Repair(budget, Float:hp, &Float:newhp, buf[]) */
cell AMX_NATIVE_CALL Veh_Repair(AMX *amx, cell *params)
{
	const int budget = params[1];
	cell *addr;
	const float hp = amx_ctof(params[2]);
	float fixamount;
	int cost;
	char buf1[11], buf[144];

	amx_GetAddr(amx, params[4], &addr);
	if (hp > 999.9f) {
		strcpy(buf, "Your vehicle doesn't need to be repaired!");
		amx_SetUString(addr, buf, sizeof(buf));
		return 0;
	}

	fixamount = 1000.0f - hp;
	cost = 150 + (int) (fixamount * 2.0f);
	if (cost > budget) {
		fixamount = (float) ((budget - 150) / 2);
		if (fixamount <= 0.0f) {
			strcpy(buf, "You can't afford the repair fee!");
			amx_SetUString(addr, buf, sizeof(buf));
			return 0;
		}
		cost = budget;
		strcpy(buf1, "partially");
	} else {
		strcpy(buf1, "fully");
	}

	sprintf(buf, INFO"Your vehicle has been %s repaired for $%d", buf1, cost);
	amx_SetUString(addr, buf, sizeof(buf));

	amx_GetAddr(amx, params[3], &addr);
	fixamount += hp;
	*addr = amx_ftoc(fixamount);
	return cost;
}

/* native Veh_ResetPanelTextCache(playerid) */
cell AMX_NATIVE_CALL Veh_ResetPanelTextCache(AMX *amx, cell *params)
{
	const int pid = params[1];
	panel_hpflcache[pid] = INVALID_HPFL_CACHE;
	panel_odocache[pid] = INVALID_ODO_CACHE;
	return 1;
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

/* native Veh_UpdateSlot(vehicleid, dbid) */
cell AMX_NATIVE_CALL Veh_UpdateSlot(AMX *amx, cell *params)
{
	const int vehicleid = params[1], dbid = params[2];
	int i = dbvehiclealloc;
	struct dbvehicle *veh = dbvehicles;
	if (dbid == -1) {
		if (gamevehicles[vehicleid].dbvehicle == NULL) {
			logprintf("Veh_UpdateSlot: slot to empty was not empty");
			return 1;
		}
		gamevehicles[vehicleid].dbvehicle->spawnedvehicleid = -1;
		gamevehicles[vehicleid].dbvehicle = NULL;
		return 1;
	}
	while (i--) {
		if (veh->id == dbid) {
			if (gamevehicles[vehicleid].dbvehicle != NULL) {
				logprintf("Veh_UpdateSlot: slot to assign was not empty");
				gamevehicles[vehicleid].dbvehicle->spawnedvehicleid = -1;
			}
			gamevehicles[vehicleid].dbvehicle = veh;
			veh->spawnedvehicleid = vehicleid;
			return 1;
		}
		veh++;
	}
	logprintf("Veh_UpdateSlot: unknown dbid: %d", dbid);
	return 0;
}
