#define MAX_ALLOWED_FAILED_LOGINS_IN_30_MINUTES 10
#define MAX_LOGIN_ATTEMPTS_IN_ONE_SESSION 4

#define BCRYPT_COST 12

/**
The userid of the user that the player is trying to login for.
*/
static int unconfirmed_userid[MAX_PLAYERS];
/**
Time since last seen of the user account that the player is trying to log in as.
*/
static int unconfirmed_timesincelastseen[MAX_PLAYERS];

/**
Failed login attempts.
*/
static char failedlogins[MAX_PLAYERS];

static char
	*LOGIN_CAPTION = "Login",
	*NAMECHANGE_CAPTION = "Change name",
	*REGISTER_CAPTION = "Register";

/*TODO cleanup this mess*/
static void login_show_dialog_login(int playerid, int show_invalid_pw_error);
static int login_change_name_from_input(int playerid, char *inputtext);
static void login_show_dialog_change_name(int playerid, int show_invalid_name_error);
static void login_spawn_as_guest(int playerid);
static void login_query_check_user_exists(int playerid);
static void login_cb_verify_password(void *data);
static void login_show_dialog_register_step1(int playerid, int pw_mismatch);
static void login_cb_register_password_hashed(void *data);
/*jeanine:p:i:29;p:8;a:r;x:6.00;n:login_cb_failed_login_added;*/
static
void login_cb_update_lastfal_after_failed_login_added(void *data)
{
	TRACE;
	csprintf(buf4096,
		"UPDATE usr "
		"SET lastfal="
			"(SELECT stamp "
			"FROM fal "
			"WHERE u=%d "
			"ORDER BY stamp DESC "
			"LIMIT 1) "
		"WHERE i=%d",
		(int) data,
		(int) data);
	NC_mysql_tquery_nocb(buf4096a);
}
/*jeanine:p:i:30;p:22;a:r;x:187.00;n:login_format_register_dialog;*/
/**
Formats text to be displayed in the register dialog box.

@param d destination buffer
@param step 0 or 1
@param pw_mismatch 1 to show an error that the passwords didn't match
*/
static
void login_format_register_dialog(char *d, int step, int pw_mismatch)
{
	TRACE;
	if (pw_mismatch) {
		d += sprintf(d, ECOL_WARN"Passwords do not match!\n\n");
	}
	d += sprintf(d, ECOL_DIALOG_TEXT"Welcome! Register your account or "
			"continue as a guest.\n\n");
	d += sprintf(d, step == 0 ? ECOL_INFO : ECOL_DIALOG_TEXT);
	d += sprintf(d, "* choose a password");
	if (step == 0) d += sprintf(d, " <<<<");
	d += sprintf(d, step == 1 ? ECOL_INFO"\n" : ECOL_DIALOG_TEXT"\n");
	d += sprintf(d, "* confirm your password");
	if (step == 1) d += sprintf(d, " <<<<");
}
/*jeanine:p:i:27;p:28;a:r;x:2.00;n:login_cb_dlg_namechange;*/
static
void login_cb_dlg_namechange(int playerid, struct DIALOG_RESPONSE response)
{
	TRACE;
	if (response.response) {
		if (!response.inputtext[0] || !stricmp(pdata[playerid]->name, response.inputtext)) {
			/*Edge case: if empty string or same name as current, go back to login dialog.*/
			login_show_dialog_login(playerid, 0);/*jeanine:s:a:r;i:26;*/
		} else if (!login_change_name_from_input(playerid, response.inputtext)) {/*jeanine:r:i:19;*/
			login_show_dialog_change_name(playerid, 1);/*jeanine:s:a:r;i:28;*/
		} else {
			unconfirmed_userid[playerid] = -1;
			dialog_ensure_transaction(playerid, DLG_TID_LOGIN);
			login_query_check_user_exists(playerid);/*jeanine:s:a:r;i:2;*/
		}
	} else {
		/*Cancel should not go back to login dialog,*/
		/*otherwise spamming escape will keep you in a loop.*/
		/*If player keep hitting ESC, they are probably as fed up with login/register*/
		/*dialogs as I am, so just spawn them as guest to let them play.*/
		login_spawn_as_guest(playerid);/*jeanine:s:a:r;i:9;*/
	}
}
/*jeanine:p:i:28;p:25;a:r;x:11.00;y:55.00;n:login_show_dialog_change_name;*/
static
void login_show_dialog_change_name(int playerid, int show_invalid_name_error)
{
	TRACE;
	struct DIALOG_INFO dialog;

	dialog_init_info(&dialog);
	dialog.transactionid = DLG_TID_LOGIN;
	dialog.style = DIALOG_STYLE_INPUT;
	dialog.caption = NAMECHANGE_CAPTION;
	dialog.info =
		(ECOL_WARN"Invalid name or name is taken (press tab).\n"
		"\n"ECOL_DIALOG_TEXT
		"Enter your new name (3-20 length, 0-9a-zA-Z=()[]$@._).\n"
		"Names starting with @ are reserved for guests.") + 60 * (show_invalid_name_error ^ 1);
	dialog.button1 = "Change";
	dialog.button2 = "Play as guest";
	dialog.handler.callback = login_cb_dlg_namechange;/*jeanine:r:i:27;*/
	dialog_show(playerid, &dialog);
}
/*jeanine:p:i:25;p:26;a:r;x:9.00;y:-19.00;n:login_cb_dlg_login_or_namechange;*/
static
void login_cb_dlg_login_or_namechange(int playerid, struct DIALOG_RESPONSE response)
{
	TRACE;
	if (response.response) {
		GameTextForPlayer(playerid, 0x800000, 3, "~b~Logging in...");
		if (pwdata[playerid]) {
			atoc(buf144, response.inputtext, 144);
			atoc(buf4096, pwdata[playerid], 144);
			common_bcrypt_check(
				buf144a,
				buf4096a,
				login_cb_verify_password,/*jeanine:r:i:8;*/
				V_MK_PLAYER_CC(playerid));
			dialog_ensure_transaction(playerid, DLG_TID_LOGIN);
		} else {
			logprintf("login_cb_dlg_login_or_namechange: no password");
		}
	} else {
		login_show_dialog_change_name(playerid, 0);/*jeanine:r:i:28;*/
	}
}
/*jeanine:p:i:26;p:1;a:r;x:57.00;y:-92.00;n:login_show_dialog_login;*/
static
void login_show_dialog_login(int playerid, int show_invalid_pw_error)
{
	TRACE;
	struct DIALOG_INFO dialog;

	dialog_init_info(&dialog);
	dialog.transactionid = DLG_TID_LOGIN;
	dialog.style = DIALOG_STYLE_PASSWORD;
	dialog.caption = LOGIN_CAPTION;
	dialog.info =
		(ECOL_WARN"Incorrect password!\n"
		"\n"ECOL_DIALOG_TEXT
		"Welcome! This account is registered.\n"
		"Please sign in or change your name.") + 37 * (show_invalid_pw_error ^ 1);
	dialog.button1 = "Login";
	dialog.button2 = "Change name";
	dialog.handler.options = DLG_OPT_NO_SANITIZE_INPUTTEXT;
	dialog.handler.callback = login_cb_dlg_login_or_namechange;/*jeanine:r:i:25;*/
	dialog_show(playerid, &dialog);
}
/*jeanine:p:i:23;p:24;a:r;x:17.00;n:login_cb_dlg_register_confirmpass;*/
static
void login_cb_dlg_register_confirmpass(int playerid, struct DIALOG_RESPONSE response)
{
	TRACE;
	char tmp_hash_buf[SHA256BUFSIZE];
	register int cmpres;

	/*Confirm | Cancel*/
	if (response.response) {
		if (!pwdata[playerid]) {
			logprintf("login_cb_dlg_register_confirmpass: confirmed pw without entering one!");
			return;
		}
		SAMP_SHA256(tmp_hash_buf, response.inputtext);
		cmpres = memcmp(tmp_hash_buf, pwdata[playerid], SHA256BUFSIZE);
		free(pwdata[playerid]);
		pwdata[playerid] = NULL;
		if (cmpres) {
			login_show_dialog_register_step1(playerid, 1);/*jeanine:s:a:r;i:22;*/
		} else {
			GameTextForPlayer(playerid, 0x800000, 3, "~b~Making your account...");
			dialog_ensure_transaction(playerid, DLG_TID_LOGIN);
			atoc(buf144, response.inputtext, 144);
			common_bcrypt_hash(
				buf144a,
				login_cb_register_password_hashed,/*jeanine:r:i:6;*/
				V_MK_PLAYER_CC(playerid));
		}
	} else {
		if (pwdata[playerid]) {
			free(pwdata[playerid]);
			pwdata[playerid] = NULL;
		}
		login_show_dialog_register_step1(playerid, 0);/*jeanine:s:a:r;i:22;*/
	}
}
/*jeanine:p:i:24;p:21;a:r;x:9.00;n:login_show_dialog_register_step2;*/
/**
Shows the register dialog box for the first step (confirm password).
*/
static
void login_show_dialog_register_step2(int playerid)
{
	TRACE;
	struct DIALOG_INFO dialog;

	dialog_init_info(&dialog);
	login_format_register_dialog(dialog.info, 1, 0);/*jeanine:s:a:r;i:30;*/
	dialog.transactionid = DLG_TID_LOGIN;
	dialog.style = DIALOG_STYLE_PASSWORD;
	dialog.caption = REGISTER_CAPTION;
	dialog.button1 = "Confirm";
	dialog.button2 = "Cancel";
	dialog.handler.options = DLG_OPT_NO_SANITIZE_INPUTTEXT;
	dialog.handler.callback = login_cb_dlg_register_confirmpass;/*jeanine:r:i:23;*/
	dialog_show(playerid, &dialog);
}
/*jeanine:p:i:21;p:22;a:r;x:4.00;n:login_cb_dlg_register_firstpass;*/
static
void login_cb_dlg_register_firstpass(int playerid, struct DIALOG_RESPONSE response)
{
	TRACE;
	/*Next | Play as guest*/
	if (!response.response) {
		login_spawn_as_guest(playerid);/*jeanine:r:i:9;*/
	} else {
		if (pwdata[playerid]) {
			free(pwdata[playerid]);
		}
		pwdata[playerid] = malloc(SHA256BUFSIZE);
		SAMP_SHA256(pwdata[playerid], response.inputtext);
		login_show_dialog_register_step2(playerid);/*jeanine:r:i:24;*/
	}
}
/*jeanine:p:i:22;p:1;a:r;x:47.00;y:139.00;n:login_show_dialog_register_step1;*/
/**
Shows the register dialog box for the first step (enter password).
*/
static
void login_show_dialog_register_step1(int playerid, int pw_mismatch)
{
	TRACE;
	struct DIALOG_INFO dialog;

	dialog_init_info(&dialog);
	login_format_register_dialog(dialog.info, 0, pw_mismatch);/*jeanine:r:i:30;*/
	dialog.transactionid = DLG_TID_LOGIN;
	dialog.style = DIALOG_STYLE_PASSWORD;
	dialog.caption = REGISTER_CAPTION;
	dialog.button1 = "Next";
	dialog.button2 = "Play as guest";
	dialog.handler.options = DLG_OPT_NO_SANITIZE_INPUTTEXT;
	dialog.handler.callback = login_cb_dlg_register_firstpass;/*jeanine:r:i:21;*/
	dialog_show(playerid, &dialog);
}
/*jeanine:p:i:19;p:27;a:r;x:8.00;y:-5.00;n:login_change_name_from_input;*/
/**
Sets the player username from user input if it's a valid non-guest username.

@return 1 if the username was valid and is now set
*/
static
int login_change_name_from_input(int playerid, char *inputtext)
{
	TRACE;
	int len;

	len = strlen(inputtext);
	/*MAX_PLAYER_NAME is 24 but client may only connect if their name
	length is at max 20*/
	return 2 < len && len < MAX_PLAYER_NAME - 3 &&
		inputtext[0] != '@' &&
		SetPlayerName(playerid, inputtext) == 1;
}
/*jeanine:p:i:20;p:17;a:t;x:3.75;n:login_create_user;*/
static
char* login_format_query_insert_user(int playerid, char *password, int groups)
{
	TRACE;

	sprintf(
		cbuf4096_,
	        "INSERT INTO "
		"usr(name,pw,registertime,lastseengame,groups,prefs) "
		"VALUES('%s','%s',UNIX_TIMESTAMP(),UNIX_TIMESTAMP(),%d,%d)",
	        pdata[playerid]->name,
		password,
		groups,
	        DEFAULTPREFS
        );
        return cbuf4096_;
}
/*jeanine:p:i:17;p:3;a:t;x:115.00;y:-14.00;n:login_create_session;*/
static
char* login_format_query_insert_session_row(int playerid)
{
	TRACE;
	char versionstring[CLIENT_VERSIONSTRING_MAXLEN * 2 + 1];

	common_mysql_escape_string(playerpool->version[playerid], versionstring, sizeof(versionstring));
	sprintf(
		cbuf4096_,
		"INSERT INTO ses(u,s,e,ip,netgameversion,versionstring) "
			"VALUES(%d,UNIX_TIMESTAMP(),UNIX_TIMESTAMP(),'%s',%d,'%s')",
		unconfirmed_userid[playerid], /*TODO: I don't like the name 'unconfirmed_userid' being used here*/
		pdata[playerid]->ip,
		player_netgame_version[playerid],
		versionstring
	);
	return cbuf4096_;
}
/*jeanine:p:i:18;p:16;a:r;x:366.00;y:-150.00;n:login_login_player;*/
/*color list grabbed from archived samp wiki, with darker colors removed and colors shuffled*/
static int player_colors[] = {
	0x10DC29FF,0x05D1CDFF,0xCE79EEFF,0x93B7E4FF,0x93AB1CFF,0xDCDE3DFF,0xFFD720FF,0x11F891FF,0xBCE635FF,0x0C8E5DFF,
	0x95BAF0FF,0x388EEAFF,0x2FC827FF,0xCF72A9FF,0x9F945CFF,0xC471BDFF,0xB98519FF,0x48C000FF,0x65ADEBFF,0x20D4ADFF,
	0x0DE018FF,0xFF1493FF,0xF4A460FF,0xEE82EEFF,0xFAFB71FF,0x829DC7FF,0x53EB10FF,0x0FD9FAFF,0xFC42A8FF,0x18F71FFF,
	0xEF6CE8FF,0xBD34DAFF,0x148B8BFF,0xE9AB2FFF,0xE59338FF,0xF0E68CFF,0x10C9C5FF,0xF2F853FF,0x14ff7fFF,0xEEDC2DFF,
	0xCEA6DFFF,0x3793FAFF,0xF09F5BFF,0x22F767FF,0x3FE65CFF,0x12D6D4FF,0x0495CDFF,0x247C1BFF,0xC1F7ECFF,0x42ACF5FF,
	0xCB7ED3FF,0x0BE472FF,0xE3AC12FF,0x2FD9DEFF,0xDFB935FF,0xD8C762FF,0xFA24CCFF
};
/**
Set the player's status as logged in and spawn them.
Broadcasting a join message is the responsability of the caller.

@param status one of LOGGED_ values
*/
static
void login_login_player(int playerid, int status)
{
	TRACE;
	int i;

	loggedstatus[playerid] = status;

	for (i = 0; i < playercount; i++){
		if (players[i] == playerid) {
			goto alreadyin; /*TODO: this should NEVER happen. recheck why this is here*/
		}
	}
	players[playercount++] = playerid;
alreadyin:

	userid[playerid] = unconfirmed_userid[playerid];
	veh_load_user_model_stats(playerid);

	if (pwdata[playerid]) {
		free(pwdata[playerid]);
		pwdata[playerid] = NULL;
	}

	if (userid[playerid] > 0) {
		csprintf(buf4096,
			"INSERT INTO gpci(u,v,c,first,last) "
			"VALUES(%d,'%s',1,UNIX_TIMESTAMP(),UNIX_TIMESTAMP()) "
			"ON DUPLICATE KEY UPDATE c=c+1,last=UNIX_TIMESTAMP()",
			userid[playerid],
			playerpool->gpci[playerid]);
		NC_mysql_tquery_nocb(buf4096a);
	}

	SendClientMessage(playerid, COL_SAMP_GREY, "Privacy note: game chat is logged and publicly archived and readable on our website. Only /pm and /r are private.");
	SendClientMessage(playerid, 0x929BEFFF, "Join our Discord server: https://basdon.net/discord"); /*discord's blue color is too dark to be readable in chat messages, so this a slightly lighter shade of blue*/

	if (status == LOGGED_GUEST) {
		money_set(playerid, MONEY_DEFAULT_AMOUNT);
	} else {
		veh_spawn_player_vehicles(playerid);
	}
	class_on_player_request_class(playerid, -1);

	SetPlayerColor(playerid, player_colors[amxrandom(sizeof(player_colors)/sizeof(player_colors[0]))]);
}
/*jeanine:p:i:15;p:11;a:r;x:19.00;n:login_cb_create_session_guest;*/
/**
Callback for query that creates a session for a guest user.
*/
static
void login_cb_create_session_guest(void *data)
{
	TRACE;
	int playerid;
	char msg144[144];

	playerid = PLAYER_CC_GETID(data);
	if (!PLAYER_CC_CHECK(data, playerid)) {
		return;
	}

	HideGameTextForPlayer(playerid);
	sessionid[playerid] = NC_cache_insert_id();
	/*
	if (sessionid[playerid] == -1) {
		Failed to create session.
		No real problem, player can still upgrade their guest account to non-guest, but this
		sucks for data integrity because playtime in user's account will be greater than sum of session times since
		this session won't be counted, same with 'last seen' data.
	}
	*/
	SendClientMessage(playerid, COL_INFO, INFO"You are now playing as a guest. You can use /register at any time to save your stats.");
	login_login_player(playerid, LOGGED_GUEST);/*jeanine:s:a:r;i:18;*/
	sprintf(msg144, "%s[%d] joined as a guest, welcome!", pdata[playerid]->name, playerid);
	SendClientMessageToAllAndIRC(ECHO_PACK12_LOGIN, COL_JOIN, msg144);
}
/*jeanine:p:i:16;p:5;a:r;x:3.75;n:login_cb_create_session_new_member;*/
/**
Callback for query that creates a session for a newly registered member.
*/
static
void login_cb_create_session_new_member(void *data)
{
	TRACE;
	int playerid;
	char msg144[144];

	playerid = PLAYER_CC_GETID(data);
	if (!PLAYER_CC_CHECK(data, playerid)) {
		return;
	}

	dialog_end_transaction(playerid, DLG_TID_LOGIN);
	HideGameTextForPlayer(playerid);
	sessionid[playerid] = NC_cache_insert_id();
	/*
	if (sessionid[playerid] == -1) {
		failed to create session
		no real problem, but time will not be registered
	}
	*/
	login_login_player(playerid, LOGGED_IN);/*jeanine:r:i:18;*/
	sprintf(msg144, "%s[%d] just registered an account, welcome!", pdata[playerid]->name, playerid);
	SendClientMessageToAllAndIRC(ECHO_PACK12_LOGIN, COL_JOIN, msg144);
}
/*jeanine:p:i:14;p:7;a:r;x:22.00;n:login_cb_create_session_existing_member;*/
/**
Callback for query that creates a session for an existing member.
*/
static
void login_cb_create_session_existing_member(void *data)
{
	TRACE;
	int playerid, n_ago;
	char msg144[144], *units_ago;

	playerid = PLAYER_CC_GETID(data);
	if (!PLAYER_CC_CHECK(data, playerid)) {
		return;
	}

	dialog_end_transaction(playerid, DLG_TID_LOGIN);
	HideGameTextForPlayer(playerid);
	sessionid[playerid] = NC_cache_insert_id();
	/*
	if (sessionid[playerid] == -1) {
		failed to create session
		No real problem, but this sucks for data integrity because playtime
		in user's account will be greater than sum of session times since
		this session won't be counted, same with 'last seen' data.
	}
	*/
	login_login_player(playerid, LOGGED_IN);/*jeanine:s:a:r;i:18;*/

	n_ago = unconfirmed_timesincelastseen[playerid];
	if (n_ago < 60 * 60) {
		n_ago = n_ago / 60;
		units_ago = "minutes";
	} else if (n_ago < 60 * 60 * 24 * 2) {
		n_ago = n_ago / 60 / 60;
		units_ago = "hours";
	} else {
		n_ago = n_ago / 60 / 60 / 24;
		units_ago = "days";
	}
	sprintf(
		msg144,
		"%s[%d] just logged in, welcome back! Last connected: %d %s ago",
		pdata[playerid]->name,
		playerid,
		n_ago,
		units_ago
	);
	SendClientMessageToAllAndIRC(ECHO_PACK12_LOGIN, COL_JOIN, msg144);
}
/*jeanine:p:i:11;p:9;a:r;x:15.00;n:login_cb_create_guest_usr;*/
/**
Callback for query that creates a guest user.
*/
static
void login_cb_create_guest_usr(void *data)
{
	TRACE;
	int playerid;
	char msg144[144];

	playerid = PLAYER_CC_GETID(data);
	if (!PLAYER_CC_CHECK(data, playerid)) {
		return;
	}

	GameTextForPlayer(playerid, 0x800000, 3, "~b~Creating game session...");
	unconfirmed_userid[playerid] = NC_cache_insert_id();
	if (unconfirmed_userid[playerid] == -1) {
		HideGameTextForPlayer(playerid);
		SendClientMessage(playerid, COL_WARN, WARN"An error occurred while creating a guest user.");
		SendClientMessage(playerid, COL_WARN, WARN"You can play, but you won't be able to save your stats later.");
		login_login_player(playerid, LOGGED_GUEST);/*jeanine:s:a:r;i:18;*/
		sprintf(msg144, "%s[%d] joined as a guest, welcome!", pdata[playerid]->name, playerid);
		SendClientMessageToAllAndIRC(ECHO_PACK12_LOGIN, COL_JOIN, msg144);
		return;
	}
	common_mysql_tquery(login_format_query_insert_session_row(playerid), login_cb_create_session_guest, V_MK_PLAYER_CC(playerid));/*jeanine:r:i:15;*/
}
/*jeanine:p:i:12;p:20;a:t;x:3.75;n:login_give_guest_name;*/
/**
Give the user a name prefixed with an '@' symbol, indicating they're a guest.

@return 0 on failure, the player will be kicked so abort all processing for player
*/
static
int login_give_guest_name(int playerid)
{
	TRACE;
	struct playerdata *p;
	char newname[MAX_PLAYER_NAME + 1];
	int attempts, i;

	p = pdata[playerid];
	newname[0] = '@';
	/*MAX_PLAYER_NAME is 24 but client may only connect if their name
	length is at max 20*/
	if (p->namelen < MAX_PLAYER_NAME - 4) {
		memcpy(newname + 1, p->name, p->namelen);
		newname[p->namelen + 1] = 0;
		if (SetPlayerName(playerid, newname) == 1) {
			return 1;
		}
	}
	newname[11] = 0;
	attempts = 5;
	while (attempts-- > 0) {
		for (i = 1; i < 10; i++) {
			newname[i] = 'a' + (char) amxrandom('z' - 'a' + 1);
		}
		if (SetPlayerName(playerid, newname) == 1) {
			return 1;
		}
	}
	logprintf("login: failed to give user a random guestname");
	SendClientMessage(playerid, COL_WARN, WARN"Fatal error, please reconnect!");
	natives_Kick(playerid, "can't login", NULL, -1);
	return 0;
}
/*jeanine:p:i:9;p:21;a:r;x:279.00;y:-59.00;n:login_spawn_as_guest;*/
/**
Create a guest account and session for player and log them in and spawn them.

If the player does not have a guest name, they will be given one (or kicked on
failure).
*/
static
void login_spawn_as_guest(int playerid)
{
	TRACE;
	char *query;

	if (pdata[playerid]->name[0] != '@' && !login_give_guest_name(playerid)) {
		return; /*user is kicked at this point*/
	}
	GameTextForPlayer(playerid, 0x800000, 3, "~b~Creating guest account...");
	query = login_format_query_insert_user(playerid, /*password*/ "", GROUP_GUEST);
	common_mysql_tquery(query, login_cb_create_guest_usr, V_MK_PLAYER_CC(playerid));/*jeanine:r:i:11;*/
}
/*jeanine:p:i:10;p:7;a:r;x:15.00;y:120.00;n:login_spawn_as_guest_WITHOUT_ACCOUNT;*/
/**
Spawns a player as a guest without account or session. ONLY USE ON ERROR!

Use login_spawn_as_guest when wanting to let player continue as a guest.

If the player does not have a guest name, they will be given one (or kicked on
failure).
*/
static
void login_spawn_as_guest_WITHOUT_ACCOUNT(int playerid)
{
	TRACE;
	char msg144[144];

	if (!login_give_guest_name(playerid)) {
		return; /*user is kicked at this point*/
	}
	login_login_player(playerid, LOGGED_GUEST);/*jeanine:s:a:r;i:18;*/
	sprintf(msg144, "%s[%d] joined as a guest (login error), welcome!", pdata[playerid]->name, playerid);
	SendClientMessageToAllAndIRC(ECHO_PACK12_LOGIN, COL_JOIN, msg144);
}
/*jeanine:p:i:7;p:8;a:r;x:84.00;n:login_cb_load_account_data;*/
static
void login_cb_load_account_data(void *data)
{
	TRACE;
	struct DIALOG_INFO *dialog;
	int playerid, *f = nc_params + 2, falng, lastfal;

	playerid = PLAYER_CC_GETID(data);
	if (!PLAYER_CC_CHECK(data, playerid)) {
		return;
	}

	if (!NC_cache_get_row_count()) {
		HideGameTextForPlayer(playerid);
		logprintf("login_cb_load_account_data: empty response");
		dialog = alloca(sizeof(struct DIALOG_INFO));
		dialog_init_info(dialog);
		dialog->info = ECOL_WARN"An error occurred, you will be spawned as a guest";
		dialog->caption = LOGIN_CAPTION;
		dialog->button1 = "Ok";
		dialog_show(playerid, dialog);
		login_spawn_as_guest_WITHOUT_ACCOUNT(playerid);/*jeanine:r:i:10;*/
		return;
	}

	NC_PARS(2);
	nc_params[1] = 0;
	money_set(playerid, (*f = 0, NC(n_cache_get_field_i)));
	/* MONEY_SET CHANGES NC_PARAMS[1]! */
	nc_params[1] = 0;
	playerodoKM[playerid] = (*f = 1, NCF(n_cache_get_field_f));
	score_flight_time[playerid] = (*f = 2, NC(n_cache_get_field_i));
	score_play_time[playerid] = (*f = 3, NC(n_cache_get_field_i));
	prefs[playerid] = (*f = 4, NC(n_cache_get_field_i));
	falng = (*f = 5, NC(n_cache_get_field_i));
	lastfal = (*f = 6, NC(n_cache_get_field_i));
	pdata[playerid]->groups = (*f = 7, NC(n_cache_get_field_i));
	nametags_max_distance[playerid] = (unsigned short) (*f = 8, NC(n_cache_get_field_i));

	score_update_score(playerid);

	csprintf(buf4096, "UPDATE usr SET lastseengame=UNIX_TIMESTAMP() WHERE i=%d LIMIT 1", unconfirmed_userid[playerid]);
	NC_mysql_tquery_nocb(buf4096a);

	GameTextForPlayer(playerid, 0x800000, 3, "~b~Creating game session...");
	common_mysql_tquery(login_format_query_insert_session_row(playerid), login_cb_create_session_existing_member, V_MK_PLAYER_CC(playerid));/*jeanine:r:i:14;*/

	if (lastfal > falng) {
		csprintf(buf4096, "UPDATE usr SET falng=%d WHERE i=%d", lastfal, unconfirmed_userid[playerid]);
		NC_mysql_tquery_nocb(buf4096a);
		dialog = alloca(sizeof(struct DIALOG_INFO));
		dialog_init_info(dialog);
		dialog->transactionid = DLG_TID_LOGIN;
		dialog->caption = "Failed logins";
		dialog->info =
			ECOL_WARN"There were one or more failed logins "
			"since your last visit.\n"
			"Check the website for more details.";
		dialog->button1 = "Ok";
		dialog_show(playerid, dialog);
	}
}
/*jeanine:p:i:8;p:25;a:r;x:139.00;n:login_cb_verify_password;*/
static
void login_cb_verify_password(void *data)
{
	TRACE;
	int playerid, fal;

	playerid = PLAYER_CC_GETID(data);
	if (!PLAYER_CC_CHECK(data, playerid)) {
		return;
	}

	if (!NC_bcrypt_is_equal()) {
		HideGameTextForPlayer(playerid);

		sprintf(cbuf4096_,
			"INSERT INTO fal(u,stamp,ip) "
			"VALUES (%d,UNIX_TIMESTAMP(),'%s')",
			unconfirmed_userid[playerid],
			pdata[playerid]->ip);
		common_mysql_tquery(cbuf4096_, login_cb_update_lastfal_after_failed_login_added, (void*) unconfirmed_userid[playerid]);/*jeanine:r:i:29;*/

		fal = failedlogins[playerid] + 1;
		if (fal > MAX_LOGIN_ATTEMPTS_IN_ONE_SESSION) {
			natives_Kick(playerid, "too many failed logins", 0, -1);
			return;
		}
		failedlogins[playerid] = fal;

		login_show_dialog_login(playerid, 1);/*jeanine:s:a:r;i:26;*/
	} else {
		GameTextForPlayer(playerid, 0x800000, 3, "~b~Loading account...");
		sprintf(cbuf4096_,
			"SELECT cash,distance,flighttime,playtime,prefs,"
			"falng,lastfal,groups,nametagdist "
			"FROM usr "
			"WHERE i=%d",
			unconfirmed_userid[playerid]);
		common_mysql_tquery(cbuf4096_, login_cb_load_account_data, data);/*jeanine:r:i:7;*/
	}
}
/*jeanine:p:i:5;p:6;a:r;x:16.00;n:login_cb_member_user_created;*/
/**
Callback for when member user has been created after registering.
*/
static
void login_cb_member_user_created(void *data)
{
	TRACE;
	int playerid;

	playerid = PLAYER_CC_GETID(data);
	if (!PLAYER_CC_CHECK(data, playerid)) {
		return;
	}

	pdata[playerid]->groups = GROUP_MEMBER;
	unconfirmed_userid[playerid] = NC_cache_insert_id();
	money_set(playerid, MONEY_DEFAULT_AMOUNT);
	if (unconfirmed_userid[playerid] == -1) {
		HideGameTextForPlayer(playerid);
		SendClientMessage(playerid, COL_WARN, WARN"An error occured while registering.");
		SendClientMessage(playerid, COL_WARN, WARN"You will be spawned as a guest.");
		login_spawn_as_guest_WITHOUT_ACCOUNT(playerid);/*jeanine:s:a:r;i:10;*/
		return;
	}
	GameTextForPlayer(playerid, 0x800000, 3, "~b~Creating game session...");
	common_mysql_tquery(login_format_query_insert_session_row(playerid), login_cb_create_session_new_member, V_MK_PLAYER_CC(playerid));/*jeanine:r:i:16;*/
}
/*jeanine:p:i:6;p:23;a:r;x:17.00;n:login_cb_register_password_hashed;*/
/**
Callback for register dialog password hash.
*/
static
void login_cb_register_password_hashed(void *data)
{
	TRACE;
	int playerid;
	char *query;

	playerid = PLAYER_CC_GETID(data);
	if (PLAYER_CC_CHECK(data, playerid)) {
		/*gametext still showing from previous*/
		NC_bcrypt_get_hash(buf144a);
		ctoa(cbuf64, buf144, 144);
		query = login_format_query_insert_user(playerid, cbuf64, GROUP_MEMBER);
		common_mysql_tquery(query, login_cb_member_user_created, V_MK_PLAYER_CC(playerid));/*jeanine:r:i:5;*/
	}
}
/*jeanine:p:i:1;p:2;a:r;x:3.75;n:login_cb_check_user_exists;*/
static
void login_cb_check_user_exists(void *data)
{
	TRACE;
	struct DIALOG_INFO *dialog;
	int playerid, failedattempts, num_rows;
	char password[PW_HASH_LENGTH];

	playerid = PLAYER_CC_GETID(data);
	if (!PLAYER_CC_CHECK(data, playerid)) {
		return;
	}

	dialog_end_transaction(playerid, DLG_TID_LOGIN);
	HideGameTextForPlayer(playerid);

	num_rows = NC_cache_get_row_count();
	if (!num_rows) {
		logprintf("login_cb_check_user_exists: empty response");
		dialog = alloca(sizeof(struct DIALOG_INFO));
		dialog_init_info(dialog);
		dialog->caption = LOGIN_CAPTION;
		dialog->info = ECOL_WARN"An error occurred, you will be spawned as a guest";
		dialog->button1 = "Ok";
		dialog_show(playerid, dialog);
		goto asguest;
	}

	NC_PARS(2);
	nc_params[1] = 0;
	failedattempts = (nc_params[2] = 3, NC(n_cache_get_field_i));

	if (failedattempts > MAX_ALLOWED_FAILED_LOGINS_IN_30_MINUTES) {
		/*TODO: send a message to all online admins about this, because users might be confused about skipping login*/
		/*TODO: change this to a dialog so the user cannot be confused why they can't login*/
asguest:
		SendClientMessage(playerid, COL_SAMP_GREEN, "You will be spawned as a guest.");
		login_spawn_as_guest(playerid);
		return;
	}
	/*Adjust failedlogins for player to allow the maximum amount possible
	without hitting MAX_ALLOWED_FAILED_LOGINS_IN_30_MINUTES. This means if
	they already had 9 failed attempts, they now have 1 more attempt. This
	equivalents to failedlogins value MAX_LOGIN_ATTEMPTS_IN_ONE_SESSION - 1,
	since the next failed attempt will kick them.*/
	failedattempts -= MAX_ALLOWED_FAILED_LOGINS_IN_30_MINUTES;
	failedattempts += MAX_LOGIN_ATTEMPTS_IN_ONE_SESSION;
	if (failedattempts > failedlogins[playerid]) {
		failedlogins[playerid] = failedattempts;
	}

	if (num_rows == 2) {
		NC_PARS(3);
		nc_params[1] = 1;
		nc_params[2] = 0;
		nc_params[3] = buf144a;
		NC(n_cache_get_field_s);
		ctoa(password, buf144, sizeof(password));
		if (pwdata[playerid] == NULL) {
			pwdata[playerid] = malloc(PW_HASH_LENGTH);
		}
		memcpy(pwdata[playerid], password, PW_HASH_LENGTH);
		unconfirmed_userid[playerid] = (nc_params[2] = 1, NC(n_cache_get_field_i));
		unconfirmed_timesincelastseen[playerid] = (nc_params[2] = 2, NC(n_cache_get_field_i));
		login_show_dialog_login(playerid, 0);/*jeanine:r:i:26;*/
	} else {
		login_show_dialog_register_step1(playerid, 0);/*jeanine:r:i:22;*/
	}
}
/*jeanine:p:i:2;p:3;a:r;x:221.00;n:login_query_check_user_exists;*/
/*start of the login process (flow also returns here if user changes their name during login)*/
static
void login_query_check_user_exists(int playerid)
{
	TRACE;
	GameTextForPlayer(playerid, 0x800000, 3, "~b~Checking username...");
	sprintf(cbuf4096_,
		"(SELECT 0,-1,0,count(u) FROM fal WHERE stamp>UNIX_TIMESTAMP()-1800 AND ip='%s')"
		" UNION ALL "
		"(SELECT pw,i,UNIX_TIMESTAMP()-lastseengame,0 FROM usr WHERE name='%s')",
		pdata[playerid]->ip,
		pdata[playerid]->name);
	common_mysql_tquery(cbuf4096_, login_cb_check_user_exists, V_MK_PLAYER_CC(playerid));/*jeanine:r:i:1;*/
}
/*jeanine:p:i:3;p:0;a:b;y:3.00;n:login_on_player_connect;*/
/**
@return 0 when player has unacceptable name
*/
static
int login_on_player_connect(int playerid)
{
	TRACE;
	struct playerdata *pd;

	if (pwdata[playerid]) {
		free(pwdata[playerid]);
		pwdata[playerid] = NULL;
	}
	loggedstatus[playerid] = LOGGED_NO;
	userid[playerid] = -1;
	unconfirmed_userid[playerid] = -1;
	sessionid[playerid] = -1;
	failedlogins[playerid] = 0;
	loggedstatus[playerid] = LOGGED_NO;
	pdata[playerid]->groups = GROUP_GUEST;

	pd = pdata[playerid];
	while (pd->name[0] == '@') {
		SendClientMessage(playerid, COL_SAMP_GREEN, "Names starting with '@' are reserved for guest players.");
		/*wiki states that SetPlayerName does not propagate for the user
		when used in OnPlayerConnect, but tests have proven otherwise.*/
		if (pd->namelen <= 3 ||
			SetPlayerName(playerid, pd->name + 1) != 1)
		{
			SendClientMessage(playerid, COL_WARN, WARN"Failed to change your nickname. Please come back with a different name.");
			natives_Kick(playerid, "invalid name", NULL, -1);
			return 0;
		}
	}

	login_query_check_user_exists(playerid);/*jeanine:r:i:2;*/
	return 1;
}
/*jeanine:p:i:4;p:3;a:b;y:1.88;n:login_on_player_disconnect;*/
void login_on_player_disconnect(int playerid, int reason)
{
	TRACE;
	char msg144[144];
	char *b;

	if (loggedstatus[playerid]) {
		b = msg144;
		b += sprintf(b,
			"%s[%d] left the server",
			pdata[playerid]->name,
			playerid);
		switch (reason) {
		case 0: strcpy(b, " (timeout)"); break;
		case 1: strcpy(b, " (quit)"); break;
		case 2: strcpy(b, " (kicked)"); break;
		}
		SendClientMessageToAll(COL_QUIT, msg144);
	}
	if (pwdata[playerid] != NULL) {
		free(pwdata[playerid]);
		pwdata[playerid] = NULL;
	}
	loggedstatus[playerid] = LOGGED_NO;
}
