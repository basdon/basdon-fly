
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "score.h"

int score_flight_time[MAX_PLAYERS];
int score_play_time[MAX_PLAYERS];

void score_on_player_connect(int playerid)
{
	score_flight_time[playerid] = 0;
	score_play_time[playerid] = 0;
}

int score_update_score(int playerid)
{
	int score;

	score =
		score_flight_time[playerid] / 60
		+ score_play_time[playerid] / 60 / 5;
	NC_SetPlayerScore(playerid, score);
	return score;
}
