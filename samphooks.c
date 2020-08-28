short drive_keystates[MAX_PLAYERS];
char drive_udkeystate[MAX_PLAYERS];

void hook_OnDriverSync(int playerid, struct SYNCDATA_Driver *data)
{
	/*TODO reset these keystate variables when player gets into the drive state?*/

	/*keystate change*/
	if (drive_keystates[playerid] != data->keys) {
		missions_driversync_keystate_change(playerid, drive_keystates[playerid], data->keys);
		drive_keystates[playerid] = data->keys;
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

static
void samphooks_init()
{
	mem_mkjmp(0x80AEC4F, &DriverSyncHook);
}
