static struct TEXTDRAW td_helpmain_0 = { "bg",         TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpmain_1 = { "title",      TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpmain_2 = { "1",          TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpmain_3 = { "2",          TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpmain_4 = { "3",          TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpmain_5 = { "close",      TEXTDRAW_ALLOC_AS_NEEDED, NULL };
#define NUM_HELPMAIN_TDS 6
STATIC_ASSERT(NUM_HELPMAIN_TDS <= NUM_HELP_TEXTDRAWS);
static struct TEXTDRAW *tds_helpmain[NUM_HELPMAIN_TDS] = {
	&td_helpmain_0, &td_helpmain_1, &td_helpmain_2, &td_helpmain_3, &td_helpmain_4, &td_helpmain_5,
};

static int current_help_tds_shown[MAX_PLAYERS];

static
void help_init()
{
	textdraws_load_from_file_a("helpmain", TEXTDRAW_HELP_BASE, NUM_HELPMAIN_TDS, tds_helpmain);
}

static
void help_dispose()
{
	int i;

	for (i = 0; i < NUM_HELPMAIN_TDS; i++) {
		free(tds_helpmain[i]->rpcdata);
	}
}

static
void help_on_player_connect(int playerid)
{
	current_help_tds_shown[playerid] = 0;
}

static
void help_hide(int playerid)
{
	if (current_help_tds_shown[playerid]) {
		textdraws_hide_consecutive(playerid, TEXTDRAW_HELP_BASE, current_help_tds_shown[playerid]);
		current_help_tds_shown[playerid] = 0;
		ui_closed(playerid, ui_help);
		panel_unhide(playerid, PANEL_HIDE_REASON_HELP);
		kneeboard_unhide(playerid, KNEEBOARD_HIDE_REASON_HELP);
	}
}

static
void help_show(int playerid, int num_tds, struct TEXTDRAW **tds)
{
	SendClientMessage(playerid, COL_INFO_LIGHT, "Tip: press F7 or hold F10 to hide the samp UI while reading help pages");
	textdraws_show_a(playerid, num_tds, tds);
	if (current_help_tds_shown[playerid] > num_tds) {
		textdraws_hide_consecutive(playerid, TEXTDRAW_HELP_BASE + num_tds, current_help_tds_shown[playerid] - num_tds);
	}
	current_help_tds_shown[playerid] = num_tds;
	panel_hide(playerid, PANEL_HIDE_REASON_HELP);
	kneeboard_hide(playerid, KNEEBOARD_HIDE_REASON_HELP);
}

static
void help_show_main(int playerid)
{
	if (ui_try_show(playerid, ui_help)) {
		help_show(playerid, NUM_HELPMAIN_TDS, tds_helpmain);
	}
}
