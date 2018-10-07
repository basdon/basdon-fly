
/* vim: set filetype=c ts=8 noexpandtab: */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include "common.h"

#define ISINVALIDAIRPORT(X) ((X).code[0] == 0)
static int airportscount;

struct runway {
	char id[4];
	float heading, x, y, z;
	struct runway *next;
};

static struct airport {
	float x, y, z;
	char code[4 + 1];
	char enabled;
	char beacon[4 + 1];
	char name[MAX_AIRPORT_NAME + 1];
	struct runway *runways;
} *airports;

void freeAirportTable()
{
	struct airport *ap = airports;
	struct runway *rnw;
	while (airportscount) {
		airportscount--;
		while (ap->runways != NULL) {
			if (ISINVALIDAIRPORT(*ap)) {
				logprintf("warning: airport %d was not initialized", airportscount);
				break;
			}
			rnw = ap->runways->next;
			free(ap->runways);
			ap->runways = rnw;
		}
		ap++;
	}
        free(airports);
}

cell AMX_NATIVE_CALL APT_Init(AMX *amx, cell *params)
{
	int i;
	if (airports != NULL) {
		logprintf("APT_Init: warning: airports table was not empty");
		freeAirportTable();
	}
	if (params[1] <= 0) {
		airportscount = 0;
		airports = NULL;
		return 0;
	}
	i = airportscount = params[1];
	airports = malloc(sizeof(struct airport) * airportscount);
	while (i--) {
		airports[i].code[0] = 0;
	}
	return 1;
}

cell AMX_NATIVE_CALL APT_Destroy(AMX *amx, cell *params)
{
	if (airports != NULL) {
		freeAirportTable();
		airports = NULL;
	}
	return 1;
}

/* native APT_Add(index, code[], enabled, name[], beacon[], Float:x, Float:y, Float:z) */
cell AMX_NATIVE_CALL APT_Add(AMX *amx, cell *params)
{
	cell *addr;
	struct airport *ap = airports + params[1];

	amx_GetAddr(amx, params[2], &addr);
	amx_GetUString(ap->code, addr, sizeof(ap->code));
	ap->enabled = params[3];
	amx_GetAddr(amx, params[4], &addr);
	amx_GetUString(ap->name, addr, sizeof(ap->name));
	amx_GetAddr(amx, params[5], &addr);
	amx_GetUString(ap->beacon, addr, sizeof(ap->beacon));
	ap->x = amx_ctof(params[6]);
	ap->y = amx_ctof(params[7]);
	ap->z = amx_ctof(params[8]);
	ap->runways = NULL;
	return 1;
}

/* native APT_AddRunway(aptindex, specifier, Float:heading, Float:x, Float:y, Float:z) */
cell AMX_NATIVE_CALL APT_AddRunway(AMX *amx, cell *params)
{
	struct airport *ap = airports + params[1];
	struct runway *rnw = malloc(sizeof(struct runway));

	sprintf(rnw->id, "%02.0f", amx_ctof(params[3]) / 10.0f);
	rnw->id[2] = params[2];
	rnw->id[3] = 0;
	rnw->heading = amx_ctof(params[3]);
	rnw->x = amx_ctof(params[4]);
	rnw->y = amx_ctof(params[5]);
	rnw->z = amx_ctof(params[6]);
	rnw->next = ap->runways;
	ap->runways = rnw;
	return 1;
}

