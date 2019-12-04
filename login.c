
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "class.h"
#include "dialog.h"
#include "login.h"
#include "money.h"
#include "playerdata.h"
#include "prefs.h"
#include "score.h"
#include "vehicles.h"
#include <string.h>
#include <time.h>

#define MAX_ALLOWED_FAILED_LOGINS_IN_30_MINUTES 10
#define MAX_LOGIN_ATTEMPTS_IN_ONE_SESSION 4

#define BCRYPT_COST 12

int userid[MAX_PLAYERS];
int sessionid[MAX_PLAYERS];
int loggedstatus[MAX_PLAYERS];
void *pwdata[MAX_PLAYERS];

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
	-> login_dlg_login_or_namechange (when user exists)
	-> login_dlg_register_firstpass (when usr doesn't exist)

login_spawn_as_guest
	-> login_cb_create_guest_usr
		-> login_login_player (as guest, when creating user failed)
		-> login_cb_create_session_guest
			-> login_login_player

login_dlg_register_firstpass
	-> login_login_player (on play as guest)
	-> login_dlg_register_confirmpass (to confirm pass)

login_dlg_register_confirmpass
	-> login_dlg_register_firstpass (on cancel or no match)
	-> login_cb_register_password_hashed
		-> login_cb_member_user_created
			-> login_spawn_as_guest (when creating user failed)
			-> login_cb_create_session_new_member

login_dlg_login_or_namechange
	-> login_dlg_namechange (on cancel response)
	-> login_cb_verify_password

login_cb_verify_password
	-> login_dlg_login_or_namechange (on wrong password)
	-> login_cb_load_account_data

login_cb_load_account_data
	-> login_spawn_as_guest (when empty response)
	-> login_cb_create_session_existing_member

login_dlg_namechange
	-> login_spawn_as_guest (on cancel)
	-> login_dlg_login_or_namechange (on empty response)
	-> login_dlg_namechange (on invalid name)
	-> login_query_check_user_exists
		-> login_cb_check_user_exists
*/

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

/**
Shows name change dialog.

@param show_invalid_name_error 1 to show invalid name error message
*/
static
void login_show_dialog_change_name(int playerid, int show_invalid_name_error)
{
	static const char *CHANGE_NAME_TEXT =
		ECOL_WARN"Invalid name or name is taken (press tab).\n"
		"\n"ECOL_DIALOG_TEXT
		"Enter your new name (3-20 length, 0-9a-zA-Z=()[]$@._).\n"
		"Names starting with @ are reserved for guests.";

	dialog_ShowPlayerDialog(
		playerid, DIALOG_LOGIN_NAMECHANGE,
		DIALOG_STYLE_INPUT, NAMECHANGE_CAPTION,
		(char*) CHANGE_NAME_TEXT + 60 * (show_invalid_name_error ^ 1),
		"Change", "Play as guest",
		TRANSACTION_LOGIN);
}

/**
Shows the login dialog.

@param show_invalid_pw_error 1 to show invalid password error message
*/
static
void login_show_dialog_login(int playerid, int show_invalid_pw_error)
{
	static const char *LOGIN_TEXT =
		ECOL_WARN"Incorrect password!\n"
		"\n"ECOL_DIALOG_TEXT
		"Welcome! This account is registered.\n"
		"Please sign in or change your name.";

	dialog_ShowPlayerDialog(
		playerid, DIALOG_LOGIN_LOGIN_OR_NAMECHANGE,
		DIALOG_STYLE_PASSWORD, LOGIN_CAPTION,
		(char*) LOGIN_TEXT + 37 * (show_invalid_pw_error ^ 1),
		"Login", "Change name",
		TRANSACTION_LOGIN);
}

/**
Shows the register dialog box for the first step (enter password).
*/
static
void login_show_dialog_register_step1(int playerid, int pw_mismatch)
{
	char data[512];

	login_format_register_dialog(data, 0, pw_mismatch);
	dialog_ShowPlayerDialog(
		playerid, DIALOG_REGISTER_FIRSTPASS, DIALOG_STYLE_PASSWORD,
		REGISTER_CAPTION, data, "Next", "Play as guest",
		TRANSACTION_LOGIN);
}

/**
Shows the register dialog box for the first step (confirm password).
*/
static
void login_show_dialog_register_step2(int playerid)
{
	char data[512];

	login_format_register_dialog(data, 1, 0);
	dialog_ShowPlayerDialog(
		playerid, DIALOG_REGISTER_CONFIRMPASS, DIALOG_STYLE_PASSWORD,
		REGISTER_CAPTION, data, "Confirm", "Cancel",
		TRANSACTION_LOGIN);
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
	        userid[playerid],
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
			goto alreadyin; /*wtf?*/
		}
	}
	players[playercount++] = playerid;
