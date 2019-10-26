
/* vim: set filetype=c ts=8 noexpandtab: */

/*This file is directly included in cmd.c,
but also added as a source file in VC2005. Check
for the IN_CMD macro to only compile this
when we're inside cmd.c*/
#ifdef IN_CMD

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

static int cmd_dev_timex(CMDPARAMS)
{
	int h, m;
	char buf[144];

	if (!cmd_get_int_param(cmdtext, &parseidx, &h) ||
		!cmd_get_int_param(cmdtext, &parseidx, &m))
	{
		amx_SetUString(buf144, WARN"Syntax: /timex <h> <m>", 144);
		NC_SendClientMessage(playerid, COL_WARN, buf144a);
	} else {
		time_h = h;
		time_m = m;
		NC_SetPlayerTime(playerid, h, m);
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

#endif /*IN_CMD*/
