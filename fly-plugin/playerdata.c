struct playerdata *pdata[MAX_PLAYERS];

void pdata_init()
{
	TRACE;
	int i;
	for (i = 0; i < MAX_PLAYERS; i++) {
		pdata[i] = NULL;
	}
}

void pdata_on_name_updated(int playerid)
{
	TRACE;
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
	TRACE;
	struct playerdata *pd;
	struct PlayerID playerID;
	struct in_addr in_addr;

	if ((pd = pdata[playerid]) == NULL) {
		pd = pdata[playerid] = malloc(sizeof(struct playerdata));
	}

	RakServer__GetPlayerIDFromIndex(&playerID, rakServer, playerid);
	in_addr.s_addr = playerID.binaryAddress;
	strcpy(pd->ip, inet_ntoa(in_addr));
	strcpy(pd->name, playerpool->names[playerid]); /*TODO: don't need to copy this if we just use the pool's value*/
	pdata_on_name_updated(playerid);

	pd->groups = GROUP_GUEST;
}

void pdata_on_player_disconnect(int playerid)
{
	TRACE;
	if (pdata[playerid] != NULL) {
		free(pdata[playerid]);
		pdata[playerid] = NULL;
	}
}
