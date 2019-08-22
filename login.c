
/* vim: set filetype=c ts=8 noexpandtab: */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include "common.h"
#include <string.h>
#include <time.h>
#include "playerdata.h"

/* actual password, stored for later checking */
static char *pwdata[MAX_PLAYERS];
/* some hash of new password, stored between dialogs when player is changing password */
static char *pwchangeconfirmdata[MAX_PLAYERS];

void login_init()
{
	int i;
	for (i = 0; i < MAX_PLAYERS; i++) {
		pwchangeconfirmdata[i] = pwdata[i] = NULL;
	}
}

/* native Login_FormatAddFailedLogin(playerid, bufq1[], bufq2[]) */
cell AMX_NATIVE_CALL Login_FormatAddFailedLogin(AMX *amx, cell *params)
{
	const int pid = params[1];
	cell *addr;
	char buf[144];
	long int t = (long int) time(NULL);
	if (pdata[pid] == NULL) {
		return 0;
	}
	sprintf(buf,
	        "INSERT INTO fal(u,stamp,ip) VALUES(%d,%ld,'%s')",
	        pdata[pid]->userid,
	        t,
	        pdata[pid]->ip);
	amx_GetAddr(amx, params[2], &addr);
	amx_SetUString(addr, buf, sizeof(buf));
	sprintf(buf,
	        "UPDATE usr SET lastfal=%ld WHERE i=%d",
	        t,
	        pdata[pid]->userid);
	amx_GetAddr(amx, params[3], &addr);
	amx_SetUString(addr, buf, sizeof(buf));
	return 1;
}

/* native Login_FormatChangePasswordBox(buf[], pwmismatch=0, step) */
cell AMX_NATIVE_CALL Login_FormatChangePasswordBox(AMX *amx, cell *params)
{
	int pwmismatch = params[2], step = params[3];
	char data[512];
	char *d = &data[0];
	cell *addr;
	if (pwmismatch) {
		d += sprintf(d, ECOL_WARN"New passwords do not match, please try again.\n\n");
	}
	d += sprintf(d, step == 0 ? ECOL_INFO : ECOL_DIALOG_TEXT);
	d += sprintf(d, "* enter your current password");
	if (step == 0) d += sprintf(d, " <<<<");
	d += sprintf(d, step == 1 ? ECOL_INFO"\n" : ECOL_DIALOG_TEXT"\n");
	d += sprintf(d, "* choose a new password");
	if (step == 1) d += sprintf(d, " <<<<");
	d += sprintf(d, step == 2 ? ECOL_INFO"\n" : ECOL_DIALOG_TEXT"\n");
	d += sprintf(d, "* confirm your new password");
	if (step == 2) d += sprintf(d, " <<<<");
	amx_GetAddr(amx, params[1], &addr);
	amx_SetUString(addr, data, sizeof(data));
	return 1;
}

/* native Login_FormatChangePassword(userid, password[], buf[]) */
cell AMX_NATIVE_CALL Login_FormatChangePassword(AMX *amx, cell *params)
{
	int uid = params[1];
	char pw[PW_HASH_LENGTH], data[512];
	cell *addr;
	amx_GetAddr(amx, params[2], &addr);
	amx_GetUString(pw, addr, sizeof(pw));
	sprintf(data, "UPDATE usr SET pw='%s' WHERE i=%d", pw, uid);
	amx_GetAddr(amx, params[3], &addr);
	amx_SetUString(addr, data, sizeof(data));
	return 1;
}

/* native Login_FormatCheckUserExist(playerid, buf[]); */
cell AMX_NATIVE_CALL Login_FormatCheckUserExist(AMX *amx, cell *params)
{
	int pid = params[1];
	char data[512];
	cell *addr;
	if (pdata[pid] == NULL) {
		return 0;
	}
	sprintf(data,
	        "SELECT "
	        "(SELECT count(u) AS c FROM fal WHERE stamp>UNIX_TIMESTAMP()-1800 AND ip='%s') AS b,"
	        "(SELECT pw FROM usr WHERE name='%s') AS p,"
	        "(SELECT i FROM usr WHERE name='%s') AS i", pdata[pid]->ip, pdata[pid]->name, pdata[pid]->name);
	amx_GetAddr(amx, params[2], &addr);
	amx_SetUString(addr, data, sizeof(data));
	return 1;
}

void formatCreateSessionQuery(char *buf, struct playerdata *pd)
{
	sprintf(buf,
	        "INSERT INTO ses(u,s,e,ip) VALUES(%d,UNIX_TIMESTAMP(),UNIX_TIMESTAMP(),'%s')",
	        pd->userid,
	        pd->ip);
}

/* native Login_FormatCreateSession(playerid, buf[]) */
cell AMX_NATIVE_CALL Login_FormatCreateSession(AMX *amx, cell *params)
{
	int pid = params[1];
	char data[255];
	cell *addr;
	if (pdata[pid] == NULL) {
		return 0;
	}
	formatCreateSessionQuery(data, pdata[pid]);
	amx_GetAddr(amx, params[2], &addr);
	amx_SetUString(addr, data, sizeof(data));
	return 1;
}

