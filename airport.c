
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"

static int airportscount;
static struct airport {
	float x, y, z;
	char code[4 + 1];
	char enabled;
	char beacon[4 + 1];
	char name[MAX_AIRPORT_NAME + 1];
} *airports;

cell AMX_NATIVE_CALL APT_Init(AMX *amx, cell *params)
{
	if (airports != NULL) {
		logprintf("APT_Init: warning: airports table was not empty");
		free(airports);
	}
	airportscount = params[1];
	airports = malloc(sizeof(struct airport) * airportscount);
	return 1;
}

cell AMX_NATIVE_CALL APT_Destroy(AMX *amx, cell *params)
{
	if (airports != NULL) {
		free(airports);
		airports = NULL;
	}
	return 1;
}

/* native APT_Add(index, code[], enabled, name[], beacon[], Float:x, Float:y, Float:z) */
cell AMX_NATIVE_CALL APT_Add(AMX *amx, cell *params)
{
	cell *addr;
	struct airport ap = airports[params[1]];

	amx_GetAddr(amx, params[2], &addr);
	amx_GetUString(ap.code, addr, sizeof(ap.code));
	ap.enabled = params[3];
	amx_GetAddr(amx, params[4], &addr);
	amx_GetUString(ap.name, addr, sizeof(ap.name));
	amx_GetAddr(amx, params[5], &addr);
	amx_GetUString(ap.beacon, addr, sizeof(ap.beacon));
	ap.x = amx_ctof(params[6]);
	ap.y = amx_ctof(params[7]);
	ap.z = amx_ctof(params[8]);
	return 1;
}
