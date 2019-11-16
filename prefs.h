
/* vim: set filetype=c ts=8 noexpandtab: */

/**
The /preferences command.

Shows a dialog with preferences which the player can modify.
*/
int prefs_cmd_preferences(CMDPARAMS);
/**
Call from DIALOG_PREFERENCES dialog response.
*/
void prefs_on_dialog_response(int playerid, int response, int idx);
/**
Reset stuff for player.
*/
void prefs_on_player_connect(int playerid);
