
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "dialog.h"
#include "prefs.h"
#include <string.h>

int prefs[MAX_PLAYERS];

/**
Appends a row for a preference to show in the preferences dialog.

Format is {description}:\t<color><ENABLED|DISABLED>\n
*/
static
char *prefs_append_pref(char *buf, char *description, const int state)
{
	while ((*(buf++) = *(description++)));
	*(--buf) = ':';
	buf++;
	*(buf++) = '\t';
	if (state) {
		strcpy(buf, "{00f600}ENABLED");
		buf += 15;
	} else {
		strcpy(buf, "{f60000}DISABLED");
		buf += 16;
	}
	*(buf++) = '\n';
	return buf;
}

/**
Shows a dialog with preferences which the player can modify.
*/
static
void prefs_show_dialog(AMX *amx, int playerid)
{
	int p = prefs[playerid];
	char buf[255], *bp = buf;

	/* must be same order as in Prefs_DoActionForRow */
	bp = prefs_append_pref(bp, "Accepting PMs", p & PREF_ENABLE_PM);
	bp = prefs_append_pref(bp, "Mission Messages",
		p & PREF_SHOW_MISSION_MSGS);
	bp = prefs_append_pref(bp, "Constant Work", p & PREF_CONSTANT_WORK);
	bp = prefs_append_pref(bp, "Auto engage nav when working",
		p & PREF_WORK_AUTONAV);
	*(--bp) = 0;

	dialog_NC_ShowPlayerDialog(
		amx,
		playerid,
		DIALOG_PREFERENCES,
		DIALOG_STYLE_TABLIST,
		"Preferences",
		buf,
		"Change",
		"Close",
		-1);
}

void prefs_on_player_connect(int playerid)
{
	prefs[playerid] = DEFAULTPREFS;
}

void prefs_on_dialog_response(AMX *amx, int playerid, int response, int idx)
{
	/*must be same order the calls to prefs_append_pref in
	prefs_cmd_preferences*/
	if (response && 0 <= idx && idx <= 3) {
		prefs[playerid] ^= 1 << idx;
		prefs_show_dialog(amx, playerid);
	}
}

int prefs_cmd_preferences(CMDPARAMS)
{
	prefs_show_dialog(amx, playerid);
	return 1;
}
