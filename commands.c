
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "playerdata.h"
#include <string.h>

/*
Hashes command part of command text (case-insensitive).
End delimiter for the command part is either a zero terminator, or anything
with a value below the space character.
*/
int commands_hash(char *cmdtext)
{
	int val, pos = 0, result = 0;

	/* same as hashCode in Java (but case insensitive) */
	while (cmdtext[pos] != 0 && cmdtext[pos] > ' ') {
		val = cmdtext[pos++];
		if ('A' <= val && val <= 'Z') {
			val |= 0x20;
		}
		result = 31 * result + val;
	}
	return result;
}

/* native Command_GetIntParam(cmdtext[], &idx, &value) */
cell AMX_NATIVE_CALL Command_GetIntParam(AMX *amx, cell *params)
{
	char cmdtext[144], *pc = cmdtext;
	cell *idx, *value;
	int sign = 1;
	amx_GetAddr(amx, params[1], &idx);
	amx_GetUString(cmdtext, idx, sizeof(cmdtext));
	amx_GetAddr(amx, params[2], &idx);
	amx_GetAddr(amx, params[3], &value);
	pc += *idx;
	*value = 0;

	while (*pc == ' ') {
		pc++;
	}
	if (*pc == '+') {
		pc++;
	} else if (*pc == '-') {
		sign = -1;
		pc++;
	} else if (*pc < '0' || '9' < *pc) {
		return 0;
	}

	while (1) {
		*value = *value * 10 + *pc - '0';
		pc++;
		if (*pc == 0 || *pc == ' ') {
			*idx = pc - cmdtext - 1;
			*value *= sign;
			return 1;
		}
		if (*pc < '0' || '9' < *pc) {
			return 0;
		}
	}
}

/* native Command_GetPlayerParam(cmdtext[], &idx, &player) */
cell AMX_NATIVE_CALL Command_GetPlayerParam(AMX *amx, cell *params)
{
	char param[MAX_PLAYER_NAME + 1], *pp = param, val, cmdtext[144], *pc = cmdtext;
	cell *addr, *player;
	int isplayerid = 1;

	amx_GetAddr(amx, params[1], &addr);
	amx_GetUString(cmdtext, addr, sizeof(cmdtext));
	amx_GetAddr(amx, params[2], &addr);
	amx_GetAddr(amx, params[3], &player);
	pc += *addr;
	*player = 0;

	while (*pc == ' ') {
		pc++;
	}
	while (1) {
		if (pp - param >= sizeof(param)) {
			return 0;
		}
		val = *pc++;
		if (val <= ' ') {
			*pp = 0;
			break;
		}
		if (isplayerid && '0' <= val && val <= '9') {
			if ((*player = *player * 10 + val - '0') >= MAX_PLAYERS) {
				isplayerid = 0;
			}
			*pp++ = val;
			continue;
		}
		isplayerid = 0;
		if ('A' <= val && val <= 'Z') {
			*pp++ = val | 0x20;
			continue;
		}
		if ((val < 'a' || 'z' < val) && (val < '0' || '9' < val) && val != '[' &&
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

	*addr = pc - cmdtext - 1;
	if (isplayerid) {
		if (pdata[*player] == NULL) {
			*player = INVALID_PLAYER_ID;
		}
		return 1;
	}
#define tmp isplayerid
	for (tmp = 0; tmp < MAX_PLAYERS; tmp++) {
		if (pdata[tmp] == NULL) {
			continue;
		}
		if (strstr(pdata[tmp]->normname, param) != NULL) {
			*player = tmp;
			return 1;
		}
	}
#undef tmp
	*player = INVALID_PLAYER_ID;
	return 1;
}

/* native Command_GetStringParam(cmdtext[], &idx, buf[]) */
cell AMX_NATIVE_CALL Command_GetStringParam(AMX *amx, cell *params)
{
	char param[144], *pp = param, *trimmedparam, val;
	cell *addr;

	amx_GetAddr(amx, params[1], &addr);
	amx_GetUString(param, addr, sizeof(param));
	amx_GetAddr(amx, params[2], &addr);

	pp += *addr;
	while (*pp == ' ') {
		pp++;
	}
	trimmedparam = pp;
	while (1) {
		val = *pp++;
		if (val <= ' ') {
			*pp = 0;
			break;
		}
	}
	if (trimmedparam[0] == 0) {
		return 0;
	}

	*addr = pp - param - 1;
	amx_GetAddr(amx, params[3], &addr);
	amx_SetUString(addr, trimmedparam, sizeof(param) + param - trimmedparam);
	return 1;
}

/* native Command_Is(cmdtext[], const cmd[], &idx) */
cell AMX_NATIVE_CALL Command_Is(AMX *amx, cell *params)
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

	amx_GetAddr(amx, params[3], &addrofcmd);
	*addrofcmd = len;

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

