#define LAST_PMTARGET_NOBODY -1
#define LAST_PMTARGET_INVALID -2

/**
The last pm target for each player, the player id that is used when doing /r.
*/
static int lastpmtarget[MAX_PLAYERS];

/**
Resets the last pm target for given player.
*/
static
void pm_on_player_connect(int playerid)
{
	lastpmtarget[playerid] = -1;
}

/**
Change every player that has given player as pm target to invalid target id.
*/
static
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
static
void pm_send(int from, int to, char *msg)
{
	static const char
		*FROM_DIS = WARN"Your PMs are disabled, use /p to enable them.",
		*TO_DIS = WARN"That player has PMs disabled.",
		*REPLYHINT = INFO"Use /r to quickly reply to the message.";

	if (!(prefs[from] & PREF_ENABLE_PM)) {
		B144((char*) FROM_DIS);
		goto errmsg;
	}
	if (!(prefs[to] & PREF_ENABLE_PM)) {
		B144((char*) TO_DIS);
		goto errmsg;
	}
	/*using buf4096 to not overflow 144 since formatted str might be long*/
	csprintf(buf4096, ">> %s(%d): %s", pdata[to]->name, to, msg);
	buf4096[143] = 0; /*message won't send if too long*/
	NC_SendClientMessage(from, COL_PRIVMSG, buf4096a);
	csprintf(buf4096, "** %s(%d): %s", pdata[from]->name, from, msg);
	buf4096[143] = 0; /*message won't send if too long*/
	NC_SendClientMessage(to, COL_PRIVMSG, buf4096a);
	NC_PlayerPlaySound0(to, 1139 /*SOUND_CHECKPOINT_RED*/);
	if (lastpmtarget[to] == LAST_PMTARGET_NOBODY) {
		B144((char*) REPLYHINT);
		NC_SendClientMessage(to, COL_PRIVMSG_HINT, buf144a);
	}
	lastpmtarget[to] = from;
	return;
errmsg:
	NC_SendClientMessage(from, COL_WARN, buf144a);
	return;
}

/**
The /pm cmd. /cmd <id|playername> <message>

Sends a pm to given player.
*/
static
int pm_cmd_pm(CMDPARAMS)
{
	static const char
		*SYNERR = WARN"Syntax: /pm [id/name] [message]",
		*NOTONLINE = WARN"That player is not online.";

	int targetid;

	if (!cmd_get_player_param(cmdtext, &parseidx, &targetid)) {
synerr:		B144((char*) SYNERR);
		goto errmsg;
	}
	if (targetid == INVALID_PLAYER_ID) {
		B144((char*) NOTONLINE);
errmsg:		NC_SendClientMessage(playerid, COL_WARN, buf144a);
		return 1;
	}
	while (cmdtext[parseidx] == ' ') {
		parseidx++;
	}
	if (cmdtext[parseidx]) {
		pm_send(playerid, targetid, (char*) cmdtext + parseidx);
	} else {
		goto synerr;
	}
	return 1;
}

/**
The /r cmd. /r <message>

Sends a reply pm to the player that last sent a pm to the invoker.
*/
static
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
			B144((char*) NOTARGET);
			goto errmsg;
		case LAST_PMTARGET_INVALID:
			B144((char*) INVALID);
			goto errmsg;
		}
		pm_send(playerid, lastpmtarget[playerid],
			(char*) cmdtext + parseidx);
	} else {
		B144((char*) SYN);
errmsg:		NC_SendClientMessage(playerid, COL_WARN, buf144a);
		return 1;
	}
	return 1;
}
