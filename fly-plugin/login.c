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

/*
login_on_player_connect
	-> login_query_check_user_exists
		-> login_cb_check_user_exists

login_cb_check_user_exists
	-> login_spawn_as_guest (when too many failed attempts)
	-> login_cb_dlg_login_or_namechange (when user exists)
	-> login_cb_dlg_register_firstpass (when usr doesn't exist)

login_spawn_as_guest
	-> login_cb_create_guest_usr
		-> login_login_player (as guest, when creating user failed)
		-> login_cb_create_session_guest
			-> login_login_player

login_cb_dlg_register_firstpass
	-> login_login_player (on play as guest)
	-> login_dlg_register_confirmpass (to confirm pass)

login_dlg_register_confirmpass
	-> login_cb_dlg_register_firstpass (on cancel or no match)
	-> login_cb_register_password_hashed
		-> login_cb_member_user_created
			-> login_spawn_as_guest (when creating user failed)
			-> login_cb_create_session_new_member

login_cb_dlg_login_or_namechange
	-> login_cb_dlg_namechange (on cancel response)
	-> login_cb_verify_password

login_cb_verify_password
	-> login_cb_dlg_login_or_namechange (on wrong password)
	-> login_cb_load_account_data

login_cb_load_account_data
	-> login_spawn_as_guest (when empty response)
	-> login_cb_create_session_existing_member

login_cb_dlg_namechange
	-> login_spawn_as_guest (on cancel)
	-> login_cb_dlg_login_or_namechange (on empty response)
	-> login_cb_dlg_namechange (on invalid name)
	-> login_query_check_user_exists
		-> login_cb_check_user_exists
*/

/*TODO cleanup this mess*/
static void login_show_dialog_login(int playerid, int show_invalid_pw_error);
static int login_change_name_from_input(int playerid, char *inputtext);
static void login_show_dialog_change_name(int playerid, int show_invalid_name_error);
static void login_spawn_as_guest(int playerid);
static void login_query_check_user_exists(int playerid);
static void login_cb_verify_password(void *data);
static void login_show_dialog_register_step1(int playerid, int pw_mismatch);
static void login_cb_register_password_hashed(void *data);

/**
Callback for insert failed login query, to update the last failed login
field for the user where the failed login was on.
*/
static
void login_cb_failed_login_added(void *data)
{
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

/**
Formats text to be displayed in the register dialog box.

@param d destination buffer
@param step 0 or 1
@param pw_mismatch 1 to show an error that the passwords didn't match
*/
static
void login_format_register_dialog(char *d, int step, int pw_mismatch)
{
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

static
void login_cb_dlg_namechange(int playerid, struct DIALOG_RESPONSE response)
{
	if (response.response) {
		if (!response.inputtext[0] || !stricmp(pdata[playerid]->name, response.inputtext)) {
			/*Edge case: if empty string or same name as current, go back to login dialog.*/
			login_show_dialog_login(playerid, 0);
		} else if (!login_change_name_from_input(playerid, response.inputtext)) {
			login_show_dialog_change_name(playerid, 1);
		} else {
			unconfirmed_userid[playerid] = -1;
			dialog_ensure_transaction(playerid, DLG_TID_LOGIN);
			login_query_check_user_exists(playerid);
		}
	} else {
		/*cancel should not go back to login dialog,
		otherwise spamming escape will keep you in a loop*/
		login_spawn_as_guest(playerid);
	}
}

/**
Shows name change dialog.

@param show_invalid_name_error 1 to show invalid name error message
*/
static
void login_show_dialog_change_name(int playerid, int show_invalid_name_error)
{
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
	dialog.handler.callback = login_cb_dlg_namechange;
	dialog_show(playerid, &dialog);
}

static
void login_cb_dlg_login_or_namechange(int playerid, struct DIALOG_RESPONSE response)
{
	if (response.response) {
		GameTextForPlayer(playerid, 0x800000, 3, "~b~Logging in...");
		if (pwdata[playerid]) {
			atoc(buf144, response.inputtext, 144);
			atoc(buf4096, pwdata[playerid], 144);
			common_bcrypt_check(
				buf144a,
				buf4096a,
				login_cb_verify_password,
				V_MK_PLAYER_CC(playerid));
			dialog_ensure_transaction(playerid, DLG_TID_LOGIN);
		} else {
			logprintf("login_cb_dlg_login_or_namechange: no password");
		}
	} else {
		login_show_dialog_change_name(playerid, 0);
	}
}

/**
Shows the login dialog.

@param show_invalid_pw_error 1 to show invalid password error message
*/
static
void login_show_dialog_login(int playerid, int show_invalid_pw_error)
{
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
	dialog.handler.callback = login_cb_dlg_login_or_namechange;
	dialog_show(playerid, &dialog);
}

static
void login_cb_dlg_register_confirmpass(int playerid, struct DIALOG_RESPONSE response)
{
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
			login_show_dialog_register_step1(playerid, 1);
		} else {
			GameTextForPlayer(playerid, 0x800000, 3, "~b~Making your account...");
			dialog_ensure_transaction(playerid, DLG_TID_LOGIN);
			atoc(buf144, response.inputtext, 144);
			common_bcrypt_hash(
				buf144a,
				login_cb_register_password_hashed,
				V_MK_PLAYER_CC(playerid));
		}
	} else {
		if (pwdata[playerid]) {
			free(pwdata[playerid]);
			pwdata[playerid] = NULL;
		}
		login_show_dialog_register_step1(playerid, 0);
	}
}

