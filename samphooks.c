int drive_keystates[MAX_PLAYERS];
char drive_udkeystate[MAX_PLAYERS];

void hook_OnDriverSync(int playerid, struct SYNCDATA_Driver *data)
{
	/*TODO reset these keystate variables when player gets into the drive state?*/
	int keys;

	keys = (data->partial_keys | (data->additional_keys << 16)) & 0x0003FFFF;

	/*keystate change*/
	if (drive_keystates[playerid] != keys) {
		missions_driversync_keystate_change(playerid, drive_keystates[playerid], keys);
		drive_keystates[playerid] = keys;
	}

	/*up/down keystate change*/
	if (data->udkey == 0) {
		if (drive_udkeystate[playerid] != 0) {
			missions_driversync_udkeystate_change(playerid, data->udkey);
			drive_udkeystate[playerid] = 0;
		}
	} else if (data->udkey < 0) {
		if (drive_udkeystate[playerid] != -1) {
			missions_driversync_udkeystate_change(playerid, data->udkey);
			drive_udkeystate[playerid] = -1;
		}
	} else {
		if (drive_udkeystate[playerid] != 1) {
			missions_driversync_udkeystate_change(playerid, data->udkey);
			drive_udkeystate[playerid] = 1;
		}
	}

	/*printf("DRIVER SYNC ud %hd rl %hd lnd %hd\n", data->udkey, data->lrkey, data->landing_gear_state);*/

	/*TODO remove this when all OnPlayerUpdates are replaced*/
	/*this is 3 because.. see PARAM definition*/
	nc_params[3] = playerid;
	B_OnPlayerUpdate(amx, nc_params);
}

/*TODO merge this with samp.c?*/
static
void samphooks_init()
{
	/*Sync data is dropped when coordinates exceed -20k/+20k, update those limits here.
	See 804B5D0 CheckSyncBounds.*/
	mem_protect(0x815070C, 0x10, PROT_READ | PROT_WRITE);
	*((int*) 0x815070C) = WORLD_XY_MAX; /*sync bounds x/y max, default 20000.0*/
	*((int*) 0x8150710) = WORLD_XY_MIN; /*sync bounds x/y min, default -20000.0*/
	/**(int*) 0x8150714 = ; *//*sync bounds z max, default 200000.0*/
	/**(int*) 0x8150718 = ; *//*sync bounds z min, default -1000.0*/

	mem_mkjmp(0x80AEC4F, &DriverSyncHook);
}
