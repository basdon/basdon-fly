
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "score.h"

int score_flight_time[MAX_PLAYERS];

void score_on_player_connect(int playerid)
{
	score_flight_time[playerid] = 0;
}