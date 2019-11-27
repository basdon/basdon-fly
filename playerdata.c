
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "login.h"
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

void pdata_on_name_updated(int playerid)
{
	char t, *nin, *nout;

	nin = pdata[playerid]->name;
	nout = pdata[playerid]->normname;
	do {
		t = *nin++;
		if ('A' <= t && t <= 'Z') {
			t |= 0x20;
		}
		*nout++ = t;
	} while (t);
	pdata[playerid]->namelen = nin - pdata[playerid]->name - 1;
}

void pdata_on_player_connect(int playerid)
{
	struct playerdata *pd;

	if ((pd = pdata[playerid]) == NULL) {
		pd = pdata[playerid] = malloc(sizeof(struct playerdata));
	}

	NC_PARS(3);
	nc_params[1] = playerid;
	nc_params[2] = buf144a;
	nc_params[3] = sizeof(pd->ip);
	NC(n_GetPlayerIp);
	amx_GetUString(pd->ip, buf144, sizeof(pd->ip));

	nc_params[3] = MAX_PLAYER_NAME + 1;
	NC(n_GetPlayerName);
	amx_GetUString(pd->name, buf144, MAX_PLAYER_NAME + 1);
	pdata_on_name_updated(playerid);

	pd->groups = GROUP_GUEST;
}

void pdata_on_player_disconnect(int playerid)
{
	if (pdata[playerid] != NULL) {
		free(pdata[playerid]);
		pdata[playerid] = NULL;
	}
}
