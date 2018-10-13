
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "playerdata.h"
#include <string.h>

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

/* native Params_GetPlayer(cmdtext[], &idx, &player) */
cell AMX_NATIVE_CALL Params_GetPlayer(AMX *amx, cell *params)
{
	char param[MAX_PLAYER_NAME + 1], *pp = param, val, cmdtext[144], *pc = cmdtext;
	cell *addr, *player;
	int idx;
	int allnums = 1;

	amx_GetAddr(amx, params[1], &addr);
	amx_GetUString(cmdtext, addr, sizeof(cmdtext));
	amx_GetAddr(amx, params[2], &addr);
	amx_GetAddr(amx, params[3], &player);
	idx = *addr;
	*player = 0;

	while (*pc == ' ') {
		pc++;
		idx++;
	}
	while (1) {
		val = *pc++;
		if (val <= ' ') {
			*pp = 0;
			break;
		}
		idx++;
		if (allnums && '0' <= val && val <= '9') {
			if ((*player = *player * 10 + val - '0') >= MAX_PLAYERS) {
				allnums = 0;
			}
			*pp++ = val;
			continue;
		}
		allnums = 0;
		if ('A' <= val && val <= 'Z') {
			*pp++ = val | 0x20;
			continue;
		}
		if ((val < 'a' || 'z' < val) && val != '[' &&
			val != ']' && val != '(' && val != ')' && val != '$' &&
			val != '@' && val != '.' && val != '_' && val != '=')
		{
			return 0;
		}
		*pp++ = val;
	}
	if (param[0] == 0) {
		return 0;
	}

	*addr = idx;
	if (allnums) {
		if (pdata[*player] == NULL) {
			*player = INVALID_PLAYER_ID;
		}
		return 1;
	}
	for (allnums = 0; allnums < MAX_PLAYERS; allnums++) {
		if (pdata[allnums] == NULL) {
			continue;
		}
		if (strstr(pdata[allnums]->normname, param) != NULL) {
			*player = allnums;
			return 1;
		}
	}
	*player = INVALID_PLAYER_ID;
	return 1;
}

