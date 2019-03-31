
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
	float vs = sqrt(dx * dx + dy * dy + dz * dz);

	if ((veh = gamevehicles[vehicleid].dbvehicle) != NULL) {
		veh->odo += vs;
		if (!veh->needsodoupdate) {
			veh->needsodoupdate = 1;
			if (vehiclestoupdate == NULL) {
				vehiclestoupdate = malloc(sizeof(struct vehnode));
				vehiclestoupdate->veh = veh;
			} else {
				updatequeue = vehiclestoupdate;
				while (updatequeue->next != NULL) {
					updatequeue = updatequeue->next;
				}
				updatequeue->next = malloc(sizeof(struct vehnode));
				updatequeue->next->veh = veh;
			}
		}
	}

	missions_add_distance(playerid, vs);
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

/* native Veh_Destroy() */
cell AMX_NATIVE_CALL Veh_Destroy(AMX *amx, cell *params)
{
	if (dbvehicles != NULL) {
		freeDbVehicleTable();
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

/* native Veh_RegisterLabel(vehicleid, playerid, PlayerText3D:labelid) */
cell AMX_NATIVE_CALL Veh_RegisterLabel(AMX *amx, cell *params)
{
	const int vehicleid = params[1], playerid = params[2], labelid = params[3];
	labelids[playerid][vehicleid] = labelid;
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
