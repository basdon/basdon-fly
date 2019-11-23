
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "playerdata.h"
#include <string.h>

struct playerdata *pdata[MAX_PLAYERS];

void pdata_init()
{
	int i;
	for (i = 0; i < MAX_PLAYERS; i++) {
		pdata[i] = NULL;
	}
}

void pdata_update_name(int playerid)
{
	struct playerdata *pd = pdata[playerid];
	char t, *nin = pd->name, *nout = pd->normname;

	NC_PARS(3);
	nc_params[1] = playerid;
	nc_params[2] = buf144a;
	nc_params[3] = MAX_PLAYER_NAME + 1;
	pd->namelen = (char) NC(n_GetPlayerName);
	amx_GetUString(pd->name, buf144, MAX_PLAYER_NAME + 1);
	/*update normname*/
	do {
		t = *nin++;
		if ('A' <= t && t <= 'Z') {
			t |= 0x20;
		}
		*nout++ = t;
	} while (t);
}

void pdata_init_player(int playerid)
{
	struct playerdata *pd;

	if ((pd = pdata[playerid]) == NULL) {
		pd = pdata[playerid] = malloc(sizeof(struct playerdata));
	}
	NC_GetPlayerIp(playerid, buf144a, sizeof(pd->ip));
	amx_GetUString(pd->ip, buf144, sizeof(pd->ip));
	pdata_update_name(playerid);
	pd->userid = -1;
	pd->groups = GROUP_GUEST;
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

/* native PlayerData_FormatUpdateQuery(userid, score, money, Float:dis, flighttime, prefs, buf[]) */
cell AMX_NATIVE_CALL PlayerData_FormatUpdateQuery(AMX *amx, cell *params)
{
	const int userid = params[1], score = params[2], money = params[3];
	const int dis = (int) amx_ctof(params[4]), flighttime = params[5], prefs = params[6];
	char buf[144];
	cell *addr;
	sprintf(buf,
	        "UPDATE usr SET score=%d,cash=%d,distance=%d,flighttime=%d,prefs=%d WHERE i=%d",
	        score,
	        money,
	        dis,
	        flighttime,
	        prefs,
	        userid);
	amx_GetAddr(amx, params[7], &addr);
	amx_SetUString(addr, buf, sizeof(buf));
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

/* native PlayerData_UpdateGroup(playerid, groups) */
cell AMX_NATIVE_CALL PlayerData_UpdateGroup(AMX *amx, cell *params)
{
	const int pid = params[1], groups = params[2];
	if (pdata[pid] == NULL) {
		return 0;
	}
	pdata[pid]->groups = groups;
	return 1;
}

/* native PlayerData_UpdateName(playerid, name[], namelen) */
cell AMX_NATIVE_CALL PlayerData_UpdateName(AMX *amx, cell *params)
{
	pdata_update_name(params[1]);
	return 1;
}
