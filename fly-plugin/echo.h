
/* vim: set filetype=c ts=8 noexpandtab: */

#define PACK_HELLO 2
#define PACK_IMTHERE 3
#define PACK_BYE 4
#define PACK_PING 5
#define PACK_PONG 6
#define PACK_STATUS_REQUEST 7
#define PACK_STATUS 8
#define PACK_CHAT 10
#define PACK_ACTION 11
#define PACK_GENERIC_MESSAGE 12
#define PACK_PLAYER_CONNECTION 30

#define ECHO_PACK12_FLIGHT_MESSAGE 0
#define ECHO_PACK12_TRAC_MESSAGE 1
#define ECHO_PACK12_IRC_MODE 3
#define ECHO_PACK12_IRC_TOPIC 4
#define ECHO_PACK12_IRC_NICK 5
#define ECHO_PACK12_PROTIP 6

#define ECHO_CONN_REASON_GAME_TIMEOUT 0
#define ECHO_CONN_REASON_GAME_QUIT 1
#define ECHO_CONN_REASON_GAME_KICK 2
#define ECHO_CONN_REASON_GAME_CONNECTED 3
#define ECHO_CONN_REASON_IRC_QUIT 6
#define ECHO_CONN_REASON_IRC_PART 7
#define ECHO_CONN_REASON_IRC_KICK 8
#define ECHO_CONN_REASON_IRC_JOIN 9

/**
Destroys the sockets used by the echo service.
Call from OnGameModeExit
*/
void echo_dispose();
/**
Creates the echo sockets.

This is a timer callback and should be called by a 0 interval timer on game
mode init. Timer will keep repeating until both listen and send sockets are
successfully created.

Call periodically, to up sockets in the case they couldn't be started from
OnGameModeInit.
*/
int echo_init(void *data);
/**
Send game chat or action to IRC echo.

Call from OnPlayerText with t 0 or from /me cmd handler with t 1.

@param t type, either 0 for normal chat or 1 for action
*/
void echo_on_game_chat_or_action(int t, int playerid, char *text);
/**
Handle received UDP packet.
Call from onUDPReceiveData
*/
void echo_on_receive(cell socket, cell data_a, char *data, int len);
