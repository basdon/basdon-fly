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
	TRACE;
	lastpmtarget[playerid] = -1;
}

/**
Change every player that has given player as pm target to invalid target id.
*/
static
void pm_on_player_disconnect(int playerid)
{
	TRACE;
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
	TRACE;
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

	PlayerPlaySound(to, PM_RECEIVE_SOUND);
	if (lastpmtarget[to] == LAST_PMTARGET_NOBODY) {
		SendClientMessage(to, COL_PRIVMSG_HINT, INFO"Use /r to quickly reply to the message.");
	}
	lastpmtarget[to] = from;
}
