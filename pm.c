
/* vim: set filetype=c ts=8 noexpandtab: */

#define _CRT_SECURE_NO_DEPRECATE
#include "common.h"
#include "cmd.h"
#include "playerdata.h"
#include <string.h>

#define LAST_PMTARGET_NOBODY -1
#define LAST_PMTARGET_INVALID -2

/**
The last pm target for each player, the player id that is used when doing /r.
*/
static int lastpmtarget[MAX_PLAYERS];

/**
Resets the last pm target for given player.
*/
void pm_on_player_connect(int playerid)
{
	lastpmtarget[playerid] = -1;
}

/**
Change every player that has given player as pm target to invalid target id.
*/
void pm_on_player_disconnect(int playerid)
{
	int pid, n = playercount;

	while (n--) {
		pid = players[n];
		if (lastpmtarget[pid] == playerid) {
			lastpmtarget[pid] = LAST_PMTARGET_INVALID;
		}
	}
}

/**
Sends a pm. Target player is assumed to be online.
*/
static void pm_send(AMX *amx, int from, int to, char *msg)
{
	static const char
		*FROM_DIS = WARN"Your PMs are disabled, use /p to enable them.",
		*TO_DIS = WARN"That player has PMs disabled.",
		*REPLYHINT = INFO"Use /r to quickly reply to the message.";

	char buf[144 + MAX_PLAYER_NAME + 20];

	if (!(prefs[from] & PREF_ENABLE_PM)) {
		amx_SetUString(buf144, FROM_DIS, 144);
		goto errmsg;
	}
	if (!(prefs[to] & PREF_ENABLE_PM)) {
		amx_SetUString(buf144, TO_DIS, 144);
		goto errmsg;
	}
	sprintf(buf, ">> %s(%d): %s", pdata[to]->name, to, msg);
	amx_SetUString(buf144, buf, 144);
	NC_SendClientMessage(from, COL_PRIVMSG, buf144a);
	sprintf(buf, "** %s(%d): %s", pdata[from]->name, from, msg);
	amx_SetUString(buf144, buf, 144);
	NC_SendClientMessage(to, COL_PRIVMSG, buf144a);
	NC_PlayerPlaySound0(to, 1139 /*SOUND_CHECKPOINT_RED*/);
	if (lastpmtarget[to] == LAST_PMTARGET_NOBODY) {
		amx_SetUString(buf144, REPLYHINT, 144);
		NC_SendClientMessage(to, COL_PRIVMSG_HINT, buf144a);
	}
	lastpmtarget[to] = from;
	return;
errmsg:
	NC_SendClientMessage(from, COL_WARN, buf144a);
	return;
}

int pm_cmd_pm(CMDPARAMS)
{
	static const char
		*SYNERR = WARN"Syntax: /pm [id/name] [message]",
		*NOTONLINE = WARN"That player is not online.";

	int targetid;

	if (!cmd_get_player_param(cmdtext, &parseidx, &targetid)) {
synerr:		amx_SetUString(buf144, SYNERR, 144);
		goto errmsg;
	}
	if (targetid == INVALID_PLAYER_ID) {
		amx_SetUString(buf144, NOTONLINE, 144);
errmsg:		NC_SendClientMessage(playerid, COL_WARN, buf144a);
		return 1;
	}
	while (cmdtext[parseidx] == ' ') {
		parseidx++;
	}
	if (cmdtext[parseidx]) {
		pm_send(amx, playerid, targetid, (char*) cmdtext + parseidx);
	} else {
		goto synerr;
	}
	return 1;
}

int pm_cmd_r(CMDPARAMS)
{
	static const char
		*SYN = WARN"Syntax: /r [message]",
		*NOTARGET = WARN"Nobody has sent you a PM yet! "
			"Use /pm [id/name] [message]",
		*INVALID = WARN"The person who last sent you a PM left";

	while (cmdtext[parseidx] == ' ') {
		parseidx++;
	}
	if (cmdtext[parseidx]) {
		switch (lastpmtarget[playerid]) {
		case LAST_PMTARGET_NOBODY:
			amx_SetUString(buf144, NOTARGET, 144);
			goto errmsg;
		case LAST_PMTARGET_INVALID:
			amx_SetUString(buf144, INVALID, 144);
			goto errmsg;
		}
		pm_send(amx, playerid, lastpmtarget[playerid],
			(char*) cmdtext + parseidx);
	} else {
		amx_SetUString(buf144, SYN, 144);
errmsg:		NC_SendClientMessage(playerid, COL_WARN, buf144a);
		return 1;
	}
	return 1;
}