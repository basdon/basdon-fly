/**
Appends a row for a preference to show in the preferences dialog.

Format is {description}:\t<color><ENABLED|DISABLED>\n
*/
static
char *prefs_append_pref(char *buf, char *description, int state)
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
void prefs_show_dialog(int playerid)
{
	int p = prefs[playerid];
	char buf[255], *bp = buf;

	/* must be same order as in Prefs_DoActionForRow */
	bp = prefs_append_pref(bp, "Accepting PMs", p & PREF_ENABLE_PM);
	bp = prefs_append_pref(bp, "Mission Messages", p & PREF_SHOW_MISSION_MSGS);
	bp = prefs_append_pref(bp, "Show GPS", p & PREF_SHOW_GPS);
	bp = prefs_append_pref(bp, "Auto engage nav when working", p & PREF_WORK_AUTONAV);
	bp = prefs_append_pref(bp, "Aviation panel night colors ("
		EQ(PANEL_NIGHT_COLORS_FROM_HR)"h-"EQ(PANEL_NIGHT_COLORS_TO_HR)"h)", p & PREF_PANEL_NIGHTCOLORS);
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
	struct vec3 pos;
	int val;

	/*must be same order the calls to prefs_append_pref in
	prefs_cmd_preferences*/
	if (response && 0 <= idx && idx <= 4) {
		val = 1 << idx;
		prefs[playerid] ^= val;
		if (val == PREF_SHOW_GPS) {
			common_GetPlayerPos(playerid, &pos);
			zones_update(playerid, pos);
		} else if (val == PREF_PANEL_NIGHTCOLORS) {
			if (panel_is_active_for(playerid)) {
				panel_reshow_if_needed(playerid);
			}
		}
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
