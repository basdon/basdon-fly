
/* vim: set filetype=c ts=8 noexpandtab: */

#define _CRT_SECURE_NO_DEPRECATE
#include "common.h"
#include "cmd.h"
#include "playerdata.h"
#include "vehicles.h"
#include "game_sa.h"
#include <string.h>

int cmd_get_int_param(const char *cmdtext, int *parseidx, int *value)
{
	char *pc = (char*) cmdtext + *parseidx;
	int val = 0, sign = 1;

	/*not using atoi since parseidx needs to be updated*/
	while (*pc == ' ') {
		pc++;
	}
	if (*pc == '+') {
		pc++;
	} else if (*pc == '-') {
		sign = -1;
		pc++;
	}

nextchar:
	if (*pc < '0' || '9' < *pc) {
		return 0;
	}
	val = val * 10 + *pc - '0';
	pc++;
	if (*pc == 0 || *pc == ' ') {
		*parseidx = pc - cmdtext;
		*value = sign * val;
		return 1;
	}
	goto nextchar;
}

int cmd_get_player_param(const char *cmdtext, int *parseidx, int *playerid)
{
	char name[MAX_PLAYER_NAME + 1], val;
	char *pc = (char*) cmdtext + *parseidx;
	char *n = name, *nend = name + sizeof(name);
	int maybe_id = 0, isnumeric = 1, i;

	while (*pc == ' ') {
		pc++;
	}

nextchar:
	if (n == nend) {
		/*longer than allowed player name length*/
		return 0;
	}
	val = *pc++;
	if (val <= ' ') {
		if (n == name) {
			/*zero length*/
			return 0;
		}
		*n = 0; /*add zero term to name buffer*/
		goto gotvalue;
	}
	*n++ = val;
	if (isnumeric && '0' <= val && val <= '9') {
		if ((maybe_id = maybe_id * 10 + val - '0') >= MAX_PLAYERS) {
			isnumeric = 0;
		}
		goto nextchar;
	}
	isnumeric = 0;
	if ('A' <= val && val <= 'Z') {
		/*adjust capitals*/
		*(n - 1) = val | 0x20;
		goto nextchar;
	}
	/*give up on invalid player name characters*/
	if ((val < 'a' || 'z' < val) && (val < '0' || '9' < val) &&
		val != '[' && val != ']' && val != '(' &&
		val != ')' && val != '$' && val != '@' &&
		val != '.' && val != '_' && val != '=')
	{
		return 0;
	}
	goto nextchar;

gotvalue:
	*parseidx = pc - cmdtext;
	*playerid = INVALID_PLAYER_ID;
	if (isnumeric) {
		if (IsPlayerConnected(maybe_id)) {
			*playerid = maybe_id;
		}
		return 1;
	}

	for (i = 0; i < MAX_PLAYERS; i++) {
		if (IsPlayerConnected(i) &&
			strstr(pdata[i]->normname, name) != NULL)
		{
			*playerid = i;
			return 1;
		}
	}
	return 1;
}

int cmd_get_str_param(const char* cmdtext, int *parseidx, char *buf)
{
	char *pc = (char*) cmdtext + *parseidx;
	char *b = buf;

	while (*pc == ' ') {
		pc++;
	}
	while ((*b = *pc) > ' ') {
		b++;
		pc++;
	}
	if (b - buf > 0) {
		*b = 0;
		*parseidx = pc - cmdtext;
		return 1;
	}
	return 0;
}

struct COMMAND {
	int hash;
	const char *cmd;
	const int groups;
	int (*handler)(AMX*, const int, const char*, int);
};

#define IN_CMD
#include "cmdhandlers.c"
#ifdef DEV
#include "cmdhandlers_dev.c"
#endif /*DEV*/

int prefs_cmd_preferences(CMDPARAMS);
int protips_cmd_protip(CMDPARAMS);
int nav_cmd_adf(CMDPARAMS);
int nav_cmd_ils(CMDPARAMS);
int nav_cmd_vor(CMDPARAMS);
int zones_cmd_loc(CMDPARAMS);

