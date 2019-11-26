
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "class.h"
#include "dialog.h"
#include "login.h"
#include "money.h"
#include "playerdata.h"
#include "vehicles.h"
#include <string.h>
#include <time.h>

#define MAX_ALLOWED_FAILED_LOGINS_IN_30_MINUTES 10
#define MAX_LOGIN_ATTEMPTS_IN_ONE_SESSION 4

#define BCRYPT_COST 12

int userid[MAX_PLAYERS];
int sessionid[MAX_PLAYERS];
int loggedstatus[MAX_PLAYERS];

/**
Some password data that needs to be persisted for password confirmation things.
*/
static char *pwdata[MAX_PLAYERS];
/**
Failed login attempts.
*/
static char failedlogins[MAX_PLAYERS];

static char *LOGIN_CAPTION = "Login", *REGISTER_CAPTION = "Register";

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

	for (i = 0; i < playercount; ){
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
Callback for query that creates a guest session.
*/
static
void login_cb_create_guest_session(void *data)
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
	sprintf(cbuf4096,
		"%s[%d] joined as a guest, welcome!",
		pdata[playerid]->name,
		playerid);
	amx_SetUString(buf144, cbuf4096, 144);
	NC_SendClientMessageToAll(COL_JOIN, buf144a);
	B144(INFO"You are now playing as a guest. "
		"You can use /register at any time to save your stats.");
	NC_SendClientMessage(playerid, COL_INFO, buf144a);
	login_login_player(playerid, LOGGED_GUEST);
}

/**
Callback for query that creates a member session.
*/
static
void login_cb_create_member_session(void *data)
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
	sprintf(cbuf4096,
		"%s[%d] just registered an account, welcome!",
		pdata[playerid]->name,
		playerid);
	amx_SetUString(buf144, cbuf4096, 144);
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
	login_create_session(playerid, login_cb_create_guest_session);
}

/**
Create a guest account and session for player and log them in and spawn them.
*/
static
void login_spawn_as_guest(int playerid)
{
	B144("~b~Creating guest account...");
	NC_GameTextForPlayer(playerid, buf144a, 0x800000, 3);
	login_create_user(playerid, "", GROUP_GUEST, login_cb_create_guest_usr);
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
	B144(WARN"Fatal error, please reconnect!");
	NC_SendClientMessage(playerid, COL_WARN, buf144a);
	natives_Kick(playerid, "can't login", NULL, -1);
	return 0;
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
Shows the register dialog box for the first step (enter password).
*/
static
void login_show_register_dialog_step1(int playerid, int pw_mismatch)
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
void login_show_register_dialog_step2(int playerid)
{
	char data[512];

	login_format_register_dialog(data, 1, 0);
	dialog_ShowPlayerDialog(
		playerid, DIALOG_REGISTER_CONFIRMPASS, DIALOG_STYLE_PASSWORD,
		REGISTER_CAPTION, data, "Confirm", "Cancel",
		TRANSACTION_LOGIN);
}

/**
Shows the login dialog.

@param show_invalid_pw_error 1 to show invalid password error message
*/
static
void login_show_login_dialog(int playerid, int show_invalid_pw_error)
{
	static const char *LOGIN_TEXT =
		""ECOL_WARN"Incorrect password!\n\n"ECOL_DIALOG_TEXT""\
		"Welcome! This account is registered.\n"\
		"Please sign in or change your name.";

	dialog_ShowPlayerDialog(
		playerid, DIALOG_LOGIN_LOGIN_OR_NAMECHANGE,
		DIALOG_STYLE_PASSWORD, LOGIN_CAPTION,
		(char*) LOGIN_TEXT[37 * show_invalid_pw_error],
		"Login", "Change name",
		TRANSACTION_LOGIN);
}

/**
Callback for when member user has been created after registering.
*/
static
void login_cb_member_user_created(void *data)
{
	int playerid;

	playerid = PLAYER_CC_GETID(data);
	if (PLAYER_CC_CHECK(data, playerid)) {
		pdata[playerid]->groups = GROUP_MEMBER;
		userid[playerid] = NC_cache_insert_id();
		if (userid[playerid] == -1) {
			common_hide_gametext_for_player(playerid);
			B144(WARN"An error occured while registering.");
			NC_SendClientMessage(playerid, COL_WARN, buf144a);
			B144(WARN"You will be spawned as a guest.");
			NC(n_SendClientMessage);
			if (login_give_guest_name(playerid)) {
				login_spawn_as_guest(playerid);
			}
			return;
		}
		B144("~b~Creating game session...");
		NC_GameTextForPlayer(playerid, buf144a, 0x800000, 3);
		login_create_session(playerid, login_cb_create_member_session);
	}
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
		amx_GetUString(cbuf64, buf144, 144);
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
	int playerid, rowcount, failedattempts, uid;
	char password[PW_HASH_LENGTH];

	playerid = PLAYER_CC_GETID(data);
	if (!PLAYER_CC_CHECK(data, playerid)) {
		return;
	}

	dialog_end_transaction(playerid, TRANSACTION_LOGIN);
	common_hide_gametext_for_player(playerid);

	rowcount = NC_cache_get_row_count();
	if (!rowcount) {
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
	uid = (nc_params[2] = 1, NC(n_cache_get_field_i));
	(nc_params[2] = 2, NC(n_cache_get_field_s));
	
	if (failedattempts > MAX_ALLOWED_FAILED_LOGINS_IN_30_MINUTES) {
asguest:
		B144("You will be spawned as a guest.");
		NC_SendClientMessage(playerid, COL_SAMP_GREEN, buf144a);
		if (login_give_guest_name(playerid)) {
			login_spawn_as_guest(playerid);
		}
		return;
	}
	failedattempts = MAX_LOGIN_ATTEMPTS_IN_ONE_SESSION -
		(MAX_ALLOWED_FAILED_LOGINS_IN_30_MINUTES - failedattempts);
	if (failedattempts > failedlogins[playerid]) {
		failedlogins[playerid] = failedattempts;
	}

	/*user doesn't exist when password is NULL*/
	amx_GetUString(password, buf144, 144);
	if (strcmp(password, "NULL") == 0) {
		login_show_register_dialog_step1(playerid, 0);
	} else {
		if (pwdata[playerid] == NULL) {
			pwdata[playerid] = malloc(PW_HASH_LENGTH);
		}
		memcpy(pwdata[playerid], password, PW_HASH_LENGTH);
		userid[playerid] = uid;
		login_show_login_dialog(playerid, 0);
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
		res = strcmp(cbuf144, pwdata[playerid]);
		free(pwdata[playerid]);
		pwdata[playerid] = NULL;
		if (res) {
			login_show_register_dialog_step1(playerid, 1);
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
		login_show_register_dialog_step1(playerid, 0);
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
		login_show_register_dialog_step2(playerid);
	} else {
		if (login_give_guest_name(playerid)) {
			login_login_player(playerid, LOGGED_GUEST);
		}
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
		amx_SetUString(buf144, cbuf64, 144);
		NC_SendClientMessageToAll(COL_QUIT, buf144a);
	}
	if (pwdata[playerid] != NULL) {
		free(pwdata[playerid]);
		pwdata[playerid] = NULL;
	}
	loggedstatus[playerid] = LOGGED_NO;
}

