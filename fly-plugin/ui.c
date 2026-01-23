static char *ui_mission_map = "mission map";
static char *ui_help = "help";

/**
The UI that is currently shown for a given player.
*/
static char *current_ui[MAX_PLAYERS];

static
void ui_on_player_connect(int playerid)
{
	TRACE;
	current_ui[playerid] = NULL;
}

/**
@param ui should be any of the ui_ variables
@return 0 on failure - can't show ui
*/
static
int ui_try_show(int playerid, char *ui)
{
	TRACE;
	char msg[144];

	if (current_ui[playerid] && current_ui[playerid] != ui) {
		sprintf(msg, WARN"Close the '%s' screen before doing that.", current_ui[playerid]);
		SendClientMessage(playerid, COL_WARN, msg);
		return 0;
	}
	current_ui[playerid] = ui;
	return 1;
}

/*
@param ui should be any of the ui_ variables
*/
static
void ui_closed(int playerid, char *ui)
{
	TRACE;
	if (current_ui[playerid] != ui) {
		logprintf("warn: closing ui '%s' while current ui is actually '%s'",
			ui, current_ui[playerid] ? "(null)" : current_ui[playerid]);
	}
	current_ui[playerid] = NULL;
}
