
/* vim: set filetype=c ts=8 noexpandtab: */

/**
Total amount of seconds a player has spent playing (= online, not afk).
*/
extern int score_play_time[MAX_PLAYERS];
/**
Total amount of seconds a player has spent flying.
*/
extern int score_flight_time[MAX_PLAYERS];

void score_on_player_connect(int playerid);
/**
Recalculate and set (and return) a player's score.
*/
int score_update_score(int playerid);