alreadyin:

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

	playerid = PLAYER_CC_GETID(data);
	if (!PLAYER_CC_CHECK(data, playerid)) {
		return;
	}

	common_hide_gametext_for_player(playerid);
	sessionid[playerid] = NC_cache_insert_id();
	/*
	if (sessionid[playerid] == -1) {
		failed to create session
		no real problem, but time will not be registered
	}
	*/
	csprintf(buf144,
		"%s[%d] joined as a guest, welcome!",
		pdata[playerid]->name,
		playerid);
	NC_SendClientMessageToAll(COL_JOIN, buf144a);
	B144(INFO"You are now playing as a guest. "
		"You can use /register at any time to save your stats.");
	NC_SendClientMessage(playerid, COL_INFO, buf144a);
	login_login_player(playerid, LOGGED_GUEST);
}

/**
Callback for query that creates a session for a newly registered member.
*/
static
void login_cb_create_session_new_member(void *data)
{
	int playerid;

	playerid = PLAYER_CC_GETID(data);
	if (!PLAYER_CC_CHECK(data, playerid)) {
		return;
	}

	dialog_end_transaction(playerid, TRANSACTION_LOGIN);
	common_hide_gametext_for_player(playerid);
	sessionid[playerid] = NC_cache_insert_id();
	/*
	if (sessionid[playerid] == -1) {
		failed to create session
		no real problem, but time will not be registered
	}
	*/
	csprintf(buf144,
		"%s[%d] just registered an account, welcome!",
		pdata[playerid]->name,
		playerid);
	NC_SendClientMessageToAll(COL_JOIN, buf144a);
	login_login_player(playerid, LOGGED_IN);
}

/**
Callback for query that creates a session for an existing member.
*/
static
void login_cb_create_session_existing_member(void *data)
{
	int playerid;

	playerid = PLAYER_CC_GETID(data);
	if (!PLAYER_CC_CHECK(data, playerid)) {
		return;
	}

	dialog_end_transaction(playerid, TRANSACTION_LOGIN);
	common_hide_gametext_for_player(playerid);
	sessionid[playerid] = NC_cache_insert_id();
	/*
	if (sessionid[playerid] == -1) {
		failed to create session
		no real problem, but time will not be registered
	}
	*/
	csprintf(buf144,
		"%s[%d] just loggin in, welcome back!",
		pdata[playerid]->name,
		playerid);
	NC_SendClientMessageToAll(COL_JOIN, buf144a);
	login_login_player(playerid, LOGGED_IN);
}

/**
Callback for query that creates a guest user.
*/
static
void login_cb_create_guest_usr(void *data)
{
	int playerid;

	playerid = PLAYER_CC_GETID(data);
	if (!PLAYER_CC_CHECK(data, playerid)) {
		return;
	}

	B144("~b~Creating game session...");
	NC_GameTextForPlayer(playerid, buf144a, 0x800000, 3);
	userid[playerid] = NC_cache_insert_id();
	if (userid[playerid] == -1) {
		common_hide_gametext_for_player(playerid);
		B144(WARN"An error occurred while creating a guest session.");
		NC_SendClientMessage(playerid, COL_WARN, buf144a);
		B144(WARN"You can play, but you won't be able to save your"
			"stats later.");
		NC(n_SendClientMessage);
		login_login_player(playerid, LOGGED_GUEST);
		return;
	}
	login_create_session(playerid, login_cb_create_session_guest);
}

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
	B144(WARN"Fatal error, please reconnect!");
	NC_SendClientMessage(playerid, COL_WARN, buf144a);
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
	B144("~b~Creating guest account...");
	NC_GameTextForPlayer(playerid, buf144a, 0x800000, 3);
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
	if (login_give_guest_name(playerid)) {
		login_login_player(playerid, LOGGED_GUEST);
	}
}

