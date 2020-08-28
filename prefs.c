static int prefs[MAX_PLAYERS];

/**
Appends a row for a preference to show in the preferences dialog.

Format is {description}:\t<color><ENABLED|DISABLED>\n
*/
static
char *prefs_append_pref(char *buf, char *description, int state, int available)
{
	while ((*(buf++) = *(description++)));
	*(--buf) = ':';
	buf++;
	*(buf++) = '\t';
	if (available) {
		if (state) {
			strcpy(buf, "{00f600}ENABLED");
			buf += 15;
		} else {
			strcpy(buf, "{f60000}DISABLED");
			buf += 16;
		}
	} else {
		strcpy(buf, "{777777}UNAVAILABLE");
		buf += 19;
	}
	*(buf++) = '\n';
	return buf;
}

/**
Shows a dialog with preferences which the player can modify.
*/
static
void prefs_show_dialog(int playerid)
{
	int p = prefs[playerid];
	char buf[255], *bp = buf;

	/* must be same order as in Prefs_DoActionForRow */
	bp = prefs_append_pref(bp, "Accepting PMs", p & PREF_ENABLE_PM, 1);
	bp = prefs_append_pref(bp, "Mission Messages", p & PREF_SHOW_MISSION_MSGS, 1);
	bp = prefs_append_pref(bp, "Constant Work", p & PREF_CONSTANT_WORK, 0);
	bp = prefs_append_pref(bp, "Auto engage nav when working", p & PREF_WORK_AUTONAV, 1);
	*(--bp) = 0;

	dialog_ShowPlayerDialog(
		playerid,
		DIALOG_PREFERENCES,
		DIALOG_STYLE_TABLIST,
		"Preferences",
		buf,
		"Change",
		"Close",
		-1);
}

static
void prefs_on_player_connect(int playerid)
{
	prefs[playerid] = DEFAULTPREFS;
}

static
void prefs_on_dialog_response(int playerid, int response, int idx)
{
	/*must be same order the calls to prefs_append_pref in
	prefs_cmd_preferences*/
	if (response && 0 <= idx && idx <= 3) {
		prefs[playerid] ^= 1 << idx;
		prefs_show_dialog(playerid);
	}
}

/**
The /preferences command.

Shows a dialog with preferences which the player can modify.
*/
static
int prefs_cmd_preferences(CMDPARAMS)
{
	prefs_show_dialog(playerid);
	return 1;
}
