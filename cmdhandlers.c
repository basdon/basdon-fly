
/* vim: set filetype=c ts=8 noexpandtab: */

/*This file is directly included in cmd.c,
but also added as a source file in VC2005. Check
for the IN_CMD macro to only compile this
when we're inside cmd.c*/
#ifdef IN_CMD

#ifdef DEV
/**
Toggles constantly showing the closest mission checkpoint for every player.
*/
static int
cmd_dev_closestmp(CMDPARAMS)
{
	void dev_missions_toggle_closest_point(AMX*);
	dev_missions_toggle_closest_point(amx);
	return 1;
}

/**
Respawns the vehicle the player is in.
*/
static int
cmd_admin_respawn(CMDPARAMS)
{
	NC_GetPlayerVehicleID(playerid);
	if (nc_result) {
		NC_SetVehicleToRespawn(nc_result);
	}
	return 1;
}

static int
cmd_dev_testparpl(CMDPARAMS)
{
	int i;
	char buf[144];

	if (cmd_get_int_param(cmdtext, &parseidx, &i)) {
		printf("int %d\n", i);
	}
	if (cmd_get_player_param(cmdtext, &parseidx, &i)) {
		printf("player %d\n", i);
	}
	if (cmd_get_str_param(cmdtext, &parseidx, buf)) {
		printf("str -%s-\n", buf);
	}
	if (cmd_get_int_param(cmdtext, &parseidx, &i)) {
		printf("int %d\n", i);
	}
	return 1;
}

/**
Toggle owner group on yourself.
*/
static int
cmd_dev_owner(CMDPARAMS)
{
	pdata[playerid]->groups ^= GROUP_OWNER;
	pdata[playerid]->groups |= GROUP_MEMBER;
	return 1;
}
#endif /*DEV*/

static int
cmd_spray(CMDPARAMS)
{
	char buf[144];
	struct dbvehicle *veh;
	int userid, col1, col2;

	NC_GetPlayerVehicleID(playerid);
	if (nc_result) {
		if ((veh = gamevehicles[nc_result].dbvehicle) &&
			pdata[playerid]->userid != veh->owneruserid &&
			!(pdata[playerid]->groups & GROUPS_ADMIN))
		{
			sprintf(buf, WARN"You are not allowed to respray "
				         "this vehicle");
			amx_SetUString(buf144, buf, sizeof(buf));
			NC_SendClientMessage(playerid, COL_WARN, buf144a);
			return 1;
		}
		if (cmd_get_int_param(cmdtext, &parseidx, &col1)) {
			if (!cmd_get_int_param(cmdtext, &parseidx, &col2)) {
				goto rand2nd;
			}
		} else {
			NC_random_(256, &col1);
rand2nd:
			NC_random_(256, &col2);
		}
		NC_ChangeVehicleColor(nc_result, col1, col2);
		veh->col1 = col1;
		veh->col2 = col2;
		sprintf(buf, "UPDATE veh SET col1=%d,col2=%d WHERE i=%d",
			col1, col2, veh->id);
		amx_SetUString(buf144, buf, sizeof(buf));
		NC_mysql_tquery_nocb(buf144a);
	}
	return 1;
}

#endif /*IN_CMD*/
