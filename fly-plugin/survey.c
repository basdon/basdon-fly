/*#define SURVEY_ACTIVE*/
#define SURVEY_CURRENT_ID 1

#ifdef SURVEY_ACTIVE
static char survey_bothered_player[MAX_PLAYERS];

static
void survey_cb_dlg(int playerid, struct DIALOG_RESPONSE response)
{
	TRACE;
	if (response.inputtext[0]) {
		SendClientMessage(playerid, 0x88FF88FF, "Survey: answer recorded, thank you! you can change your answer with /survey");
		common_mysql_escape_string(response.inputtext, cbuf144, sizeof(cbuf144));
		csprintf(buf4096,
			"INSERT INTO srv(u,s,a)"
				"VALUES (%d,%d,'%s')"
				"ON DUPLICATE KEY UPDATE a='%s'",
			userid[playerid],
			SURVEY_CURRENT_ID,
			cbuf144,
			cbuf144
		);
		NC_mysql_tquery_nocb(buf4096a);
	}
}

static
void survey_show_dialog(int playerid, char *current_answer)
{
	TRACE;
	struct DIALOG_INFO dialog;
	char *info;

	dialog_init_info(&dialog);
	info = dialog.info;
	info += sprintf(info, "%s", ECOL_DIALOG_TEXT);
	info += sprintf(info, "%s", "We are doing a 2026 kick-off groupflight on Jan 30 (Friday) or 31 (Saturday)\n");
	info += sprintf(info, "%s", "If you want to join, what time would work for you?\n");
	info += sprintf(info, "%s", "Please include your timezone\n\n");
	info += sprintf(info, "%s", "Example: GMT-3 friday 8-11pm saturday 9am-2pm 9pm-12am");
	if (current_answer) {
		info += sprintf(info, "%s%s", "\n\nYour current answer: {ffffff}", current_answer);
	}
	dialog.transactionid = DLG_TID_SURVEY;
	dialog.style = DIALOG_STYLE_INPUT;
	dialog.caption = "S U R V E Y";
	dialog.button1 = "Submit";
	dialog.button2 = "Ask later";
	dialog.handler.callback = survey_cb_dlg;
	dialog_show(playerid, &dialog);
}

static
void survey_cb_loadquery_review(void *data)
{
	TRACE;
	int playerid;

	playerid = PLAYER_CC_GETID(data);
	if (!PLAYER_CC_CHECK(data, playerid)) {
		return;
	}

	if (NC_cache_get_row_count()) {
		NC_PARS(3);
		nc_params[1] = 0;
		nc_params[2] = 0;
		nc_params[3] = buf144a;
		NC(n_cache_get_field_s);
		ctoa(cbuf64, buf144, sizeof(cbuf64));

		survey_show_dialog(playerid, cbuf64);
	} else {
		survey_show_dialog(playerid, "(no answer)");
	}
}

static
void survey_cb_loadquery_show_or_prompt(void *data)
{
	TRACE;
	int playerid;

	playerid = PLAYER_CC_GETID(data);
	if (!PLAYER_CC_CHECK(data, playerid)) {
		return;
	}

	if (!NC_cache_get_row_count()) {
		survey_show_dialog(playerid, NULL);
		return;
	}

	NC_PARS(3);
	nc_params[1] = 0;
	nc_params[2] = 0;
	nc_params[3] = buf144a;
	NC(n_cache_get_field_s);
	ctoa(cbuf64, buf144, sizeof(cbuf64));

	SendClientMessage(playerid, 0x88FF88FF, "Survey: when can you join a groupflight Jan 30/31");
	sprintf(cbuf144, "Survey: your answer: %s", cbuf64);
	SendClientMessage(playerid, 0x88FF88FF, cbuf144);
	SendClientMessage(playerid, 0x88FF88FF, "Survey: if you want to change your answer, use /survey");
}

static
void survey_query_current_answer(int playerid, cb_t callback)
{
	TRACE;
	sprintf(
		cbuf4096_,
		"SELECT a "
			"FROM srv "
			"WHERE s=%d AND u=%d "
			"LIMIT 1",
		SURVEY_CURRENT_ID,
		userid[playerid]
	);
	common_mysql_tquery(cbuf4096_, callback, V_MK_PLAYER_CC(playerid));
}
#endif

static
void survey_review_answer_from_cmd(int playerid)
{
	TRACE;
#ifdef SURVEY_ACTIVE
	if (userid[playerid] <= 0) {
		SendClientMessage(playerid, COL_WARN, WARN"you don't have an active user session");
	} else {
		survey_query_current_answer(playerid, survey_cb_loadquery_review);
	}
#else
	SendClientMessage(playerid, COL_WARN, WARN"no survey active currently");
#endif
}

static
void survey_on_player_spawn(int playerid)
{
#ifdef SURVEY_ACTIVE
	TRACE;
	if (!survey_bothered_player[playerid]) {
		survey_bothered_player[playerid] = 1;
		if (userid[playerid] > 0 && loggedstatus[playerid] == LOGGED_IN) {
			survey_query_current_answer(playerid, survey_cb_loadquery_show_or_prompt);
		}
	}
#endif
}

static
void survey_on_player_connect(int playerid)
{
#ifdef SURVEY_ACTIVE
	TRACE;
	survey_bothered_player[playerid] = 0;
#endif
}
