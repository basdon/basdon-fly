
/* vim: set filetype=c ts=8 noexpandtab: */

/**
The /register command allows guests to register their account.
*/
int guestreg_cmd_register(CMDPARAMS);
void guestreg_dlg_change_name(int playerid, int response, char *inputtext);
void guestreg_dlg_register_firstpass(int playerid,
	int response, cell inputtexta, cell *inputtext);
void guestreg_dlg_register_confirmpass(int playerid,
	int response, cell inputtexta, cell *inputtext);
