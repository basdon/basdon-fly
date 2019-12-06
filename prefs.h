
/* vim: set filetype=c ts=8 noexpandtab: */

#define PREF_ENABLE_PM 1
#define PREF_SHOW_MISSION_MSGS 2
#define PREF_CONSTANT_WORK 4
#define PREF_WORK_AUTONAV 8

#define DEFAULTPREFS \
	(PREF_ENABLE_PM | PREF_SHOW_MISSION_MSGS | PREF_WORK_AUTONAV)

/**
Player preferences, see PREF_ constants.
*/
extern int prefs[MAX_PLAYERS];

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