/* native Login_FormatCreateUser(playerid, buf[], password[], group) */
cell AMX_NATIVE_CALL Login_FormatCreateUser(AMX *amx, cell *params)
{
	int pid = params[1], group = params[4];
	char data[255], pw[144];
	cell *addr;
	if (pdata[pid] == NULL) {
		return 0;
	}
	amx_GetAddr(amx, params[3], &addr);
	amx_GetUString(pw, addr, sizeof(pw));
	sprintf(data,
	        "INSERT INTO usr(name,pw,registertime,lastseengame,groups,prefs)"
		"VALUES('%s','%s',UNIX_TIMESTAMP(),UNIX_TIMESTAMP(),%d,%d)",
	        pdata[pid]->name,
		pw,
		group,
	        DEFAULTPREFS);
	amx_GetAddr(amx, params[2], &addr);
	amx_SetUString(addr, data, sizeof(data));
	return 1;
}

/* native Login_FormatCreateUserSession(playerid, buf[]) */
cell AMX_NATIVE_CALL Login_FormatCreateUserSession(AMX *amx, cell *params)
{
	int pid = params[1];
	char data[255];
	cell *addr;
	int p;
	if (pdata[pid] == NULL) {
		return 0;
	}
	p = sprintf(data,
	            "UPDATE usr SET lastseengame=UNIX_TIMESTAMP() WHERE i=%d LIMIT 1",
	            (int) params[1]);
	if (p < 0) {
		return 0;
	}
	amx_GetAddr(amx, params[2], &addr);
	p += 2;
	*addr = p;
	amx_SetUString(addr + 1, data, sizeof(data));
	formatCreateSessionQuery(data, pdata[pid]);
	amx_SetUString(addr + p, data, sizeof(data));
	return 1;
}

/* native Login_FormatGuestRegisterBox(playerid, buf[], invalid_name_error=0, pwmismatch=0, step) */
cell AMX_NATIVE_CALL Login_FormatGuestRegisterBox(AMX *amx, cell *params)
{
	int pid = params[1], invalid_name_error = params[3], pwmismatch = params[4], step = params[5];
	char data[512];
	char *d = &data[0];
	cell *addr;
	if (invalid_name_error) {
		d += sprintf(d, ECOL_WARN"Invalid name or name is taken (press tab).\n\n");
	} else if (pwmismatch) {
		d += sprintf(d, ECOL_WARN"Passwords do not match, please try again.\n\n");
	}
	d += sprintf(d, step == 0 ? ECOL_INFO : ECOL_DIALOG_TEXT);
	d += sprintf(d, "* choose a name");

	if (step == 0) {
		d += sprintf(d, " <<<<");
		d += sprintf(d, "\n    must be 3-20 length, 0-9a-zA-Z=()[]$@._");
		d += sprintf(d, "\n    must not start with @");
		if (pdata[pid] != NULL) {
			d += sprintf(d, "\n    Current name: %s", pdata[pid]->name);
		}
	}
	d += sprintf(d, step == 1 ? ECOL_INFO"\n" : ECOL_DIALOG_TEXT"\n");
	d += sprintf(d, "* choose a password");
	if (step == 1) d += sprintf(d, " <<<<");
	d += sprintf(d, step == 2 ? ECOL_INFO"\n" : ECOL_DIALOG_TEXT"\n");
	d += sprintf(d, "* confirm your password");
	if (step == 2) d += sprintf(d, " <<<<");
	amx_GetAddr(amx, params[2], &addr);
	amx_SetUString(addr, data, sizeof(data));
	return 1;
}

/* native Login_FormatLoadAccountData(userid, buf[]) */
cell AMX_NATIVE_CALL Login_FormatLoadAccountData(AMX *amx, cell *params)
{
	char data[512];
	cell *addr;
	sprintf(data, "SELECT score,cash,distance,flighttime,prefs FROM usr WHERE i=%d", (int) params[1]);
	amx_GetAddr(amx, params[2], &addr);
	amx_SetUString(addr, data, sizeof(data));
	return 1;
}

/* native Login_FormatOnJoinRegisterBox(buf[], .pwmismatch=0, step) */
cell AMX_NATIVE_CALL Login_FormatOnJoinRegisterBox(AMX *amx, cell *params)
{
	int pwmismatch = params[2], step = params[3];
	char data[512];
	char *d = &data[0];
	cell *addr;
	if (pwmismatch) {
		d += sprintf(d, ECOL_WARN"Passwords do not match!\n\n");
	}
	d += sprintf(d, ECOL_DIALOG_TEXT"Welcome! Register your account or continue as a guest.\n\n");
	d += sprintf(d, step == 0 ? ECOL_INFO : ECOL_DIALOG_TEXT);
	d += sprintf(d, "* choose a password");
	if (step == 0) d += sprintf(d, " <<<<");
	d += sprintf(d, step == 1 ? ECOL_INFO"\n" : ECOL_DIALOG_TEXT"\n");
	d += sprintf(d, "* confirm your password");
	if (step == 1) d += sprintf(d, " <<<<");
	amx_GetAddr(amx, params[1], &addr);
	amx_SetUString(addr, data, sizeof(data));
	return 1;
}

