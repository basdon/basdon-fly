#define CHANGELOG_ENTRIES_TO_SHOW_ON_CONNECT 3
/*And +2 entries for previous/next page.*/
#define CHANGELOG_ENTRIES_PER_DIALOG_PAGE 8
#define CHANGELOG_ENTRY_MAXLEN 144

static char changelog_entry[CHANGELOG_ENTRIES_TO_SHOW_ON_CONNECT][CHANGELOG_ENTRY_MAXLEN];
static char num_changelog_entries;
/**
Since changelog data is loaded with async queries,
this value is incremented every time a player requests a page.
When the page data is loaded, it should be discarded when
the request id does not match the current value in this variable.
*/
static unsigned char changelog_request_id[MAX_PLAYERS];

static
void changelog_init()
{
	int dbcache, i;

	atoc(buf4096, "SELECT stamp,entry FROM chg ORDER BY id DESC LIMIT "EQ(CHANGELOG_ENTRIES_TO_SHOW_ON_CONNECT), 4096);
	dbcache = NC_mysql_query(buf4096a);
	num_changelog_entries = NC_cache_get_row_count();
	for (i = 0; i < num_changelog_entries; i++) {
		NC_cache_get_field_str(i, 0, buf4096a);
		ctoa(changelog_entry[i], buf4096, 12);
		changelog_entry[i][11] = ':';
		changelog_entry[i][12] = ' ';
		NC_cache_get_field_str(i, 1, buf4096a);
		ctoa(changelog_entry[i] + 13, buf4096, sizeof(changelog_entry[i]) - 13);
	}
	NC_cache_delete(dbcache);

#ifdef DEV
	atoc(buf4096, "SELECT COUNT(stamp) FROM chg", 4096);
	dbcache = NC_mysql_query(buf4096a);
	if (NC_cache_get_field_int(0, 0) > CHANGELOG_ENTRIES_PER_DIALOG_PAGE * DIALOG_CHANGELOG_NUM_PAGES) {
		printf("WARN: not enough changelog dialog pages to show all entries\n");
	}
	NC_cache_delete(dbcache);
#endif
}

static
void changelog_on_player_connect(int playerid)
{
	int i;

	SendClientMessage(playerid, COL_CHANGELOG, INFO"Last updates (for more, see /updates):");
	for (i = 0; i < num_changelog_entries; i++) {
		SendClientMessage(playerid, COL_CHANGELOG, changelog_entry[i]);
	}
}

struct CB_CHANGELOG_LOADINFO {
	int player_cc;
	int page_from_0;
	unsigned char request_id;
};

static
void changelog_cb_page_loaded(void *_data)
{
#if (CHANGELOG_ENTRY_MAXLEN + 11 + 1 + 1) * CHANGELOG_ENTRIES_PER_DIALOG_PAGE + 200 > LIMIT_DIALOG_INFO
#error "reduce amount of changelog entries per page"
#endif

	struct CB_CHANGELOG_LOADINFO cbdata;
	int playerid;
	char info[4096], title[32], *b;
	int i, rowcount;

	cbdata = *((struct CB_CHANGELOG_LOADINFO*) _data);
	free(_data);
	PLAYER_CC_GET_CHECK_RETURN(cbdata.player_cc, playerid);

	if (cbdata.request_id != changelog_request_id[playerid]) {
		return;
	}

	rowcount = NC_cache_get_row_count();
	b = info;
	if (cbdata.page_from_0) {
		/*Need leading space or somehow that column won't exist.*/
		b += sprintf(b, " \t"ECOL_SAMP_GREY"< newer entries\n");
	}
	for (i = 0; i < rowcount && i < CHANGELOG_ENTRIES_PER_DIALOG_PAGE; i++) {
		NC_cache_get_field_str(i, 0, buf32a); ctoai(cbuf32);
		NC_cache_get_field_str(i, 1, buf144a); ctoai(cbuf144);
		b += sprintf(b, "%s\t%s\n", cbuf32, cbuf144);
	}
	if (rowcount > CHANGELOG_ENTRIES_PER_DIALOG_PAGE) {
		/*Need leading space or somehow that column won't exist.*/
		sprintf(b, " \t"ECOL_SAMP_GREY"older entries >\n");
	}
	/*All entries in a list must end with \n apparently.*/

	sprintf(title, "Changelog page %d", cbdata.page_from_0 + 1);
	dialog_ShowPlayerDialog(
		playerid,
		DIALOG_CHANGELOG_PAGE_0 + cbdata.page_from_0,
		DIALOG_STYLE_TABLIST,
		title,
		info,
		"Select", "Close",
		TRANSACTION_CHANGELOG);
}

static
void changelog_show_dialog(int playerid, int page_from_0)
{
	struct CB_CHANGELOG_LOADINFO *cbdata;
	char query[128], title[32];

	cbdata = malloc(sizeof(struct CB_CHANGELOG_LOADINFO));
	cbdata->player_cc = MK_PLAYER_CC(playerid);
	cbdata->page_from_0 = page_from_0;
	cbdata->request_id = ++changelog_request_id[playerid];
	sprintf(query, "SELECT stamp,entry FROM chg ORDER BY id DESC LIMIT %d OFFSET %d",
			CHANGELOG_ENTRIES_PER_DIALOG_PAGE + 1, /*Doing +1 because then we know if there's a next page.*/
			page_from_0 * CHANGELOG_ENTRIES_PER_DIALOG_PAGE);
	common_mysql_tquery(query, changelog_cb_page_loaded, cbdata);

	sprintf(title, "Changelog page %d", page_from_0 + 1);
	dialog_ShowPlayerDialog(
		playerid,
		DIALOG_CHANGELOG_PAGE_0 + page_from_0,
		DIALOG_STYLE_LIST,
		title,
		ECOL_SAMP_GREY"<loading>",
		"Close", "",
		TRANSACTION_CHANGELOG);
}

static
void changelog_on_dialog_response(int playerid, int response, int dialogid, int listitem)
{
	int page_from_0;

	page_from_0 = dialogid - DIALOG_CHANGELOG_PAGE_0;
	changelog_request_id[playerid]++;

	if (response) {
		if (listitem == 0) {
			if (page_from_0) {
				changelog_show_dialog(playerid, page_from_0 - 1);
			}
		} else if (listitem == CHANGELOG_ENTRIES_PER_DIALOG_PAGE + !!page_from_0) {
			changelog_show_dialog(playerid, page_from_0 + 1);
		}
	}
}
