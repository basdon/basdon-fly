
/* vim: set filetype=c ts=8 noexpandtab: */

/**
User id of player, -1 if no valid session (should be extremely rare)

Starts from 1
*/
extern int userid[MAX_PLAYERS];
/**
Player's session id, -1 if no valid session (should be extremely rare)

Starts from 1
*/
extern int sessionid[MAX_PLAYERS];
/**
Logged-in status of each player (one of the LOGGED_ definitions).
*/
extern int loggedstatus[MAX_PLAYERS];

void login_init();
void login_on_player_connect(int playerid);