/**
Shows the register dialog box for the first step (confirm password).
*/
static
void login_show_dialog_register_step2(int playerid)
{
	struct DIALOG_INFO dialog;

	dialog_init_info(&dialog);
	login_format_register_dialog(dialog.info, 1, 0);
	dialog.transactionid = DLG_TID_LOGIN;
	dialog.style = DIALOG_STYLE_PASSWORD;
	dialog.caption = REGISTER_CAPTION;
	dialog.button1 = "Confirm";
	dialog.button2 = "Cancel";
	dialog.handler.options = DLG_OPT_NO_SANITIZE_INPUTTEXT;
	dialog.handler.callback = login_cb_dlg_register_confirmpass;
	dialog_show(playerid, &dialog);
}

static
void login_cb_dlg_register_firstpass(int playerid, struct DIALOG_RESPONSE response)
{
	/*Next | Play as guest*/
	if (response.response) {
		if (pwdata[playerid]) {
			free(pwdata[playerid]);
		}
		pwdata[playerid] = malloc(SHA256BUFSIZE);
		SAMP_SHA256(pwdata[playerid], response.inputtext);
		login_show_dialog_register_step2(playerid);
	} else {
		login_spawn_as_guest(playerid);
	}
}

/**
Shows the register dialog box for the first step (enter password).
*/
static
void login_show_dialog_register_step1(int playerid, int pw_mismatch)
{
	struct DIALOG_INFO dialog;

	dialog_init_info(&dialog);
	login_format_register_dialog(dialog.info, 0, pw_mismatch);
	dialog.transactionid = DLG_TID_LOGIN;
	dialog.style = DIALOG_STYLE_PASSWORD;
	dialog.caption = REGISTER_CAPTION;
	dialog.button1 = "Next";
	dialog.button2 = "Play as guest";
	dialog.handler.options = DLG_OPT_NO_SANITIZE_INPUTTEXT;
	dialog.handler.callback = login_cb_dlg_register_firstpass;
	dialog_show(playerid, &dialog);
}

/**
Sets the player username from user input if it's a valid non-guest username.

@return 1 if the username was valid and is now set
*/
static
int login_change_name_from_input(int playerid, char *inputtext)
{
	int len;

	len = strlen(inputtext);
	/*MAX_PLAYER_NAME is 24 but client may only connect if their name
	length is at max 20*/
	return 2 < len && len < MAX_PLAYER_NAME - 3 &&
		inputtext[0] != '@' &&
		natives_SetPlayerName(playerid, inputtext) == 1;
}


/**
Execute query to create a user (might be guest user).
*/
static
void login_create_user(int playerid, char *password, int groups, cb_t callback)
{
	sprintf(cbuf4096_,
	        "INSERT INTO "
		"usr(name,pw,registertime,lastseengame,groups,prefs) "
		"VALUES('%s','%s',UNIX_TIMESTAMP(),UNIX_TIMESTAMP(),%d,%d)",
	        pdata[playerid]->name,
		password,
		groups,
	        DEFAULTPREFS);
	common_mysql_tquery(cbuf4096_, callback, V_MK_PLAYER_CC(playerid));
}

/**
Formats a query that will create a new game session.
*/
static
void login_create_session(int playerid, cb_t callback)
{
	sprintf(cbuf4096_,
	        "INSERT INTO ses(u,s,e,ip) "
		"VALUES(%d,UNIX_TIMESTAMP(),UNIX_TIMESTAMP(),'%s')",
	        unconfirmed_userid[playerid],
	        pdata[playerid]->ip);
	common_mysql_tquery(cbuf4096_, callback, V_MK_PLAYER_CC(playerid));
}

