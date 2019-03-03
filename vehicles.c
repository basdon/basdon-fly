
/* vim: set filetype=c ts=8 noexpandtab: */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include "common.h"
#include <string.h>
#include <math.h>
#include "game_sa.h"
#include "vehicles.h"

static struct dbvehicle *dbvehicles;
int dbvehiclenextid, dbvehiclealloc;
static struct vehicle gamevehicles[MAX_VEHICLES];

void freeDbVehicleTable()
{
	struct dbvehicle *veh = dbvehicles;
	while (dbvehiclealloc) {
		dbvehiclealloc--;
		if (veh->ownerstring != NULL) {
			free(veh->ownerstring);
			veh->ownerstring = NULL;
		}
		free(veh);
		veh++;
	}
        dbvehicles = NULL;
}

/* native Veh_Add(id, model, owneruserid, Float:x, Float:y, Float:z, Float:r, col1, col2, ownername[]) */
cell AMX_NATIVE_CALL Veh_Add(AMX *amx, cell *params)
{
	cell *ownernameaddr;
	char ownername[MAX_PLAYER_NAME + 1];
	int model = params[2];
	struct dbvehicle *veh = dbvehicles + dbvehiclenextid;
	if (dbvehiclenextid == dbvehiclealloc) {
		/* TODO: resize */
		return 0;
	}
	dbvehiclenextid++;
	veh->id = params[1];
	veh->model = model;
	veh->owneruserid = params[3];
	veh->x = amx_ctof(params[4]);
	veh->y = amx_ctof(params[5]);
	veh->z = amx_ctof(params[6]);
	veh->r = amx_ctof(params[7]);
	veh->col1 = params[8];
	veh->col2 = params[9];
	if (veh->owneruserid != 0 && 400 <= model && model <= 611) {
		amx_GetAddr(amx, params[10], &ownernameaddr);
		amx_GetUString(ownername, ownernameaddr, sizeof(ownername));
		veh->ownerstring = malloc((9 + strlen(vehnames[model - 400]) + strlen(ownername)) * sizeof(char));
		sprintf(veh->ownerstring, "%s Owner\n%s", vehnames[model - 400], ownername);
	} else {
		veh->ownerstring = NULL;
	}
	return 1;
}

/* native Veh_Destroy() */
cell AMX_NATIVE_CALL Veh_Destroy(AMX *amx, cell *params)
{
	if (dbvehicles != NULL) {
		freeDbVehicleTable();
	}
	return 1;
}

/* native Veh_Init(dbvehiclecount) */
cell AMX_NATIVE_CALL Veh_Init(AMX *amx, cell *params)
{
	int i = dbvehiclealloc = params[1];
	if (dbvehicles != NULL) {
		logprintf("Veh_Init: warning: dbvehicles table was not empty");
		freeDbVehicleTable();
	}
	dbvehiclenextid = 0;
	if (dbvehiclealloc <= 0) {
		dbvehiclealloc = 0;
		dbvehicles = NULL;
		return 0;
	}
	dbvehicles = malloc(sizeof(struct dbvehicle) * dbvehiclealloc);
	while (i--) {
		dbvehicles[i].ownerstring = NULL;
	}
	while (i < MAX_VEHICLES) {
		gamevehicles[i].id = i;
		gamevehicles[i].dbvehicle = NULL;
		i++;
	}
	return 1;
}
