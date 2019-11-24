
/* vim: set filetype=c ts=8 noexpandtab: */

/**
Player's session id, -1 if no valid session (should be extremely rare)
*/
extern int sessionid[MAX_PLAYERS];
/**
Logged-in status of each player (one of the LOGGED_ definitions).
*/
extern int loggedstatus[MAX_PLAYERS];

void login_init();