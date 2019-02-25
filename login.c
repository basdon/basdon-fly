
/* vim: set filetype=c ts=8 noexpandtab: */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include "common.h"
#include <string.h>
#include "playerdata.h"

static char *pwdata[MAX_PLAYERS];

void login_init()
{
	int i;
	for (i = 0; i < MAX_PLAYERS; i++) {
		pwdata[i] = NULL;
	}
}

cell AMX_NATIVE_CALL ResetPasswordConfirmData(AMX *amx, cell *params)
{
	int playerid = params[1];
	if (pwdata[playerid] != NULL) {
		free(pwdata[playerid]);
		pwdata[playerid] = NULL;
	}
	return 1;
}

cell AMX_NATIVE_CALL SetPasswordConfirmData(AMX *amx, cell *params)
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

cell AMX_NATIVE_CALL ValidatePasswordConfirmData(AMX *amx, cell *params)
{
	int result;
	int playerid = params[1];
	cell *inaddr = NULL;
	char pwdata2[PW_HASH_LENGTH];

	if (pwdata[playerid] == NULL) {
		return 0;
	}
	amx_GetAddr(amx, params[2], &inaddr);
	amx_GetUString(pwdata2, inaddr, PW_HASH_LENGTH);

	result = strcmp(pwdata[playerid], pwdata2);
	free(pwdata[playerid]);
	pwdata[playerid] = NULL;

	return result == 0;
}

void urlenc(char *in, char *out)
{
	while (*in) {
		sprintf(out, "%%%02X", *in);
		out += 3;
		++in;
	}
	*out = 0;
}

int appendEncodedPasswordAndIp(AMX *amx, cell *params, int prevlen, char *data, char *ip)
{
	int len = prevlen;
	char encpw[128 * 3 + 1];
	cell *addr;

	amx_GetAddr(amx, params[0], &addr);
	amx_GetUString(data, addr, 129);
	data[128] = 0;
	urlenc(data, encpw);

	len += sprintf(data, "&p=%s&j=%s", encpw, ip);
	amx_GetAddr(amx, params[1], &addr);
	amx_SetUString(addr, data - prevlen, len + 1);
	return 1;
}

cell AMX_NATIVE_CALL FormatLoginApiRegister(AMX *amx, cell *params)
{
	int pid = params[1], len;
	char data[2 + (MAX_PLAYER_NAME * 3) + 3 + (128 * 3) + 3 + 15 + 2];
	if (pdata[pid] == NULL) {
		return 0;
	}
	len = sprintf(data, "n=%s", pdata[pid]->name);
	return appendEncodedPasswordAndIp(amx, params + 2, len, data + len, pdata[pid]->ip);
}

cell AMX_NATIVE_CALL FormatLoginApiGuestRegister(AMX *amx, cell *params)
{
	int pid = params[1], uid = params[2], len;
	char encpw[128 * 3 + 1], encname[MAX_PLAYER_NAME * 3 + 1];
	char data[2 + 10 + 3 + (MAX_PLAYER_NAME * 3) + 3 + (128 * 3) + 4 + 2];
	cell *addr;

	if (pdata[pid] == NULL) {
		return 0;
	}

	amx_GetAddr(amx, params[3], &addr);
	amx_GetUString(data, addr, 129);
	data[128] = 0;
	urlenc(data, encpw);
	urlenc(pdata[pid]->name, encname);

	len = sprintf(data, "i=%d&n=%s&p=%s&g=4", uid, encname, encpw);
	amx_GetAddr(amx, params[4], &addr);
	amx_SetUString(addr, data, len + 1);
	return 1;
}

cell AMX_NATIVE_CALL FormatLoginApiCheckChangePass(AMX *amx, cell *params)
{
	int uid = params[1], len;
	char encpw[128 * 3 + 1];
	char data[2 + 10 + 3 + (128 * 3) + 2];
	cell *addr;

	amx_GetAddr(amx, params[2], &addr);
	amx_GetUString(data, addr, 129);
	data[128] = 0;
	urlenc(data, encpw);

	len = sprintf(data, "i=%d&p=%s", uid, encpw);
	amx_GetAddr(amx, params[3], &addr);
	amx_SetUString(addr, data, len + 1);
	return 1;
}

cell AMX_NATIVE_CALL FormatLoginApiUserExistsGuest(AMX *amx, cell *params)
{
	int pid = params[1], len;
	char encname[MAX_PLAYER_NAME * 3 + 1];
	char data[2 + (MAX_PLAYER_NAME * 3) + 3 + 15 + 2];
	cell *addr;

	if (pdata[pid] == NULL) {
		return 0;
	}
	urlenc(pdata[pid]->name, encname);

	len = sprintf(data, "n=%s&j=%s", encname, pdata[pid]->ip);
	amx_GetAddr(amx, params[2], &addr);
	amx_SetUString(addr, data, len + 1);
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
	        "(SELECT count(u) AS c FROM fal WHERE t>UNIX_TIMESTAMP()-1800 AND j='%s') AS b,"
	        "(SELECT p FROM usr WHERE n='%s') AS p,"
	        "(SELECT i FROM usr WHERE n='%s') AS i", pdata[pid]->ip, pdata[pid]->name, pdata[pid]->name);
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
	            "UPDATE usr SET l=UNIX_TIMESTAMP() WHERE i=%d LIMIT 1",
	            (int) params[1]);
	if (p < 0) {
		return 0;
	}
	amx_GetAddr(amx, params[2], &addr);
	p += 2;
	*addr = p;
	amx_SetUString(addr + 1, data, sizeof(data));
	sprintf(data,
	        "INSERT INTO ses(u,s,e,j) VALUES(%d,UNIX_TIMESTAMP(),UNIX_TIMESTAMP(),'%s')",
	        pdata[pid]->userid,
	        pdata[pid]->ip);
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
	sprintf(data, "SELECT s FROM usr WHERE i=%d", (int) params[1]);
	amx_GetAddr(amx, params[2], &addr);
	amx_SetUString(addr, data, sizeof(data));
	return 1;
}

/* native Login_FormatRegisterGuestAccount(playerid, password[], buf[]) */
cell AMX_NATIVE_CALL Login_FormatRegisterGuestAcc(AMX *amx, cell *params)
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
	        "UPDATE usr SET p='%s',n='%s',g=4 WHERE i=%d",
	        pw,
		pdata[pid]->name,
	        pdata[pid]->userid);
	amx_GetAddr(amx, params[3], &addr);
	amx_SetUString(addr, data, sizeof(data));
	return 1;
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

/* native Login_GetPassword(playerid, buf[]); */
cell AMX_NATIVE_CALL Login_GetPassword(AMX *amx, cell *params)
{
	int playerid = params[1];
	cell *outaddr = NULL;
	const char dummy = 0;
	amx_GetAddr(amx, params[2], &outaddr);
	if (pwdata[playerid] == NULL) {
		amx_SetUString(outaddr, &dummy, PW_HASH_LENGTH);
		return 1;
	}
	amx_SetUString(outaddr, pwdata[playerid], PW_HASH_LENGTH);
	return 1;
}

