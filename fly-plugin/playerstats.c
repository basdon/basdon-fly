#define LAST_SEEN_TIMER_VALUE 30000

char isafk[MAX_PLAYERS];
unsigned long lastupdate[MAX_PLAYERS];

/**
Contains playtime of players that hasn't been saved into the db yet.

When the player is afk, this value contains the playtime (in seconds) of the
player that hasn't been written to the db yet. If the player is not afk, this
contains the timestamp of last commit, thus to get the actual uncommitted
playtime, one would do time_timestamp - value.
*/
static int uncommitted_playtime[MAX_PLAYERS];

/**
Updates some player data/stats values in db

Updates last seen (usr and session when on a valid session) and total/actual
time value in db, along with other player stats (score, cash, ..).

This function first checks if the player has a valid userid. Also checks for a
valid session id before updating session values.

When isdisconnect is 0, 30 gets added to player's total time (inaccurate),
otherwise player's totaltime is set to sum of session times (accurate).

@param playerid playerid to update
@param isdisconnecting is this call made from OnPlayerDisconnect?
*/
static
void playerstats_commit_to_db(int playerid, int isdisconnecting)
{
	unsigned long now;
	int temp_ms, playtime_to_add;

	if (isafk[playerid]) {
		temp_ms = uncommitted_playtime[playerid];
		playtime_to_add = temp_ms / 1000;
		uncommitted_playtime[playerid] = temp_ms % 1000;
	} else {
		now = time_timestamp();
		temp_ms = now - uncommitted_playtime[playerid];
		playtime_to_add = temp_ms / 1000;
		uncommitted_playtime[playerid] = now - temp_ms % 1000;
	}

	/*TODO: why does this happen*/
	if (playtime_to_add > 60) {
		logprintf("it happened %d isafk %d isdisconnect %d",
			playtime_to_add, isafk[playerid], isdisconnecting);
		playtime_to_add = 5;
	}

	score_play_time[playerid] += playtime_to_add;
	score_update_score(playerid);

	if (userid[playerid] == -1) {
		return;
	}

	csprintf(buf4096,
		"UPDATE usr "
		"SET lastseengame=UNIX_TIMESTAMP(),onlinetime=onlinetime+%d,"
		"playtime=playtime+%d,"
		"score=%d,cash=%d,distance=%f,flighttime=%d,prefs=%d "
		"WHERE i=%d",
		isdisconnecting ? 0 : 30,
		playtime_to_add,
		score_update_score(playerid),
		money_get(playerid),
		playerodoKM[playerid],
		score_flight_time[playerid],
		prefs[playerid],
		userid[playerid]);
	NC_mysql_tquery_nocb(buf4096a);

	if (sessionid[playerid] == -1) {
		return;
	}

	csprintf(buf4096,
		"UPDATE ses "
		"SET e=UNIX_TIMESTAMP() "
		"WHERE i=%d",
		sessionid[playerid]);
	NC_mysql_tquery_nocb(buf4096a);

	if (!isdisconnecting) {
		return;
	}

	csprintf(buf4096,
		"UPDATE usr "
		"SET onlinetime=(SELECT SUM(e-s) FROM ses WHERE u=usr.i) "
		"WHERE i=%d",
		userid[playerid]);
	NC_mysql_tquery_nocb(buf4096a);
}

static
int playerstats_update_all(void *data)
{
	int n = playercount;
	while (n--) {
		playerstats_commit_to_db(players[n], 0);
		veh_save_user_model_stats(players[n]);
	}
	return LAST_SEEN_TIMER_VALUE;
}

void playerstats_init()
{
	timer_set(LAST_SEEN_TIMER_VALUE, playerstats_update_all, NULL);
}

void playerstats_check_for_afk()
{
	unsigned long now = time_timestamp();
	unsigned long asecondago = now - 2000;
	int n, playerid;

	n = playercount;
	while (n--) {
		playerid = players[n];
		if (!isafk[playerid] && lastupdate[playerid] < asecondago) {
			/*player now afk*/
			isafk[playerid] = 1;
			uncommitted_playtime[playerid] = now - uncommitted_playtime[playerid];

			panel_on_player_now_afk(playerid);
		}
	}
}

void playerstats_on_player_connect(int playerid)
{
	/*not changing lastupdate because player will be marked afk anyways*/
	/*player is afk because they're going into class selection*/
	isafk[playerid] = 1;
	/*playtime to 0 since the player is afk (see variable doc)*/
	uncommitted_playtime[playerid] = 0;
}

void playerstats_on_player_disconnect(int playerid)
{
	playerstats_commit_to_db(playerid, 1);
}

void playerstats_on_player_update(int playerid)
{
	unsigned long now = time_timestamp();

	if (isafk[playerid]) {
		/*player back from afk*/
		isafk[playerid] = 0;
		uncommitted_playtime[playerid] = now - uncommitted_playtime[playerid];

		panel_on_player_was_afk(playerid);
		timecyc_on_player_was_afk(playerid);
	}
	lastupdate[playerid] = now;
}