/* see sharedsymbols.h for GROUPS_ definitions */
/* command must prefixed by forward slash and be lower case */
static struct COMMAND cmds[] = {
#ifdef DEV
	{ 0, "/testparpl", GROUPS_ALL, cmd_dev_testparpl },
	{ 0, "//closestmp", GROUPS_ALL, cmd_dev_closestmp },
	{ 0, "//owner", GROUPS_ALL, cmd_dev_owner },
#endif /*DEV*/
	{ 0, "//respawn", GROUPS_ADMIN, cmd_admin_respawn },
	{ 0, "/adf", GROUPS_ALL, nav_cmd_adf },
	{ 0, "/camera", GROUPS_ALL, cmd_camera },
	{ 0, "/ils", GROUPS_ALL, nav_cmd_ils },
	{ 0, "/loc", GROUPS_ALL, zones_cmd_loc },
	{ 0, "/park", GROUPS_ALL, cmd_park },
	{ 0, "/p", GROUPS_ALL, prefs_cmd_preferences },
	{ 0, "/preferences", GROUPS_ALL, prefs_cmd_preferences },
	{ 0, "/protip", GROUPS_ALL, protips_cmd_protip },
	{ 0, "/reclass", GROUPS_ALL, cmd_reclass },
	{ 0, "/respawn", GROUPS_ALL, cmd_respawn },
	{ 0, "/spray", GROUPS_ALL, cmd_spray },
	{ 0, "/vor", GROUPS_ALL, nav_cmd_vor },
}, *cmds_end = cmds + sizeof(cmds)/sizeof(cmds[0]);

/*
Hashes command part of command text (case-insensitive).
End delimiter for the command part is either a zero terminator, or anything
with a value below the space character.
*/
int cmd_hash(const char *cmdtext)
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
static int cmd_is(const char *cmdtext, const char *cmd, int *parseidx)
{
	int pos = 0;

nextchar:
	/* starts checking at 1 because 0 is always a forward slash */
	++pos;
	if (cmdtext[pos] == cmd[pos] || cmd[pos] == 0) {
		if (cmdtext[pos] == 0 || cmdtext[pos] == ' ') {
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
	char cmdtext[144];
	cell *idx, *value;

	amx_GetAddr(amx, params[1], &idx);
	amx_GetUString(cmdtext, idx, sizeof(cmdtext));
	amx_GetAddr(amx, params[2], &idx);
	amx_GetAddr(amx, params[3], &value);
	return cmd_get_int_param(cmdtext, (int*) idx, (int*) value);
}

/* native Command_GetPlayerParam(cmdtext[], &idx, &player) */
cell AMX_NATIVE_CALL Command_GetPlayerParam(AMX *amx, cell *params)
{
	char cmdtext[144];
	cell *idx, *player;

	amx_GetAddr(amx, params[1], &idx);
	amx_GetUString(cmdtext, idx, sizeof(cmdtext));
	amx_GetAddr(amx, params[2], &idx);
	amx_GetAddr(amx, params[3], &player);
	return cmd_get_player_param(cmdtext, (int*) idx, (int*) player);
}

/* native Command_GetStringParam(cmdtext[], &idx, buf[]) */
cell AMX_NATIVE_CALL Command_GetStringParam(AMX *amx, cell *params)
{
	char cmdtext[144], param[144];
	cell *addr;

	amx_GetAddr(amx, params[1], &addr);
	amx_GetUString(cmdtext, addr, sizeof(cmdtext));
	amx_GetAddr(amx, params[2], &addr);

	if (cmd_get_str_param(cmdtext, (int*) addr, param)) {
		amx_GetAddr(amx, params[3], &addr);
		amx_SetUString(addr, param, sizeof(param));
		return 1;
	}
	return 0;
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
	return cmd_is(cmdtext, cmd, (int*) addr);
}

/*
Precalcs all command hashes.
*/
void cmd_init()
{
	struct COMMAND *c = cmds;

	while (c != cmds_end) {
		c->hash = cmd_hash(c->cmd);
		c++;
	}
}

/*
Checks incoming command and calls handler if one found and group matched.
*/
int
cmd_check(AMX *amx, const int playerid, const int hash, const char *cmdtext)
{
	struct COMMAND *c = cmds;
	int parseidx;

	while (c != cmds_end) {
		if (hash == c->hash &&
			(pdata[playerid]->groups & c->groups) &&
			cmd_is(cmdtext, c->cmd, &parseidx))
		{
			return c->handler(amx, playerid, cmdtext, parseidx);
		}
		c++;
	}
	return 0;
}

