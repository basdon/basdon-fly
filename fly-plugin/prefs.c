static void prefs_show_dialog(int playerid);

static
void prefs_cb_dlg_nametagdistance(int playerid, struct DIALOG_RESPONSE response)
{
	unsigned int dist;

	if (response.response && response.inputtext[0]) {
		dist = (unsigned int) atoi(response.inputtext);
		if (dist > 50000) {
			nametags_max_distance[playerid] = 60000;
		} else {
			nametags_max_distance[playerid] = (unsigned short) dist;
		}
	}
	prefs_show_dialog(playerid);
}

static
void prefs_cb_dlg_prefs(int playerid, struct DIALOG_RESPONSE response)
{
	struct DIALOG_INFO *dialog;
	struct vec3 pos;
	int val;

	if (response.response) {
		if (response.listitem < 7) {
			/*0 is the dummy entry*/
			if (response.listitem > 0) {
				val = 1 << (response.listitem - 1);
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
		} else if (response.listitem == 7) {
			dialog = alloca(sizeof(struct DIALOG_INFO));
			dialog_init_info(dialog);
			dialog->transactionid = DLG_TID_PREFERENCES_NAMETAGDISTANCE;
			dialog->style = DIALOG_STYLE_INPUT;
			dialog->caption = "Name tag draw distance";
			dialog->info =
				"Set the new name tag draw distance in units.\n"
				"Use -1 or any value above 50000 for unlimited.";
			dialog->button1 = "Change";
			dialog->button2 = "Cancel";
			dialog->handler.callback = prefs_cb_dlg_nametagdistance;
			dialog_show(playerid, dialog);
		}
	}
}

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
	struct DIALOG_INFO dialog;
	register int p;
	char *bp;

	p = prefs[playerid];
	dialog_init_info(&dialog);
	/*Pressing enter on a setting will change it and reshow the dialog, but the selection goes back to
	the first entry when the dialog is shown. Thus, when pressing enter one too many times, one might
	change the first entry without wanting to. Show a dummy entry as first to prevent this.*/
	bp = dialog.info + sprintf(dialog.info, ECOL_SAMP_GREY"dummy entry\t\n");
	/*This ordering is important, as the value must equals to 1 << (list_entry_idx - 1).*/
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
	dialog.transactionid = DLG_TID_PREFERENCES;
	dialog.style = DIALOG_STYLE_TABLIST;
	dialog.caption = "Preferences";
	dialog.button1 = "Change";
	dialog.button2 = "Close";
	dialog.handler.callback = prefs_cb_dlg_prefs;
	dialog_show(playerid, &dialog);
}

static
void prefs_on_player_connect(int playerid)
{
	prefs[playerid] = DEFAULTPREFS;
}
