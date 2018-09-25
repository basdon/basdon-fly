
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include <string.h>

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

