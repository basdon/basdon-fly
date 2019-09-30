
/* vim: set filetype=c ts=8 noexpandtab: */

/*This file is directly included in cmd.c,
but also added as a source file in VC2005. Check
for the IN_CMD macro to only compile this
when we're inside cmd.c*/
#ifdef IN_CMD

int cmd_admin_spray(CMDPARAMS)
{
	logprintf("respray");
	return 1;
}

#endif /*IN_CMD*/
