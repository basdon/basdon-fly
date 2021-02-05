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
	char msg288[288];

	if (!(prefs[from] & PREF_ENABLE_PM)) {
		SendClientMessage(from, COL_WARN, WARN"Your PMs are disabled, use /p to enable them.");
		return;
	}
	if (!(prefs[to] & PREF_ENABLE_PM)) {
		SendClientMessage(from, COL_WARN, WARN"That player has PMs disabled.");
		return;
	}

	sprintf(msg288, ">> %s(%d): %s", pdata[to]->name, to, msg);
	SendClientMessage(from, COL_PRIVMSG, msg288);
	sprintf(msg288, "** %s(%d): %s", pdata[from]->name, from, msg);
	SendClientMessage(to, COL_PRIVMSG, msg288);

	NC_PlayerPlaySound0(to, 1139 /*SOUND_CHECKPOINT_RED*/);
	if (lastpmtarget[to] == LAST_PMTARGET_NOBODY) {
		SendClientMessage(to, COL_PRIVMSG_HINT, INFO"Use /r to quickly reply to the message.");
	}
	lastpmtarget[to] = from;
}

/**
The /pm cmd. /cmd <id|playername> <message>

Sends a pm to given player.
*/
static
int pm_cmd_pm(struct COMMANDCONTEXT cmdctx)
{
	int targetid;

	if (!cmd_get_player_param(&cmdctx, &targetid)) {
synerr:
		SendClientMessage(cmdctx.playerid, COL_WARN, WARN"Syntax: /pm [id/name] [message]");
		return 1;
	}
	if (targetid == INVALID_PLAYER_ID) {
		SendClientMessage(cmdctx.playerid, COL_WARN, WARN"That player is not online.");
		return 1;
	}
	while (cmdctx.cmdtext[cmdctx.parseidx] == ' ') {
		cmdctx.parseidx++;
	}
	if (cmdctx.cmdtext[cmdctx.parseidx]) {
		pm_send(cmdctx.playerid, targetid, cmdctx.cmdtext + cmdctx.parseidx);
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
int pm_cmd_r(struct COMMANDCONTEXT cmdctx)
{
	while (cmdctx.cmdtext[cmdctx.parseidx] == ' ') {
		cmdctx.parseidx++;
	}

	if (cmdctx.cmdtext[cmdctx.parseidx]) {
		switch (lastpmtarget[cmdctx.playerid]) {
		case LAST_PMTARGET_NOBODY:
			SendClientMessage(cmdctx.playerid, COL_WARN, WARN"Nobody has sent you a PM yet! Use /pm [id/name] [message]");
			return 1;
		case LAST_PMTARGET_INVALID:
			SendClientMessage(cmdctx.playerid, COL_WARN, WARN"The person who last sent you a PM left");
			return 1;
		default:
			pm_send(cmdctx.playerid, lastpmtarget[cmdctx.playerid], cmdctx.cmdtext + cmdctx.parseidx);
			return 1;
		}
	}

	SendClientMessage(cmdctx.playerid, COL_WARN, WARN"Syntax: /r [message]");
	return 1;
}
