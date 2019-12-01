
/* vim: set filetype=c ts=8 noexpandtab: */

/**
The /changepassword command allows one to change their password.

Must only be available to registered users.
*/
int chpw_cmd_changepassword(CMDPARAMS);
void chpw_dlg_confirm_password(int playerid, int result,
		cell inputtexta, cell *inputtext);
void chpw_dlg_new_password(int playerid, int result,
		cell inputtexta, cell *inputtext);
void chpw_dlg_previous_password(int playerid, int result,
		cell inputtexta, cell *inputtext);
void chpw_on_player_connect(int playerid);
void chpw_on_player_disconnect(int playerid);