/**
Callback when player logged in and account load query is done.
*/
static
void login_cb_load_account_data(void *data)
{
	int playerid, *f = nc_params + 2, score, falng, lastfal;

	playerid = PLAYER_CC_GETID(data);
	if (!PLAYER_CC_CHECK(data, playerid)) {
		return;
	}

	if (!NC_cache_get_row_count()) {
		common_hide_gametext_for_player(playerid);
		logprintf("login_cb_load_account_data: empty response");
		dialog_ShowPlayerDialog(
			playerid, DIALOG_DUMMY, DIALOG_STYLE_MSGBOX,
			LOGIN_CAPTION, ECOL_WARN"An error occurred, "
			"you will be spawned as a guest",
			"Ok", "",
			-1);
		login_spawn_as_guest_WITHOUT_ACCOUNT(playerid);
		return;
	}

	NC_PARS(2);
	nc_params[1] = 0;
	score = (*f = 0, NC(n_cache_get_field_i));
	money_set(playerid, (*f = 1, NC(n_cache_get_field_i)));
	playerodoKM[playerid] = (*f = 2, NCF(n_cache_get_field_f));
	score = (*f = 3, NC(n_cache_get_field_i));
	prefs[playerid] = (*f = 4, NC(n_cache_get_field_i));
	falng = (*f = 5, NC(n_cache_get_field_i));
	lastfal = (*f = 6, NC(n_cache_get_field_i));
	pdata[playerid]->groups = (*f = 7, NC(n_cache_get_field_i));

	NC_SetPlayerScore(playerid, score);

	csprintf(buf4096,
            "UPDATE usr SET lastseengame=UNIX_TIMESTAMP() WHERE i=%d LIMIT 1",
            userid[playerid]);
	NC_mysql_tquery_nocb(buf4096a);

	B144("~b~Creating game session...");
	NC_GameTextForPlayer(playerid, buf144a, 0x800000, 3);
	login_create_session(playerid, login_cb_create_session_existing_member);

	if (lastfal > falng) {
		csprintf(buf4096,
			"UPDATE usr SET falng=%d WHERE i=%d",
			lastfal,
			userid[playerid]);
		NC_mysql_tquery_nocb(buf4096a);
		dialog_ShowPlayerDialog(
			playerid, DIALOG_DUMMY, DIALOG_STYLE_MSGBOX,
			"Failed logins",
			ECOL_WARN"There were one or more failed logins "
				"since your last visit.\n"
				"Check the website for more details.",
			"Ok", "",
			TRANSACTION_LOGIN);

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
		common_hide_gametext_for_player(playerid);

		sprintf(cbuf4096_,
			"INSERT INTO fal(u,stamp,ip) "
			"VALUES (%d,UNIX_TIMESTAMP(),'%s')",
			userid[playerid],
			pdata[playerid]->ip);
		common_mysql_tquery(cbuf4096_,
			login_cb_failed_login_added, (void*) userid[playerid]);

		fal = failedlogins[playerid] + 1;
		if (fal > MAX_LOGIN_ATTEMPTS_IN_ONE_SESSION) {
			natives_Kick(playerid, "too many failed logins", 0, -1);
		}
		failedlogins[playerid] = fal;

		login_show_dialog_login(playerid, 1);
	} else {
		B144("~b~Loading account...");
		NC_GameTextForPlayer(playerid, buf144a, 0x800000, 3);
		sprintf(cbuf4096_,
			"SELECT score,cash,distance,flighttime,prefs,"
			"falng,lastfal,groups "
			"FROM usr "
			"WHERE i=%d",
			userid[playerid]);
		common_mysql_tquery(cbuf4096_,
			login_cb_load_account_data, data);
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
	userid[playerid] = NC_cache_insert_id();
	if (userid[playerid] == -1) {
		common_hide_gametext_for_player(playerid);
		B144(WARN"An error occured while registering.");
		NC_SendClientMessage(playerid, COL_WARN, buf144a);
		B144(WARN"You will be spawned as a guest.");
		NC(n_SendClientMessage);
		login_spawn_as_guest_WITHOUT_ACCOUNT(playerid);
		return;
	}
	B144("~b~Creating game session...");
	NC_GameTextForPlayer(playerid, buf144a, 0x800000, 3);
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
	int playerid, failedattempts;
	char password[PW_HASH_LENGTH];

	playerid = PLAYER_CC_GETID(data);
	if (!PLAYER_CC_CHECK(data, playerid)) {
		return;
	}

	dialog_end_transaction(playerid, TRANSACTION_LOGIN);
	common_hide_gametext_for_player(playerid);

	if (!NC_cache_get_row_count()) {
		logprintf("login_cb_check_user_exists: empty response");
		dialog_ShowPlayerDialog(
			playerid, DIALOG_DUMMY, DIALOG_STYLE_MSGBOX,
			LOGIN_CAPTION, ECOL_WARN"An error occurred, "
			"you will be spawned as a guest",
			"Ok", "",
			-1);
		goto asguest;
	}

	NC_PARS(2);
	nc_params[1] = 0;
	nc_params[3] = buf144a;
	failedattempts = (nc_params[2] = 0, NC(n_cache_get_field_i));
	
	if (failedattempts > MAX_ALLOWED_FAILED_LOGINS_IN_30_MINUTES) {
asguest:
		B144("You will be spawned as a guest.");
		NC_SendClientMessage(playerid, COL_SAMP_GREEN, buf144a);
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

	/*user doesn't exist when password is NULL*/
	(nc_params[2] = 1, NC(n_cache_get_field_s));
	ctoa(password, buf144, 144);
	if (strcmp(password, "NULL") == 0) {
		login_show_dialog_register_step1(playerid, 0);
	} else {
		if (pwdata[playerid] == NULL) {
			pwdata[playerid] = malloc(PW_HASH_LENGTH);
		}
		memcpy(pwdata[playerid], password, PW_HASH_LENGTH);
		userid[playerid] = (nc_params[2] = 2, NC(n_cache_get_field_i));
		login_show_dialog_login(playerid, 0);
	}
}

/**
Execute query to check if a user exists. Shows gametext.

Callback will be called with CC
*/
static
void login_query_check_user_exists(int playerid, cb_t callback)
{
	B144("~b~Contacting login server...");
	NC_GameTextForPlayer(playerid, buf144a, 0x800000, 3);
	sprintf(cbuf4096_,
	        "SELECT "
	        "(SELECT count(u) "
			"FROM fal "
			"WHERE stamp>UNIX_TIMESTAMP()-1800 AND ip='%s'),"
	        "(SELECT pw "
			"FROM usr "
			"WHERE name='%s'),"
	        "(SELECT i "
			"FROM usr "
			"WHERE name='%s')",
		pdata[playerid]->ip,
		pdata[playerid]->name,
		pdata[playerid]->name);
	common_mysql_tquery(cbuf4096_, callback, V_MK_PLAYER_CC(playerid));
}

void login_dlg_login_or_namechange(int playerid, int response, char *inputtext)
{
	/*Login | Change name*/
	if (response) {
		B144("~b~Logging in...");
		NC_GameTextForPlayer(playerid, buf144a, 0x800000, 3);
		if (pwdata[playerid] == NULL) {
			logprintf("login_dlg_login_or_namechange: no password");
		} else {
			atoc(buf144, inputtext, 144);
			atoc(buf4096, pwdata[playerid], 144);
			common_bcrypt_check(
				buf144a,
				buf4096a,
				login_cb_verify_password,
				V_MK_PLAYER_CC(playerid));
			dialog_ensure_transaction(playerid, TRANSACTION_LOGIN);
		}
	} else {
		login_show_dialog_change_name(playerid, 0);
	}
}

void login_dlg_namechange(int playerid, int response, char *inputtext)
{
	/*Change | Play as guest*/
	if (response) {
		if (!inputtext[0] ||
			strcmp(pdata[playerid]->name, inputtext) == 0)
		{
			/*edge case: if empty string or same name as current,
			go back to login dialog*/
			login_show_dialog_login(playerid, 0);
		} else if (!login_change_name_from_input(playerid, inputtext)) {
			login_show_dialog_change_name(playerid, 1);
		} else {
			userid[playerid] = -1;
			dialog_ensure_transaction(playerid, TRANSACTION_LOGIN);
			login_query_check_user_exists(playerid,
				login_cb_check_user_exists);
		}
	} else {
		/*cancel should not go back to login dialog,
		otherwise spamming escape will keep you in a loop*/
		login_spawn_as_guest(playerid);
	}
}

void login_dlg_register_confirmpass(
	int playerid, int response, cell inputaddr, char *inputtext)
{
	int res;

	/*Confirm | Cancel*/
	if (response) {
		if (pwdata[playerid] == NULL) {
			logprintf("login: confirmed pw without entering one!");
			return;
		}
		NC_PARS(4);
		nc_params[1] = nc_params[2] = inputaddr;
		nc_params[3] = buf144a;
		nc_params[4] = PW_HASH_LENGTH;
		NC(n_SHA256_PassHash);
		res = memcmp(cbuf144, pwdata[playerid], 256);
		free(pwdata[playerid]);
		pwdata[playerid] = NULL;
		if (res) {
			login_show_dialog_register_step1(playerid, 1);
		} else {
			B144("~b~Making your account...");
			NC_GameTextForPlayer(playerid, buf144a, 0x800000, 3);
			dialog_ensure_transaction(playerid, TRANSACTION_LOGIN);
			common_bcrypt_hash(
				inputaddr,
				login_cb_register_password_hashed,
				V_MK_PLAYER_CC(playerid));
		}
	} else {
		if (pwdata[playerid] != NULL) {
			free(pwdata[playerid]);
			pwdata[playerid] = NULL;
		}
		login_show_dialog_register_step1(playerid, 0);
	}
}

void login_dlg_register_firstpass(int playerid, int response, cell inputaddr)
{
	/*Next | Play as guest*/
	if (response) {
		NC_PARS(4);
		nc_params[1] = nc_params[2] = inputaddr;
		nc_params[3] = buf144a;
		nc_params[4] = PW_HASH_LENGTH;
		NC(n_SHA256_PassHash);
		if (pwdata[playerid] != NULL) {
			free(pwdata[playerid]);
		}
		pwdata[playerid] = malloc(4 * PW_HASH_LENGTH);
		memcpy(pwdata[playerid], buf144, 4 * PW_HASH_LENGTH);
		login_show_dialog_register_step2(playerid);
	} else {
		login_spawn_as_guest(playerid);
	}
}

int login_on_player_connect(int playerid)
{
	static const char
		*ATNOTICE = "Names starting with '@' are reserved "
				"for guest players.",
		*FAILEDCHANGE = WARN"Failed to change your nickname. "
				"Please come back with a different name.",
		*INVALIDNAME = "invalid name";

	struct playerdata *pd;

	pwdata[playerid] = NULL;
	userid[playerid] = -1;
	sessionid[playerid] = -1;
	failedlogins[playerid] = 0;
	pdata[playerid]->groups = GROUP_GUEST;

	pd = pdata[playerid];
	while (pd->name[0] == '@') {
		B144((char*) ATNOTICE);
		NC_SendClientMessage(playerid, COL_SAMP_GREEN, buf144a);
		/*wiki states that SetPlayerName does not propagate for the user
		when used in OnPlayerConnect, but tests have proven otherwise.*/
		if (pd->namelen <= 3 ||
			natives_SetPlayerName(playerid, pd->name + 1) != 1)
		{
			B144((char*) FAILEDCHANGE);
			NC_SendClientMessage(playerid, COL_WARN, buf144a);
			natives_Kick(playerid, (char*) INVALIDNAME, NULL, -1);
			return 0;
		}
	}

	login_query_check_user_exists(playerid, login_cb_check_user_exists);
	return 1;
}

void login_on_player_disconnect(int playerid, int reason)
{
	char *b;

	if (userid[playerid] != -1) {
		csprintf(buf4096,
			"UPDATE usr SET score=%d,cash=%d,distance=%d,"
			"flighttime=%d,prefs=%d "
			"WHERE i=%d",
			NC_GetPlayerScore(playerid),
			money_get(playerid),
			playerodoKM[playerid],
			score_flight_time[playerid],
			prefs[playerid],
			userid[playerid]);
		NC_mysql_tquery_nocb(buf4096a);
	}
	if (loggedstatus[playerid]) {
		b = cbuf64;
		b += sprintf(b,
			"%s[%d] left the server",
			pdata[playerid]->name,
			playerid);
		switch (reason) {
		case 0: strcpy(b, " (timeout)"); break;
		case 1: strcpy(b, " (quit)"); break;
		case 2: strcpy(b, " (kicked)"); break;
		}
		atoc(buf144, cbuf64, 144);
		NC_SendClientMessageToAll(COL_QUIT, buf144a);
	}
	if (pwdata[playerid] != NULL) {
		free(pwdata[playerid]);
		pwdata[playerid] = NULL;
	}
	loggedstatus[playerid] = LOGGED_NO;
}
