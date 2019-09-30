
/* vim: set filetype=c ts=8 noexpandtab: */

/*This file is directly included in cmd.c,
but also added as a source file in VC2005. Check
for the IN_CMD macro to only compile this
when we're inside cmd.c*/
#ifdef IN_CMD

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

static int
cmd_admin_spray(CMDPARAMS)
{
	logprintf("respray");
	return 1;
}

#endif /*IN_CMD*/
