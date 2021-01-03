static int score_flight_time[MAX_PLAYERS];
static int score_play_time[MAX_PLAYERS];

static
void score_on_player_connect(int playerid)
{
	score_flight_time[playerid] = 0;
	score_play_time[playerid] = 0;
}

static
int score_update_score(int playerid)
{
	int ft, pt, score;

	ft = score_flight_time[playerid];
	pt = score_play_time[playerid];

	score = (ft + (pt - ft) / 5) / 60;
	NC_SetPlayerScore(playerid, score);
	return score;
}
