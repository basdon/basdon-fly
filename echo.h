
/* vim: set filetype=c ts=8 noexpandtab: */

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
