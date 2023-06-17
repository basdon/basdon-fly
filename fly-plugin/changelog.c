/*And +2 entries for previous/next page.*/
#define CHANGELOG_ENTRIES_PER_DIALOG_PAGE 8
#define CHANGELOG_ENTRY_MAXLEN 144

#if (CHANGELOG_ENTRY_MAXLEN + 11 + 1 + 1) * CHANGELOG_ENTRIES_PER_DIALOG_PAGE + 200 > LIMIT_DIALOG_INFO
#error "too many changelog entries per dialog page, exceeding dialog info length limit"
#endif

static char changelog_onconnect_line[2048];
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
	int dbcache;

	atoc(buf4096, "SELECT stamp,entry FROM chg ORDER BY id DESC LIMIT 1", 4096);
	dbcache = NC_mysql_query(buf4096a);
	if (NC_cache_get_row_count()) {
		memcpy(changelog_onconnect_line, "Last update (/updates): ", 24);
		NC_cache_get_field_str(0, 0, buf4096a);
		ctoa(changelog_onconnect_line + 24, buf4096, 12);
		changelog_onconnect_line[24 + 11] = ':';
		changelog_onconnect_line[24 + 12] = ' ';
		NC_cache_get_field_str(0, 1, buf4096a);
		ctoa(changelog_onconnect_line + 24 + 13, buf4096, sizeof(changelog_onconnect_line) - 24 - 13);
	}
	NC_cache_delete(dbcache);
}

static
void changelog_on_player_connect(int playerid)
{
	if (changelog_onconnect_line[0]) {
		SendClientMessage(playerid, COL_CHANGELOG, changelog_onconnect_line);
	}

	changelog_request_id[playerid]++;
}

static void changelog_show_dialog(int, int);

static
void changelog_cb_dialog_response(int playerid, struct DIALOG_RESPONSE response)
{
	unsigned int page_from_0;
	unsigned int has_more_pages;

	changelog_request_id[playerid]++;

	if (response.response) {
		page_from_0 = (unsigned int) response.data & 0x7FFFFFFF;
		has_more_pages = (unsigned int) response.data & 0x80000000;
		if (response.listitem == 0) {
			if (page_from_0) {
				changelog_show_dialog(playerid, page_from_0 - 1);
			}
		} else if (has_more_pages && response.listitem == CHANGELOG_ENTRIES_PER_DIALOG_PAGE + !!page_from_0) {
			changelog_show_dialog(playerid, page_from_0 + 1);
		}
	}
}

struct CB_CHANGELOG_LOADINFO {
	int player_cc;
	int page_from_0;
	unsigned char request_id;
};

static
void changelog_cb_query_page_loaded(void *data)
{
	struct DIALOG_INFO dialog;
	struct CB_CHANGELOG_LOADINFO cbdata;
	int playerid;
	unsigned int dialogdata;
	char *info;
	int i, rowcount;

	cbdata = *((struct CB_CHANGELOG_LOADINFO*) data);
	free(data);

	PLAYER_CC_GET_CHECK_RETURN(cbdata.player_cc, playerid);

	if (cbdata.request_id != changelog_request_id[playerid]) {
		return;
	}

	if (dialog_get_transaction(playerid) != DLG_TID_CHANGELOG) {
		/*Player already closed dialog.*/
		return;
	}

	dialog_init_info(&dialog);
	info = dialog.info;
	rowcount = NC_cache_get_row_count();
	dialogdata = cbdata.page_from_0;
	if (cbdata.page_from_0) {
		/*Need leading space or that column won't exist.*/
		info += sprintf(info, " \t"ECOL_SAMP_GREY"< newer entries\n");
	}
	for (i = 0; i < rowcount && i < CHANGELOG_ENTRIES_PER_DIALOG_PAGE; i++) {
		NC_cache_get_field_str(i, 0, buf32a); ctoai(cbuf32);
		NC_cache_get_field_str(i, 1, buf144a); ctoai(cbuf144);
		info += sprintf(info, "%s\t%s\n", cbuf32, cbuf144);
	}
	if (rowcount > CHANGELOG_ENTRIES_PER_DIALOG_PAGE) {
		/*Need leading space or that column won't exist.*/
		sprintf(info, " \t"ECOL_SAMP_GREY"older entries >\n");
		dialogdata |= 0x80000000;
	}
	/*All entries in a list must end with \n apparently.*/

	sprintf(dialog.caption, "Changelog page %d", cbdata.page_from_0 + 1);
	dialog.transactionid = DLG_TID_CHANGELOG;
	dialog.style = DIALOG_STYLE_TABLIST;
	dialog.button1 = "Select";
	dialog.button2 = "Close";
	dialog.handler.data = (void*) dialogdata;
	dialog.handler.callback = changelog_cb_dialog_response;
	dialog_show(playerid, &dialog);
}

static
void changelog_show_dialog(int playerid, int page_from_0)
{
	struct DIALOG_INFO dialog;
	struct CB_CHANGELOG_LOADINFO *cbdata;
	char query[128];

	cbdata = malloc(sizeof(struct CB_CHANGELOG_LOADINFO));
	cbdata->player_cc = MK_PLAYER_CC(playerid);
	cbdata->page_from_0 = page_from_0;
	cbdata->request_id = ++changelog_request_id[playerid];
	sprintf(query, "SELECT stamp,entry FROM chg ORDER BY id DESC LIMIT %d OFFSET %d",
			CHANGELOG_ENTRIES_PER_DIALOG_PAGE + 1, /*Doing +1 because then we know if there's a next page.*/
			page_from_0 * CHANGELOG_ENTRIES_PER_DIALOG_PAGE);
	common_mysql_tquery(query, changelog_cb_query_page_loaded, cbdata);

	dialog_init_info(&dialog);
	dialog.transactionid = DLG_TID_CHANGELOG;
	dialog.style = DIALOG_STYLE_LIST;
	dialog.info = ECOL_SAMP_GREY"<loading>";
	sprintf(dialog.caption, "Changelog page %d", page_from_0 + 1);
	dialog.button1 = "Close";
	dialog_show(playerid, &dialog);
}