/**
Set the player's status as logged in and spawn them.

@param status one of LOGGED_ values
*/
static
void login_login_player(int playerid, int status)
{
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
		NC_gpci(playerid, buf64a, 64);
		ctoai(cbuf64);
		csprintf(buf4096,
			"INSERT INTO gpci(u,v,c,first,last) "
			"VALUES(%d,'%s',1,UNIX_TIMESTAMP(),UNIX_TIMESTAMP()) "
			"ON DUPLICATE KEY UPDATE c=c+1,last=UNIX_TIMESTAMP()",
			userid[playerid],
			cbuf64);
		NC_mysql_tquery_nocb(buf4096a);
	}

	if (status == LOGGED_GUEST) {
		money_set(playerid, MONEY_DEFAULT_AMOUNT);
	} else {
		veh_spawn_player_vehicles(playerid);
	}
	class_on_player_request_class(playerid, -1);
}

/**
Callback for query that creates a session for a guest user.
*/
static
void login_cb_create_session_guest(void *data)
{
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
		failed to create session
		no real problem, but time will not be registered
	}
	*/
	SendClientMessage(playerid, COL_INFO, INFO"You are now playing as a guest. You can use /register at any time to save your stats.");
	login_login_player(playerid, LOGGED_GUEST);
	sprintf(msg144, "%s[%d] joined as a guest, welcome!", pdata[playerid]->name, playerid);
	SendClientMessageToAllAndIRC(ECHO_PACK12_LOGIN, COL_JOIN, msg144);
}

/**
Callback for query that creates a session for a newly registered member.
*/
static
void login_cb_create_session_new_member(void *data)
{
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
	login_login_player(playerid, LOGGED_IN);
	sprintf(msg144, "%s[%d] just registered an account, welcome!", pdata[playerid]->name, playerid);
	SendClientMessageToAllAndIRC(ECHO_PACK12_LOGIN, COL_JOIN, msg144);
}

static
void login_append_last_connected(int playerid, char *dest)
{
	int timespan;

	timespan = unconfirmed_timesincelastseen[playerid];
	if (timespan < 60 * 60) {
		sprintf(dest, "%d minutes ago", timespan / 60);
	} else if (timespan < 60 * 60 * 24 * 2) {
		sprintf(dest, "%d hours ago", timespan / (60 * 60));
	} else {
		sprintf(dest, "%d days ago", timespan / (60 * 60 * 24));
	}
}

/**
Callback for query that creates a session for an existing member.
*/
static
void login_cb_create_session_existing_member(void *data)
{
	int playerid;
	char msg144[144], *msgptr;

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
	login_login_player(playerid, LOGGED_IN);
	msgptr = msg144 + sprintf(msg144, "%s[%d] just logged in, welcome back! Last connected: ", pdata[playerid]->name, playerid);
	login_append_last_connected(playerid, msgptr);
	SendClientMessageToAllAndIRC(ECHO_PACK12_LOGIN, COL_JOIN, msg144);
}

/**
Callback for query that creates a guest user.
*/
static
void login_cb_create_guest_usr(void *data)
{
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
		SendClientMessage(playerid, COL_WARN, WARN"An error occurred while creating a guest session.");
		SendClientMessage(playerid, COL_WARN, WARN"You can play, but you won't be able to save your stats later.");
		login_login_player(playerid, LOGGED_GUEST);
		sprintf(msg144, "%s[%d] joined as a guest, welcome!", pdata[playerid]->name, playerid);
		SendClientMessageToAllAndIRC(ECHO_PACK12_LOGIN, COL_JOIN, msg144);
		return;
	}
	login_create_session(playerid, login_cb_create_session_guest);
}

/**
Give the user a name prefixed with an '@' symbol, indicating they're a guest.

@return 0 on failure and player will be kicked
*/
static
int login_give_guest_name(int playerid)
{
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
		if (natives_SetPlayerName(playerid, newname) == 1) {
			return 1;
		}
	}
	newname[11] = 0;
	attempts = 5;
	while (attempts-- > 0) {
		NC_PARS(1);
		nc_params[1] = 'z' - 'a' + 1;
		for (i = 1; i < 10; i++) {
			newname[i] = 'a' + (char) NC(n_random);
		}
		if (natives_SetPlayerName(playerid, newname) == 1) {
			return 1;
		}
	}
	logprintf("login: failed to give user a random guestname");
	SendClientMessage(playerid, COL_WARN, WARN"Fatal error, please reconnect!");
	natives_Kick(playerid, "can't login", NULL, -1);
	return 0;
}

