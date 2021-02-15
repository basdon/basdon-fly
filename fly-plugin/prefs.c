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
		strcpy(buf, ECOL_SUCC"yes");
		buf += 11;
	} else {
		strcpy(buf, ECOL_WARN"no");
		buf += 10;
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
	char buf[2048], *bp;

	/*Pressing enter on a setting will change it and reshow the dialog, but the selection goes back to
	the first entry when the dialog is shown. Thus, when pressing enter one too many times, one might
	change the first entry without wanting to. Show a dummy entry as first to prevent this.*/
	bp = buf + sprintf(buf, ECOL_SAMP_GREY"dummy entry\t\n");
	bp = prefs_append_pref(bp, "Accepting PMs", p & PREF_ENABLE_PM);
	bp = prefs_append_pref(bp, "Mission Messages", p & PREF_SHOW_MISSION_MSGS);
	bp = prefs_append_pref(bp, "Show GPS", p & PREF_SHOW_GPS);
	bp = prefs_append_pref(bp, "Show Kneeboard", p & PREF_SHOW_KNEEBOARD);
	bp = prefs_append_pref(bp, "Auto engage nav when working", p & PREF_WORK_AUTONAV);
	bp = prefs_append_pref(bp, "Aviation panel night colors ("
		EQ(PANEL_NIGHT_COLORS_FROM_HR)"h-"EQ(PANEL_NIGHT_COLORS_TO_HR)"h)", p & PREF_PANEL_NIGHTCOLORS);
	bp += sprintf(bp, "Name tag draw distance:\t");
	if (nametags_max_distance[playerid] > 50000) {
		bp += sprintf(bp, "unlimited\n");
	} else {
		bp += sprintf(bp, "%d\n", nametags_max_distance[playerid]);
	}
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
void prefs_on_dialog_response_nametagdist(int playerid, int response, char *inputtext)
{
	unsigned int dist;

	if (response) {
		dist = (unsigned int) atoi(inputtext);
		if (dist > 50000) {
			nametags_max_distance[playerid] = 60000;
		} else {
			nametags_max_distance[playerid] = (unsigned short) dist;
		}
	}
	prefs_show_dialog(playerid);
}

static
void prefs_on_dialog_response(int playerid, int response, int idx)
{
	struct vec3 pos;
	int val;

	if (response) {
		if (idx < 7) {
			/*0 is the dummy entry*/
			if (idx > 0) {
				/*must be same order the calls to prefs_append_pref in
				prefs_cmd_preferences*/
				val = 1 << (idx - 1);
				prefs[playerid] ^= val;
				if (val == PREF_SHOW_GPS) {
					GetPlayerPos(playerid, &pos);
					zones_update(playerid, pos);
				} else if (val == PREF_SHOW_KNEEBOARD) {
					GetPlayerPos(playerid, &pos);
					kneeboard_update_all(playerid, &pos);
				} else if (val == PREF_PANEL_NIGHTCOLORS) {
					if (panel_is_active_for(playerid)) {
						panel_reshow_if_needed(playerid);
					}
				}
			}
			prefs_show_dialog(playerid);
		} else if (idx == 7) {
			dialog_ShowPlayerDialog(
				playerid,
				DIALOG_PREFERENCES_NAMETAGDISTANCE,
				DIALOG_STYLE_INPUT,
				"Name tag draw distance",
				"Set the new name tag draw distance in units.\n"
				"Use -1 or any value above 50000 for unlimited.",
				"Change",
				"Cancel",
				DIALOG_PREFERENCES);
		}
	}
}

/**
The /preferences command.

Shows a dialog with preferences which the player can modify.
*/
static
int prefs_cmd_preferences(struct COMMANDCONTEXT cmdctx)
{
	prefs_show_dialog(cmdctx.playerid);
	return 1;
}
