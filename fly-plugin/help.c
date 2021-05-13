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

static struct TEXTDRAW td_helpadf_0 = { "bg",         TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpadf_1 = { "title",      TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpadf_2 = { "1",          TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpadf_3 = { "2",          TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpadf_4 = { "disexp",     TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpadf_5 = { "disarr",     TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpadf_6 = { "altexp",     TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpadf_7 = { "altarr",     TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpadf_8 = { "crsexp",     TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpadf_9 = { "crsarr",     TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpadf_A = { "navtxt",     TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpadf_B = { "navdis",     TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpadf_C = { "navalt",     TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpadf_D = { "navcrs",     TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpadf_E = { "close",      TEXTDRAW_ALLOC_AS_NEEDED, NULL };
#define NUM_HELPADF_TDS 15
STATIC_ASSERT(NUM_HELPADF_TDS <= NUM_HELP_TEXTDRAWS);
static struct TEXTDRAW *tds_helpadf[NUM_HELPADF_TDS] = {
	&td_helpadf_0, &td_helpadf_1, &td_helpadf_2, &td_helpadf_3, &td_helpadf_4, &td_helpadf_5,
	&td_helpadf_6, &td_helpadf_7, &td_helpadf_8, &td_helpadf_9, &td_helpadf_A, &td_helpadf_B,
	&td_helpadf_C, &td_helpadf_D, &td_helpadf_E,
};

static struct TEXTDRAW td_helpvor_0 = { "bg",         TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpvor_1 = { "title",      TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpvor_2 = { "1",          TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpvor_3 = { "2",          TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpvor_4 = { "3",          TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpvor_5 = { "disexp",     TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpvor_6 = { "disarr",     TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpvor_7 = { "altexp",     TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpvor_8 = { "altarr",     TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpvor_9 = { "crsexp",     TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpvor_A = { "crsarr",     TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpvor_B = { "navtxt",     TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpvor_C = { "navdis",     TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpvor_D = { "navalt",     TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpvor_E = { "navcrs",     TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpvor_F = { "close",      TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpvor_G = { "vorbar1",    TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpvor_H = { "vorbar2",    TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpvor_I = { "vorbarind1", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpvor_J = { "vorbarind2", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
#define NUM_HELPVOR_TDS 20
STATIC_ASSERT(NUM_HELPVOR_TDS <= NUM_HELP_TEXTDRAWS);
static struct TEXTDRAW *tds_helpvor[NUM_HELPVOR_TDS] = {
	&td_helpvor_0, &td_helpvor_1, &td_helpvor_2, &td_helpvor_3, &td_helpvor_4, &td_helpvor_5,
	&td_helpvor_6, &td_helpvor_7, &td_helpvor_8, &td_helpvor_9, &td_helpvor_A, &td_helpvor_B,
	&td_helpvor_C, &td_helpvor_D, &td_helpvor_E, &td_helpvor_F, &td_helpvor_G, &td_helpvor_H,
	&td_helpvor_I, &td_helpvor_J,
};

static struct TEXTDRAW td_helpils_0 = { "bg",         TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpils_1 = { "title",      TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpils_2 = { "1",          TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpils_3 = { "2",          TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpils_4 = { "3",          TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpils_5 = { "verexp",     TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpils_6 = { "verarr",     TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpils_7 = { "horexp",     TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpils_8 = { "horarr",     TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpils_9 = { "numexp",     TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpils_A = { "numarr",     TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpils_B = { "close",      TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpils_C = { "ils1",       TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpils_D = { "ilsno",      TEXTDRAW_ALLOC_AS_NEEDED, NULL };
#define NUM_HELPILS_TDS 14
STATIC_ASSERT(NUM_HELPILS_TDS <= NUM_HELP_TEXTDRAWS);
static struct TEXTDRAW *tds_helpils[NUM_HELPILS_TDS] = {
	&td_helpils_0, &td_helpils_1, &td_helpils_2, &td_helpils_3, &td_helpils_4, &td_helpils_5,
	&td_helpils_6, &td_helpils_7, &td_helpils_8, &td_helpils_9, &td_helpils_A, &td_helpils_B,
	&td_helpils_C, &td_helpils_D,
};

static struct TEXTDRAW td_helpnav_0 = { "bg",         TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpnav_1 = { "title",      TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpnav_2 = { "1",          TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpnav_3 = { "2",          TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_helpnav_4 = { "close",      TEXTDRAW_ALLOC_AS_NEEDED, NULL };
#define NUM_HELPNAV_TDS 5
STATIC_ASSERT(NUM_HELPNAV_TDS <= NUM_HELP_TEXTDRAWS);
static struct TEXTDRAW *tds_helpnav[NUM_HELPNAV_TDS] = {
	&td_helpnav_0, &td_helpnav_1, &td_helpnav_2, &td_helpnav_3, &td_helpnav_4,
};

static struct TEXTDRAW td_credits_0 = { "bg",         TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_credits_1 = { "title",      TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_credits_2 = { "sections",   TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_credits_3 = { "prog",       TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_credits_4 = { "mapp",       TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_credits_5 = { "beta",       TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_credits_6 = { "bran",       TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_credits_7 = { "spec",       TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_credits_8 = { "insp",       TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_credits_9 = { "close",      TEXTDRAW_ALLOC_AS_NEEDED, NULL };
#define NUM_CREDITS_TDS 10
STATIC_ASSERT(NUM_CREDITS_TDS <= NUM_HELP_TEXTDRAWS);
static struct TEXTDRAW *tds_credits[NUM_CREDITS_TDS] = {
	&td_credits_0, &td_credits_1, &td_credits_2, &td_credits_3, &td_credits_4, &td_credits_5,
	&td_credits_6, &td_credits_7, &td_credits_8, &td_credits_9,
};

static int current_help_tds_shown[MAX_PLAYERS];

static
void help_init()
{
	textdraws_load_from_file_a("helpmain", TEXTDRAW_HELP_BASE, NUM_HELPMAIN_TDS, tds_helpmain);
	textdraws_load_from_file_a("helpadf", TEXTDRAW_HELP_BASE, NUM_HELPADF_TDS, tds_helpadf);
	textdraws_load_from_file_a("helpvor", TEXTDRAW_HELP_BASE, NUM_HELPVOR_TDS, tds_helpvor);
	textdraws_load_from_file_a("helpils", TEXTDRAW_HELP_BASE, NUM_HELPILS_TDS, tds_helpils);
	textdraws_load_from_file_a("helpnav", TEXTDRAW_HELP_BASE, NUM_HELPNAV_TDS, tds_helpnav);
	textdraws_load_from_file_a("credits", TEXTDRAW_HELP_BASE, NUM_CREDITS_TDS, tds_credits);
}

static
void help_dispose()
{
	int i;

	for (i = 0; i < NUM_HELPMAIN_TDS; i++) {
		free(tds_helpmain[i]->rpcdata);
	}
	for (i = 0; i < NUM_HELPADF_TDS; i++) {
		free(tds_helpadf[i]->rpcdata);
	}
	for (i = 0; i < NUM_HELPVOR_TDS; i++) {
		free(tds_helpvor[i]->rpcdata);
	}
	for (i = 0; i < NUM_HELPILS_TDS; i++) {
		free(tds_helpils[i]->rpcdata);
	}
	for (i = 0; i < NUM_HELPNAV_TDS; i++) {
		free(tds_helpnav[i]->rpcdata);
	}
	for (i = 0; i < NUM_CREDITS_TDS; i++) {
		free(tds_credits[i]->rpcdata);
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
		textdraws_hide_consecutive(playerid, current_help_tds_shown[playerid], TEXTDRAW_HELP_BASE);
		current_help_tds_shown[playerid] = 0;
		ui_closed(playerid, ui_help);
		panel_unhide(playerid, PANEL_HIDE_REASON_HELP);
		kneeboard_unhide(playerid, KNEEBOARD_HIDE_REASON_HELP);
	}
}

static
void help_show(int playerid, int num_tds, struct TEXTDRAW **tds)
{
	register int num_to_hide;

	if (!current_help_tds_shown[playerid]) {
		SendClientMessage(playerid, COL_INFO_LIGHT, "Tip: press F7 or hold F10 to hide the samp UI while reading help pages");
	}
	textdraws_show_a(playerid, num_tds, tds);
	num_to_hide = current_help_tds_shown[playerid] - num_tds;
	if (num_to_hide > 0) {
		textdraws_hide_consecutive(playerid, num_to_hide, TEXTDRAW_HELP_BASE + num_tds);
	}
	current_help_tds_shown[playerid] = num_tds;
	panel_hide(playerid, PANEL_HIDE_REASON_HELP);
	kneeboard_hide(playerid, KNEEBOARD_HIDE_REASON_HELP);
}