/**
Create a guest account and session for player and log them in and spawn them.

If the player does not have a guest name, they will be given one (or kicked on
failure).
*/
static
void login_spawn_as_guest(int playerid)
{
	if (pdata[playerid]->name[0] != '@' &&
		!login_give_guest_name(playerid))
	{
		/*user is kicked at this point*/
		return;
	}
	GameTextForPlayer(playerid, 0x800000, 3, "~b~Creating guest account...");
	login_create_user(playerid, "", GROUP_GUEST, login_cb_create_guest_usr);
}

/**
Spawns a player as a guest without account or session. ONLY USE ON ERROR!

Use login_spawn_as_guest when wanting to let player continue as a guest.

If the player does not have a guest name, they will be given one (or kicked on
failure).
*/
static
void login_spawn_as_guest_WITHOUT_ACCOUNT(int playerid)
{
	char msg144[144];

	if (login_give_guest_name(playerid)) {
		login_login_player(playerid, LOGGED_GUEST);
		sprintf(msg144, "%s[%d] joined as a guest (login error), welcome!", pdata[playerid]->name, playerid);
		SendClientMessageToAllAndIRC(ECHO_PACK12_LOGIN, COL_JOIN, msg144);
	}
}

/**
Callback when player logged in and account load query is done.
*/
static
void login_cb_load_account_data(void *data)
{
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
		login_spawn_as_guest_WITHOUT_ACCOUNT(playerid);
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
	login_create_session(playerid, login_cb_create_session_existing_member);

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

/**
Callback for bcrypt password check when logging in.
*/
static
void login_cb_verify_password(void *data)
{
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
		common_mysql_tquery(cbuf4096_, login_cb_failed_login_added, (void*) unconfirmed_userid[playerid]);

		fal = failedlogins[playerid] + 1;
		if (fal > MAX_LOGIN_ATTEMPTS_IN_ONE_SESSION) {
			natives_Kick(playerid, "too many failed logins", 0, -1);
		}
		failedlogins[playerid] = fal;

		login_show_dialog_login(playerid, 1);
	} else {
		GameTextForPlayer(playerid, 0x800000, 3, "~b~Loading account...");
		sprintf(cbuf4096_,
			"SELECT cash,distance,flighttime,playtime,prefs,"
			"falng,lastfal,groups,nametagdist "
			"FROM usr "
			"WHERE i=%d",
			unconfirmed_userid[playerid]);
		common_mysql_tquery(cbuf4096_, login_cb_load_account_data, data);
	}
}

/**
Callback for when member user has been created after registering.
*/
static
void login_cb_member_user_created(void *data)
{
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
		login_spawn_as_guest_WITHOUT_ACCOUNT(playerid);
		return;
	}
	GameTextForPlayer(playerid, 0x800000, 3, "~b~Creating game session...");
	login_create_session(playerid, login_cb_create_session_new_member);
}

/**
Callback for register dialog password hash.
*/
static
void login_cb_register_password_hashed(void *data)
{
	int playerid;

	playerid = PLAYER_CC_GETID(data);
	if (PLAYER_CC_CHECK(data, playerid)) {
		/*gametext still showing from previous*/
		NC_bcrypt_get_hash(buf144a);
		ctoa(cbuf64, buf144, 144);
		login_create_user(playerid, cbuf64,
			GROUP_MEMBER, login_cb_member_user_created);
	}
}

/**
Callback for login_query_check_user_exists
*/
static
void login_cb_check_user_exists(void *data)
{
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
		login_show_dialog_login(playerid, 0);
	} else {
		login_show_dialog_register_step1(playerid, 0);
	}
}

/**
Execute query to check if a user exists. Shows gametext.

Callback will be called with CC
*/
static
void login_query_check_user_exists(int playerid)
{
	GameTextForPlayer(playerid, 0x800000, 3, "~b~Checking username...");
	sprintf(cbuf4096_,
		"(SELECT 0,-1,0,count(u) FROM fal WHERE stamp>UNIX_TIMESTAMP()-1800 AND ip='%s')"
		" UNION ALL "
		"(SELECT pw,i,UNIX_TIMESTAMP()-lastseengame,0 FROM usr WHERE name='%s')",
		pdata[playerid]->ip,
		pdata[playerid]->name);
	common_mysql_tquery(cbuf4096_, login_cb_check_user_exists, V_MK_PLAYER_CC(playerid));
}

/**
@return 0 when player has unacceptable name
*/
static
int login_on_player_connect(int playerid)
{
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
			natives_SetPlayerName(playerid, pd->name + 1) != 1)
		{
			SendClientMessage(playerid, COL_WARN, WARN"Failed to change your nickname. Please come back with a different name.");
			natives_Kick(playerid, "invalid name", NULL, -1);
			return 0;
		}
	}

	login_query_check_user_exists(playerid);
	return 1;
}

void login_on_player_disconnect(int playerid, int reason)
{
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
