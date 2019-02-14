
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

cell AMX_NATIVE_CALL FormatLoginApiLogin(AMX *amx, cell *params)
{
	int pid = params[1], uid = params[2], len;
	char data[2 + 10 + 3 + (128 * 3) + 3 + 15 + 2];
	if (pdata[pid] == NULL) {
		return 0;
	}
	len = sprintf(data, "i=%d", uid);
	return appendEncodedPasswordAndIp(amx, params + 3, len, data + len, pdata[pid]->ip);
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

