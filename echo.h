
/* vim: set filetype=c ts=8 noexpandtab: */

#define PACK_HELLO 2
#define PACK_IMTHERE 3
#define PACK_BYE 4
#define PACK_PING 5
#define PACK_PONG 6
#define PACK_CHAT 10
#define PACK_ACTION 11
#define PACK_FLIGHT_MESSAGE 12
#define PACK_PLAYER_CONNECTION 30

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

Call periodically, to up sockets in the case they couldn't be started from
OnGameModeInit.
*/
void echo_init();
/**
Send flight finish message to IRC echo.

Uses buf4096.
*/
void echo_on_flight_finished(char *text);
/**
Send game chat or action to IRC echo.

Call from OnPlayerText with t 0 or from /me cmd handler with t 1.

@param t type, either 0 for normal chat or 1 for action
*/
void echo_on_game_chat_or_action(int t, int playerid, char *text);
/**
Send player connection packet to IRC echo.

@param playerid playerid that (dis)connected
@param reason 3 when OnPlayerConnection, reason when OnPlayerDisconnect
*/
void echo_on_player_connection(int playerid, int reason);
/**
Handle received UDP packet.
Call from onUDPReceiveData
*/
void echo_on_receive(cell socket, cell data_a, char *data, int len);