/* native Login_FormatSavePlayerName(playerid, buf[]) */
cell AMX_NATIVE_CALL Login_FormatSavePlayerName(AMX *amx, cell *params)
{
	int pid = params[1];
	char data[144];
	cell *addr;
	if (pdata[pid] == NULL) {
		return 0;
	}
	sprintf(data,
	        "UPDATE usr SET name='%s' WHERE i=%d",
	        pdata[pid]->name,
	        pdata[pid]->userid);
	amx_GetAddr(amx, params[2], &addr);
	amx_SetUString(addr, data, sizeof(data));
	return 1;
}

/* native Login_FormatUpgradeGuestAcc(playerid, password[], buf[]) */
cell AMX_NATIVE_CALL Login_FormatUpgradeGuestAcc(AMX *amx, cell *params)
{
	int pid = params[1];
	char pw[144], data[512];
	cell *addr;
	if (pdata[pid] == NULL) {
		return 0;
	}
	amx_GetAddr(amx, params[2], &addr);
	amx_GetUString(pw, addr, sizeof(pw));
	sprintf(data,
		"UPDATE usr SET pw='%s',name='%s',groups=%d WHERE i=%d",
	        pw,
		pdata[pid]->name,
		GROUP_MEMBER,
	        pdata[pid]->userid);
	amx_GetAddr(amx, params[3], &addr);
	amx_SetUString(addr, data, sizeof(data));
	return 1;
}

/* native Login_FreePassword(playerid); */
cell AMX_NATIVE_CALL Login_FreePassword(AMX *amx, cell *params)
{
	int playerid = params[1];
	if (pwdata[playerid] != NULL) {
		free(pwdata[playerid]);
		pwdata[playerid] = NULL;
	}
	return 1;
}

/* native Login_GetPassword(playerid, buf[]); */
cell AMX_NATIVE_CALL Login_GetPassword(AMX *amx, cell *params)
{
	int playerid = params[1];
	cell *outaddr = NULL;
	if (pwdata[playerid] == NULL) {
		return 0;
	}
	amx_GetAddr(amx, params[2], &outaddr);
	amx_SetUString(outaddr, pwdata[playerid], PW_HASH_LENGTH);
	return 1;
}

/* native Login_PasswordConfirmFree(playerid) */
cell AMX_NATIVE_CALL Login_PasswordConfirmFree(AMX *amx, cell *params)
{
	int playerid = params[1];
	if (pwchangeconfirmdata[playerid] != NULL) {
		free(pwchangeconfirmdata[playerid]);
		pwchangeconfirmdata[playerid] = NULL;
	}
	return 1;
}

/* native Login_PasswordConfirmStore(playerid, pwhash[]) */
cell AMX_NATIVE_CALL Login_PasswordConfirmStore(AMX *amx, cell *params)
{
	int playerid = params[1];
	cell *inaddr = NULL;

	if (pwchangeconfirmdata[playerid] == NULL) {
		pwchangeconfirmdata[playerid] = (char *) malloc(PW_HASH_LENGTH * sizeof(char));
	}
	amx_GetAddr(amx, params[2], &inaddr);
	amx_GetUString(pwchangeconfirmdata[playerid], inaddr, PW_HASH_LENGTH);

	return 1;
}

/* native Login_PasswordConfirmValidate(playerid, pwhash[]) */
cell AMX_NATIVE_CALL Login_PasswordConfirmValidate(AMX *amx, cell *params)
{
	int result;
	int playerid = params[1];
	cell *inaddr = NULL;
	char second[PW_HASH_LENGTH];

	if (pwchangeconfirmdata[playerid] == NULL) {
		return 0;
	}
	amx_GetAddr(amx, params[2], &inaddr);
	amx_GetUString(second, inaddr, PW_HASH_LENGTH);

	result = strcmp(pwchangeconfirmdata[playerid], second);
	free(pwchangeconfirmdata[playerid]);
	pwchangeconfirmdata[playerid] = NULL;

	return result == 0;
}

/* native Login_UsePassword(playerid, buf[]); */
cell AMX_NATIVE_CALL Login_UsePassword(AMX *amx, cell *params)
{
	int playerid = params[1];
	cell *inaddr = NULL;
	if (pwdata[playerid] == NULL) {
		pwdata[playerid] = (char *) malloc(PW_HASH_LENGTH * sizeof(char));
	}
	amx_GetAddr(amx, params[2], &inaddr);
	amx_GetUString(pwdata[playerid], inaddr, PW_HASH_LENGTH);
	return 1;
}

