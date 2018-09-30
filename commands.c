
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"

cell AMX_NATIVE_CALL CommandHash(AMX *amx, cell *params)
{
	char buf[144];
	int val, pos = 0, result = 0;

	cell *inaddr = NULL;
	amx_GetAddr(amx, params[1], &inaddr);
	amx_GetUString(buf, inaddr, 144);

	/* same as hashCode in Java (but case insensitive) */
	while (buf[pos] != 0 && buf[pos] > ' ') {
		val = buf[pos++];
		if ('A' <= val && val <= 'Z') {
			val |= 0x20;
		}
		result = 31 * result + val;
	}

	return result;
}

cell AMX_NATIVE_CALL IsCommand(AMX *amx, cell *params)
{
	char cmdtext[50], cmd[50];
	int len;

	cell *addrofcmdtext = NULL, *addrofcmd = NULL;
	amx_GetAddr(amx, params[1], &addrofcmdtext);
	amx_GetAddr(amx, params[2], &addrofcmd);
	amx_GetUString(cmdtext, addrofcmdtext, 50);
	amx_GetUString(cmd, addrofcmd, 50);
	amx_StrLen(addrofcmd, &len);

	if (cmdtext[len] > ' ') {
		return 0;
	}

	/* gt 0 because no need to check / */
	while (--len > 0) {
		if (cmdtext[len] == cmd[len]) {
			continue;
		}
		if ('A' <= cmdtext[len] && cmdtext[len] <= 'Z' && (cmdtext[len] | 0x20) == cmd[len]) {
			continue;
		}
		return 0;
	}

	return 1;
}

