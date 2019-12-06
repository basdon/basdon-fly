
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "a_samp.h"
#include "airport.h"
#include "changepassword.h"
#include "echo.h"
#include "game_sa.h"
#include "guestregister.h"
#include "playerdata.h"
#include "pm.h"
#include "prefs.h"
#include "protips.h"
#include "nav.h"
#include "missions.h"
#include "money.h"
#include "servicepoints.h"
#include "timecyc.h"
#include "vehicles.h"
#include "zones.h"
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
	int (*handler)(const int, const char*, int);
};

#define IN_CMD
#include "cmdhandlers.c"
#ifdef DEV
#include "cmdhandlers_dev.c"
#endif /*DEV*/

/* see sharedsymbols.h for GROUPS_ definitions */
/* command must prefixed by forward slash and be lower case */
static struct COMMAND cmds[] = {
#ifdef DEV
	{ 0, "/*m", GROUPS_ALL, cmd_dev_STARm },
	{ 0, "/closestmp", GROUPS_ALL, cmd_dev_closestmp },
	{ 0, "/cp", GROUPS_ALL, cmd_dev_cp },
	{ 0, "/crashme", GROUPS_ALL, cmd_dev_crashme },
	{ 0, "/gt", GROUPS_ALL, cmd_dev_gt },
	{ 0, "/jetpack", GROUPS_ALL, cmd_dev_jetpack },
	{ 0, "/kickme", GROUPS_ALL, cmd_dev_kickme },
	{ 0, "/kill", GROUPS_ALL, cmd_dev_kill },
	{ 0, "/platform", GROUPS_ALL, cmd_dev_platform },
	{ 0, "/testparpl", GROUPS_ALL, cmd_dev_testparpl },
	{ 0, "/timex", GROUPS_ALL, cmd_dev_timex },
	{ 0, "//owner", GROUPS_ALL, cmd_dev_owner },
	{ 0, "/fweather", GROUPS_ALL, timecyc_cmd_dev_fweather },
	{ 0, "/nweather", GROUPS_ALL, timecyc_cmd_dev_nweather },
	{ 0, "/sound", GROUPS_ALL, cmd_dev_sound },
	{ 0, "/timecyc", GROUPS_ALL, timecyc_cmd_dev_timecyc },
	{ 0, "/tweather", GROUPS_ALL, timecyc_cmd_dev_tweather },
	{ 0, "/v", GROUPS_ALL, cmd_dev_v },
	{ 0, "/vdamage", GROUPS_ALL, cmd_dev_vdamage },
	{ 0, "/vehrespawn", GROUPS_ALL, cmd_dev_vehrespawn },
	{ 0, "/vhp", GROUPS_ALL, cmd_dev_vhp },
	{ 0, "/vhpnan", GROUPS_ALL, cmd_dev_vhpnan },
	{ 0, "/vhpninf", GROUPS_ALL, cmd_dev_vhpninf },
	{ 0, "/vhppinf", GROUPS_ALL, cmd_dev_vhppinf },
#endif /*DEV*/
	{ 0, "//respawn", GROUPS_ADMIN, cmd_admin_respawn },
	{ 0, "/adf", GROUPS_ALL, nav_cmd_adf },
	{ 0, "/at400", GROUPS_ALL, cmd_at400 },
	{ 0, "/automission", GROUPS_ALL, missions_cmd_automission },
	{ 0, "/autow", GROUPS_ALL, missions_cmd_automission },
	{ 0, "/beacons", GROUPS_ALL, airport_cmd_beacons },
	{ 0, "/camera", GROUPS_ALL, cmd_camera },
	{ 0, "/cancelwork", GROUPS_ALL, missions_cmd_cancelmission },
	{ 0, "/changepassword", GROUPS_REGISTERED, chpw_cmd_changepassword },
	{ 0, "/chute", GROUPS_ALL, cmd_chute },
	{ 0, "/fix", GROUPS_ALL, svp_cmd_repair },
	{ 0, "/getspray", GROUPS_ALL, cmd_getspray },
	{ 0, "/helpkeys", GROUPS_ALL, cmd_helpkeys },
	{ 0, "/ils", GROUPS_ALL, nav_cmd_ils },
	{ 0, "/loc", GROUPS_ALL, zones_cmd_loc },
	{ 0, "/nearest", GROUPS_ALL, airport_cmd_nearest },
	{ 0, "/m", GROUPS_ALL, missions_cmd_mission },
	{ 0, "/me", GROUPS_ALL, cmd_me },
	{ 0, "/metar", GROUPS_ALL, timecyc_cmd_metar },
	{ 0, "/mission", GROUPS_ALL, missions_cmd_mission },
	{ 0, "/park", GROUPS_ALL, cmd_park },
	{ 0, "/p", GROUPS_ALL, prefs_cmd_preferences },
	{ 0, "/pm", GROUPS_ALL, pm_cmd_pm },
	{ 0, "/preferences", GROUPS_ALL, prefs_cmd_preferences },
	{ 0, "/protip", GROUPS_ALL, protips_cmd_protip },
	{ 0, "/r", GROUPS_ALL, pm_cmd_r },
	{ 0, "/reclass", GROUPS_ALL, cmd_reclass },
	{ 0, "/register", GROUP_GUEST, guestreg_cmd_register },
	{ 0, "/repair", GROUPS_ALL, svp_cmd_repair },
	{ 0, "/refuel", GROUPS_ALL, svp_cmd_refuel },
	{ 0, "/respawn", GROUPS_ALL, cmd_respawn },
	{ 0, "/s", GROUPS_ALL, missions_cmd_cancelmission },
	{ 0, "/tickrate", GROUPS_ALL, cmd_tickrate },
	{ 0, "/spray", GROUPS_ALL, cmd_spray },
	{ 0, "/vor", GROUPS_ALL, nav_cmd_vor },
	{ 0, "/w", GROUPS_ALL, missions_cmd_mission },
	{ 0, "/weather", GROUPS_ALL, timecyc_cmd_metar },
}, *cmds_end = cmds + sizeof(cmds)/sizeof(cmds[0]);

/*
Hashes command part of command text (case-insensitive).
End delimiter for the command part is either a zero terminator, or anything
with a value below the space character.
*/
static
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
static
int cmd_is(const char *cmdtext, const char *cmd, int *parseidx)
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
int cmd_check(const int playerid, const char *cmdtext)
{
	struct COMMAND *c = cmds;
	int parseidx, hash;

	hash = cmd_hash(cmdtext);

	while (c != cmds_end) {
		if (hash == c->hash &&
			(pdata[playerid]->groups & c->groups) &&
			cmd_is(cmdtext, c->cmd, &parseidx))
		{
			return c->handler(playerid, cmdtext, parseidx);
		}
		c++;
	}
	return 0;
}

