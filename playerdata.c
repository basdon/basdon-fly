
/* vim: set filetype=c ts=8 noexpandtab: */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include "common.h"
#include <string.h>
#include "playerdata.h"

struct playerdata *pdata[MAX_PLAYERS];

void pdata_init()
{
	int i;
	for (i = 0; i < MAX_PLAYERS; i++) {
		pdata[i] = NULL;
	}
}

void setName(struct playerdata *data, AMX *amx, cell addrparam, int namelen)
{
	char *nin = data->name;
	char *nout = data->normname;
	char t;
	cell *addr;
	amx_GetAddr(amx, addrparam, &addr);
	amx_GetUString(data->name, addr, MAX_PLAYER_NAME + 1);
	data->namelen = namelen;
	while(1) {
		t = *nin++;
		if ('A' <= t && t <= 'Z') {
			t |= 0x20;
		}
		if ((*nout++ = t) == 0) {
			break;
		}
	}
}

/* native PlayerData_Clear(playerid) */
cell AMX_NATIVE_CALL PlayerData_Clear(AMX *amx, cell *params)
{
	int pid = params[1];
	if (pdata[pid] != NULL) {
		free(pdata[pid]);
		pdata[pid] = NULL;
	}
	return 1;
}

/* native PlayerData_Init(playerid, ip[], name[], namelen) */
cell AMX_NATIVE_CALL PlayerData_Init(AMX *amx, cell *params)
{
	int pid = params[1];
	cell *addr;
	if (pdata[pid] == NULL) {
		pdata[pid] = malloc(sizeof(struct playerdata));
	}
	amx_GetAddr(amx, params[2], &addr);
	amx_GetUString(pdata[pid]->ip, addr, 16);
	setName(pdata[pid], amx, params[3], params[4]);
	pdata[pid]->userid = -1;
	return 1;
}

/* native PlayerData_SetUserId(playerid, id) */
cell AMX_NATIVE_CALL PlayerData_SetUserId(AMX *amx, cell *params)
{
	int pid = params[1];
	if (pdata[pid] == NULL) {
		return 0;
	}
	pdata[pid]->userid = params[2];
	return 1;
}

/* native PlayerData_UpdateName(playerid, name[], namelen) */
cell AMX_NATIVE_CALL PlayerData_UpdateName(AMX *amx, cell *params)
{
	int pid = params[1];
	if (pdata[pid] == NULL) {
		return 0;
	}
	setName(pdata[pid], amx, params[2], params[3]);
	return 1;
}