
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "playerdata.h"
#include <string.h>

/*
Hashes command part of command text (case-insensitive).
End delimiter for the command part is either a zero terminator, or anything
with a value below the space character.
*/
int cmds_hash(const char *cmdtext)
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

/*
Check if the command in cmdtext is same as cmd (case insensensitive).
Parseidx is not written to if it didn't match.
On match, parseidx is the index right after the command, so either space or \0.
*/
static int cmds_is(const char *cmdtext, const char *cmd, int *parseidx)
{
	int pos = 0;

nextchar:
	/* starts checking at 1 because 0 is always a forward slash */
	++pos;
	if (cmdtext[pos] == cmd[pos]) {
		if (cmdtext[pos] == 0) {
			*parseidx = pos;
			return 1;
		}
		goto nextchar;
	}
	if ('A' <= cmdtext[pos] && cmdtext[pos] <= 'Z' &&
		(cmdtext[pos] | 0x20) == cmd[pos])
	{
		goto nextchar;
	}
	return 0;
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
	cell *addr;
	int len;

	amx_GetAddr(amx, params[1], &addr);
	amx_GetUString(cmdtext, addr, 50);
	amx_GetAddr(amx, params[2], &addr);
	amx_GetUString(cmd, addr, 50);
	amx_StrLen(addr, &len);

	if (len > 49) {
		return 0;
	}
	amx_GetAddr(amx, params[3], &addr);
	return cmds_is(cmdtext, cmd, (int*) addr);
}

#define CMDPARAMS const int playerid, const char *cmdtext, int parseidx

int cmd_admin_spray(CMDPARAMS)
{
	logprintf("respray");
	return 1;
}

struct COMMAND {
	int hash;
	const char *cmd;
	const int groups;
	int (*handler)(int, const char*, int);
};

/* see sharedsymbols.h for GROUPS_ definitions */
/* command must prefixed by forward slash and be lower case */
struct COMMAND cmds[] = {
	{ 0, "//spray", GROUPS_ALL, cmd_admin_spray },
}, *cmds_end = cmds + sizeof(cmds)/sizeof(cmds[0]);

/*
Precalcs all command hashes.
*/
void cmds_init()
{
	struct COMMAND *c = cmds;

	while (c != cmds_end) {
		c->hash = cmds_hash(c->cmd);
		c++;
	}
}

/*
Checks incoming command and calls handler if one found and group matched.
*/
int cmds_check(const int playerid, const int hash, const char *cmdtext)
{
	struct COMMAND *c = cmds;
	int parseidx;

	while (c != cmds_end) {
		if (hash == c->hash &&
			(pdata[playerid]->groups & c->groups) &&
			cmds_is(cmdtext, c->cmd, &parseidx))
		{
			return c->handler(playerid, cmdtext, parseidx);
		}
		c++;
	}
	return 0;
}

