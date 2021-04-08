#define LOGGED_NO 0
#define LOGGED_IN 1
#define LOGGED_GUEST 2

#define ISPLAYING(PLAYERID) (loggedstatus[PLAYERID])

#define MONEY_DEFAULT_AMOUNT 15000

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
/**
Some data saved to use in the progress of login in, registering, or changing pw.
*/
extern void *pwdata[MAX_PLAYERS];
